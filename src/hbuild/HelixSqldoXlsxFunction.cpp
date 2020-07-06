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

#include "HelixSqldoXlsxFunction.h"
using namespace Helix::Build;


HelixSqldoXlsxFunction::HelixSqldoXlsxFunction() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoXlsxFunction::HelixSqldoXlsxFunction(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	for(auto n : XmlHelpers::FindChildren( elem, "Field" ) ){
		fields.push_back( HelixSqldoParam( n ) );
	}
}

twine HelixSqldoXlsxFunction::GenCPPHeader(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPHeader(const twine& className)");

	if(fields.size() == 0){
		return "";
	}

	return 
		"\t\t/** This is used to create Excel documents from vectors of " + className + " in a standard way.\n"
		"\t\t  * This produces an object that will create Excel xlsx format files.  The caller must take ownership\n"
		"\t\t  * of the returned object and ensure its deletion.\n"
		"\t\t  */\n"
		"\t\tstatic Xlsx* " + name + "( const twine& outputFileName, const twine& firstSheetName, vector<" + className + "*>* dv );\n"
		"\n"
	;
}

twine HelixSqldoXlsxFunction::GenCPPBody(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className)");

	if(fields.size() == 0){
		return "";
	}

	twine ret;

	int tabLevel = 1;

	ret.append(
		"Xlsx* " + className + "::" + name + "(const twine& outputFileName, const twine& firstSheetName, vector<" + className + "*>* dv )\n"
		"{\n"
		"\tEnEx ee(FL, \"" + className + "::" + name + "(const twine& outputFileName, const twine& firstSheetName, vector<" + className + "*>* dv)\");\n"
		"\n"
		"\t// Set up our Xlsx object with the given file name, and first worksheet name\n"
		"\tdptr<Xlsx> ret( new Xlsx( outputFileName ) );\n"
		"\tret->AddSheet( firstSheetName );\n"
		"\n"
		"\t// Write all of the header columns to the spreadsheet - these are always in bold\n"
		"\tauto bold_style = ret->LookupStyle( XLSX_STYLE_BOLD );\n"
	);

	int col_i = 0;
	for(auto& field : fields){
		twine tmp; 
		tmp.format( "\tret->Set( 0, %d, \"%s\", bold_style );\n", col_i++, field.header() );
		ret.append( tmp );
	}

	ret.append(
		"\n"
		"\t// Look up all of the required styles here so we only pay this cost once\n"
	);
	for(auto& field : fields){
		if(field.format.empty()) continue; // Only fields with format specifiers
		twine tmp;
		tmp.format( "\tauto %s_format = ret->LookupStyle( \"%s\" );\n", field.name(), field.format() );
		ret.append( tmp );
	}

	ret.append(
		"\n"
		"\t// Write the data array into the Xlsx object in the current sheet\n"
		"\tint row_i = 1;\n"
		"\tfor(auto obj : *dv){\n"
	);	
	col_i = 0;
	for(auto& field : fields){
		twine tmp;
		tmp.format( "\t\tret->Set( row_i, %d, obj->%s", col_i++, field.name() );
		if(field.format.empty() == false){
			tmp.append( ", " ).append( field.name ).append( "_format" );
		}
		tmp.append( " );\n" );
		ret.append(tmp);
	}
	ret.append(
		"\t\trow_i ++;\n"
		"\t}\n"
		"\n"
		"\treturn ret.release(); // Caller must take ownership of this memory\n"
		"}\n"
		"\n"
	);

	return ret;
}
