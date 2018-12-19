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
#include "HelixLinkTask.h"
using namespace Helix::Build;


HelixLinkTask::HelixLinkTask(HelixFSFolder folder) : m_folder( folder )
{
	EnEx ee(FL, "HelixLinkTask::HelixLinkTask()");

}

HelixLinkTask::HelixLinkTask(const HelixLinkTask& c) : m_folder( c.m_folder )
{
	EnEx ee(FL, "HelixLinkTask::HelixLinkTask(const HelixLinkTask& c)");

}

HelixLinkTask& HelixLinkTask::operator=(const HelixLinkTask& c)
{
	EnEx ee(FL, "HelixLinkTask::operator=(const HelixLinkTask& c)");

	return *this;
}

HelixLinkTask::HelixLinkTask(const HelixLinkTask&& c) : m_folder( c.m_folder )
{
	EnEx ee(FL, "HelixLinkTask::HelixLinkTask(const HelixLinkTask&& c)");

}

HelixLinkTask& HelixLinkTask::operator=(const HelixLinkTask&& c)
{
	EnEx ee(FL, "HelixLinkTask::operator=(const HelixLinkTask&& c)");


	return *this;
}

HelixLinkTask::~HelixLinkTask()
{
	EnEx ee(FL, "HelixLinkTask::~HelixLinkTask()");

}

HelixFSFolder HelixLinkTask::Folder()
{
	return m_folder;
}

twine HelixLinkTask::GetCommandLine()
{
	EnEx ee(FL, "HelixLinkTask::GetCommandLine()");

	twine cmd;

	if(m_folder->FolderName() == "logic/util"){
		// Nothing to do - logic/util gets linked into helix glob.
	} else if(m_folder->FolderName() == "logic/admin"){
		// Nothing to do - logic/admin gets linked into helix glob.
	} else if(m_folder->FolderName() == "glob"){
		twine tp( "..\\..\\..\\..\\3rdParty\\" );
		cmd = "cd " + FixPhysical(m_folder->PhysicalFolderName()) + " && link.exe -machine:x64 -subsystem:console "
			"/OUT:..\\bin\\libhelix.glob.dll /DLL *.obj "
			"..\\logic\\util\\*.obj ..\\logic\\util\\sqldo\\*.obj "
			"..\\logic\\admin\\*.obj ..\\logic\\admin\\sqldo\\*.obj " 
			+ tp + "lib\\libeay32.lib "
			+ tp + "lib\\ssleay32.lib "
			+ tp + "lib\\libxml2.lib "
			+ tp + "lib\\libSLib.lib "
			+ tp + "lib\\zdll.lib "
			+ tp + "lib\\libcurl.lib "
			+ tp + "lib\\libiconv.lib "
			+ tp + "lib\\libhpdf.lib "
			"ws2_32.lib odbc32.lib rpcrt4.lib Advapi32.lib";
	} else if(m_folder->FolderName() == "server"){
		// Nothing to do
	} else if(m_folder->FolderName() == "client"){
		twine tp( "..\\..\\..\\..\\3rdParty\\" );
		cmd = "cd " + FixPhysical(m_folder->PhysicalFolderName()) + " && link.exe -machine:x64 -subsystem:console "
			"/OUT:..\\bin\\libhelix.client.dll /DLL HelixApi_Part1.obj HelixApi_Part2.obj "
			+ tp + "lib\\libeay32.lib "
			+ tp + "lib\\ssleay32.lib "
			+ tp + "lib\\libxml2.lib "
			+ tp + "lib\\libSLib.lib "
			+ tp + "lib\\zdll.lib "
			+ tp + "lib\\libcurl.lib "
			+ tp + "lib\\libiconv.lib "
			"ws2_32.lib odbc32.lib rpcrt4.lib "
			"..\\bin\\libhelix.glob.lib";
	} else if(m_folder->FolderName().startsWith("logic/") && !m_folder->FolderName().endsWith("/sqldo")){
		auto splits = twine(m_folder->FolderName()).split("/");
		auto subFolder = splits[1];
		twine tp( "..\\..\\..\\..\\..\\3rdParty\\" );
		cmd = "cd " + FixPhysical(m_folder->PhysicalFolderName()) + " && link.exe -machine:x64 -subsystem:console "
			"/OUT:..\\..\\bin\\libhelix.logic." + subFolder + ".dll /DLL *.obj sqldo\\*.obj "
			+ tp + "lib\\libeay32.lib "
			+ tp + "lib\\ssleay32.lib "
			+ tp + "lib\\libxml2.lib "
			+ tp + "lib\\libSLib.lib "
			+ tp + "lib\\zdll.lib "
			+ tp + "lib\\libcurl.lib "
			+ tp + "lib\\libiconv.lib "
			+ tp + "lib\\libhpdf.lib "
			"ws2_32.lib odbc32.lib rpcrt4.lib "
			"..\\..\\bin\\libhelix.glob.lib "
			"..\\..\\bin\\libhelix.client.lib ";

		// Pick up any dependent folders from our config file
		for(auto depName : HelixConfig::getInstance().LogicDepends( subFolder ) ){
			cmd.append( "..\\..\\bin\\libhelix.logic." + depName + ".lib " );
		}

	} else if(m_folder->FolderName().startsWith("logic/") && m_folder->FolderName().endsWith("/sqldo")){
		// Nothing to do
	} else if(m_folder->FolderName() == "HelixMain"){
		twine tp( "..\\..\\..\\..\\3rdParty\\" );
		cmd = "cd .\\bin && link.exe -machine:x64 -subsystem:console "
			"/OUT:HelixMain.exe ..\\server\\HelixMain.obj "
			"libhelix.glob.lib "
			+ tp + "lib\\libeay32.lib "
			+ tp + "lib\\ssleay32.lib "
			+ tp + "lib\\libxml2.lib "
			+ tp + "lib\\libSLib.lib "
			+ tp + "lib\\zdll.lib "
			"ws2_32.lib odbc32.lib advapi32.lib user32.lib Shlwapi.lib rpcrt4.lib ";

	} else if(m_folder->FolderName() == "HelixDaemon"){
		cmd = "cd .\\bin && link.exe -machine:x64 -subsystem:console "
			"/OUT:HelixSvc.exe ..\\server\\HelixSvc.obj "
			"advapi32.lib user32.lib Shlwapi.lib";
	}

	return cmd;
}

twine HelixLinkTask::FixPhysical(const twine& path)
{
	twine ret(path);
#ifdef _WIN32
	ret.replace( '/', '\\' );
#endif
	return ret;
}

twine HelixLinkTask::LinkTarget()
{
	if(m_folder->FolderName() == "logic/util"){
		return ""; // No library to link
	} else if(m_folder->FolderName() == "logic/admin"){
		return ""; // no library to link
	} else if(m_folder->FolderName() == "glob"){
		return "./bin/libhelix.glob.dll";
	} else if(m_folder->FolderName() == "server"){
		return ""; // no library to link
	} else if(m_folder->FolderName() == "client"){
		return "./bin/libhelix.client.dll";
	} else if(m_folder->FolderName().startsWith("logic/") && !m_folder->FolderName().endsWith("/sqldo")){
		vector<twine> splits = twine(m_folder->FolderName()).split("/");
		twine subFolder = splits[1];
		return "./bin/libhelix.logic." + subFolder + ".dll";
	} else if(m_folder->FolderName().startsWith("logic/") && m_folder->FolderName().endsWith("/sqldo")){
		return ""; // no library to link
	} else if(m_folder->FolderName() == "HelixMain"){
		return "./bin/HelixMain.exe";
	} else if(m_folder->FolderName() == "HelixDaemon"){
		return "./bin/HelixSvc.exe";
	} else {
		return "";
	}
}

bool HelixLinkTask::RequiresLink()
{
	// Requires a re-link if the output link target is missing
	if(!File::Exists( LinkTarget() )){
		return true;
	}

	// Special case for glob - if any of the obj files in glob, logic/util, logic/admin are newer than our link
	// target, then we need to re-link glob because it combines all of them
	if(m_folder->FolderName() == "glob"){
		twine globLinkTarget( LinkTarget() );
		HelixFSFolder logicUtil = HelixFS::getInstance().FindPath( "logic/util" );
		if(logicUtil){
			vector<HelixFSFile> cppFiles;
			logicUtil->FindFilesByType(".cpp", cppFiles);
			for(auto file : cppFiles){
				if(HelixFSFile_Bare::IsNewerThan( file->PhysicalDotOh(), globLinkTarget )){
					return true; // Found a .o that is newer than our link target.  Re-link required
				}
			}
		}
		HelixFSFolder logicAdmin = HelixFS::getInstance().FindPath( "logic/admin" );
		if(logicUtil){
			vector<HelixFSFile> cppFiles;
			logicUtil->FindFilesByType(".cpp", cppFiles);
			for(auto file : cppFiles){
				if(HelixFSFile_Bare::IsNewerThan( file->PhysicalDotOh(), globLinkTarget )){
					return true; // Found a .o that is newer than our link target.  Re-link required
				}
			}
		}
	}

	return false;
}
