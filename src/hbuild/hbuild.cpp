/* ***************************************************************************

	Copyright: 2013 Hericus Software, LLC

	License: The MIT License (MIT)

	Authors: Steven M. Cherry

*************************************************************************** */
#include <signal.h>

#include <AnException.h>
#include <Log.h>
#include <xmlinc.h>
#include <XmlHelpers.h>
#include <twine.h>
#include <File.h>
#include <Timer.h>
#include <Date.h>
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
#include "HelixScanUnused.h"
#include "HelixJSGenTask.h"
#include "HelixSqldo.h"
using namespace Helix::Build;

Date m_start_date;
Date m_end_date;
twine m_platform;
std::vector<twine> m_targets;

static bool in_sighandler = false;
void sighandler(int param);

void handleAll();
void handleClean();
void handleTest( int argIndex );
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
void handleTestGen(int argIndex);
void handleSqldoGen(int argIndex);
void handleCrudGen(int argIndex);
void handleRepGen(int argIndex);
void handleUnused(int argIndex);
void testDep();
void CopyCore();
void describe();
int internalMain(int argc, char**argv);

#ifdef _WIN32
LONG WINAPI OurCrashHandler(EXCEPTION_POINTERS* /*Exception Info*/);
#endif

int main(int argc, char** argv)
{
	// Set up our signal handlers to catch and gracefully terminate if something goes wrong
	signal( SIGABRT, sighandler);
	signal( SIGFPE, sighandler);
	signal( SIGILL, sighandler);
	signal( SIGINT, sighandler);
	signal( SIGSEGV, sighandler);
	signal( SIGTERM, sighandler);

#ifdef _WIN32
	::SetUnhandledExceptionFilter(OurCrashHandler);
#endif

	return internalMain( argc, argv );
}

#ifdef _WIN32
LONG WINAPI OurCrashHandler(EXCEPTION_POINTERS* ep)
{
	sighandler(SIGSEGV);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

/** This is our signal handler.  It is used to catch all signals that may be
 *  sent for this program.  This will dump our logs and then
 *  ensure that everything shuts down properly.
 */
void sighandler(int param)
{
	char* signame;
	switch(param){
		case SIGABRT: signame = (char*)"SIGABRT"; break;
		case SIGFPE: signame = (char*)"SIGFPE"; break;
		case SIGILL: signame = (char*)"SIGILL"; break;
		case SIGINT: signame = (char*)"SIGINT"; break;
		case SIGSEGV: signame = (char*)"SIGSEGV"; break;
		case SIGTERM: signame = (char*)"SIGTERM"; break;
		default: signame = (char*)"UNKNOWN";
	}

	if(in_sighandler){
		// We're getting called again.  Something is going wrong during
		// shutdown.  Log this to the console and exit.
		printf("%s: called recursively.  Error in signal handler and shutdown!\n", signame);
		if(param == SIGABRT){
			// just return from this method - that will allow the usual abort process to go through
			return;
		}
		// otherwise - raise the abort ourselves.
		abort();
	}
	in_sighandler = true; // watch for recursive calls to this method.

	PANIC(FL, "%s called.  Flushing and closing logs and shutting down hbuild.", signame);
	printf("%s: called.  Flushing and closing logs and shutting down hbuild.\n", signame);
	EnEx::PrintStackTrace( 0 ); // dump our stack trace to the Panic channel.
	EnEx::PrintStackTrace( ); // dump our stack trace to stdout
	printf("%s: Program shutdown complete.\n", signame);
	exit(0);
}

int internalMain (int argc, char** argv)
{
	m_start_date.SetCurrent();

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
#elif __APPLE__
	m_platform = "MAC";
#else
	m_platform = "LINUX_x64";
#endif
	printf("%s - Helix Build Running for a %s platform.\n", m_start_date.GetValue()(), m_platform());
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
			bool didClean = false;
			for( size_t i = 0; i < m_targets.size(); i++){
				twine targ( m_targets[i] );
				if(didClean){
					didClean = false;
					CopyCore();
				}
				if(targ == "clean") { handleClean(); didClean = true; }
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
				else if(targ == "test") {
					handleTest( (int)i );
					break; // All the rest of the arguments belong to the test command
				}
				else if(targ == "testgen") {
					handleTestGen( (int)i );
					break; // All the rest of the arguments belong to the test command
				}
				else if(targ == "sqldogen") {
					handleSqldoGen( (int)i );
					break; // All the rest of the arguments belong to the test command
				}
				else if(targ == "crudgen") {
					handleCrudGen( (int)i );
					break; // All the rest of the arguments belong to the test command
				}
				else if(targ == "repgen") {
					handleRepGen( (int)i );
					break; // All the rest of the arguments belong to the test command
				}
				else if(targ == "unused") {
					handleUnused( (int)i );
					break; // All the rest of the arguments belong to the test command
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
		m_end_date.SetCurrent();
		auto duration = m_end_date - m_start_date;
		printf("%s\n", e.Msg());
		printf("%s\n", e.Stack());
		printf("============================================================================\n");
		printf("== %s (%ds) hbuild complete - with errors\n", m_end_date.GetValue()(), duration.Sec() );
		printf("============================================================================\n");
		return 1; // Errors
	} catch(std::exception& e){
		m_end_date.SetCurrent();
		auto duration = m_end_date - m_start_date;
		printf("%s\n", e.what());
		printf("============================================================================\n");
		printf("== %s (%ds) hbuild complete - with errors\n", m_end_date.GetValue()(), duration.Sec() );
		printf("============================================================================\n");
		return 1; // Errors
	} catch(...){
		m_end_date.SetCurrent();
		auto duration = m_end_date - m_start_date;
		printf("Unknown exception caught\n");
		printf("============================================================================\n");
		printf("== %s (%ds) hbuild complete - with errors\n", m_end_date.GetValue()(), duration.Sec() );
		printf("============================================================================\n");
		return 1; // Errors
	}

	m_end_date.SetCurrent();
	auto duration = m_end_date - m_start_date;
	if(HelixWorker::getInstance().HasError()){
		printf("============================================================================\n");
		printf("== %s (%ds) hbuild complete - with errors\n", m_end_date.GetValue()(), duration.Sec() );
		printf("============================================================================\n");
		return 1; // Errors
	} else {
		printf("============================================================================\n");
		printf("== %s (%ds) hbuild complete.\n", m_end_date.GetValue()(), duration.Sec() );
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
	printf("== test - Builds the system with testing enabled, and then launches the test app\n");
	printf("== testgen - creates a test program with the following options\n");
	printf("             logic=... - specifies which logic folder to use\n");
	printf("             api=... - specifies which server API to generate tests for\n");
	printf("             do=... - specifies which server Data Object to generate full crud/paging tests for\n");
	printf("             crud=true/false - specifies whether crud testing is generated (true by default)\n");
	printf("             page=true/false - specifies whether page testing is generated (true by default)\n");
	printf("== sqldogen - creates a .sql.xml file with the following options\n");
	printf("             db=... - specifies which db.xml file to read\n");
	printf("             table=... - specifies which table in the db.xml file to pull the structure from\n");
	printf("             logic=... - specifies which logic folder to use\n");
	printf("             do=... - specifies which server Data Object to generate\n");
	printf("== crudgen - creates a set of CRUD files  with the following options\n");
	printf("             logic=... - specifies which logic folder to use\n");
	printf("             do=... - specifies which server Data Object to generate\n");
	printf("             page=true/false - specifies whether paging is generated (true by default)\n");
	printf("== repgen -  creates a set of Bulk Replication files with the following options\n");
	printf("             logic=... - specifies which logic folder to use\n");
	printf("             do=... - specifies which server Data Object to generate\n");
	printf("== unused -  scans sql.xml data objects for unused methods with the following options\n");
	printf("             logic=... - specifies which logic folder to scan (optional)\n");
	printf("             do=... - specifies which server Data Object to scan (optional)\n");
	printf("             only=true/false - indicates whether to only show unused or counts on all methods (default true)\n");
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
		builder.Clean( "HelixDaemon" );
	}
	builder.Clean( "HelixMain" ); // Clean's out the bin folder for core and non-core projects

	builder.CleanCS();
	builder.CleanCSTest();
}

void handleTest(int argIndex )
{
	printf("============================================================================\n");
	printf("== Test Target - Build\n");
	printf("============================================================================\n");

	// Turn on the flag to get testing included
	HelixConfig::getInstance().IncludeTest( true );

	handleAll();

	HelixBuilder builder;	
	builder.Build("HelixTest");

	if(HelixWorker::getInstance().HasError()){
		return; // don't run the tests if we have a compile error
	}

	// Then run the tests with the rest of our arguments
	/*
	printf("============================================================================\n");
	printf("== Test Target - Run\n");
	printf("============================================================================\n");
#ifdef _WIN32
	twine cmd = "cd bin && ./HelixTest.exe";
#else
	twine cmd = "cd bin && ./HelixTest";
#endif

	for(size_t i = argIndex; i < m_targets.size(); i++){
		cmd += " " + m_targets[i];
	}
	int ret = std::system( cmd() );
	if(ret != 0){
		HelixWorker::getInstance().CompileError();
	}
	*/
}

void handleTestGen(int argIndex )
{
	printf("============================================================================\n");
	printf("== Test Generation Target - Build\n");
	printf("============================================================================\n");

	twine logic; // Picks up argument: logic=ttvx 
	twine forApi; // Picks up argument: api=/logic/ttvx/UpdateITOStatus
	twine forDO; // Picks up argument do=ITOStatus
	twine forCrud; // Picks up argument crud=true
	twine forPage; // Picks up argument page=true
	
	for(size_t i = argIndex; i < m_targets.size(); i++){
		auto splits = m_targets[i].split("=");
		if(splits[0] == "logic"){
			logic = splits[1];
		} else if(splits[0] == "api"){
			forApi = splits[1];
		} else if(splits[0] == "do"){
			forDO = splits[1];
		} else if(splits[0] == "crud"){
			forCrud = splits[1];
		} else if(splits[0] == "page"){
			forPage = splits[1];
		}
	}

	if(logic.empty()){
		throw AnException(0, FL, "logic=... parameter required for testgen option");
	}
	if(forApi.empty() && forDO.empty()){
		throw AnException(0, FL, "At least one of api=... do=... must be specified for testgen option");
	}

	if(!forDO.empty()){
		printf("Generating test for %s in logic/%s/test/\n", forDO(), logic() );

		twine path = "logic/" + logic;
		auto folder = HelixFS::getInstance().FindPath( path );
		if(!folder){
			WARN(FL, "Unknown target path: %s", path() );
			return;
		}
		twine fileName = forDO + ".sql.xml";
		auto sqldoFile = folder->FindFile( fileName );
		if(!sqldoFile){
			WARN(FL, "Unknown target data object: %s/%s", path(), fileName() );
			return;
		}
		HelixSqldo sqldo( folder, sqldoFile );
		twine testHeader = sqldo.CPPTestHeaderFileName();
		File::EnsurePath( testHeader );
		if(!File::Exists( testHeader )){
			printf( "Writing Test Header to: %s\n", testHeader() );
			File::writeToFile( testHeader, sqldo.GenCPPTestHeader( forCrud != "false", forPage != "false" ) );
		}
		twine testBody = sqldo.CPPTestBodyFileName();
		printf("Writing Test Body to: %s\n", testBody() );
		File::writeToFile( testBody, sqldo.GenCPPTestBody( forCrud != "false", forPage != "false" ) );
	}
}

void handleCrudGen(int argIndex )
{
	printf("============================================================================\n");
	printf("== CRUD Generation Target - Build\n");
	printf("============================================================================\n");

	twine logic; // Picks up argument: logic=ttvx 
	twine forDO; // Picks up argument do=ITOStatus
	twine forPage; // Picks up argument page=true
	
	for(size_t i = argIndex; i < m_targets.size(); i++){
		auto splits = m_targets[i].split("=");
		if(splits[0] == "logic"){
			logic = splits[1];
		} else if(splits[0] == "do"){
			forDO = splits[1];
		} else if(splits[0] == "page"){
			forPage = splits[1];
		}
	}

	if(logic.empty()){
		throw AnException(0, FL, "logic=... parameter required for crudgen option");
	}
	if(forDO.empty()){
		throw AnException(0, FL, "do=... parameter required for crudgen option");
	}

	printf("Generating CRUD for %s in logic/%s/\n", forDO(), logic() );

	HelixSqldo sqldo;
	twine fileName; 
	twine path = File::PathCombine( "logic", logic );

	fileName.format("Delete%s.h", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::EnsurePath( fileName ); // Only need to do this once
	File::writeToFile( fileName, sqldo.GenCRUDDeleteHeader( logic, forDO ) );

	fileName.format("Delete%s.cpp", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenCRUDDeleteBody( logic, forDO ) );

	fileName.format("GetAll%s.h", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenCRUDGetAllHeader( logic, forDO ) );

	fileName.format("GetAll%s.cpp", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenCRUDGetAllBody( logic, forDO ) );

	fileName.format("GetOne%s.h", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenCRUDGetOneHeader( logic, forDO ) );

	fileName.format("GetOne%s.cpp", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenCRUDGetOneBody( logic, forDO ) );

	fileName.format("Update%s.h", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenCRUDUpdateHeader( logic, forDO ) );

	fileName.format("Update%s.cpp", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenCRUDUpdateBody( logic, forDO ) );

	if(forPage != "false"){
		fileName.format("GetPaged%s.h", forDO() );
		fileName = File::PathCombine( path, fileName );
		File::writeToFile( fileName, sqldo.GenCRUDGetPagedHeader( logic, forDO ) );

		fileName.format("GetPaged%s.cpp", forDO() );
		fileName = File::PathCombine( path, fileName );
		File::writeToFile( fileName, sqldo.GenCRUDGetPagedBody( logic, forDO ) );
	}
}

void handleRepGen(int argIndex )
{
	printf("============================================================================\n");
	printf("== Replication Generation Target - Build\n");
	printf("============================================================================\n");

	twine logic; // Picks up argument: logic=ttvx 
	twine forDO; // Picks up argument do=ITOStatus
	
	for(size_t i = argIndex; i < m_targets.size(); i++){
		auto splits = m_targets[i].split("=");
		if(splits[0] == "logic"){
			logic = splits[1];
		} else if(splits[0] == "do"){
			forDO = splits[1];
		}
	}

	if(logic.empty()){
		throw AnException(0, FL, "logic=... parameter required for repgen option");
	}
	if(forDO.empty()){
		throw AnException(0, FL, "do=... parameter required for repgen option");
	}

	printf("Generating Bulk Replication for %s::%s in logic/rep\n", logic(), forDO() );

	HelixSqldo sqldo;
	twine fileName; 
	twine path( "logic/rep" );

	fileName.format("BulkReplicateSend%s.h", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::EnsurePath( fileName ); // Only need to do this once
	File::writeToFile( fileName, sqldo.GenRepSendHeader( logic, forDO ) );

	fileName.format("BulkReplicateSend%s.cpp", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenRepSendBody( logic, forDO ) );

	fileName.format("BulkReplicateRecv%s.h", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenRepRecvHeader( logic, forDO ) );

	fileName.format("BulkReplicateRecv%s.cpp", forDO() );
	fileName = File::PathCombine( path, fileName );
	File::writeToFile( fileName, sqldo.GenRepRecvBody( logic, forDO ) );
}

void handleUnused(int argIndex )
{
	printf("============================================================================\n");
	printf("== Scan Data Objects for Unused Methods\n");
	printf("============================================================================\n");

	twine logic; // Picks up argument: logic=ttvx 
	twine forDO; // Picks up argument do=ITOStatus
	twine only;
	
	for(size_t i = argIndex; i < m_targets.size(); i++){
		auto splits = m_targets[i].split("=");
		if(splits[0] == "logic"){
			logic = splits[1];
		} else if(splits[0] == "do"){
			forDO = splits[1];
		} else if(splits[0] == "only"){
			only = splits[1];
		}
	}

	if(logic.empty()){
		logic = "all";
	}
	if(forDO.empty()){
		forDO = "all";
	}
	if(only.empty()){
		only = "true";
	}

	HelixScanUnused scanUnused;	
	scanUnused.Generate(logic, forDO, only == "true");

}

void handleSqldoGen(int argIndex)
{
	printf("============================================================================\n");
	printf("== SqlDO Generation Target - Build\n");
	printf("============================================================================\n");

	/*
	printf("== sqldogen - creates a .sql.xml file with the following options\n");
	printf("             db=... - specifies which db.xml file to read\n");
	printf("             table=... - specifies which table in the db.xml file to pull the structure from\n");
	printf("             logic=... - specifies which logic folder to use\n");
	printf("             do=... - specifies which server Data Object to generate\n");
	*/

	twine forDB; // Picks up argument: db=...
	twine forTable; // Picks up argument: table=...
	twine logic; // Picks up argument: logic=ttvx 
	twine forDO; // Picks up argument do=ITOStatus
	
	for(size_t i = argIndex; i < m_targets.size(); i++){
		auto splits = m_targets[i].split("=");
		if(splits[0] == "logic"){
			logic = splits[1];
		} else if(splits[0] == "db"){
			forDB = splits[1];
		} else if(splits[0] == "do"){
			forDO = splits[1];
		} else if(splits[0] == "table"){
			forTable = splits[1];
		}
	}

	if(forDB.empty()){
		throw AnException(0, FL, "db=... parameter required for sqldogen option");
	}
	if(forTable.empty()){
		throw AnException(0, FL, "table=... parameter required for sqldogen option");
	}
	if(logic.empty()){
		throw AnException(0, FL, "logic=... parameter required for sqldogen option");
	}
	if(forDO.empty()){
		throw AnException(0, FL, "do=... parameter required for sqldogen option");
	}

	twine sqlFileName = forDO + ".sql.xml";
	twine fullPath = File::PathCombine( "logic", File::PathCombine( logic, sqlFileName ) );
	File::EnsurePath( fullPath );

	HelixSqldo sqldo;
	printf("Generating sqldo in %s\n", fullPath() );
	File::writeToFile( fullPath, sqldo.GenSqlXml( forDB, forTable, logic, forDO ) );
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
		twine platform(install, "platform");
		HelixWorker::getInstance().Add( new HelixInstallTask( from, ends, to, newName, platform ) );
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
		twine platform(install, "platform");
		HelixWorker::getInstance().Add( new HelixInstallTask( from, ends, to, newName, platform ) );
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
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixSvc.exe", "bin", "", "WIN32"	));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixMain.exe", "bin", "", "WIN32"	));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.dll", "bin", "", "WIN32"	));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.lib", "bin", "", "WIN32"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.dll", "bin", "", "WIN32"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.lib", "bin", "", "WIN32"	));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.dll", "bin", "", "WIN32"	));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.lib", "bin", "", "WIN32"	));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dm.dll", "bin", "", "WIN32"	));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dm.lib", "bin", "", "WIN32"));
#elif __APPLE__
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixDaemon", "bin", "", "MAC"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixMain", "bin", "", "MAC"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.so", "bin", "", "MAC"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.so", "bin", "", "MAC"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.so", "bin", "", "MAC"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dm.so", "bin", "", "MAC"));
#else
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixDaemon", "bin", "", "LINUX"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "HelixMain", "bin", "", "LINUX"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.so", "bin", "", "LINUX"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.so", "bin", "", "LINUX"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.so", "bin", "", "LINUX"));
	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dm.so", "bin", "", "LINUX"));
#endif

	HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "db.xml", "bin", "", ""));
	if(HelixConfig::getInstance().UseDebug()){
		HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.client.pdb", "bin", "", ""));
		HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.glob.pdb", "bin", "", ""	));
		HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dev.pdb", "bin", "", ""));
		HelixWorker::getInstance().Add( new HelixInstallTask(core + "/server/c/bin", "libhelix.logic.dm.pdb", "bin", "", ""));
	}
}
