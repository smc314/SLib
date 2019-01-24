/* ***************************************************************************

	Copyright: 2013 Hericus Software, LLC

	License: The MIT License (MIT)

	Authors: Steven M. Cherry

*************************************************************************** */

#include <AnException.h>
#include <Log.h>
#include <xmlinc.h>
#include <XmlHelpers.h>
#include <twine.h>
#include <File.h>
#include <Timer.h>
using namespace SLib;

#include <vector>

#include "HelixConfig.h"
#include "HelixFS.h"
#include "HelixBuilder.h"
#include "HelixWorker.h"
#include "HelixAutoAsset.h"
#include "HelixExtractStrings.h"
#include "HelixJSGenTask.h"
using namespace Helix::Build;

twine m_platform;
std::vector<twine> m_targets;

void handleAll();
void handleClean();
void handleGen(bool displayBanner = true);
void handleReGen(bool displayBanner = true);
void handleJSApiGen(bool displayBanner = true);
void handleJSGen(bool displayBanner = true);
void handleCS(bool displayBanner = true);
void handleInstall(bool displayBanner = true);
void handleDeploy(bool displayBanner = true);
void handleAsset(bool displayBanner = true);
void handleStrings(bool displayBanner = true);
void testDep();
void CopyCore();

int main (int argc, char** argv)
{
	if(argc > 1){
		for(int i = 1; i < argc; i++){
			twine argv1(argv[i]);
			m_targets.push_back( argv1 );
		}
	}

	Log::SetPanic( true );
	Log::SetError( true );
	Log::SetWarn( true );
	Log::SetInfo( true );
	Log::SetDebug( false );
	Log::SetTrace( false );

	printf("============================================================================\n");
#ifdef _WIN32
#	ifdef _X86_
	m_platform = "WIN32_x86";
#	else
	m_platform = "WIN32_x64";
#	endif
#else
	m_platform = "LINUX_x64";
#endif
	printf("Helix Build Running for a %s platform.\n", m_platform());
	printf("============================================================================\n");

	Timer timer;
	try {
		// Load our config file and make sure it exists
		HelixConfig::getInstance();
		CopyCore();

		printf("Loading the helix filesystem");
		timer.Start();
		HelixFS::getInstance();
		timer.Finish(); printf(" - duration: %f seconds\n", timer.Duration() );

		if(m_targets.size() == 0){
			handleAll();
		} else {
			for( auto const& targ : m_targets){
				if(targ == "clean") handleClean();
				else if(targ == "all") handleAll();
				else if(targ == "gen") handleGen();
				else if(targ == "regen") handleReGen();
				else if(targ == "jsapi") handleJSApiGen();
				else if(targ == "jsgen") handleJSGen();
				else if(targ == "cs") handleCS();
				else if(targ == "install") handleInstall();
				else if(targ == "dep") handleDeploy();
				else if(targ == "testdep") testDep();
				else if(targ == "asset") handleAsset();
				else if(targ == "strings") handleStrings();
			}
		}
	
		HelixWorker::getInstance().Finish();
		printf("============================================================================\n");
		if(HelixWorker::getInstance().HasError()){
			printf("== hbuild complete - with errors\n");
		} else {
			printf("== hbuild complete.\n");
		}
		printf("============================================================================\n");

		HelixFS::getInstance().SaveCache();

		/* Use this for profiling what's going on and what's taking up the time
		twine hitmap;
		EnEx::PrintGlobalHitMap( hitmap );
		printf("\n\n%s\n\n", hitmap() );
		*/

	} catch(AnException& e){
		printf("%s\n", e.Msg());
		printf("%s\n", e.Stack());
	}

	if(HelixWorker::getInstance().HasError()){
		return 1; // Errors
	} else {
		return 0; // No error
	}
}

void handleAll()
{
	printf("============================================================================\n");
	printf("== All Target\n");
	printf("============================================================================\n");

	HelixBuilder builder;	
	handleGen(false);

	if(!HelixConfig::getInstance().UseCore()){
		// Only build these if we're not using a core folder
		builder.Build( "logic/util" );
		builder.Build( "logic/admin" );
		builder.Build( "glob" );
		builder.Build( "server" );
		builder.Build( "client" );
	}

	// Use the config file to tell us which logics to build, and in what order
	for(auto& logic : HelixConfig::getInstance().Logics() ){
		twine repo = HelixConfig::getInstance().LogicRepo( logic );
		if(repo.empty()){
			// Normal local logic folder
			builder.Build( "logic/" + logic );
		} else {
			// Logic folder from another repo - adjust the path
			builder.Build( "../../../" + repo + "/server/c/logic/" + logic );
		}
	}

	if(!HelixConfig::getInstance().UseCore()){
		// Only build these if we're not using a core folder
		builder.Build( "HelixMain" );
		builder.Build( "HelixDaemon" );
	}

	handleAsset(false);
	handleStrings(false);
	handleJSApiGen(false);

	if(HelixWorker::getInstance().NeedsCSRebuild()){
		handleCS( false );
	}

	// Handle all of our installation tasks
	handleInstall( false );

}

void handleClean()
{
	printf("============================================================================\n");
	printf("== Clean Target\n");
	printf("============================================================================\n");

	HelixBuilder builder;	
	builder.CleanSqldo();

	if(!HelixConfig::getInstance().UseCore()){
		// Only build these if we're not using a core folder
		builder.Clean( "logic/util" );
		builder.Clean( "logic/admin" );
		builder.Clean( "glob" );
		builder.Clean( "server" );
	}

	// Use the config file to tell us which logics to build, and in what order
	for(auto& logic : HelixConfig::getInstance().Logics() ){
		twine repo = HelixConfig::getInstance().LogicRepo( logic );
		if(repo.empty()){
			// Normal local logic folder
			builder.Clean( "logic/" + logic);
		} else {
			// Logic folder from another repo - adjust the path
			builder.Clean( "../../../" + repo + "/server/c/logic/" + logic );
		}
	}

	if(!HelixConfig::getInstance().UseCore()){
		// Only build these if we're not using a core folder
		builder.Clean( "client" );
		builder.Clean( "HelixMain" );
		builder.Clean( "HelixDaemon" );
	}

	builder.CleanCS();
}

void handleGen(bool displayBanner)
{
	if(displayBanner){
		printf("============================================================================\n");
		printf("== Gen Target\n");
		printf("============================================================================\n");
	}

	HelixBuilder builder;	
	builder.GenerateSqldo();
	HelixFS::getInstance().Load(); // Re-load the file system because GenerateSqldo creates files
}

void handleReGen(bool displayBanner)
{
	if(displayBanner){
		printf("============================================================================\n");
		printf("== Re-Gen Target\n");
		printf("============================================================================\n");
	}

	HelixBuilder builder;	
	builder.GenerateSqldo(true);
	HelixFS::getInstance().Load(); // Re-load the file system because GenerateSqldo creates files
}

void handleAsset(bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== Auto-Asset Target\n");
		printf("============================================================================\n");
	}

	HelixAutoAsset asset;	
	asset.Generate();
}

void handleStrings(bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== Extract Strings Target\n");
		printf("============================================================================\n");
	}

	HelixExtractStrings strings;	
	strings.Generate();
}

void handleJSApiGen(bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== JS API Gen Target\n");
		printf("============================================================================\n");
	}

	HelixBuilder builder;	
	builder.GenerateJSApi();
}

void handleJSGen(bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== JS Gen Target\n");
		printf("============================================================================\n");
	}

	HelixJSGenTask jsgen;	
	for(auto app : HelixConfig::getInstance().QxAppsAll()){
		jsgen.Generate( app );
	}
}

void handleCS(bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== CS Target\n");
		printf("============================================================================\n");
	}

	HelixBuilder builder;	
	builder.BuildCS();
}

void handleInstall(bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== Install Target\n");
		printf("============================================================================\n");
	}

	for(auto install : HelixConfig::getInstance().Installs() ){
		twine from(install, "from");
		twine ends(install, "endsWith");
		twine to(install, "to");
		twine newName(install, "newName");
		HelixWorker::getInstance().Add( new HelixInstallTask( from, ends, to, newName ) );
	}
}

void handleDeploy(bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== Deploy Target\n");
		printf("============================================================================\n");
	}

	for(auto install : HelixConfig::getInstance().Deploy() ){
		twine from(install, "from");
		twine ends(install, "endsWith");
		twine to(install, "to");
		twine newName(install, "newName");
		HelixWorker::getInstance().Add( new HelixInstallTask( from, ends, to, newName) );
	}
}

void testDep()
{
	HelixFS& fs = HelixFS::getInstance();
	auto fsFile = fs.FindFile( "GenerateWeeklyTimesheetReport.cpp" );
	if(fsFile){
		printf("GenerateWeeklyTimesheetReport.cpp - dependencies:\n");
		for(auto file : fsFile->Dependencies()){
			printf( "\t%s\n", file->PhysicalFileName()() );
		}
	}
}

void CopyCore()
{
	if(HelixConfig::getInstance().UseCore() == false){
		// Just make sure that the bin folder exists
		File::EnsurePath( "./bin/" );
		return;
	}

	printf("============================================================================\n");
	printf("== Copy Core Binaries and Files\n");
	printf("============================================================================\n");

	// Copy over the core binaries that we will need
	twine core = HelixConfig::getInstance().CoreFolder();
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixDaemon", "bin", ""));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixMain", "bin", ""));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.so", "bin", ""));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.so", "bin", ""));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.so", "bin", ""));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "db.xml", "bin", ""));
}
