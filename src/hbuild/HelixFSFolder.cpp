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

#include "HelixFSFolder.h"
#include "HelixConfig.h"
using namespace Helix::Build;


HelixFSFolder::HelixFSFolder(const twine& folderName) : m_name( folderName )
{
	EnEx ee(FL, "HelixFSFolder::HelixFSFolder(const twine& folderName)");

	m_files = HelixFSFile_svect( new std::vector<HelixFSFile*>() );
	m_folders = HelixFSFolder_svect( new std::vector<HelixFSFolder*>() );

	auto splits = m_name.split("/");	
	if(splits.size() > 0){
		m_last_name = splits[ splits.size() - 1 ];
	}
}

HelixFSFolder::~HelixFSFolder()
{
	EnEx ee(FL, "HelixFSFolder::~HelixFSFolder()");

}

void HelixFSFolder::Load()
{
	EnEx ee(FL, "HelixFSFolder::Load()");

	// Pick up all of our files:
	auto files = File::listFiles( PhysicalFolderName() );
	std::sort( files.begin(), files.end() );
	for(auto& file : files ) {
		if(file.endsWith( ".h" ) ||
			file.endsWith( ".c" ) ||
			file.endsWith( ".cpp" ) ||
			file.endsWith( ".xml" ) ||
			file.endsWith( ".sql" ) ||
			file.endsWith( ".jpg" ) ||
			file.endsWith( ".obj" ) ||
			file.endsWith( ".o" )
		){
			auto fsFile = new HelixFSFile( m_name, file );
			m_files->push_back( fsFile );
		}
	}

	if(m_name == "root"){
		return; // Don't recurse on the root folder
	}

	// Pick up all of our sub-folders
	for(auto& folder : File::listFolders( PhysicalFolderName() )){
		if(folder == "." || folder == ".." || folder.startsWith( "." )) continue; // Ignore these
		dptr<HelixFSFolder>fsFolder( new HelixFSFolder( m_name + "/" + folder ) );
		fsFolder->Load();
		m_folders->push_back( fsFolder.release() );
	}
}

const twine& HelixFSFolder::FolderName() const
{
	return m_name;
}

twine HelixFSFolder::PhysicalFolderName() const
{
	if(m_name == "root"){
		return "./";
	} else {
		return "./" + m_name;
	}
}

bool HelixFSFolder::FromCore() const
{
	bool useCore = HelixConfig::getInstance().UseCore();
	if(!useCore){
		return false; // Not using core, so never comes from core
	}
	auto coreFolder = HelixConfig::getInstance().CoreFolder();
	if(m_name.startsWith( coreFolder )){
		return true;
	} else {
		return false;
	}
}

const twine& HelixFSFolder::LastName() const
{
	return m_last_name;
}

vector<HelixFSFolder*>& HelixFSFolder::SubFolders()
{
	return *m_folders;
}

vector<HelixFSFile*>& HelixFSFolder::Files()
{
	return *m_files;
}

HelixFSFile* HelixFSFolder::FindFile(const twine& fileName)
{
	EnEx ee(FL, "HelixFSFolder::FindFile(const twine& fileName)");

	// Check the files that we own first:
	for(auto file : *m_files) if(file->FileName() == fileName) return file;

	// Search our list of sub-folders
	for(auto folder : *m_folders){
		auto ret = folder->FindFile( fileName );
		if(ret != nullptr){
			return ret;
		}
	}

	// Didn't find anything - return the null pointer
	return nullptr;
}

void HelixFSFolder::FindFilesByType(const twine& fileType, vector<HelixFSFile*>& results)
{
	EnEx ee(FL, "HelixFSFolder::FindFilesByType(const twine& fileType, vector<HelixFSFile*>& results)");

	// Check the files that we own first:
	for(auto file : *m_files){
		if(file->FileName().endsWith(fileType)){
			results.push_back( file );
		}
	}

	// Search our list of sub-folders
	for(auto folder : *m_folders){
		folder->FindFilesByType( fileType, results );
	}
}

HelixFSFolder* HelixFSFolder::FindFolder(const twine& folderName)
{
	EnEx ee(FL, "HelixFSFolder::FindFolder(const twine& folderName)");

	//printf("Looking for %s in %s\n", folderName(), m_name() );

	// All of our subfolders will start with our own folder name.
	twine search;
	if(!folderName.startsWith( m_name )){
		search = m_name + "/" + folderName;
	} else {
		search = folderName;
	}
	//printf("Search is %s\n", search() );

	// Search our list of sub-folders
	for(auto folder : *m_folders){
		//printf("Checking sub folder (%s) == search (%s)", folder.FolderName()(), search() );
		if(folder->FolderName() == search){
			//printf("...yes\n");
			return folder;
		}
		//printf("...no\n");
	}

	// Didn't find anything - return the null pointer
	return nullptr;
}

void HelixFSFolder::DeleteFile(const twine& fileName)
{
	EnEx ee(FL, "HelixFSFolder::DeleteFile(const twine& fileName)");

	// Check to see if we have the file:
	for(size_t i = 0; i < m_files->size(); i++){
		if(m_files->at(i)->FileName() == fileName){
			DEBUG(FL, "Removing FSFile Reference: %s/%s", m_name(), fileName() );
			delete m_files->at(i); // Delete the object
			m_files->erase( m_files->begin() + i ); // Erase the vector element
			break;
		}
	}

	// Physically remove the file if it exists
	twine physicalName( PhysicalFolderName() + "/" + fileName );
	if(File::Exists( physicalName )){
		DEBUG(FL, "Removing Physical File: %s", physicalName() );
		File::Delete( physicalName );
	}

}
