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

#include "HelixSqldoMatchFunction.h"
using namespace Helix::Build;


HelixSqldoMatchFunction::HelixSqldoMatchFunction() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoMatchFunction::HelixSqldoMatchFunction(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	for(auto n : XmlHelpers::FindChildren( elem, "Field" ) ){
		fields.push_back( HelixSqldoParam( n ) );
	}
}

twine HelixSqldoMatchFunction::GenCPPHeader(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPHeader(const twine& className)");

	return	
		"\t\t/** This is used to compare instances of " + className + " in a standard way.\n"
		"\t\t  * This works with FilterDataVector from Statics.h\n"
		"\t\t  */\n"
		"\t\tstatic bool " + name + "(" + className + "* a, const twine& searchString);\n"
		"\n"
	;

}

twine HelixSqldoMatchFunction::GenCPPBody(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className)");

	twine ret;

	ret.append(
		"bool " + className  + "::" + name + "(" + className  + "* a, const twine& searchString)\n"
		"{\n"
		"\ttwine ucaseField;\n"
		"\n"
	);
	for(auto& sortField : fields){
		ret.append(
			"\t// Match on field " + sortField.name + "\n"
			"\tucaseField = a->" + sortField.name + ";\n"
			"\tucaseField.ucase();\n"
		);
		if( sortField.type == "contains"){
			ret.append(
				"\tif(ucaseField.find( searchString ) != TWINE_NOT_FOUND) return true;\n"
				"\n"
			);
		} else if(sortField.type == "starts"){
			ret.append(
				"\tif(ucaseField.startsWith( searchString)) return true;\n"
				"\n"
			);
		} else if(sortField.type == "ends"){
			ret.append(
				"\tif(ucaseField.endsWith( searchString)) return true;\n"
				"\n"
			);
		}
	}
	ret.append(
		"\treturn false; // If we get to the end, nothing matched - return false for no match.\n"
		"}\n"
		"\n"
	);
	return ret;
}

