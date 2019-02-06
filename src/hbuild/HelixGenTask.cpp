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
#include "HelixConfig.h"
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
	if(!m_file->FromCore()){
		twine target = m_sqldo.CPPHeaderFileName();
		DEBUG(FL, "Writing CPP Header to: %s", target() );
		File::EnsurePath( target );
		File::writeToFile( target, m_sqldo.GenCPPHeader() );
	}

	// Generate the C++ Body File
	if(!m_file->FromCore()){
		twine target = m_sqldo.CPPBodyFileName();
		DEBUG(FL, "Writing CPP Body to: %s", target() );
		File::EnsurePath( target );
		File::writeToFile( target, m_sqldo.GenCPPBody() );
	}

	// Generate the C# Object file - also generate everything from Core into our local C# project
	if(HelixConfig::getInstance().SkipPdfGen() == false){	
		twine target = m_sqldo.CSBodyFileName();
		DEBUG(FL, "Writing CS Body to: %s", target() );
		File::EnsurePath( target );
		File::writeToFile( target, m_sqldo.GenCSBody() );
		HelixWorker::getInstance().NeedsCSRebuild( true );
	}

	// Generate the Javascript data object file in all of the correct qd app folders
	if(!m_file->FromCore()){
		for(auto& app : HelixConfig::getInstance().QxApps()){
			twine target = m_sqldo.JSBodyFileName(app);
			DEBUG(FL, "Writing JS Body to: %s", target() );
			File::EnsurePath( target );
			File::writeToFile( target, m_sqldo.GenJSBody(app) );
		}
	}	
}

