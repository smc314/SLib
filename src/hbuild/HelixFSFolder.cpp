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

#include "HelixFSFolder.h"
using namespace Helix::Build;


HelixFSFolder_Bare::HelixFSFolder_Bare(const twine& folderName) : m_name( folderName )
{
	EnEx ee(FL, "HelixFSFolder_Bare::HelixFSFolder_Bare(const twine& folderName)");

	vector<twine> splits = m_name.split("/");	
	m_last_name = splits[ splits.size() - 1 ];
}

HelixFSFolder_Bare::~HelixFSFolder_Bare()
{
	EnEx ee(FL, "HelixFSFolder_Bare::~HelixFSFolder_Bare()");

}

void HelixFSFolder_Bare::Load()
{
	EnEx ee(FL, "HelixFSFolder_Bare::Load()");

	// Pick up all of our files:
	for(auto& file : File::listFiles( PhysicalFolderName() ) ){
		if(file.endsWith( ".h" ) ||
			file.endsWith( ".c" ) ||
			file.endsWith( ".cpp" ) ||
			file.endsWith( ".xml" ) ||
			file.endsWith( ".sql" ) ||
			file.endsWith( ".jpg" ) ||
			file.endsWith( ".obj" )
		){
			auto fsFile = std::make_shared<HelixFSFile_Bare>( m_name, file );
			m_files.push_back( fsFile );
		}
	}

	if(m_name == "root"){
		return; // Don't recurse on the root folder
	}

	// Pick up all of our sub-folders
	for(auto& folder : File::listFolders( PhysicalFolderName() )){
		if(folder == "." || folder == ".." || folder.startsWith( "." )) continue; // Ignore these
		auto fsFolder = std::make_shared<HelixFSFolder_Bare>( m_name + "/" + folder );
		fsFolder->Load();
		m_folders.push_back( fsFolder );
	}
}

const twine& HelixFSFolder_Bare::FolderName() const
{
	return m_name;
}

twine HelixFSFolder_Bare::PhysicalFolderName() const
{
	if(m_name == "root"){
		return "./";
	} else {
		return "./" + m_name;
	}
}

const twine& HelixFSFolder_Bare::LastName() const
{
	return m_last_name;
}

vector<HelixFSFolder>& HelixFSFolder_Bare::SubFolders()
{
	return m_folders;
}

vector<HelixFSFile>& HelixFSFolder_Bare::Files()
{
	return m_files;
}

HelixFSFile HelixFSFolder_Bare::FindFile(const twine& fileName)
{
	EnEx ee(FL, "HelixFSFolder_Bare::FindFile(const twine& fileName)");

	// Check the files that we own first:
	for(auto file : m_files){
		if(file->FileName() == fileName){
			return file;
		}
	}

	// Search our list of sub-folders
	for(auto folder : m_folders){
		HelixFSFile ret = folder->FindFile( fileName );
		if(ret){
			return ret;
		}
	}

	// Didn't find anything - return the null pointer
	return nullptr;
}

void HelixFSFolder_Bare::FindFilesByType(const twine& fileType, vector<HelixFSFile>& results)
{
	EnEx ee(FL, "HelixFSFolder_Bare::FindFilesByType(const twine& fileType, vector<HelixFSFile>& results)");

	// Check the files that we own first:
	for(auto file : m_files){
		if(file->FileName().endsWith(fileType)){
			results.push_back( file );
		}
	}

	// Search our list of sub-folders
	for(auto folder : m_folders){
		folder->FindFilesByType( fileType, results );
	}
}

HelixFSFolder HelixFSFolder_Bare::FindFolder(const twine& folderName)
{
	EnEx ee(FL, "HelixFSFolder_Bare::FindFolder(const twine& folderName)");

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
	for(auto folder : m_folders){
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

void HelixFSFolder_Bare::DeleteFile(const twine& fileName)
{
	EnEx ee(FL, "HelixFSFolder_Bare::DeleteFile(const twine& fileName)");

	// Check to see if we have the file:
	for(size_t i = 0; i < m_files.size(); i++){
		if(m_files[i]->FileName() == fileName){
			DEBUG(FL, "Removing FSFile Reference: %s/%s", m_name(), fileName() );
			m_files.erase( m_files.begin() + i ); // Erase the vector element
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
