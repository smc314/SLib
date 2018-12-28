/* **************************************************************************

   Copyright (c): 2008 - 2013 Hericus Software, LLC

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#include <AnException.h>
#include <EnEx.h>
#include <Log.h>
#include <XmlHelpers.h>
#include <Timer.h>
using namespace SLib;

#include "HelixFS.h"
#include "HelixConfig.h"
using namespace Helix::Build;

// Our global instance of the filesystem
static HelixFS* m_helix_fs = nullptr;

HelixFS& HelixFS::getInstance()
{
	EnEx ee(FL, "HelixFS::getInstance()", true);

	if(m_helix_fs == nullptr){
		m_helix_fs = new HelixFS();
		m_helix_fs->Load();
	}
	return *m_helix_fs;
}

HelixFS::HelixFS()
{
	EnEx ee(FL, "HelixFS::HelixFS()");

}

HelixFS::~HelixFS()
{
	EnEx ee(FL, "HelixFS::~HelixFS()");

}

void HelixFS::Load()
{
	EnEx ee(FL, "HelixFS::Load()");

	// Clear out everything we might have loaded and reload from scratch
	m_folders.clear();

	// Hard code our top level folders
	auto root = std::make_shared<HelixFSFolder_Bare>( "root" ); root->Load(); m_folders.push_back( root );
	if(HelixConfig::getInstance().UseCore() == false){
		// Load these folders only when not using a core project helper
		auto build = std::make_shared<HelixFSFolder_Bare>( "build" ); 
		build->Load(); m_folders.push_back( build );
		auto client = std::make_shared<HelixFSFolder_Bare>( "client" ); 
		client->Load(); m_folders.push_back( client );
		auto glob = std::make_shared<HelixFSFolder_Bare>( "glob" ); 
		glob->Load(); m_folders.push_back( glob );
		auto server = std::make_shared<HelixFSFolder_Bare>( "server" ); 
		server->Load(); m_folders.push_back( server );
	}
	auto logic = std::make_shared<HelixFSFolder_Bare>( "logic" ); logic->Load(); m_folders.push_back( logic );
}

HelixFSFile HelixFS::FindFile(const twine& fileName)
{
	twine searchName;
	if(fileName.startsWith( "../" )){
		searchName = fileName.substr(3);
	} else if(fileName.startsWith( "./" )){
		searchName = fileName.substr(2);
	} else {
		searchName = fileName;
	}
	if(searchName.find( "/" ) != TWINE_NOT_FOUND){
		// Folder + filename.  Find the folder first, and then find the file in the target folder
		size_t lastSlash = searchName.rfind( "/" );
		twine searchPath( searchName.substr(0, lastSlash) ); // Everything but the last part
		twine searchFile( searchName.substr( lastSlash + 1 ) ); // Only the last part
		auto targetFolder = FindPath( searchPath );
		if(targetFolder){
			auto ret = targetFolder->FindFile( searchFile );
			if(ret){
				return ret;
			} else {
				return nullptr;
			}
		} else {
			return nullptr;
		}
	}

	// Search our list of folders one by one for the first match of the file name
	for(auto folder : m_folders){
		HelixFSFile ret = folder->FindFile( fileName );
		if(ret){
			return ret;
		}
	}
	
	// Didn't find anything - return the null pointer
	return nullptr;
}

vector<HelixFSFile> HelixFS::FindFilesByType( const twine& fileType )
{
	vector<HelixFSFile> ret;
	for(auto folder : m_folders){
		folder->FindFilesByType( fileType, ret );
	}
	
	return ret;
}

HelixFSFolder HelixFS::FindFolder(const twine& folderName)
{
	// Search our list of sub-folders
	for(auto folder : m_folders){
		if(folder->FolderName() == folderName){
			return folder;
		}
	}

	// Didn't find anything - return the null pointer
	return nullptr;
}

HelixFSFolder HelixFS::FindPath(const twine& folderPath)
{
	twine path( folderPath );
	vector<twine> pathSplits = path.split("/");
	if(pathSplits.size() == 0){
		return nullptr;
	}

	// Find the top level folder first:
	HelixFSFolder currentFolder = FindFolder( pathSplits[0] );
	if(currentFolder == nullptr) return nullptr;

	for(size_t i = 1; i < pathSplits.size(); i++){
		HelixFSFolder subFolder = currentFolder->FindFolder( pathSplits[i] );
		if(!subFolder) return nullptr;
		currentFolder = subFolder;
	}

	return currentFolder;
}

twine HelixFS::FixPhysical(const twine& path)
{
	twine ret(path);
#ifdef _WIN32
	ret.replace( '/', '\\' );
#endif
	return ret;
}

xmlDocPtr HelixFS::GetCache()
{
	if(m_hbuild_cache == nullptr){
		if(File::Exists( "hbuild.cache" )){
			m_hbuild_cache = xmlParseFile( "hbuild.cache" );
			if(m_hbuild_cache == nullptr){
				throw AnException(0, FL, "Error parsing hbuild.cache as xml.");
			}
		} else {
			m_hbuild_cache = xmlNewDoc((const xmlChar*)"1.0");
			m_hbuild_cache->children = xmlNewDocNode(m_hbuild_cache, NULL, (const xmlChar*)"HBuild", NULL);
		}
	}
	return m_hbuild_cache;
}

void HelixFS::SaveCache()
{
	xmlSaveFile( "hbuild.cache", GetCache() );
}

