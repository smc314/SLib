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

#include "HelixSqldoMapFunction.h"
using namespace Helix::Build;


HelixSqldoMapFunction::HelixSqldoMapFunction() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoMapFunction::HelixSqldoMapFunction(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	for(auto n : XmlHelpers::FindChildren( elem, "Field" ) ){
		fields.push_back( HelixSqldoParam( n ) );
	}
}

twine HelixSqldoMapFunction::GenCPPHeader(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPHeader(const twine& className)");

	return	
		"\t\t/** This is used to produce a string map of " + className + " in a standard way.\n"
		"\t\t  */\n"
		"\t\tstd::map<twine, twine> " + name + "();\n"
		"\n"
		"\t\t/** This is used to add to a string map of " + className + " in a standard way.\n"
		"\t\t  */\n"
		"\t\tvoid " + name + "(std::map<twine, twine>& map);\n"
		"\n"
	;

}

twine HelixSqldoMapFunction::GenCPPBody(const twine& className, map<twine, HelixSqldoParam>& allParams) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className, map<twine, HelixSqldoParam>& allParams)");

	twine ret;

	ret.append(
		"std::map<twine, twine> " + className  + "::" + name + "()\n"
		"{\n"
		"\tstd::map< twine, twine > ret;\n"
		"\n"
		"\t// Call the version that adds to the given map to fill it out.\n"
		"\t" + name + "( ret );\n"
		"\n"
		"\treturn ret;\n"
		"\n"
		"}\n"
		"\n"
		"void " + className  + "::" + name + "(std::map<twine, twine>& map)\n"
		"{\n"
		"\ttwine tmp;\n"
		"\n"
	);

	for(auto& p : allParams){
		if(p.second.type == "twine"){
			ret.append("\tmap[ \"" + className + "." + p.second.name + "\" ] = " + p.second.name + ";\n" );
		} else if(p.second.type == "int" || p.second.type == "autogen"){
			ret.append("\ttmp = (intptr_t)" + p.second.name + "; map[ \"" + className + "." + p.second.name + "\" ] = tmp;\n" );
		} else if(p.second.type == "long"){
			ret.append("\ttmp = (intptr_t)" + p.second.name + "; map[ \"" + className + "." + p.second.name + "\" ] = tmp;\n" );
		} else if(p.second.type == "float"){
			ret.append("\ttmp = (float)" + p.second.name + "; map[ \"" + className + "." + p.second.name + "\" ] = tmp;\n" );
		} else if(p.second.type == "bool"){
			ret.append("\ttmp = (intptr_t)" + p.second.name + "; map[ \"" + className + "." + p.second.name + "\" ] = tmp;\n" );
		} else if(p.second.type == "cdata"){
			ret.append("\tmap[ \"" + className + "." + p.second.name + "\" ] = " + p.second.name + ";\n" );
		} else if(p.second.type == "base64"){
			ret.append("\tmap[ \"" + className + "." + p.second.name + "\" ] = " + p.second.name + ";\n" );
		} else if(p.second.type == "guid" || p.second.type == "Guid" ){
			ret.append("\tmap[ \"" + className + "." + p.second.name + "\" ] = " + p.second.name + ";\n" );
		} else if(p.second.type == "bin"){
			ret.append("\t// " + p.second.name + " is of type bin - not added to map\n" );
		} else if(p.second.type == "timestamp" || p.second.type == "Timestamp" || p.second.type == "Date" || p.second.type == "DateTime"){
			ret.append("\t" + p.second.name + ".PrepDayW(); // Do this to make sure Day of Week is correct.\n" );
			ret.append("\tmap[ \"" + className + "." + p.second.name + "\" ] = " + p.second.name + ".GetValue();\n" );
			ret.append("\tmap[ \"" + className + "." + p.second.name + "_DATE\" ] = " + p.second.name + ".GetValue(\"%m/%d/%Y\");\n" );
			ret.append("\tmap[ \"" + className + "." + p.second.name + "_DATE2\" ] = " + p.second.name + ".GetValue(\"%Y-%m-%d\");\n" );
			ret.append("\tmap[ \"" + className + "." + p.second.name + "_EXPANDED\" ] = " + p.second.name + ".GetValue(\"%A, %B %d, %Y\");\n" );
			ret.append("\tmap[ \"" + className + "." + p.second.name + "_TIME\" ] = " + p.second.name + ".GetValue(\"%H:%M:%S\");\n" );
			ret.append("\tmap[ \"" + className + "." + p.second.name + "_TIME_AMPM\" ] = " + p.second.name + ".GetValue(\"%I:%M:%S %p\");\n" );
		}
	}

	ret.append(
		"\n"
		"}\n"
		"\n"
	);
	return ret;
}

