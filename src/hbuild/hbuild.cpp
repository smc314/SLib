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
#include "HelixCountLines.h"
#include "HelixFind.h"
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
void handleFind(const twine& searchString, bool displayBanner = true);
void handleLines(bool displayBanner = true);
void testDep();
void CopyCore();
void describe();

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

		printf("Loading the helix filesystem");
		timer.Start();
		HelixFS::getInstance();
		timer.Finish(); printf(" - duration: %f seconds\n", timer.Duration() );

		if(m_targets.size() == 0){
			handleAll();
		} else {
			bool didClean = false;
			for( size_t i = 0; i < m_targets.size(); i++){
				twine targ( m_targets[i] );
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
				else if(targ == "lines") handleLines();
				else if(targ == "find") {
					if( i + 1 < m_targets.size()){
						twine findArg( m_targets[i+1] );
						i++;
						handleFind(findArg);
					} else {
						throw AnException(0, FL, "find requires an argument.");
					}
				}
				else if(targ == "?") describe();
				else if(targ == "help") describe();
				else if(targ == "-?") describe();
			}
		}
	
		HelixWorker::getInstance().Finish();
		HelixFS::getInstance().SaveCache();

		/* Use this for profiling what's going on and what's taking up the time
		twine hitmap;
		EnEx::PrintGlobalHitMap( hitmap );
		printf("\n\n%s\n\n", hitmap() );
		*/

	} catch(AnException& e){
		printf("%s\n", e.Msg());
		printf("%s\n", e.Stack());
		printf("============================================================================\n");
		printf("== hbuild complete - with errors\n");
		printf("============================================================================\n");
		return 1; // Errors
	}

	if(HelixWorker::getInstance().HasError()){
		printf("============================================================================\n");
		printf("== hbuild complete - with errors\n");
		printf("============================================================================\n");
		return 1; // Errors
	} else {
		printf("============================================================================\n");
		printf("== hbuild complete.\n");
		printf("============================================================================\n");
		return 0; // No error
	}
}

void describe()
{
	printf("============================================================================\n");
	printf("== HBuild Options\n");
	printf("============================================================================\n");
	printf("== clean - cleans the build target\n");
	printf("== all - does everything except dep\n");
	printf("== gen - Generates the C++, JS, and C# from .sql.xml files as required\n");
	printf("== regen - Forces a regeneration of the C++, JS, and C# from all .sql.xml\n");
	printf("== jsapi - Generates the Api.js file in each of our QX projects\n");
	printf("== jsgen - runs generate.py for all QX projects (build and source-hybrid)\n");
	printf("== cs - recompiles the C# HelixPdfGen project if present\n");
	printf("== install - Runs the install lines from the hbuild.xml file\n");
	printf("== dep - Runs the Deploy section from the hbuild.xml file\n");
	printf("== asset - Runs the auto-asset function against all QX projects\n");
	printf("== strings - Runs the extract-strings process to build translation files\n");
	printf("== lines - Runs a count of all source code files and lines\n");
	printf("== find searchString - Searches the source code for the given searchString\n");
	printf("\n");
	printf("== If no target is specified, the 'all' target is invoked.\n");
	printf("\n");
}

void handleAll()
{
	printf("============================================================================\n");
	printf("== All Target\n");
	printf("============================================================================\n");

	HelixBuilder builder;	
	handleGen(false);

	twine corePrefix = "";
	if(HelixConfig::getInstance().UseCore()){
		corePrefix = HelixConfig::getInstance().CoreFolder() + "/server/c/"; // these are from core
	} // otherwise, they're from this repo

	builder.Build( corePrefix + "logic/util" );
	builder.Build( corePrefix + "logic/admin" );
	builder.Build( corePrefix + "glob" );
	builder.Build( corePrefix + "server" );
	builder.Build( corePrefix + "client" );

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

	builder.Build( "HelixMain" );
	builder.Build( "HelixDaemon" );

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
		builder.Clean( "HelixDaemon" );
	}
	builder.Clean( "HelixMain" ); // Clean's out the bin folder for core and non-core projects

	builder.CleanCS();
	builder.CleanCSTest();
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

void handleLines(bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== Count Lines Target\n");
		printf("============================================================================\n");
	}

	HelixCountLines lines;	
	lines.Generate();
}

void handleFind(const twine& searchString, bool displayBanner)
{
	if(HelixWorker::getInstance().HasError()){
		return;
	}
	if(displayBanner){
		printf("============================================================================\n");
		printf("== Search Target\n");
		printf("============================================================================\n");
	}

	HelixFind find;	
	find.Generate(searchString);
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
	builder.BuildCSTest();
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

#ifdef _WIN32
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixSvc.exe", "bin", ""));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixMain.exe", "bin", ""));
//	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.dll", "bin", ""));
//	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.lib", "bin", ""));
//	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.dll", "bin", ""));
//	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.lib", "bin", ""));
//	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.dll", "bin", ""));
//	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.lib", "bin", ""));
//	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dm.dll", "bin", ""));
//	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dm.lib", "bin", ""));
#else
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixDaemon", "bin", ""));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixMain", "bin", ""));
	//HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.so", "bin", ""));
	//HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.so", "bin", ""));
	//HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.so", "bin", ""));
	//HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dm.so", "bin", ""));
#endif

	// TODO: This line looks like it's the only one from CopyCore() that's necessary anymore
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "db.xml", "bin", ""));
}
