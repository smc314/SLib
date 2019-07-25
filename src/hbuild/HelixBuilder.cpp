/* **************************************************************************

   Copyright (c): 2008 - 2013 Hericus Software, LLC

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#include <cstdlib>

#include <AnException.h>
#include <EnEx.h>
#include <Log.h>
#include <XmlHelpers.h>
#include <Timer.h>
#include <Tools.h>
#include <suvector.h>
using namespace SLib;

#include "HelixFS.h"
#include "HelixConfig.h"
#include "HelixBuilder.h"
#include "HelixWorker.h"
#include "HelixJSApiGenTask.h"
using namespace Helix::Build;


HelixBuilder::HelixBuilder()
{
	EnEx ee(FL, "HelixBuilder::HelixBuilder( HelixFS& helixfs )");

}

HelixBuilder::~HelixBuilder()
{
	EnEx ee(FL, "HelixBuilder::~HelixBuilder()");

}

void HelixBuilder::Build( const twine& folderPath )
{
	EnEx ee(FL, "HelixBuilder::Build( const twine& folderPath )");

	if(HelixWorker::getInstance().HasError()){
		return; // Bail out early on any previous errors
	}

	// Some special cases
	if(folderPath == "HelixMain" || folderPath == "HelixDaemon"){
		dptr<HelixLinkTask> lt = new HelixLinkTask( std::make_shared<HelixFSFolder_Bare>(folderPath)  );
		if(lt->RequiresLink()){
			HelixWorker::getInstance().Add( lt.release() );
		}
		return;
	}


	HelixFSFolder folder = HelixFS::getInstance().FindPath( folderPath );
	if(!folder){
		WARN(FL, "Unknown target build path: %s", folderPath() );
		return;
	}

	INFO(FL, "Checking path: %s", folderPath() );
	bool relinkRequired = false;

	// Main folder given
	for(auto file : folder->Files()){
		if(folderPath == "glob" && file->FileName() == "Jvm.cpp") continue; // Skip this one
#ifdef _WIN32
		if(folderPath == "server" && file->FileName() == "HelixDaemon.cpp") continue; // Skip this one
#else
		if(folderPath == "server" && file->FileName() == "HelixSvc.cpp") continue; // Skip this one
#endif
		if( (file->FileName().endsWith(".c") || file->FileName().endsWith(".cpp")) && file->NeedsRebuild( )){
			HelixWorker::getInstance().Add( new HelixCompileTask( folder, file ) );
			relinkRequired = true;
		}
	}

	// Check for a sqldo sub-folder
	HelixFSFolder sqldo = folder->FindFolder( "sqldo" );
	if(sqldo){
		for(auto file : sqldo->Files()){
			// Check to see if this is an orphan file by looking for the sql.xml file in the parent folder:
			twine fn( file->FileName() );
			auto splits = fn.split( "." );
			twine sqlXml( splits[0] + ".sql.xml");
			auto sqlXmlFile = folder->FindFile( sqlXml );
			if(sqlXmlFile == nullptr){
				WARN(FL, "Found orphan sqldo file: %s - deleting", file->PhysicalFileName()() );
				File::Delete( file->PhysicalFileName() );
			} else if( file->FileName().endsWith(".cpp") && file->NeedsRebuild( )){
				HelixWorker::getInstance().Add( new HelixCompileTask( sqldo, file ) );
				relinkRequired = true;
			}
		}
	}

	dptr<HelixLinkTask> linkTask = new HelixLinkTask( folder );
	if(relinkRequired || linkTask->RequiresLink()){
		HelixWorker::getInstance().Add( linkTask.release() );
	}

}

void HelixBuilder::Clean( const twine& folderPath )
{
	EnEx ee(FL, "HelixBuilder::Clean( const twine& folderPath )");

	// Some special cases
	if(folderPath == "HelixMain"){
		INFO(FL, "Cleaning HelixMain");
		CleanAllRuntime( "./bin" );
		return;
	} else if(folderPath == "HelixDaemon"){
		INFO(FL, "Cleaning HelixDaemon");
		CleanAllRuntime( "./bin" );
		return;
	}

	HelixFSFolder folder = HelixFS::getInstance().FindPath( folderPath );
	if(!folder){
		WARN(FL, "Unknown target build path: %s", folderPath() );
		return;
	}
	if(folder->FromCore()){
		// Don't clean this location - it's from core
		return;
	}

	INFO(FL, "Cleaning path: %s", folderPath() );

	// Main folder given
	for(auto file : folder->Files()){
		if( file->FileName().endsWith(".c") || file->FileName().endsWith(".cpp") ){
			DEBUG(FL, "Cleaning File: %s/%s", file->FolderName()(), file->DotOh()() );
			File::Delete( folder->PhysicalFolderName() + "/" + file->DotOh() );
		}
	}
	CleanAllRuntime( folder->PhysicalFolderName() );

	// Check for a sqldo sub-folder
	HelixFSFolder sqldo = folder->FindFolder( "sqldo" );
	if(sqldo){
		for(auto file : sqldo->Files()){
			// Check to see if this is an orphan file by looking for the sql.xml file in the parent folder:
			twine fn( file->FileName() );
			auto splits = fn.split( "." );
			twine sqlXml( splits[0] + ".sql.xml");
			auto sqlXmlFile = folder->FindFile( sqlXml );
			if(sqlXmlFile == nullptr){
				WARN(FL, "Found orphan sqldo file: %s - deleting", file->PhysicalFileName()() );
				File::Delete( file->PhysicalFileName() );
			} else if( file->FileName().endsWith(".cpp") ){
				DEBUG(FL, "Cleaning File: %s/%s", file->FolderName()(), file->DotOh()() );
				File::Delete( sqldo->PhysicalFolderName() + "/" + file->DotOh() );
			}
		}
		CleanAllRuntime( sqldo->PhysicalFolderName() );
	}

	HelixLinkTask hlt( folder );
	twine linkTarget( hlt.LinkTarget() );
	if(!linkTarget.empty() && File::Exists( linkTarget ) ){
		DEBUG(FL, "Cleaning %s", linkTarget() );
		File::Delete( linkTarget );
	}

}

void HelixBuilder::CleanAllRuntime( const twine& physicalPath )
{
	EnEx ee(FL, "HelixBuilder::CleanAllRuntime( const twine& physicalPath )");

	try {
		auto files = File::listFiles( physicalPath );
		for(auto const& file : files){
			if(file.endsWith( ".exe" ) ||
				file.endsWith( ".dll" ) ||
				file.endsWith( ".lib" ) ||
				file.endsWith( ".exp" ) ||
				file.endsWith( ".obj" ) ||
				file.endsWith( ".o" ) ||
				file.endsWith( ".so" ) ||
				file == "AutoAsset" ||
				file == "CopyCommon" ||
				file == "CopyIcons" ||
				file == "ExtractStrings" ||
				file == "GenerateSqlClasses" ||
				file == "HelixDaemon" ||
				file == "HelixMain" ||
				file == "LogDump" ||
				file == "MakeMakefiles" ||
				file == "SLogDump" ||
				file == "SqlDoCheck" ||
				file == "SqlShell" ||
				file == "hbuild"
			){
				try {
					File::Delete( physicalPath + "/" + file );
				} catch (AnException& ){
					// Ignore this
				}
			}
		}
	} catch (AnException&){
		// Ignore this - means the directory was not found
	}
}

void HelixBuilder::BuildCS()
{
	EnEx ee(FL, "HelixBuilder::BuildCS()");

	if(HelixConfig::getInstance().SkipPdfGen() == true){
		return; // Don't do this one
	}

	if(HelixWorker::getInstance().HasError()){
		return; // Bail out early on any previous errors
	}

	twine cmd;
	twine pdfGen;
	INFO(FL, "Building HelixPdfGen" );

	// Run the restore of all NuGet packages
#ifdef _WIN32
	pdfGen = "..\\c#\\HelixPdfGen";
	cmd = "cd " + pdfGen + " && c:\\software\\NuGet.exe restore HelixPdfGen.sln";
#elif __APPLE__
	pdfGen = "../c#/HelixPdfGen";
	cmd = "cd " + pdfGen + " && mono ~/bin/nuget.exe restore HelixPdfGen.sln";
#elif __linux__
	pdfGen = "../c#/HelixPdfGen";
	cmd = "cd " + pdfGen + " && mono nuget.exe restore HelixPdfGen.sln";
#else
	throw AnException(0, FL, "Unknown compile environment.");
#endif
	if(std::system( cmd() ) != 0) throw AnException(0, FL, "C# - NuGet restore failed.");

	// Run the build of the project
	cmd = "cd " + pdfGen + " && msbuild HelixPdfGen.sln /p:Configuration=Release /p:Platform=\"Any CPU\" /t:Rebuild";
	if(std::system( cmd() ) != 0) throw AnException(0, FL, "C# - Build failed.");

	// Make sure we have the right output path created
	File::EnsurePath( "./bin/HelixPdfGen/" );

	// Copy the release files over into our output folder
	for(auto& file : File::listFiles( pdfGen + "/HelixPdfGen/bin/Release" )){
		if(file.endsWith( ".dll" ) ||
			file.endsWith( ".exe" ) ||
			file.endsWith( ".xml" )
		){
			File::Copy( pdfGen + "/HelixPdfGen/bin/Release/" + file, "./bin/HelixPdfGen/" + file );
		}
	}
}

void HelixBuilder::CleanCS()
{
	EnEx ee(FL, "HelixBuilder::CleanCS()");

	if(HelixConfig::getInstance().SkipPdfGen() == true){
		return; // Don't do this one
	}

	INFO(FL, "Cleaning HelixPdfGen" );
	twine pdfGen = "../c#/HelixPdfGen/HelixPdfGen/";

	// Several folders that we want to remove from the C# project:
	File::RmDir( pdfGen + "bin/" );
	File::RmDir( pdfGen + "obj/" );

	// Remove all cs files from the DO subfolders
	for(auto& folder : File::listFolders( pdfGen + "DO/" )){
		if(folder == "." || folder == "..") continue; // Skip these
		for(auto& file : File::listFiles( pdfGen + "DO/" + folder )){
			if(file.endsWith( ".cs" )){
				twine rmTarget( pdfGen + "DO/" + folder + "/" + file );
				DEBUG(FL, "Removing file: %s", rmTarget() );
				File::Delete( rmTarget );
			}
		}
	}
}

void HelixBuilder::BuildCSTest()
{
	EnEx ee(FL, "HelixBuilder::BuildCSTest()");

	if(HelixConfig::getInstance().SkipPdfGen() == true){
		return; // Don't do this one
	}

	if(HelixWorker::getInstance().HasError()){
		return; // Bail out early on any previous errors
	}

	twine cmd;
	twine testRoot;
	INFO(FL, "Building Helix.Test" );

	// Run the restore of all NuGet packages
#ifdef _WIN32
	testRoot = "..\\..\\test\\Helix.Test";
	cmd = "cd " + testRoot + " && c:\\software\\NuGet.exe restore Helix.Test.sln";
#elif __APPLE__
	testRoot = "../../test/Helix.Test";
	cmd = "cd " + testRoot + " && mono ~/bin/nuget.exe restore Helix.Test.sln";
#elif __linux__
	testRoot = "../../test/Helix.Test";
	cmd = "cd " + testRoot + " && mono ~/bin/nuget.exe restore Helix.Test.sln";
#else
	throw AnException(0, FL, "Unknown compile environment.");
#endif
	if(std::system( cmd() ) != 0) throw AnException(0, FL, "C# - Helix.Test NuGet restore failed.");

	// Run the build of the project
	cmd = "cd " + testRoot + " && msbuild Helix.Test.sln /p:Configuration=Release /p:Platform=\"Any CPU\" /t:Rebuild";
	if(std::system( cmd() ) != 0) throw AnException(0, FL, "C# - Helix.Test Build failed.");
}

void HelixBuilder::CleanCSTest()
{
	EnEx ee(FL, "HelixBuilder::CleanCSTest()");

	if(HelixConfig::getInstance().SkipPdfGen() == true){
		return; // Don't do this one
	}

	INFO(FL, "Cleaning Helix.Test" );
	twine testRoot = "../../test/Helix.Test/Helix.Test/";

	// Several folders that we want to remove from the C# project:
	File::RmDir( testRoot + "bin/" );
	File::RmDir( testRoot + "obj/" );
}

void HelixBuilder::GenerateSqldo(bool forceRegen)
{
	EnEx ee(FL, "HelixBuilder::GenerateSqldo(bool forceRegen)");

	// Find all .sql.xml files
	vector<HelixFSFile> allSqldo = HelixFS::getInstance().FindFilesByType( ".sql.xml" );

	twine currentFolderName;
	HelixFSFolder currentFolder = nullptr;
	HelixFSFolder currentSqldoFolder = nullptr;
	for(auto file : allSqldo){
		if(currentFolderName != file->FolderName()){
			// Look up the folder so we can access more details
			currentFolder = HelixFS::getInstance().FindPath( file->FolderName() );
			if(!currentFolder){
				throw AnException(0, FL, "Unable to find folder: %s\n", file->FolderName()() );
			}
			currentSqldoFolder = currentFolder->FindFolder( "sqldo" );
			if(!currentSqldoFolder){
				currentFolder->SubFolders().push_back( 
					std::make_shared<HelixFSFolder_Bare>( currentFolder->FolderName() + "/sqldo") 
				);
				currentSqldoFolder = currentFolder->FindFolder( "sqldo" );
				if(!currentSqldoFolder){
					throw AnException(0, FL, "Could not find new sqldo folder after adding it.");
				}
			}

			currentFolderName = file->FolderName();
		}

		twine logicRepo( HelixConfig::getInstance().LogicRepo( currentFolder->LastName()() ) );
		twine logicFolder;
		if(logicRepo.empty()){
			// Regular local logic folder
			logicFolder = "logic/" + currentFolder->LastName();
		} else {
			// Logic folder lives in another repository
			logicFolder = "../../../" + logicRepo + "/server/c/logic/" + currentFolder->LastName();
		}
		if( logicFolder != currentFolderName ){
			DEBUG( FL, "Skipping %s because %s != %s", file->FileName()(), logicFolder(), currentFolderName() );
			continue;
		}

		// What's the data object being generated?  Usually matches the name of the .sql.xml file, but double-check
		twine doName( file->DataObjectName() );
		
		// Check our sql.xml file against the sqldo/cpp file
		HelixFSFile sqldoFile = currentSqldoFolder->FindFile( doName + ".cpp" );
		if(forceRegen){
			DEBUG(FL, "Data Object: %s - forcing regeneration.", file->PhysicalFileName()() );
			HelixWorker::getInstance().Add( new HelixGenTask( currentSqldoFolder, file ) );
		} else if( file->IsNewerThan( sqldoFile ) ){
			DEBUG(FL, "Data Object: %s is newer than .cpp - triggering regeneration.", file->PhysicalFileName()() );
			HelixWorker::getInstance().Add( new HelixGenTask( currentSqldoFolder, file ) );
		} else if( file->FromCore() && HelixConfig::getInstance().SkipPdfGen() == false ){
			// If the file is from core, double-check if we need to generate the C# data object.
			HelixSqldo s( currentSqldoFolder, file );
			if( file->IsNewerThan( s.CSBodyFileName() ) ) {
				DEBUG(FL, "Data Object: %s is newer than .cs - triggering regeneration.", file->PhysicalFileName()() );
				HelixWorker::getInstance().Add( new HelixGenTask( currentSqldoFolder, file ) );
			}
		}

	}

	// Update the HelixPdfGen.csproj file to include references to all data objects
	if(HelixConfig::getInstance().SkipPdfGen() == false){
		UpdateHelixPdfGenProj( allSqldo );	
	}

	// Wait for all generator threads to finish
	HelixWorker::getInstance().WaitForGenerators();
	if(HelixWorker::getInstance().HasError()){
		return;
	}

}

void HelixBuilder::GenerateJSApi()
{
	EnEx ee(FL, "HelixBuilder::GenerateJSApi()");

	HelixJSApiGenTask jsApiGen;
	for(auto app : HelixConfig::getInstance().QxApps()){
		jsApiGen.Generate( app );
	}

}

void HelixBuilder::UpdateHelixPdfGenProj(vector<HelixFSFile>& allSqldo)
{
	EnEx ee(FL, "HelixBuilder::UpdateHelixPdfGenProj(vector<HelixFSFile>& allSqldo)");

	bool modified = false;
	twine csprojName; csprojName = "../c#/HelixPdfGen/HelixPdfGen/HelixPdfGen.csproj";
	xmlKeepBlanksDefault(0);
	sptr<xmlDoc, xmlFreeDoc> csproj = xmlParseFile( csprojName() );
	if(csproj == nullptr){
		throw AnException(0, FL, "Error opening HelixPdfGen.csproj for update");
	}
	xmlNodePtr root = xmlDocGetRootElement( csproj );
	for(auto item : XmlHelpers::FindChildren( root, "ItemGroup" )){
		// Check to see if we have compile items in this group:
		xmlNodePtr check = XmlHelpers::FindChild( item, "Compile" );
		if(check == nullptr) continue; // Find another ItemGroup

		// Found compiles - Remove all that start with DO\ then replace them
		// with our updated list of data objects.
		for(auto compile : XmlHelpers::FindChildren( item, "Compile" ) ){
			twine include( compile, "Include" );
			if(include.startsWith( "DO\\" )){
				xmlUnlinkNode( compile ); // Remove it from thedocument
				xmlFreeNode( compile ); // Free the node memory
			}
		}

		// Use a sorted-unique vector to eliminate duplicates
		suvector<twine> donames;
		for(auto sqldo : allSqldo){
			twine includeName( "DO\\" + sqldo->LastFolderName() + "\\" + sqldo->DataObjectName() + ".cs" );
			donames.push_back( includeName );
		}

		// add all of the unique sqldo names to the cs compile list
		for(auto& sqldo : donames){
			// Add it in:
			xmlNodePtr csInclude = xmlNewChild( item, NULL, (const xmlChar*)"Compile", NULL);
			xmlSetProp( csInclude, (const xmlChar*)"Include", sqldo);
			modified = true;
		}
		break; // We found the ItemGroup with compiles in it - no need to keep searching the file
	}

	if(modified){
		// Save the updated file:
		File::writeToFile( csprojName, XmlHelpers::docToStringPretty( csproj ) );
	}
}

void HelixBuilder::CleanSqldo()
{
	EnEx ee(FL, "HelixBuilder::CleanSqldo()");
	INFO(FL, "Cleaning Sqldo" );

	// Find all .sql.xml files
	vector<HelixFSFile> allSqldo = HelixFS::getInstance().FindFilesByType( ".sql.xml" );

	twine currentFolderName;
	HelixFSFolder currentFolder;
	HelixFSFolder currentSqldoFolder;
	for(auto file : allSqldo){
		if(currentFolderName != file->FolderName()){
			// Look up the folder so we can access more details
			currentFolder = HelixFS::getInstance().FindPath( file->FolderName() );
			if(!currentFolder){
				throw AnException(0, FL, "Unable to find folder: %s\n", file->FolderName()() );
			}
			currentSqldoFolder = currentFolder->FindFolder( "sqldo" );
			if(!currentSqldoFolder){
				currentFolder->SubFolders().push_back( 
					std::make_shared<HelixFSFolder_Bare>( currentFolder->FolderName() + "/sqldo") 
				);
				currentSqldoFolder = currentFolder->FindFolder( "sqldo" );
				if(!currentSqldoFolder){
					throw AnException(0, FL, "Could not find new sqldo folder after adding it.");
				}
			}

			currentFolderName = file->FolderName();
		}

		if( file->FromCore() ){
			continue; // Don't clean sqldo files from core
		}

		// What's the data object being generated?  Usually matches the name of the .sql.xml file, but double-check
		twine doName( file->DataObjectName() );
		
		// Remove the h and cpp file
		INFO(FL, "Removing Generated SqlDo File: %s.h and %s.cpp %s", doName(), doName(), file->PhysicalFileName()() );
		currentSqldoFolder->DeleteFile( doName + ".h" );
		currentSqldoFolder->DeleteFile( doName + ".cpp" );
	}
}

