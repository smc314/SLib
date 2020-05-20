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


HelixFSFile::HelixFSFile(const twine& folderName, const twine& fileName) 
	: m_folder( folderName ), m_file( fileName )
{
	EnEx ee(FL, "HelixFSFile::HelixFSFile()");

	if(folderName.empty()){
		throw AnException(0, FL, "Missing folder name");
	}
	if(fileName.empty()){
		throw AnException(0, FL, "Missing file name");
	}
	m_physical_file = "";

	try {
		File tmp( PhysicalFileName() );
		m_file_last_modified = tmp.lastModified();
		m_file_size = tmp.size();
	} catch(AnException&){
		// Means the file doesn't exist
		m_file_last_modified.SetMinValue();
		m_file_size = 0;
	}
}

HelixFSFile::~HelixFSFile()
{
	EnEx ee(FL, "HelixFSFile::~HelixFSFile()");

}

void HelixFSFile::Reload()
{
	EnEx ee(FL, "HelixFSFile::Reload()");
	try {
		File tmp( PhysicalFileName() );
		m_file_last_modified = tmp.lastModified();
		m_file_size = tmp.size();
	} catch(AnException&){
		// Means the file doesn't exist
		m_file_last_modified.SetMinValue();
		m_file_size = 0;
	}
	m_lines.clear();
	m_deps.clear();
	m_deps_loaded = false;
	m_xml_doc = nullptr;
	m_api_list.clear();
}

const twine& HelixFSFile::FileName() const
{
	return m_file;
}

const twine& HelixFSFile::FolderName() const
{
	return m_folder;
}

long HelixFSFile::FileSize()
{
	return m_file_size;
}

Date HelixFSFile::LastModified()
{
	return m_file_last_modified;
}

twine HelixFSFile::LastFolderName()
{
	EnEx ee(FL, "HelixFSFile::LastFolderName()");
	auto splits = m_folder.split("/");
	if(splits.size() == 0){
		return "";
	} else {
		return splits[ splits.size() - 1 ];
	}
}

const twine& HelixFSFile::PhysicalFileName() const
{
	EnEx ee(FL, "HelixFSFile::PhysicalFileName()");
	if(m_physical_file.empty()){
		if(m_folder == "root"){
			m_physical_file.append("./").append(m_file);
		} else {
			m_physical_file.append("./").append(m_folder).append("/").append(m_file);
		}
	} 
	return m_physical_file;
}

const twine& HelixFSFile::PhysicalDotOh() const
{
	EnEx ee(FL, "HelixFSFile::PhysicalDotOh()");
	if(m_physical_dotoh.empty()){
		if(m_folder == "root"){
			m_physical_dotoh = "./" + DotOh();
		} else {
			m_physical_dotoh = "./" + m_folder + "/" + DotOh();
		}
	}
	return m_physical_dotoh;
}

bool HelixFSFile::FromCore() const
{
	EnEx ee(FL, "HelixFSFile::FromCore()");
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

const vector<twine>& HelixFSFile::Lines()
{
	EnEx ee(FL, "HelixFSFile::Lines()");

	// If we already have it - just return immediately
	if(m_lines.size() != 0) return m_lines;

	File sourceFile( PhysicalFileName() );
	m_lines = sourceFile.readLines();
	return m_lines;
}

vector<pair<twine, twine>>& HelixFSFile::Apis()
{
	EnEx ee(FL, "HelixFSFile::Apis()");

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

bool HelixFSFile::NeedsRebuild()
{
	EnEx ee(FL, "HelixFSFile::NeedsRebuild()");

	// First check to see if we are newer than our .o file:
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
	vector<twine> splits = m_file.split(".");	
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

const vector<HelixFSFile*>& HelixFSFile::Dependencies()
{
	EnEx ee(FL, "HelixFSFile::Dependencies()");

	// If we already have it - just return immediately
	if(m_deps_loaded) return m_deps;

	if(m_deps.size() == 0){
		LoadDependenciesExplicitly();
		m_deps_loaded = true;
	}

	// Now run through our list of explicit dependencies, and add in all of their dependencies to
	// get both our own explicit dependencies and also our implicit dependencies
	for(size_t i = 0; i < m_deps.size(); i++){ // Don't use a range-for, others are changing this as we go
		AddChildDeps( m_deps[i] );
	}

	return m_deps;
}

void HelixFSFile::LoadDependenciesExplicitly()
{
	EnEx ee(FL, "HelixFSFile::LoadDependenciesExplicitly()");

	//printf("%s - LoadDependenciesExplicitly\n", FileName()() );

	// Explicit dependencies are the #include's that we have directly in our own file
	twine tokenlist(TWINE_WS); tokenlist.append("#\"");
	for(auto& line : Lines()){
		if(line.startsWith("#include")){
			vector<twine> tokens = line.tokenize( tokenlist );			
			if(tokens.size() != 2) continue; // Don't know hot to deal with this
			if(tokens[1][0] == '<') continue; // #include of a system file - ignore these

			auto dep = FindDep( tokens[1] );
			if(dep != nullptr){
				AddUniqueDep( dep );
			}
		}
	}
}

void HelixFSFile::AddChildDeps(HelixFSFile* dep)
{
	EnEx ee(FL, "HelixFSFile::AddChildDeps(HelixFSFile* dep)");

	if(dep == nullptr){
		return; // Handed a nullptr object - skip it
	}

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

bool HelixFSFile::AddUniqueDep(HelixFSFile* dep)
{
	EnEx ee(FL, "HelixFSFile::AddUniqueDep(HelixFSFile* dep)");

	if(dep == nullptr){
		return false; // bad dependency
	}

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

HelixFSFile* HelixFSFile::FindDep(const twine& dependentFile)
{
	EnEx ee(FL, "HelixFSFile::FindDep(const twine& dependentFile)");

	// First check in our current folder:
	auto currentFolder = HelixFS::getInstance().FindPath( m_folder );
	if(currentFolder == nullptr){
		WARN(FL, "Danger! Current Path (%s) not found in HelixFS.FindPath\n", m_folder() );
		return HelixFS::getInstance().FindFile( dependentFile );	
	} else {
		auto fsFile = currentFolder->FindFile( dependentFile ); // Search here first
		if(!fsFile){
			return HelixFS::getInstance().FindFile( dependentFile ); // Search everywhere else
		} else {
			return fsFile;
		}
	}
}

xmlDocPtr HelixFSFile::Xml()
{
	EnEx ee(FL, "HelixFSFile::Xml()");

	// Check to see if we've loaded the doc before - if not, then load it now
	if(m_xml_doc == nullptr){
		m_xml_doc = xmlParseFile( PhysicalFileName()() );
		if(m_xml_doc == nullptr){
			throw AnException(0, FL, "Error parsing file as xml: %s", PhysicalFileName()() );
		}
	}

	return m_xml_doc;
}

twine HelixFSFile::DataObjectName()
{
	EnEx ee(FL, "HelixFSFile::DataObjectName()");

	xmlNodePtr root = xmlDocGetRootElement( Xml() );
	twine doName( root, "generateClass" );
	return doName;
}

const twine& HelixFSFile::DotOh() const
{
	EnEx ee(FL, "HelixFSFile::DotOh()");

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

bool HelixFSFile::IsNewerThan(HelixFSFile* other)
{
	EnEx ee(FL, "HelixFSFile::IsNewerThan(HelixFSFile* other)");
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

bool HelixFSFile::IsNewerThan(const twine& otherFilePath)
{
	EnEx ee(FL, "HelixFSFile::IsNewerThan(const twine& otherFilePath)");

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

bool HelixFSFile::IsNewerThan(const twine& ourPhysicalName, const twine& theirPhysicalName)
{
	EnEx ee(FL, "HelixFSFile::IsNewerThan(const twine& ourPhysicalName, const twine& theirPhysicalName)");

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
