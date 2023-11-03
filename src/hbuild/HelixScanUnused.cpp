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
#include "HelixScanUnused.h"
#include "HelixWorker.h"
#include "HelixSqldo.h"
using namespace Helix::Build;


HelixScanUnused::HelixScanUnused() 
{
	EnEx ee(FL, "HelixScanUnused::HelixScanUnused()");

}

HelixScanUnused::HelixScanUnused(const HelixScanUnused& c)
{
	EnEx ee(FL, "HelixScanUnused::HelixScanUnused(const HelixScanUnused& c)");

}

HelixScanUnused& HelixScanUnused::operator=(const HelixScanUnused& c)
{
	EnEx ee(FL, "HelixScanUnused::operator=(const HelixScanUnused& c)");

	return *this;
}

HelixScanUnused::HelixScanUnused(const HelixScanUnused&& c)
{
	EnEx ee(FL, "HelixScanUnused::HelixScanUnused(const HelixScanUnused&& c)");

}

HelixScanUnused& HelixScanUnused::operator=(const HelixScanUnused&& c)
{
	EnEx ee(FL, "HelixScanUnused::operator=(const HelixScanUnused&& c)");


	return *this;
}

HelixScanUnused::~HelixScanUnused()
{
	EnEx ee(FL, "HelixScanUnused::~HelixScanUnused()");

}

void HelixScanUnused::Generate( const twine& logic, const twine& forDO, bool onlyShowUnused )
{
	EnEx ee(FL, "HelixScanUnused::Generate( const twine& logic, const twine& forDO, bool onlyShowUnused )");

	m_qdRoot = "../../qd";
	m_targetLogic = logic;
	m_targetDO = forDO;

	WARN(FL, "Searching \"%s:%s\" for unused methods", m_targetLogic(), m_targetDO() );

	auto allSqldo = HelixFS::getInstance().FindFilesByType( ".sql.xml" );
	for(auto file : allSqldo){
		twine folderName = file->FolderName();
		if(m_targetLogic != "all"){
			// If we are filtering to a certain logic folder only - make sure we match it here
			if(!folderName.endsWith( m_targetLogic )){
				DEBUG(FL, "Skipping %s", file->PhysicalFileName()() );
				continue;
			}
		}
		if(m_targetDO != "all"){
			if(file->DataObjectName() != m_targetDO){
				DEBUG(FL, "Skipping %s", file->PhysicalFileName()() );
				continue;
			}
		}
		if(onlyShowUnused == false){
			WARN(FL, "Processing %s", file->PhysicalFileName()() );
		}

		HelixFSFolder* currentFolder = HelixFS::getInstance().FindPath( file->FolderName() );
		if(!currentFolder){
			throw AnException(0, FL, "Unable to find folder: %s\n", file->FolderName()() );
		}
		HelixSqldo currentSqldo( currentFolder, file );
		auto methods = currentSqldo.Methods();
		for(auto method : methods){
			if(method.name.startsWith( "pagingSql" ) ||
				method.name.startsWith( "dummy" ) ||
				method.name.startsWith( "Dummy" ) ||
				method.name.startsWith( "insertWithId" )
			){
				continue; // Skip these methods
			}
			m_searchString.format( "%s::%s", file->DataObjectName()(), method.name() );
			m_foundMethodUse = 0;
			FindInCPPAndH();
			if(onlyShowUnused){
				if(m_foundMethodUse == 0){
					WARN(FL, "    %s (%d) references", m_searchString(), m_foundMethodUse );
				}
			} else {
				WARN(FL, "    %s (%d) references", m_searchString(), m_foundMethodUse );
			}

		}
	}


	/*
	FindInCPPAndH();
	FindInSqlDo();
	FindInJS();
	FindInLayout();
	*/

}

void HelixScanUnused::FindInCPPAndH()
{
	EnEx ee(FL, "HelixScanUnused::FindInCPPAndH()");

	for(auto file : HelixFS::getInstance().FindFilesByType( ".cpp" ) ){
		if(file->FolderName().endsWith( "sqldo" )){
			continue; // Skip searching in sqldo generated folders
		}
		FindInLines( file->PhysicalFileName(), file->Lines() );
	}

	for(auto file : HelixFS::getInstance().FindFilesByType( ".c" ) ){
		if(file->FolderName().endsWith( "sqldo" )){
			continue; // Skip searching in sqldo generated folders
		}
		FindInLines( file->PhysicalFileName(), file->Lines() );
	}

	for(auto file : HelixFS::getInstance().FindFilesByType( ".h" ) ){
		if(file->FolderName().endsWith( "sqldo" )){
			continue; // Skip searching in sqldo generated folders
		}
		FindInLines( file->PhysicalFileName(), file->Lines() );
	}
}

void HelixScanUnused::FindInSqlDo()
{
	EnEx ee(FL, "HelixScanUnused::FindInSqlDo()");
	for(auto file : HelixFS::getInstance().FindFilesByType( ".sql.xml" ) ){
		FindInLines( file->PhysicalFileName(), file->Lines() );
	}
}

void HelixScanUnused::FindInJS()
{
	EnEx ee(FL, "HelixScanUnused::FindInJS()");
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		FindAllJSFiles( File::PathCombine(m_qdRoot, m_currentPackage) );
	}

}

void HelixScanUnused::FindAllJSFiles( const twine& start )
{
	EnEx ee(FL, "HelixScanUnused::FindAllJSFiles( const twine& start )");

	// check all of the normal files
	auto files = File::listFiles(start);
	for(auto file : files){
		twine child( File::PathCombine(start, file) );
		if(child.endsWith( twine(".js") )){
			ProcessJSFile( child );
		}
	}

	// Then recurse through all of the subdirectories
	auto folders = File::listFolders( start );
	for(auto folder : folders){
		if(folder != "." && folder != ".."){
			FindAllJSFiles( File::PathCombine( start, folder ) );
		}
	}
}

void HelixScanUnused::ProcessJSFile( const twine& fileName )
{
	EnEx ee(FL, "HelixScanUnused::ProcessJSFile( const twine& fileName )");
	auto pathElements = fileName.split("/");
	if(pathElements[4] != "source" || pathElements[5] != "class"){
		return; // skip this file
	}
	//WARN(FL, "Lines: %s", fileName() );
	File jsFile( fileName );
	auto lines = jsFile.readLines();
	FindInLines( fileName, lines );
}

void HelixScanUnused::FindInLayout()
{
	EnEx ee(FL, "HelixScanUnused::FindInLayout()");
	// Walk the layout files in all of the correct qd app folders
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		FindAllLayoutFiles( );
	}
}

void HelixScanUnused::FindAllLayoutFiles()
{
	EnEx ee(FL, "HelixScanUnused::FindAllLayoutFiles()");

	// Layout folder starts at QD Root + Package + source/layouts
	twine start; start.format("%s/%s/source/layouts", m_qdRoot(), m_currentPackage() );
	
	// check all of the normal files:
	vector<twine> files;
	try {
		files = File::listFiles(start);
	} catch (AnException& e){
		// Exceptions here simply mean the folder was empty
	}
	for(auto file : files){
		twine child( File::PathCombine(start, file) );
		if(child.endsWith(twine(".xml"))){
			//WARN(FL, "Lines: %s", child() );
			File layoutFile( child );
			auto lines = layoutFile.readLines();
			FindInLines( child, lines );
		}
	}

	// Layout files are all at the top level.  Don't recurse
}

void HelixScanUnused::FindInLines(const twine& fileName, const vector<twine>& lines)
{
	EnEx ee(FL, "HelixScanUnused::FindInLines()");
	for(size_t i = 0; i < lines.size(); i++){
		if(lines[i].size() != 0 && lines[i].cifind( m_searchString ) != TWINE_NOT_FOUND){
			m_foundMethodUse++;
			//printf("%.4d: %s\n", (int)i, lines[i]() );
		}
	}
}
