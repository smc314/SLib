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
#include "HelixGenTask.h"
#include "HelixWorker.h"
using namespace Helix::Build;


HelixGenTask::HelixGenTask(HelixFSFolder folder, HelixFSFile file) 
	: m_folder( folder ), m_file( file ), m_sqldo( folder, file )
{
	EnEx ee(FL, "HelixGenTask::HelixGenTask()");

}

HelixGenTask::HelixGenTask(const HelixGenTask& c) :
	m_folder( c.m_folder ), m_file( c.m_file ), m_sqldo( c.m_folder, c.m_file )
{
	EnEx ee(FL, "HelixGenTask::HelixGenTask(const HelixGenTask& c)");

}

HelixGenTask& HelixGenTask::operator=(const HelixGenTask& c)
{
	EnEx ee(FL, "HelixGenTask::operator=(const HelixGenTask& c)");

	return *this;
}

HelixGenTask::HelixGenTask(const HelixGenTask&& c) :
	m_folder( c.m_folder ), m_file( c.m_file ), m_sqldo( c.m_folder, c.m_file )
{
	EnEx ee(FL, "HelixGenTask::HelixGenTask(const HelixGenTask&& c)");

}

HelixGenTask& HelixGenTask::operator=(const HelixGenTask&& c)
{
	EnEx ee(FL, "HelixGenTask::operator=(const HelixGenTask&& c)");


	return *this;
}

HelixGenTask::~HelixGenTask()
{
	EnEx ee(FL, "HelixGenTask::~HelixGenTask()");

}

HelixFSFile HelixGenTask::File()
{
	return m_file;
}

HelixFSFolder HelixGenTask::Folder()
{
	return m_folder;
}

void HelixGenTask::Generate()
{
	EnEx ee(FL, "HelixGenTask::Generate");

	// Run a sanity check first
	if(m_sqldo.SanityCheck() == true){
		throw AnException(0, FL, "Sanity check on %s failed.", m_file->PhysicalFileName()() );
	}

	// Generate the C++ Header file
	twine target = m_sqldo.CPPHeaderFileName();
	File::EnsurePath( target );
	File::writeToFile( target, m_sqldo.GenCPPHeader() );

	// Generate the C++ Body File
	target = m_sqldo.CPPBodyFileName();
	File::EnsurePath( target );
	File::writeToFile( target, m_sqldo.GenCPPBody() );

	// Generate the C# Object file
	target = m_sqldo.CSBodyFileName();
	File::EnsurePath( target );
	File::writeToFile( target, m_sqldo.GenCSBody() );
	HelixWorker::getInstance().NeedsCSRebuild( true );

	// Generate the Javascript data object file in all of the correct qd app folders
	vector<twine> apps { "dev", "atm", "ttvx" };
	for(auto& app : apps){
		target = m_sqldo.JSBodyFileName(app);
		File::EnsurePath( target );
		File::writeToFile( target, m_sqldo.GenJSBody(app) );
	}
	
	// Not Us - Notes - Create ApiTests2.xml
	
	// Not Us - Notes - Create Api Test 1, 2, 3 xml files
	
	// Not Us - Notes - HelixApi.h
	
	// Not Us - Notes - HelixApi_Part2.cpp
	
	// Not Us - Notes - Api.js
	
}

