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
using namespace SLib;

#include "HelixFS.h"
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
		if(folderPath == "server" && file->FileName() == "HelixDaemon.cpp") continue; // Skip this one
		if( (file->FileName().endsWith(".c") || file->FileName().endsWith(".cpp")) && file->NeedsRebuild( )){
			HelixWorker::getInstance().Add( new HelixCompileTask( folder, file ) );
			relinkRequired = true;
		}
	}

	// Check for a sqldo sub-folder
	HelixFSFolder sqldo = folder->FindFolder( "sqldo" );
	if(sqldo){
		for(auto file : sqldo->Files()){
			if( file->FileName().endsWith(".cpp") && file->NeedsRebuild( )){
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
			if( file->FileName().endsWith(".cpp") ){
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

	auto files = File::listFiles( physicalPath );
	for(auto const& file : files){
		if(file.endsWith( ".exe" ) ||
			file.endsWith( ".dll" ) ||
			file.endsWith( ".lib" ) ||
			file.endsWith( ".exp" ) ||
			file.endsWith( ".obj" ) ||
			file.endsWith( ".o" )
		){
			try {
				File::Delete( physicalPath + "/" + file );
			} catch (AnException& ){
				// Ignore this
			}
		}
	}
}

void HelixBuilder::BuildCS()
{
	EnEx ee(FL, "HelixBuilder::BuildCS()");

	if(HelixWorker::getInstance().HasError()){
		return; // Bail out early on any previous errors
	}

	INFO(FL, "Building HelixPdfGen" );
	twine pdfGen = "..\\c#\\HelixPdfGen";

	twine cmd;
	// Run the restore of all NuGet packages
	cmd = "cd " + pdfGen + " && c:\\software\\NuGet.exe restore HelixPdfGen.sln";
	if(std::system( cmd() ) != 0) throw AnException(0, FL, "C# - NuGet restore failed.");

	// Run the build of the project
	cmd = "cd " + pdfGen + " && msbuild HelixPdfGen.sln /p:Configuration=Release /p:Platform=\"Any CPU\" /t:Rebuild";
	if(std::system( cmd() ) != 0) throw AnException(0, FL, "C# - Build failed.");

	// Make sure we have the right output path created
	File::EnsurePath( "./HelixPdfGen/" );

	// Copy the release files over into our output folder
	for(auto& file : File::listFiles( pdfGen + "/HelixPdfGen/bin/Release" )){
		if(file.endsWith( ".dll" ) ||
			file.endsWith( ".exe" ) ||
			file.endsWith( ".xml" )
		){
			File::Copy( pdfGen + "/HelixPdfGen/bin/Release/" + file, "./HelixPdfGen/" + file );
		}
	}

}

void HelixBuilder::CleanCS()
{
	EnEx ee(FL, "HelixBuilder::CleanCS()");

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
				INFO(FL, "Removing file: %s", rmTarget() );
				File::Delete( rmTarget );
			}
		}
	}
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

		// What's the data object being generated?  Usually matches the name of the .sql.xml file, but double-check
		twine doName( file->DataObjectName() );
		
		// Check our sql.xml file against the sqldo/cpp file
		HelixFSFile sqldoFile = currentSqldoFolder->FindFile( doName + ".cpp" );
		if( file->IsNewerThan( sqldoFile ) ){
			DEBUG(FL, "Data Object: %s is newer than .cpp - triggering regeneration.", file->PhysicalFileName()() );
			HelixWorker::getInstance().Add( new HelixGenTask( currentSqldoFolder, file ) );
		} else if(forceRegen){
			DEBUG(FL, "Data Object: %s - forcing regeneration.", file->PhysicalFileName()() );
			HelixWorker::getInstance().Add( new HelixGenTask( currentSqldoFolder, file ) );
		}

	}

	// Update the HelixPdfGen.csproj file to include references to all data objects
	UpdateHelixPdfGenProj( allSqldo );	

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
	jsApiGen.Generate( "dev" );
	jsApiGen.Generate( "atm" );
	jsApiGen.Generate( "ttvx" );

}

void HelixBuilder::UpdateHelixPdfGenProj(vector<HelixFSFile>& allSqldo)
{
	EnEx ee(FL, "HelixBuilder::UpdateHelixPdfGenProj(vector<HelixFSFile>& allSqldo)");

	bool modified = false;
	twine csprojName; csprojName = "../c#/HelixPdfGen/HelixPdfGen/HelixPdfGen.csproj";
	sptr<xmlDoc, xmlFreeDoc> csproj = xmlParseFile( csprojName() );
	if(csproj == nullptr){
		throw AnException(0, FL, "Error opening HelixPdfGen.csproj for update");
	}
	xmlNodePtr root = xmlDocGetRootElement( csproj );
	for(auto item : XmlHelpers::FindChildren( root, "ItemGroup" )){
		// Check to see if we have compile items in this group:
		xmlNodePtr check = XmlHelpers::FindChild( item, "Compile" );
		if(check == nullptr) continue; // Find another ItemGroup

		// Found compiles - check to ensure all of our sqldo items are present
		// If not, add them.
		for(auto sqldo : allSqldo){
			twine includeName( "DO\\" + sqldo->LastFolderName() + "\\" + sqldo->DataObjectName() + ".cs" );
			xmlNodePtr csInclude = XmlHelpers::FindChildWithAttribute( item, "Compile", "Include", includeName() );
			if(csInclude == nullptr){
				// Add it in:
				csInclude = xmlNewChild( item, NULL, (const xmlChar*)"Compile", NULL);
				xmlSetProp( csInclude, (const xmlChar*)"Include", includeName );
				modified = true;
			}
		}
		break; // We found the ItemGroup with compiles in it - no need to keep searching the file
	}

	if(modified){
		// Save the updated file:
		xmlSaveFile( csprojName(), csproj );
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

		// What's the data object being generated?  Usually matches the name of the .sql.xml file, but double-check
		twine doName( file->DataObjectName() );
		
		// Remove the h and cpp file
		DEBUG(FL, "Removing Generated SqlDo File: %s.h and %s.cpp", doName(), doName() );
		currentSqldoFolder->DeleteFile( doName + ".h" );
		currentSqldoFolder->DeleteFile( doName + ".cpp" );
	}
}
