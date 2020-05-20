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
	m_folders = HelixFSFolder_svect( new std::vector<HelixFSFolder*>() );

	// Hard code our top level folders
	auto root = new HelixFSFolder( "root" ); root->Load(); m_folders->push_back( root );
	if(HelixConfig::getInstance().UseCore() == false){
		// Load these folders only when not using a core project helper
		//auto build = std::make_shared<HelixFSFolder_Bare>( "build" ); 
		//build->Load(); m_folders.push_back( build );
		auto client = new HelixFSFolder( "client" ); 
		client->Load(); m_folders->push_back( client );
		auto glob = new HelixFSFolder( "glob" ); 
		glob->Load(); m_folders->push_back( glob );
		auto server = new HelixFSFolder( "server" ); 
		server->Load(); m_folders->push_back( server );
	} else {
		// Load client, glob, server, logic/util, logic/admin from core because we need to do dependency
		// checking and c# code generation from them
		auto coreFolder = HelixConfig::getInstance().CoreFolder();
		auto client = new HelixFSFolder( coreFolder + "/server/c/client" ); 
		client->Load(); m_folders->push_back( client );
		auto glob = new HelixFSFolder( coreFolder + "/server/c/glob" ); 
		glob->Load(); m_folders->push_back( glob );
		auto server = new HelixFSFolder( coreFolder + "/server/c/server" ); 
		server->Load(); m_folders->push_back( server );
		auto admin = new HelixFSFolder( coreFolder + "/server/c/logic/admin" ); 
		admin->Load(); m_folders->push_back( admin );
		auto util = new HelixFSFolder( coreFolder + "/server/c/logic/util" ); 
		util->Load(); m_folders->push_back( util );
	}

	// Load our own local logic folder
	auto logic = new HelixFSFolder( "logic" ); logic->Load(); m_folders->push_back( logic );

	// Also load the logic folder for all external repos that we reference
	for(auto& repoName : HelixConfig::getInstance().LogicRepos() ){
		auto external_logic = new HelixFSFolder( "../../../" + repoName + "/server/c/logic" );
		external_logic->Load();
		m_folders->push_back( external_logic );
	}
}

HelixFSFile* HelixFS::FindFile(const twine& fileName)
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
	for(auto folder : *m_folders){
		auto ret = folder->FindFile( fileName );
		if(ret){
			return ret;
		}
	}
	
	// Didn't find anything - return the null pointer
	return nullptr;
}

vector<HelixFSFile*> HelixFS::FindFilesByType( const twine& fileType )
{
	vector<HelixFSFile*> ret;
	for(auto folder : *m_folders){
		folder->FindFilesByType( fileType, ret );
	}
	
	return ret;
}

HelixFSFolder* HelixFS::FindFolder(const twine& folderName)
{
	// Search our list of sub-folders
	for(auto folder : *m_folders){
		if(folder->FolderName() == folderName){
			return folder;
		}
	}

	// Didn't find anything - return the null pointer
	return nullptr;
}

HelixFSFolder* HelixFS::FindPath(const twine& folderPath)
{
	EnEx ee(FL, "HelixFS::FindPath(const twine& folderPath)");

	twine path( folderPath );

	// See which of our top level folders best matches the given path
	HelixFSFolder* topFolder = nullptr;
	for(auto folder : *m_folders){
		//DEBUG(FL,"FindPath comparing target(%s) to root folder(%s)", folderPath(),folder->FolderName()() );
		if(path.startsWith( folder->FolderName() )){
			topFolder = folder;
			break;
		}
	}
	if(topFolder == nullptr){
		//DEBUG(FL, "FindPath root folder not found for target(%s)", folderPath() );
		return nullptr;
	}

	if(path.length() == topFolder->FolderName().length()){
		// Exact match, just return topFolder
		//DEBUG(FL, "FindPath root folder match for target(%s)", folderPath() );
		return topFolder;
	}

	// Now look for the rest of the path from that top folder - minus the prefix that already matched
	path = path.substr( topFolder->FolderName().length() );
	if(path.startsWith( "/" )){
		path = path.substr( 1 ); // remove the leading slash
	}

	auto pathSplits = path.split("/");
	if(pathSplits.size() == 0){
		//DEBUG(FL, "FindPath pathSplits == 0");
		return nullptr;
	}

	// Start with the top level folder first:
	auto currentFolder = topFolder->FindFolder( pathSplits[0] );
	if(currentFolder == nullptr) {
		//DEBUG(FL, "FindPath could not find child folder %s in root folder %s", pathSplits[0](), topFolder->FolderName()() );
		return nullptr;
	}
	if(pathSplits.size() == 1){
		return currentFolder; // Already found it - there's only 1 subfolder in the path
	} else {
		pathSplits.erase( pathSplits.begin() ); // we've already looked this one up
	}

	// Walk through the remaining path items
	for(auto& pathItem : pathSplits){
		auto subFolder = currentFolder->FindFolder( pathItem );
		if(!subFolder) {
			DEBUG(FL, "FindPath: Leaf node (%s) not found for target path %s", pathItem(), folderPath() );
			return nullptr;
		}
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

twine HelixFS::OurRepo()
{
	twine pwd = File::Pwd();
#ifdef _WIN32
	pwd.replace( '\\', '/' ); // Normalize all path separators to forward-slash
#endif

	auto splits = pwd.split( "/" );
	// We live in the folder blah/blah/blah/reponame/server/c folder  We want size - 3 as the reponame
	if(splits.size() < 3){
		throw AnException(0, FL, "Cannot determine our repo - path must have at least 3 components: %s", pwd() );
	}
	return splits[ splits.size() - 3 ];
}
