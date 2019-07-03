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
#include "HelixFind.h"
#include "HelixWorker.h"
using namespace Helix::Build;


HelixFind::HelixFind() 
{
	EnEx ee(FL, "HelixFind::HelixFind()");

}

HelixFind::HelixFind(const HelixFind& c)
{
	EnEx ee(FL, "HelixFind::HelixFind(const HelixFind& c)");

}

HelixFind& HelixFind::operator=(const HelixFind& c)
{
	EnEx ee(FL, "HelixFind::operator=(const HelixFind& c)");

	return *this;
}

HelixFind::HelixFind(const HelixFind&& c)
{
	EnEx ee(FL, "HelixFind::HelixFind(const HelixFind&& c)");

}

HelixFind& HelixFind::operator=(const HelixFind&& c)
{
	EnEx ee(FL, "HelixFind::operator=(const HelixFind&& c)");


	return *this;
}

HelixFind::~HelixFind()
{
	EnEx ee(FL, "HelixFind::~HelixFind()");

}

void HelixFind::Generate( const twine& searchString )
{
	EnEx ee(FL, "HelixFind::Generate( const twine& searchString )");

	m_qdRoot = "../../qd";
	m_searchString = searchString;

	WARN(FL, "Searching for \"%s\" in all source code", m_searchString() );

	FindInCPPAndH();
	FindInSqlDo();
	FindInJS();
	FindInLayout();

}

void HelixFind::FindInCPPAndH()
{
	EnEx ee(FL, "HelixFind::FindInCPPAndH()");

	for(auto file : HelixFS::getInstance().FindFilesByType( ".cpp" ) ){
		FindInLines( file->PhysicalFileName(), file->Lines() );
	}

	for(auto file : HelixFS::getInstance().FindFilesByType( ".c" ) ){
		FindInLines( file->PhysicalFileName(), file->Lines() );
	}

	for(auto file : HelixFS::getInstance().FindFilesByType( ".h" ) ){
		FindInLines( file->PhysicalFileName(), file->Lines() );
	}

}

void HelixFind::FindInSqlDo()
{
	EnEx ee(FL, "HelixFind::FindInSqlDo()");
	for(auto file : HelixFS::getInstance().FindFilesByType( ".sql.xml" ) ){
		FindInLines( file->PhysicalFileName(), file->Lines() );
	}
}

void HelixFind::FindInJS()
{
	EnEx ee(FL, "HelixFind::FindInJS()");
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		FindAllJSFiles( File::PathCombine(m_qdRoot, m_currentPackage) );
	}

}

void HelixFind::FindAllJSFiles( const twine& start )
{
	EnEx ee(FL, "HelixFind::FindAllJSFiles( const twine& start )");

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

void HelixFind::ProcessJSFile( const twine& fileName )
{
	EnEx ee(FL, "HelixFind::ProcessJSFile( const twine& fileName )");
	auto pathElements = fileName.split("/");
	if(pathElements[4] != "source" || pathElements[5] != "class"){
		return; // skip this file
	}
	//WARN(FL, "Lines: %s", fileName() );
	File jsFile( fileName );
	auto lines = jsFile.readLines();
	FindInLines( fileName, lines );
}

void HelixFind::FindInLayout()
{
	EnEx ee(FL, "HelixFind::FindInLayout()");
	// Walk the layout files in all of the correct qd app folders
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		FindAllLayoutFiles( );
	}
}

void HelixFind::FindAllLayoutFiles()
{
	EnEx ee(FL, "HelixFind::FindAllLayoutFiles()");

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

void HelixFind::FindInLines(const twine& fileName, const vector<twine>& lines)
{
	EnEx ee(FL, "HelixFind::FindInLines()");
	bool found = false;
	for(size_t i = 0; i < lines.size(); i++){
		if(lines[i].size() != 0 && lines[i].cifind( m_searchString ) != TWINE_NOT_FOUND){
			found = true;
			printf("%.4d: %s\n", (int)i, lines[i]() );
		}
	}
	if(found){
		printf("%s\n\n", fileName() );
	}
}
