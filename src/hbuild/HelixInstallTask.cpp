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
#include "HelixInstallTask.h"
using namespace Helix::Build;


HelixInstallTask::HelixInstallTask(const twine& sourceFolder, const twine& filePattern, const twine& targetFolder,
		const twine& newName) 
	: m_source_folder( sourceFolder), m_file_pattern( filePattern ), m_target_folder( targetFolder ),
	m_new_name( newName )
{
	EnEx ee(FL, "HelixInstallTask::HelixInstallTask()");

}

HelixInstallTask::HelixInstallTask(const HelixInstallTask& c) :
	m_source_folder( c.m_source_folder ), m_file_pattern( c.m_file_pattern), m_target_folder( c.m_target_folder),
	m_new_name( c.m_new_name)
{
	EnEx ee(FL, "HelixInstallTask::HelixInstallTask(const HelixInstallTask& c)");

}

HelixInstallTask& HelixInstallTask::operator=(const HelixInstallTask& c)
{
	EnEx ee(FL, "HelixInstallTask::operator=(const HelixInstallTask& c)");

	m_source_folder = c.m_source_folder;
	m_file_pattern = c.m_file_pattern;
	m_target_folder = c.m_target_folder;
	m_new_name = c.m_new_name;

	return *this;
}

HelixInstallTask::HelixInstallTask(const HelixInstallTask&& c) :
	m_source_folder( c.m_source_folder ), m_file_pattern( c.m_file_pattern), m_target_folder( c.m_target_folder),
	m_new_name( c.m_new_name)
{
	EnEx ee(FL, "HelixInstallTask::HelixInstallTask(const HelixInstallTask&& c)");

}

HelixInstallTask& HelixInstallTask::operator=(const HelixInstallTask&& c)
{
	EnEx ee(FL, "HelixInstallTask::operator=(const HelixInstallTask&& c)");

	m_source_folder = std::move(c.m_source_folder);
	m_file_pattern = std::move(c.m_file_pattern);
	m_target_folder = std::move(c.m_target_folder);
	m_new_name = std::move(c.m_new_name);

	return *this;
}

HelixInstallTask::~HelixInstallTask()
{
	EnEx ee(FL, "HelixInstallTask::~HelixInstallTask()");

}

void HelixInstallTask::Execute()
{
	EnEx ee(FL, "HelixInstallTask::Execute()");

	if(m_file_pattern.empty()){
		CopyFolder( m_source_folder, m_target_folder );
	} else {
		for(auto& file : File::listFiles( m_source_folder )){
			if(file.endsWith( m_file_pattern )){
				twine sourceFileName( m_source_folder + "/" + file );
				twine targetFileName;
				if(m_new_name.empty()){
					targetFileName = m_target_folder + "/" + file;
				} else {
					targetFileName = m_target_folder + "/" + m_new_name;
				}
				File::EnsurePath( targetFileName );
				if(HelixFSFile::IsNewerThan( sourceFileName, targetFileName )){
					INFO(FL, "%s is newer than %s - installing file.", sourceFileName(), targetFileName() );
					File::Copy( sourceFileName(), targetFileName() );
				}
			}
		}
	}
}

void HelixInstallTask::CopyFolder(const twine& fromFolder, const twine& toFolder )
{
	EnEx ee(FL, "HelixInstallTask::CopyFolder(const twine& fromFolder, const twine& toFolder )");

	try {
		// First copy over all of the files
		for(auto& file : File::listFiles( fromFolder ) ){
			twine sourceFileName( fromFolder + "/" + file );
			twine targetFileName( toFolder + "/" + file );
			File::EnsurePath( targetFileName );
			if(HelixFSFile::IsNewerThan( sourceFileName, targetFileName )){
				INFO(FL, "%s is newer than %s - installing file.", sourceFileName(), targetFileName() );
				File::Copy( sourceFileName(), targetFileName() );
			}
		}

		// Now handle all of the child directories
		for(auto& folder : File::listFolders( fromFolder ) ){
			if(folder == "." || folder == "..") continue; // Skip these

			CopyFolder( fromFolder + "/" + folder, toFolder + "/" + folder );
		}
	} catch (AnException& e){
		WARN(FL, "Directory does not exist: %s", fromFolder() );
	}
}
