/* **************************************************************************

   Copyright (c): 2008 - 2013 Hericus Software, LLC

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#include <algorithm>

#include <AnException.h>
#include <EnEx.h>
#include <Log.h>
#include <XmlHelpers.h>
#include <Timer.h>
using namespace SLib;

#include "HelixFS.h"
#include "HelixFSFile.h"
#include "HelixConfig.h"
using namespace Helix::Build;


HelixFSFile_Bare::HelixFSFile_Bare(const twine& folderName, const twine& fileName) 
	: m_folder( folderName ), m_file( fileName )
{
	EnEx ee(FL, "HelixFSFile_Bare::HelixFSFile_Bare()");

	try {
		File tmp( PhysicalFileName() );
		m_file_last_modified = tmp.lastModified();
		m_file_size = tmp.size();
	} catch(AnException& e){
		// Means the file doesn't exist
		m_file_last_modified.SetMinValue();
		m_file_size = 0;
	}
}

HelixFSFile_Bare::~HelixFSFile_Bare()
{
	EnEx ee(FL, "HelixFSFile_Bare::~HelixFSFile_Bare()");

}

void HelixFSFile_Bare::Reload()
{
	try {
		File tmp( PhysicalFileName() );
		m_file_last_modified = tmp.lastModified();
		m_file_size = tmp.size();
	} catch(AnException& e){
		// Means the file doesn't exist
		m_file_last_modified.SetMinValue();
		m_file_size = 0;
	}
	m_lines.clear();
}

const twine& HelixFSFile_Bare::FileName() const
{
	return m_file;
}

const twine& HelixFSFile_Bare::FolderName() const
{
	return m_folder;
}

long HelixFSFile_Bare::FileSize()
{
	return m_file_size;
}

Date HelixFSFile_Bare::LastModified()
{
	return m_file_last_modified;
}

twine HelixFSFile_Bare::LastFolderName()
{
	auto splits = m_folder.split("/");
	if(splits.size() == 0){
		return "";
	} else {
		return splits[ splits.size() - 1 ];
	}
}

const twine& HelixFSFile_Bare::PhysicalFileName() const
{
	if(m_physical_file.empty()){
		if(m_folder == "root"){
			m_physical_file = "./" + m_file;
		} else {
			m_physical_file = "./" + m_folder + "/" + m_file;
		}
	} 
	return m_physical_file;
}

const twine& HelixFSFile_Bare::PhysicalDotOh() const
{
	if(m_physical_dotoh.empty()){
		if(m_folder == "root"){
			m_physical_dotoh = "./" + DotOh();
		} else {
			m_physical_dotoh = "./" + m_folder + "/" + DotOh();
		}
	}
	return m_physical_dotoh;
}

bool HelixFSFile_Bare::FromCore() const
{
	bool useCore = HelixConfig::getInstance().UseCore();
	if(!useCore){
		return false; // Not using core, so never comes from core
	}
	auto coreFolder = HelixConfig::getInstance().CoreFolder();
	if(m_folder.startsWith( coreFolder )){
		return true;
	} else {
		return false;
	}
}

const vector<twine>& HelixFSFile_Bare::Lines()
{
	EnEx ee(FL, "HelixFSFile_Bare::Lines()");

	// If we already have it - just return immediately
	if(m_lines.size() != 0) return m_lines;

	File sourceFile( PhysicalFileName() );
	m_lines = sourceFile.readLines();
	return m_lines;
}

vector<pair<twine, twine>>& HelixFSFile_Bare::Apis()
{
	EnEx ee(FL, "HelixFSFile_Bare::Apis()");

	// If we already have it - just return it
	if(m_api_list.size() != 0){
		return m_api_list;
	}

	// If not, build it first, then return it
	for(auto& line : Lines()){
		if(line.startsWith("// LOGICCODEGEN")){
			twine parse(TWINE_WS);
			parse.append("=");
			vector<twine> tokens = line.tokenize( parse );
			if(tokens.size() != 6) continue; // Don't understand this line
			m_api_list.push_back({ tokens[3], tokens[5] });
		}
	}

	return m_api_list;
}

bool HelixFSFile_Bare::NeedsRebuild()
{
	EnEx ee(FL, "HelixFSFile_Bare::NeedsRebuild()");

	// First check to see if we are newer than our .o file:
	vector<twine> splits = m_file.split(".");	
	if(IsNewerThan( PhysicalDotOh() )){
		DEBUG(FL, "%s is newer than %s - rebuild required", PhysicalFileName()(), PhysicalDotOh()() );
		return true; // We are newer than our .o file, no need to check deps, need to build
	}

	// Walk our dependencies and see if any of them are newer than our .o file
	// If so, then we need a rebuild.
	for(auto file : Dependencies() ){
		if(file->IsNewerThan( PhysicalDotOh() )){
			DEBUG(FL, "Dependency %s is newer than us %s - rebuild required", file->FileName()(), PhysicalDotOh()() );
			return true; // Found a dependency that is newer than us
		}
	}

	/*
	// Find our .h file and see if anything it depends on needs to be rebuilt
	if(splits[1] == "cpp"){
		twine dothName( splits[0] + ".h" );
		HelixFSFile doth = FindDep( dothName );
		if(doth){
			if(doth->NeedsRebuild()){
				return true; // Our .h file has a dependency newer than us.
			}
		}
	}
	*/

	return false;
}

const vector<HelixFSFile>& HelixFSFile_Bare::Dependencies()
{
	EnEx ee(FL, "HelixFSFile_Bare::Dependencies()");

	// If we already have it - just return immediately
	if(m_deps_loaded) return m_deps;

	// Load from cache first
	//LoadDependenciesFromCache();
	if(m_deps.size() == 0){
		LoadDependenciesExplicitly();
	}

	// Now run through our list of explicit dependencies, and add in all of their dependencies to
	// get both our own explicit dependencies and also our implicit dependencies
	size_t explicit_count = m_deps.size();
	for(size_t i = 0; i < explicit_count; i++){
		AddChildDeps( m_deps[ i ] );
	}

	m_deps_loaded = true;
	return m_deps;
}

void HelixFSFile_Bare::LoadDependenciesFromCache()
{
	EnEx ee(FL, "HelixFSFile_Bare::LoadDependenciesFromCache()");

	auto cache = HelixFS::getInstance().GetCache();
	if(cache == nullptr) {
		printf("Cache not found\n");
		return;
	}
	auto root = xmlDocGetRootElement( cache );
	if(root == nullptr) {
		printf("Cache root not found\n");
		return;
	}
	auto deps = XmlHelpers::FindChild( root, "FileDeps" );
	if(deps == nullptr) {
		//printf("FileDeps not found\n");
		return;
	}
	auto ourFile = XmlHelpers::FindChildWithAttribute( deps, "File", "Physical", PhysicalFileName()() );
	if(ourFile == nullptr) {
		//printf("Our File: %s not found in cache.\n", PhysicalFileName()() );
		return;
	}

	Date depsCreated = XmlHelpers::getDateAttr( ourFile, "Created" );
	File physFile( PhysicalFileName() );
	if( physFile.lastModified() > depsCreated){
		//printf("Our File: %s is newer than cache timestamp: %s\n", depsCreated.GetValue("%Y/%m/%d %H:%M:%S")() );
		return; // Our file is newer than our cache of dependencies.  This invalidates the cache.  Don't read it.
	}

	// Dependency cache is still up to date.  Read them in:
	auto depFiles = XmlHelpers::FindChildren( ourFile, "Dependency" );
	for(auto depFile : depFiles){
		twine depName( depFile, "name" );
		//printf("File: %s - loaded Dep %s from cache\n", PhysicalFileName()(), depName() );
		HelixFSFile dep = FindDep( depName );
		if(dep != nullptr){
			AddUniqueDep( dep );
		}
	}

}

void HelixFSFile_Bare::LoadDependenciesExplicitly()
{
	EnEx ee(FL, "HelixFSFile_Bare::LoadDependenciesExplicitly()");

	//printf("%s - LoadDependenciesExplicitly\n", FileName()() );

	// Explicit dependencies are the #include's that we have directly in our own file
	twine tokenlist(TWINE_WS); tokenlist.append("#\"");
	for(auto& line : Lines()){
		if(line.startsWith("#include")){
			vector<twine> tokens = line.tokenize( tokenlist );			
			if(tokens.size() != 2) continue; // Don't know hot to deal with this
			if(tokens[1][0] == '<') continue; // #include of a system file - ignore these

			HelixFSFile dep = FindDep( tokens[1] );
			if(dep != nullptr){
				AddUniqueDep( dep );
			}
		}
	}

	//SaveExplicitDependenciesToCache();
}

void HelixFSFile_Bare::SaveExplicitDependenciesToCache()
{
	EnEx ee(FL, "HelixFSFile_Bare::SaveExplicitDependenciesToCache()");

	auto cache = HelixFS::getInstance().GetCache();
	if(cache == nullptr) return;
	auto root = xmlDocGetRootElement( cache );
	if(root == nullptr) return;
	auto deps = XmlHelpers::FindChild( root, "FileDeps" );
	if(deps == nullptr){
		deps = xmlNewChild( root, NULL, (const xmlChar*)"FileDeps", NULL);
	}
	auto ourFile = XmlHelpers::FindChildWithAttribute( deps, "File", "Physical", PhysicalFileName()() );
	if(ourFile != nullptr) {
		// Wipe out our node and any children that our file node may already have:
		xmlUnlinkNode( ourFile );
		xmlFreeNode( ourFile );
	}

	// Re-create our node from scratch
	ourFile = xmlNewChild( deps, NULL, (const xmlChar*)"File", NULL );
	xmlSetProp( ourFile, (const xmlChar*)"Physical", PhysicalFileName() );
	Date now;
	XmlHelpers::setDateAttr( ourFile, "Created", now);

	// Add all of our explicit dependencies
	for(auto dep : m_deps){
		auto depNode = xmlNewChild( ourFile, NULL, (const xmlChar*)"Dependency", NULL );
		xmlSetProp( depNode, (const xmlChar*)"name", dep->FileName() );
	}
}

void HelixFSFile_Bare::AddChildDeps(HelixFSFile dep)
{
	EnEx ee(FL, "HelixFSFile_Bare::AddChildDeps(HelixFSFile dep)");

	if(dep->PhysicalFileName() == PhysicalFileName()){
		return; // Child dep is the same as ourself - hit a loop, break out
	}

	for(auto file : dep->Dependencies()){
		bool newDep = AddUniqueDep( file );
		if(newDep){
			AddChildDeps( file );
		}
	}
}

bool HelixFSFile_Bare::AddUniqueDep(HelixFSFile dep)
{
	EnEx ee(FL, "HelixFSFile_Bare::AddUniqueDep(HelixFSFile dep)");

	// Check to see if we already have this dependency
	for(auto file : m_deps){
		if(file->PhysicalFileName() == dep->PhysicalFileName()){
			return false; // Already have this one.
		}
	}

	// Go ahead and add it to our list of dependencies
	m_deps.push_back( dep );

	// Indicate that we added something new
	return true;
}

HelixFSFile HelixFSFile_Bare::FindDep(const twine& dependentFile)
{
	EnEx ee(FL, "HelixFSFile_Bare::FindDep(const twine& dependentFile)");

	HelixFSFile fsFile;
	// First check in our current folder:
	HelixFSFolder currentFolder = HelixFS::getInstance().FindPath( m_folder );
	if(!currentFolder){
		WARN(FL, "Danger! Current Path (%s) not found in HelixFS.FindPath\n", m_folder() );
		fsFile = HelixFS::getInstance().FindFile( dependentFile );	
	} else {
		fsFile = currentFolder->FindFile( dependentFile ); // Search here first
		if(!fsFile){
			fsFile = HelixFS::getInstance().FindFile( dependentFile ); // Search everywhere else
		}
	}
	
	return fsFile;
}

xmlDocPtr HelixFSFile_Bare::Xml()
{
	EnEx ee(FL, "HelixFSFile_Bare::Xml()");

	// Check to see if we've loaded the doc before - if not, then load it now
	if(m_xml_doc == nullptr){
		m_xml_doc = xmlParseFile( PhysicalFileName()() );
		if(m_xml_doc == nullptr){
			throw AnException(0, FL, "Error parsing file as xml: %s", PhysicalFileName()() );
		}
	}

	return m_xml_doc;
}

twine HelixFSFile_Bare::DataObjectName()
{
	EnEx ee(FL, "HelixFSFile_Bare::DataObjectName()");

	xmlNodePtr root = xmlDocGetRootElement( Xml() );
	twine doName( root, "generateClass" );
	return doName;
}

const twine& HelixFSFile_Bare::DotOh() const
{
	EnEx ee(FL, "HelixFSFile_Bare::DotOh()");

	if(m_dotoh.empty()){
		vector<twine> splits = twine(m_file).split(".");
#ifdef _WIN32
		m_dotoh = splits[0] + ".obj";
#else
		m_dotoh = splits[0] + ".o";
#endif
	}
	return m_dotoh;
}

bool HelixFSFile_Bare::IsNewerThan(HelixFSFile other)
{
	EnEx ee(FL, "HelixFSFile_Bare::IsNewerThan(HelixFSFile other)");
	if(!other){
		// They don't exist, we're newer
		return true;
	}

	bool weExist = m_file_size != 0;
	bool theyExist = other->m_file_size != 0;
	if(!weExist && !theyExist) return false; // Both don't exist - kind of nonsensical
	if(weExist && !theyExist) return true;
	if(!weExist && theyExist) return false;

	// Now we know both exist - compare the dates
	if( LastModified() > other->LastModified() ){
		return true;
	} else {
		return false;
	}
}

bool HelixFSFile_Bare::IsNewerThan(const twine& otherFilePath)
{
	EnEx ee(FL, "HelixFSFile_Bare::IsNewerThan(const twine& otherFilePath)");

	// Try to find the other file in our file system first
	auto otherFile = HelixFS::getInstance().FindFile( otherFilePath );
	if(otherFile){
		return IsNewerThan( otherFile );
	}

	// Didn't find it in our file system, check the physical disk

	bool weExist = m_file_size != 0;
	bool theyExist = File::Exists( otherFilePath );
	if(!weExist && !theyExist) return false; // Both don't exist - kind of nonsensical
	if(weExist && !theyExist) return true;
	if(!weExist && theyExist) return false;

	// Now we know both exist - compare the dates
	File theirFile( otherFilePath );
	if( LastModified() > theirFile.lastModified() ){
		return true;
	} else {
		return false;
	}
}

bool HelixFSFile_Bare::IsNewerThan(const twine& ourPhysicalName, const twine& theirPhysicalName)
{
	EnEx ee(FL, "HelixFSFile_Bare::IsNewerThan(const twine& ourPhysicalName, const twine& theirPhysicalName)");

	auto ourFSFile = HelixFS::getInstance().FindFile( ourPhysicalName );
	if(ourFSFile){
		return ourFSFile->IsNewerThan( theirPhysicalName );
	}
	auto theirFSFile = HelixFS::getInstance().FindFile( theirPhysicalName );
	if(theirFSFile){
		return !theirFSFile->IsNewerThan( ourPhysicalName );
	}

	// Neither one is in the file system - compare them manually
	bool weExist = File::Exists( ourPhysicalName );
	bool theyExist = File::Exists( theirPhysicalName );

	// If they both don't exist - return false.  Kind of nonsensical compare
	if(!weExist && !theyExist) return false;

	// If we exist - but they don't exist, return true
	if(weExist && !theyExist) return true;

	// If we don't exist - but they do, return false
	if(!weExist && theyExist) return false;

	// Now we know both files exist - open them and check their lastModified dates
	File ourFile( ourPhysicalName );
	File theirFile( theirPhysicalName );
	if( ourFile.lastModified() > theirFile.lastModified()){
		return true;
	} else {
		return false;
	}
}
