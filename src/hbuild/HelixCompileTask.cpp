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
	cmd = "cd " + FixPhysical(m_folder->PhysicalFolderName()) + " && cl.exe /std:c++14 -c -wd4251 -Zp8 -EHsc -O2 -D_WIN64 -DWIN64 -D_AMD64=1 -D__64BIT__ -DWIN32 -D_MT -D_DLL -DLINT_ARGS -MP8 -MD -W3 -D \"_CRT_SECURE_NO_DEPRECATE\" -D \"_CRT_NON_COMFORMING_SWPRINTFS\" -D CS_NO_LARGE_IDENTIFIERS ";

	if(m_file->FolderName() == "logic/util"){
		cmd.append("-I " + tpl5 + "/include -I " + tpl5 + 
			"/include/libxml2 -I . -I sqldo -I ../../glob -I ../../client -I ../admin -I ../admin/sqldo " 
		);
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName() == "logic/admin"){
		cmd.append("-I " + tpl5 + "/include -I " + tpl5 + 
			"/include/libxml2 -I . -I sqldo -I ../../glob -I ../../client -I ../util -I ../util/sqldo "
		);
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName() == "glob"){
		if(m_file->FileName() == "Jvm.cpp") return ""; // Skip this one
		cmd.append("-I " + tpl4 + "/include -I " + tpl4 + 
			"/include/libxml2 -I. -I../logic/util -I../logic/util/sqldo -I ../logic/admin -I ../logic/admin/sqldo "
		);
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName() == "server"){
		if(m_file->FileName() == "HelixDaemon.cpp") return ""; // Skip this one
		cmd.append("-I " + tpl4 + "/include -I " + tpl4 + 
			"/include/libxml2 -I ../glob -I ../logic/admin -I../logic/util "
		);
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName() == "client"){
		cmd.append("-I " + tpl4 + "/include -I " + tpl4 + "/include/libxml2 "
			"-I../glob -I../logic/admin -I../logic/admin/sqldo -I../logic/util -I../logic/util/sqldo "
		);
		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName().startsWith("logic/") && !m_file->FolderName().endsWith("/sqldo")){
		cmd.append("-I " + tpl5 + "/include -I " + tpl5 + "/include/libxml2 -I. -I sqldo -I../../glob "
			"-I../../client -I../util -I../util/sqldo -I../admin -I../admin/sqldo "
		);

		// Pick up any dependent folders from our config file
		auto splits = twine(m_file->FolderName()).split("/");
		auto& logicName = splits[ splits.size() - 1 ];
		for(auto& depName : HelixConfig::getInstance().LogicDepends( logicName ) ){
			cmd.append( "-I ../" + depName + " -I ../" + depName + "/sqldo " );
		}

		cmd.append( m_file->FileName() );
	} else if(m_file->FolderName().startsWith("logic/") && m_file->FolderName().endsWith("/sqldo")){
		cmd.append("-I " + tpl6 + "/include -I " + tpl6 + "/include/libxml2 -I. -I sqldo -I../../glob "
			"-I../../client -I../util -I../util/sqldo -I../admin -I../admin/sqldo "
		);

		// Pick up any dependent folders from our config file
		auto splits = twine(m_file->FolderName()).split("/");
		auto& logicName = splits[ splits.size() - 2 ];
		printf("Logic name is %s\n", logicName() );
		auto deps = HelixConfig::getInstance().LogicDepends( logicName );
		for(auto depName : deps){
			cmd.append( " -I ../" + depName + "/sqldo " );
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

