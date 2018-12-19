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


HelixInstallTask::HelixInstallTask(const twine& sourceFolder, const twine& filePattern, const twine& targetFolder) 
	: m_source_folder( sourceFolder), m_file_pattern( filePattern ), m_target_folder( targetFolder )
{
	EnEx ee(FL, "HelixInstallTask::HelixInstallTask()");

}

HelixInstallTask::HelixInstallTask(const HelixInstallTask& c) :
	m_source_folder( c.m_source_folder ), m_file_pattern( c.m_file_pattern), m_target_folder( c.m_target_folder)
{
	EnEx ee(FL, "HelixInstallTask::HelixInstallTask(const HelixInstallTask& c)");

}

HelixInstallTask& HelixInstallTask::operator=(const HelixInstallTask& c)
{
	EnEx ee(FL, "HelixInstallTask::operator=(const HelixInstallTask& c)");

	m_source_folder = c.m_source_folder;
	m_file_pattern = c.m_file_pattern;
	m_target_folder = c.m_target_folder;

	return *this;
}

HelixInstallTask::HelixInstallTask(const HelixInstallTask&& c) :
	m_source_folder( c.m_source_folder ), m_file_pattern( c.m_file_pattern), m_target_folder( c.m_target_folder)
{
	EnEx ee(FL, "HelixInstallTask::HelixInstallTask(const HelixInstallTask&& c)");

}

HelixInstallTask& HelixInstallTask::operator=(const HelixInstallTask&& c)
{
	EnEx ee(FL, "HelixInstallTask::operator=(const HelixInstallTask&& c)");

	m_source_folder = std::move(c.m_source_folder);
	m_file_pattern = std::move(c.m_file_pattern);
	m_target_folder = std::move(c.m_target_folder);

	return *this;
}

HelixInstallTask::~HelixInstallTask()
{
	EnEx ee(FL, "HelixInstallTask::~HelixInstallTask()");

}

void HelixInstallTask::Execute()
{
	EnEx ee(FL, "HelixInstallTask::Execute()");

	for(auto& file : File::listFiles( m_source_folder )){
		if(file.endsWith( m_file_pattern )){
			twine sourceFileName( m_source_folder + "/" + file );
			twine targetFileName( m_target_folder + "/" + file );
			File::EnsurePath( targetFileName );
			if(HelixFSFile_Bare::IsNewerThan( sourceFileName, targetFileName )){
				INFO(FL, "%s is newer than %s - installing file.", sourceFileName(), targetFileName() );
				File::Copy( sourceFileName(), targetFileName() );
			}
		}
	}
}

