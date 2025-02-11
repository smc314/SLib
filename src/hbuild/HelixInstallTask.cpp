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
		const twine& newName, const twine& platform) 
	: m_source_folder( sourceFolder), m_file_pattern( filePattern ), m_target_folder( targetFolder ),
	m_new_name( newName ), m_platform( platform )
{
	EnEx ee(FL, "HelixInstallTask::HelixInstallTask()");

}

HelixInstallTask::HelixInstallTask(const HelixInstallTask& c) :
	m_source_folder( c.m_source_folder ), m_file_pattern( c.m_file_pattern), m_target_folder( c.m_target_folder),
	m_new_name( c.m_new_name), m_platform( c.m_platform)
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
	m_platform = c.m_platform;
	return *this;
}

HelixInstallTask::HelixInstallTask(const HelixInstallTask&& c) :
	m_source_folder( c.m_source_folder ), m_file_pattern( c.m_file_pattern), m_target_folder( c.m_target_folder),
	m_new_name( c.m_new_name), m_platform( c.m_platform)
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
	m_platform = std::move(c.m_platform);
	return *this;
}

HelixInstallTask::~HelixInstallTask()
{
	EnEx ee(FL, "HelixInstallTask::~HelixInstallTask()");

}

void HelixInstallTask::Execute()
{
	EnEx ee(FL, "HelixInstallTask::Execute()");

#ifdef _WIN32
	twine platform = "WIN32";
#elif __APPLE__
	twine platform = "MAC";
#elif __linux__
	twine platform = "LINUX";
#else
	throw AnException(0, FL, "Unknown build platform");
#endif

	// If the platform is not empty, and it does not match the current platform, then skip	
	if(!m_platform.empty()){
		if(m_platform != platform){
			return;
		}
	}

	// If the file pattern is empty, then copy the entire folder
	if(m_file_pattern.empty()){
		CopyFolder( m_source_folder, m_target_folder );
	} else {
		try {
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
		} catch (AnException& e){
			WARN(FL, "WARNING: Directory does not exist: %s", m_source_folder() );
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
		WARN(FL, "WARNING: Directory does not exist: %s", fromFolder() );
	}
}
