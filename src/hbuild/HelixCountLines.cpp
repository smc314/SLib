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
#include "HelixCountLines.h"
#include "HelixWorker.h"
using namespace Helix::Build;


HelixCountLines::HelixCountLines() 
{
	EnEx ee(FL, "HelixCountLines::HelixCountLines()");

}

HelixCountLines::HelixCountLines(const HelixCountLines& c)
{
	EnEx ee(FL, "HelixCountLines::HelixCountLines(const HelixCountLines& c)");

}

HelixCountLines& HelixCountLines::operator=(const HelixCountLines& c)
{
	EnEx ee(FL, "HelixCountLines::operator=(const HelixCountLines& c)");

	return *this;
}

HelixCountLines::HelixCountLines(const HelixCountLines&& c)
{
	EnEx ee(FL, "HelixCountLines::HelixCountLines(const HelixCountLines&& c)");

}

HelixCountLines& HelixCountLines::operator=(const HelixCountLines&& c)
{
	EnEx ee(FL, "HelixCountLines::operator=(const HelixCountLines&& c)");


	return *this;
}

HelixCountLines::~HelixCountLines()
{
	EnEx ee(FL, "HelixCountLines::~HelixCountLines()");

}

void HelixCountLines::Generate()
{
	EnEx ee(FL, "HelixCountLines::Generate");

	m_qdRoot = "../../qd";
	CountCPPAndH();
	CountSqlDo();
	CountJS();
	CountLayout();

	printf("Total Source Count: #Files = %d, #Lines = %d\n", 
		(int)m_overall_file_count, 
		(int)m_overall_line_count 
	);
	printf("Breakdown:\n");
	printf("C/C++ #Files = %d, #Lines = %d\n",
		(int)(m_cpp_file_count + m_c_file_count + m_h_file_count),
		(int)(m_cpp_line_count + m_c_line_count + m_h_line_count)
	);
	printf("SQL #Files = %d, #Lines = %d\n",
		(int)(m_sqldo_file_count),
		(int)(m_sqldo_line_count)
	);
	printf("UI JS #Files = %d, #Lines = %d\n",
		(int)(m_js_file_count),
		(int)(m_js_line_count)
	);
	printf("UI Layout #Files = %d, #Lines = %d\n",
		(int)(m_layout_file_count),
		(int)(m_layout_line_count)
	);
}

void HelixCountLines::CountCPPAndH()
{

	for(auto file : HelixFS::getInstance().FindFilesByType( ".cpp" ) ){
		if(!file->FromCore()){
			//WARN(FL, "Lines: %s", file->PhysicalFileName()() );
			m_cpp_line_count += file->Lines().size();
			m_cpp_file_count ++;
			m_overall_line_count += file->Lines().size();
			m_overall_file_count ++;
		}
	}

	for(auto file : HelixFS::getInstance().FindFilesByType( ".c" ) ){
		if(!file->FromCore()){
			//WARN(FL, "Lines: %s", file->PhysicalFileName()() );
			m_c_line_count += file->Lines().size();
			m_c_file_count ++;
			m_overall_line_count += file->Lines().size();
			m_overall_file_count ++;
		}
	}

	for(auto file : HelixFS::getInstance().FindFilesByType( ".h" ) ){
		if(!file->FromCore()){
			//WARN(FL, "Lines: %s", file->PhysicalFileName()() );
			m_h_line_count += file->Lines().size();
			m_h_file_count ++;
			m_overall_line_count += file->Lines().size();
			m_overall_file_count ++;
		}
	}

}

void HelixCountLines::CountSqlDo()
{
	for(auto file : HelixFS::getInstance().FindFilesByType( ".sql.xml" ) ){
		if(!file->FromCore()){
			//WARN(FL, "Lines: %s", file->PhysicalFileName()() );
			m_sqldo_line_count += file->Lines().size();
			m_sqldo_file_count ++;
			m_overall_line_count += file->Lines().size();
			m_overall_file_count ++;
		}
	}
}

void HelixCountLines::CountJS()
{
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		FindAllJSFiles( File::PathCombine(m_qdRoot, m_currentPackage) );
	}

}

void HelixCountLines::FindAllJSFiles( const twine& start )
{
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

void HelixCountLines::ProcessJSFile( const twine& fileName )
{
	auto pathElements = fileName.split("/");
	if(pathElements[4] != "source" || pathElements[5] != "class"){
		return; // skip this file
	}
	//WARN(FL, "Lines: %s", fileName() );
	File jsFile( fileName );
	auto lines = jsFile.readLines();
	m_js_line_count += lines.size();
	m_js_file_count ++;
	m_overall_line_count += lines.size();
	m_overall_file_count ++;
}

void HelixCountLines::CountLayout()
{
	// Walk the layout files in all of the correct qd app folders
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		FindAllLayoutFiles( );
	}
}

void HelixCountLines::FindAllLayoutFiles()
{
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
			m_layout_line_count += lines.size();
			m_layout_file_count ++;
			m_overall_line_count += lines.size();
			m_overall_file_count ++;
		}
	}

	// Layout files are all at the top level.  Don't recurse
}
