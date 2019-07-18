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
#include "HelixWorker.h"
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
		twine tp( "../../../../3rdParty/" );
		cmd = "cd " + FixPhysical(m_folder->PhysicalFolderName()) + " && " +
			Link("../bin/", "libhelix.glob") +
			ObjList( "./" ) + 
			ObjList( "../logic/util/" ) + ObjList( "../logic/util/sqldo/" ) +
			ObjList( "../logic/admin/" ) + ObjList( "../logic/admin/sqldo/" ) +
			LinkLibs1( tp );

	} else if(m_folder->FolderName() == "server"){
		// Nothing to do
	} else if(m_folder->FolderName() == "client"){
		twine tp( "../../../../3rdParty/" );
		cmd = "cd " + FixPhysical(m_folder->PhysicalFolderName()) + " && " +
			Link("../bin/", "libhelix.client") +
			"HelixApi_Part1" + ObjExt() + 
			"HelixApi_Part2" + ObjExt() +
			LinkLibs2( tp ) +
			BinLib( "../bin", "helix.glob" );
	} else if(m_folder->FolderName().find("logic/") != TWINE_NOT_FOUND ){
		if(m_folder->FolderName().endsWith("/sqldo")){
			// Nothing to do here
		} else if(m_folder->FolderName().endsWith("/test")){
			// Nothing to do here
		} else {
			auto splits = twine(m_folder->FolderName()).split("/");
			auto subFolder = splits[ splits.size() - 1];
			twine tp( "../../../../3rdParty/" );
			cmd = "cd bin && " + 
				Link("./", "libhelix.logic." + subFolder ) +
				ObjList( "../" + m_folder->FolderName() + "/" ) + 
				ObjList( "../" + m_folder->FolderName() + "/sqldo/" ) +
				LinkLibs3( tp ) +
				BinLib( ".", "helix.glob" ) +
				BinLib( ".", "helix.client" );

			// Pick up any dependent folders from our config file
			for(auto depName : HelixConfig::getInstance().LogicDepends( subFolder ) ){
				cmd.append( 
					BinLib( ".", "helix.logic." + depName )
				);
			}
		}
	} else if(m_folder->FolderName() == "HelixMain"){
		twine tp( "../../../../3rdParty/" );
		cmd = "cd " + FixPhysical("./bin") + " && " +
			LinkMain( "./", "HelixMain" ) +
			FixPhysical("../server/HelixMain") + ObjExt() + 
			BinLib( ".", "helix.glob" ) +
			LinkLibs4( tp );

	} else if(m_folder->FolderName() == "HelixTest" ){
		if(HelixConfig::getInstance().IncludeTest() == false){
			return "";
		}
		twine tp( "../../../../3rdParty/" );
		cmd = "cd " + FixPhysical("./bin") + " && " +
			LinkMain( "./", "HelixTest" );

		if(HelixConfig::getInstance().UseCore()){
			cmd +=
				FixPhysical("../" + HelixConfig::getInstance().CoreFolder() + "/server/c/server/HelixTest") + ObjExt() +
				ObjList( "../" + HelixConfig::getInstance().CoreFolder() + "/server/c/logic/admin/test/" ) +
				ObjList( "../" + HelixConfig::getInstance().CoreFolder() + "/server/c/logic/util/test/" ) ;
		} else {
			cmd +=
				FixPhysical("../server/HelixTest") + ObjExt() +
				ObjList( "../logic/admin/test/" ) +
				ObjList( "../logic/util/test/" ) ;
		}

		for(auto& logic : HelixConfig::getInstance().Logics() ){
			twine repo( HelixConfig::getInstance().LogicRepo( logic ) );
			if(repo.empty()){
				cmd += ObjList( "../logic/" + logic + "/test/" );
			} else {
				cmd += ObjList( "../../../../" + repo + "/server/c/logic/" + logic + "/test/" );
			}
		}

		cmd += BinLib( ".", "helix.glob" ) ;
		for(auto& logic : HelixConfig::getInstance().Logics() ){
			cmd += BinLib( ".", "helix.logic." + logic );
		}

		cmd += LinkLibs4( tp );

	} else if(m_folder->FolderName() == "HelixDaemon"){
		cmd = "cd " + FixPhysical("./bin") + " && " +
#ifdef _WIN32
			LinkMain( "./", "HelixSvc" ) + FixPhysical("../server/HelixSvc") + 
#else
			LinkMain( "./", "HelixDaemon" ) + FixPhysical("../server/HelixDaemon") + 
#endif
			ObjExt() + LinkLibs5( );
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
		return "./bin/libhelix.glob" + DLLExt();
	} else if(m_folder->FolderName() == "server"){
		return ""; // no library to link
	} else if(m_folder->FolderName() == "client"){
		return "./bin/libhelix.client" + DLLExt();
	} else if(m_folder->FolderName().find("logic/") != TWINE_NOT_FOUND && !m_folder->FolderName().endsWith("/sqldo")){
		vector<twine> splits = twine(m_folder->FolderName()).split("/");
		twine subFolder = splits[ splits.size() - 1 ];
		return "./bin/libhelix.logic." + subFolder + DLLExt();
	} else if(m_folder->FolderName().find("logic/") != TWINE_NOT_FOUND && m_folder->FolderName().endsWith("/sqldo")){
		return ""; // no library to link
	} else if(m_folder->FolderName() == "HelixMain"){
		return "./bin/HelixMain" + BinExt();
	} else if(m_folder->FolderName() == "HelixDaemon"){
#ifdef _WIN32
		return "./bin/HelixSvc.exe";
#else
		return "./bin/HelixDaemon";
#endif
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
	if(m_folder->FolderName().endsWith("glob") ){
		HelixWorker::getInstance().WaitForCompilers();
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
		if(logicAdmin){
			vector<HelixFSFile> cppFiles;
			logicAdmin->FindFilesByType(".cpp", cppFiles);
			for(auto file : cppFiles){
				if(HelixFSFile_Bare::IsNewerThan( file->PhysicalDotOh(), globLinkTarget )){
					return true; // Found a .o that is newer than our link target.  Re-link required
				}
			}
		}
	}

	return false;
}

twine HelixLinkTask::LinkLibs1( const twine& tpl )
{
#ifdef _WIN32
#	ifdef _X86_
	// ///////////////////////////////////////////////////////////////////////////////
	// 32-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	throw AnException(0, FL, "Linking on 32-bit Windows not yet supported");

#	else
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	twine win3pl = FixPhysical( tpl );
	return 	
		win3pl + "lib\\libeay32.lib "
		+ win3pl + "lib\\ssleay32.lib "
		+ win3pl + "lib\\libxml2.lib "
		+ win3pl + "lib\\libSLib.lib "
		+ win3pl + "lib\\zdll.lib "
		+ win3pl + "lib\\libcurl.lib "
		+ win3pl + "lib\\libiconv.lib "
		+ win3pl + "lib\\libhpdf.lib "
		"ws2_32.lib odbc32.lib rpcrt4.lib Advapi32.lib";
#	endif
#elif __APPLE__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit mac
	// ///////////////////////////////////////////////////////////////////////////////
	return "-L/usr/local/opt/openssl/lib -lssl -lcrypto -lpthread -lresolv -lxml2 -lz -lodbc -lcurl -liconv -lhpdf -L" + tpl + "/lib -lSLib ";

#elif __linux__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit linux
	// ///////////////////////////////////////////////////////////////////////////////
	return "-L/usr/lib -lssl -lcrypto -lpthread -lresolv -lxml2 -luuid -lz -lodbc -lcurl -lrt -lhpdf -L" + tpl + "/lib -lSLib ";

#else
	throw AnException(0, FL, "Unknown link environment.");
#endif

}

twine HelixLinkTask::LinkLibs2( const twine& tpl )
{
#ifdef _WIN32
#	ifdef _X86_
	// ///////////////////////////////////////////////////////////////////////////////
	// 32-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	throw AnException(0, FL, "Linking on 32-bit Windows not yet supported");

#	else
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	twine win3pl = FixPhysical( tpl );
	return 	
		win3pl + "lib\\libeay32.lib "
		+ win3pl + "lib\\ssleay32.lib "
		+ win3pl + "lib\\libxml2.lib "
		+ win3pl + "lib\\libSLib.lib "
		+ win3pl + "lib\\zdll.lib "
		+ win3pl + "lib\\libcurl.lib "
		+ win3pl + "lib\\libiconv.lib "
		"ws2_32.lib odbc32.lib rpcrt4.lib ";

#	endif
#elif __APPLE__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit mac
	// ///////////////////////////////////////////////////////////////////////////////
	return LinkLibs1( tpl );

#elif __linux__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit linux
	// ///////////////////////////////////////////////////////////////////////////////
	return LinkLibs1( tpl );


#else
	throw AnException(0, FL, "Unknown link environment.");
#endif

}

twine HelixLinkTask::LinkLibs3( const twine& tpl )
{
#ifdef _WIN32
#	ifdef _X86_
	// ///////////////////////////////////////////////////////////////////////////////
	// 32-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	throw AnException(0, FL, "Linking on 32-bit Windows not yet supported");

#	else
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	twine win3pl = FixPhysical( tpl );
	return 	
		win3pl + "lib\\libeay32.lib "
		+ win3pl + "lib\\ssleay32.lib "
		+ win3pl + "lib\\libxml2.lib "
		+ win3pl + "lib\\libSLib.lib "
		+ win3pl + "lib\\zdll.lib "
		+ win3pl + "lib\\libcurl.lib "
		+ win3pl + "lib\\libiconv.lib "
		+ win3pl + "lib\\libhpdf.lib "
		"ws2_32.lib odbc32.lib rpcrt4.lib ";

#	endif
#elif __APPLE__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit mac
	// ///////////////////////////////////////////////////////////////////////////////
	return LinkLibs1( tpl );

#elif __linux__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit linux
	// ///////////////////////////////////////////////////////////////////////////////
	return LinkLibs1( tpl );

#else
	throw AnException(0, FL, "Unknown link environment.");
#endif

}

twine HelixLinkTask::LinkLibs4( const twine& tpl )
{
#ifdef _WIN32
#	ifdef _X86_
	// ///////////////////////////////////////////////////////////////////////////////
	// 32-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	throw AnException(0, FL, "Linking on 32-bit Windows not yet supported");

#	else
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	twine win3pl = FixPhysical( tpl );
	return 	
		win3pl + "lib\\libeay32.lib "
		+ win3pl + "lib\\ssleay32.lib "
		+ win3pl + "lib\\libxml2.lib "
		+ win3pl + "lib\\libSLib.lib "
		+ win3pl + "lib\\zdll.lib "
		"ws2_32.lib odbc32.lib advapi32.lib user32.lib Shlwapi.lib rpcrt4.lib ";

#	endif
#elif __APPLE__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit mac
	// ///////////////////////////////////////////////////////////////////////////////
	return LinkLibs1( tpl );

#elif __linux__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit linux
	// ///////////////////////////////////////////////////////////////////////////////
	return LinkLibs1( tpl );

#else
	throw AnException(0, FL, "Unknown link environment.");
#endif

}

twine HelixLinkTask::LinkLibs5()
{
#ifdef _WIN32
#	ifdef _X86_
	// ///////////////////////////////////////////////////////////////////////////////
	// 32-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	throw AnException(0, FL, "Linking on 32-bit Windows not yet supported");

#	else
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	return 	
		"advapi32.lib user32.lib Shlwapi.lib";

#	endif
#elif __APPLE__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit mac
	// ///////////////////////////////////////////////////////////////////////////////
	return ""; // Empty on purpose

#elif __linux__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit linux
	// ///////////////////////////////////////////////////////////////////////////////
	return ""; // Empty on purpose

#else
	throw AnException(0, FL, "Unknown link environment.");
#endif

}

twine HelixLinkTask::Link(const twine& bin, const twine& outLib)
{
#ifdef _WIN32
#	ifdef _X86_
	// ///////////////////////////////////////////////////////////////////////////////
	// 32-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	throw AnException(0, FL, "Linking on 32-bit Windows not yet supported");

#	else
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	return 	
		"link.exe -machine:x64 -subsystem:console /DLL "
			"/OUT:" + FixPhysical( bin ) + outLib + ".dll ";

#	endif
#elif __APPLE__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit mac
	// ///////////////////////////////////////////////////////////////////////////////
	return "g++ -shared -o " + FixPhysical( bin ) + outLib + ".so ";

#elif __linux__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit linux
	// ///////////////////////////////////////////////////////////////////////////////
	return "g++ -shared -o " + FixPhysical( bin ) + outLib + ".so ";

#else
	throw AnException(0, FL, "Unknown link environment.");
#endif

}

twine HelixLinkTask::LinkMain(const twine& bin, const twine& outLib)
{
#ifdef _WIN32
#	ifdef _X86_
	// ///////////////////////////////////////////////////////////////////////////////
	// 32-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	throw AnException(0, FL, "Linking on 32-bit Windows not yet supported");

#	else
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit windows
	// ///////////////////////////////////////////////////////////////////////////////
	return 	
		"link.exe -machine:x64 -subsystem:console "
			"/OUT:" + FixPhysical( bin ) + outLib + ".exe ";

#	endif
#elif __APPLE__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit mac
	// ///////////////////////////////////////////////////////////////////////////////
	return "g++ -o " + FixPhysical( bin ) + outLib + " ";

#elif __linux__
	// ///////////////////////////////////////////////////////////////////////////////
	// 64-bit linux
	// ///////////////////////////////////////////////////////////////////////////////
	return "g++ -o " + FixPhysical( bin ) + outLib + " ";

#else
	throw AnException(0, FL, "Unknown link environment.");
#endif

}

twine HelixLinkTask::ObjList(const twine& folder)
{
	return FixPhysical( folder ) + "*" + ObjExt();
}

twine HelixLinkTask::ObjExt()
{
#ifdef _WIN32
	return ".obj ";
#else
	return ".o ";
#endif
}

twine HelixLinkTask::LibExt()
{
#ifdef _WIN32
	return ".lib";
#else
	return ".so";
#endif
}

twine HelixLinkTask::DLLExt()
{
#ifdef _WIN32
	return ".dll";
#else
	return ".so";
#endif
}

twine HelixLinkTask::BinExt()
{
#ifdef _WIN32
	return ".exe";
#else
	return "";
#endif
}

twine HelixLinkTask::BinLib(const twine& bin, const twine& libName)
{
#ifdef _WIN32
	return FixPhysical( bin ) + "\\lib" + libName + LibExt() + " ";
#else
	return "-L" + FixPhysical( bin ) + " -l" + libName + " ";
#endif
}

