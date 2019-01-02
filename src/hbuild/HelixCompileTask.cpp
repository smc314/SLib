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

#include "HelixConfig.h"
#include "HelixFS.h"
#include "HelixCompileTask.h"
using namespace Helix::Build;


HelixCompileTask::HelixCompileTask(HelixFSFolder folder, HelixFSFile file) 
	: m_folder( folder ), m_file( file )
{
	EnEx ee(FL, "HelixCompileTask::HelixCompileTask()");

}

HelixCompileTask::HelixCompileTask(const HelixCompileTask& c) :
	m_folder( c.m_folder ), m_file( c.m_file )
{
	EnEx ee(FL, "HelixCompileTask::HelixCompileTask(const HelixCompileTask& c)");

}

HelixCompileTask& HelixCompileTask::operator=(const HelixCompileTask& c)
{
	EnEx ee(FL, "HelixCompileTask::operator=(const HelixCompileTask& c)");

	return *this;
}

HelixCompileTask::HelixCompileTask(const HelixCompileTask&& c) :
	m_folder( c.m_folder ), m_file( c.m_file )
{
	EnEx ee(FL, "HelixCompileTask::HelixCompileTask(const HelixCompileTask&& c)");

}

HelixCompileTask& HelixCompileTask::operator=(const HelixCompileTask&& c)
{
	EnEx ee(FL, "HelixCompileTask::operator=(const HelixCompileTask&& c)");


	return *this;
}

HelixCompileTask::~HelixCompileTask()
{
	EnEx ee(FL, "HelixCompileTask::~HelixCompileTask()");

}

HelixFSFile HelixCompileTask::File()
{
	return m_file;
}

HelixFSFolder HelixCompileTask::Folder()
{
	return m_folder;
}

twine HelixCompileTask::GetCommandLine()
{
	EnEx ee(FL, "HelixCompileTask::GetCommandLine()");

	twine tpl6( "../../../../../../3rdParty" );
	twine tpl5( "../../../../../3rdParty" );
	twine tpl4( "../../../../3rdParty" );
	twine cmd;
	cmd = "cd " + FixPhysical(m_folder->PhysicalFolderName()) + " && ";

	if(m_file->FolderName() == "logic/util"){
		cmd.append(CC(tpl5) + "-I sqldo -I ../../glob -I ../../client -I ../admin -I ../admin/sqldo " 
		);
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName() == "logic/admin"){
		cmd.append(CC(tpl5) + "-I sqldo -I ../../glob -I ../../client -I ../util -I ../util/sqldo ");
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName() == "glob"){
		if(m_file->FileName() == "Jvm.cpp") return ""; // Skip this one
		cmd.append(CC(tpl4) + 
			"-I../logic/util -I../logic/util/sqldo -I ../logic/admin -I ../logic/admin/sqldo "
		);
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName() == "server"){
#ifdef _WIN32
		if(m_file->FileName() == "HelixDaemon.cpp") return ""; // Skip this one
#else
		if(m_file->FileName() == "HelixSvc.cpp") return ""; // Skip this one
#endif
		cmd.append(CC(tpl4) + "-I ../glob -I ../logic/admin -I../logic/util ");
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName() == "client"){
		cmd.append(CC(tpl4) + 
			"-I../glob -I../logic/admin -I../logic/admin/sqldo -I../logic/util -I../logic/util/sqldo "
		);
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName().startsWith("logic/") && !m_file->FolderName().endsWith("/sqldo")){
		cmd.append(CC(tpl5) + "-I sqldo " + LogicIncludes());

		// Pick up any dependent folders from our config file
		auto splits = twine(m_file->FolderName()).split("/");
		auto& logicName = splits[ splits.size() - 1 ];
		for(auto& depName : HelixConfig::getInstance().LogicDepends( logicName ) ){
			cmd.append( "-I ../" + depName + " -I ../" + depName + "/sqldo " );
		}

		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName().startsWith("logic/") && m_file->FolderName().endsWith("/sqldo")){
		cmd.append(CC(tpl6) + LogicIncludes(true));

		// Pick up any dependent folders from our config file
		auto splits = twine(m_file->FolderName()).split("/");
		auto& logicName = splits[ splits.size() - 2 ];
		printf("Logic name is %s\n", logicName() );
		auto deps = HelixConfig::getInstance().LogicDepends( logicName );
		for(auto depName : deps){
			cmd.append( " -I ../../" + depName + "/sqldo " );
		}
		printf("Finished adding dependencies\n");

		cmd.append( m_file->FileName() );
	}

	return cmd;
}

twine HelixCompileTask::FixPhysical(const twine& path)
{
	twine ret(path);
#ifdef _WIN32
	ret.replace( '/', '\\' );
#endif
	return ret;
}

twine HelixCompileTask::CC(const twine& tpl)
{
#ifdef _WIN32
#	ifdef _X86_
	// ///////////////////////////////////////////////////////////////////////////////////
	// 32-bit windows
	// ///////////////////////////////////////////////////////////////////////////////////
	throw AnException(0, FL, "Compiling on 32-bit Windows not yet supported");

#	else
	// ///////////////////////////////////////////////////////////////////////////////////
	// 64-bit windows
	// ///////////////////////////////////////////////////////////////////////////////////
	return "cl.exe /std:c++14 -c -wd4251 -Zp8 -EHsc -O2 -D_WIN64 -DWIN64 -D_AMD64=1 -D__64BIT__ -DWIN32 -D_MT -D_DLL -DLINT_ARGS -MP8 -MD -W3 -D \"_CRT_SECURE_NO_DEPRECATE\" -D \"_CRT_NON_COMFORMING_SWPRINTFS\" -D CS_NO_LARGE_IDENTIFIERS -I " + tpl + "/include -I " + tpl + "/include/libxml2 -I . ";
#	endif
#elif __APPLE__
	// ///////////////////////////////////////////////////////////////////////////////////
	// 64-bit mac
	// ///////////////////////////////////////////////////////////////////////////////////
	return "g++ -std=c++14 -c -g -Wall -D_REENTRANT -fPIC -O2 -I/usr/local/opt/openssl/include -I/usr/include -I/usr/include/libxml2 -I" + tpl + "/include -I. ";

#elif __linux__
	// ///////////////////////////////////////////////////////////////////////////////////
	// 64-bit linux
	// ///////////////////////////////////////////////////////////////////////////////////
	return "g++ -std=c++14 -c -g -Wall -D_REENTRANT -fPIC -O2 -rdynamic -I/usr/include -I/usr/include/libxml2 -I" + tpl + "/include -I. ";

#else
	throw AnException(0, FL, "Unknown compile environment.");
#endif
}

twine HelixCompileTask::LogicIncludes(bool fromSqldo)
{
	if(HelixConfig::getInstance().UseCore()){
		twine coreFolder = HelixConfig::getInstance().CoreFolder();
		if(fromSqldo){
			return "-I../../../" + coreFolder + "/server/c/glob "
				"-I../../../" + coreFolder + "/server/c/client "
				"-I../../../" + coreFolder + "/server/c/logic/util "
				"-I../../../" + coreFolder + "/server/c/logic/util/sqldo "
				"-I../../../" + coreFolder + "/server/c/logic/admin "
				"-I../../../" + coreFolder + "/server/c/logic/admin/sqldo ";
		} else {
			return "-I../../" + coreFolder + "/server/c/glob "
				"-I../../" + coreFolder + "/server/c/client "
				"-I../../" + coreFolder + "/server/c/logic/util "
				"-I../../" + coreFolder + "/server/c/logic/util/sqldo "
				"-I../../" + coreFolder + "/server/c/logic/admin "
				"-I../../" + coreFolder + "/server/c/logic/admin/sqldo ";
		}
	} else {
		if(fromSqldo){
			return "-I../../../glob "
				"-I../../../client "
				"-I../../util -I../../util/sqldo "
				"-I../../admin -I../../admin/sqldo ";
		} else {
			return "-I../../glob "
				"-I../../client "
				"-I../util -I../util/sqldo "
				"-I../admin -I../admin/sqldo ";
		}
	}

}
