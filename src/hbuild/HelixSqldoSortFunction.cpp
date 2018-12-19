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

#include "HelixSqldoSortFunction.h"
using namespace Helix::Build;


HelixSqldoSortFunction::HelixSqldoSortFunction() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoSortFunction::HelixSqldoSortFunction(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	for(auto n : XmlHelpers::FindChildren( elem, "Field" ) ){
		fields.push_back( HelixSqldoParam( n ) );
	}
}

twine HelixSqldoSortFunction::GenCPPHeader(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPHeader(const twine& className)");

	return 
		"\t\t/** This is used to compare instances of " + className + " in a standard way.\n"
		"\t\t  * This works with std::sort, std::lower_bound, and the other standard library functions.\n"
		"\t\t  */\n"
		"\t\tstatic bool " + name + "(" + className + "* a, " + className + "* b );\n"
		"\n"
	;
}

twine HelixSqldoSortFunction::GenCPPBody(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className)");

	twine ret;

	int tabLevel = 1;

	ret.append(
		"bool " + className + "::" + name + "(" + className + "* a, " + className + "* b )\n"
		"{\n"
	);
	twine tabs;
	for(auto& sortField : fields){
		tabs.erase(); for(int i = 0; i < tabLevel; i++) tabs.append("\t");
		if(sortField.desc){
			ret.append(
				tabs + "if( a->" + sortField.name + " > b->" + sortField.name + "){\n" +
				tabs + "\treturn true; // Strictly greater-than returns true\n" +
				tabs + "} else if( a->" + sortField.name + " < b->" + sortField.name + "){\n" +
				tabs + "\t return false; // Strictly less-than retuns false\n" +
				tabs + "} else {\n"
			);
		} else {
			ret.append(
				tabs + "if( a->" + sortField.name + " < b->" + sortField.name + "){\n" +
				tabs + "\treturn true; // Strictly less-than returns true\n" +
				tabs + "} else if( a->" + sortField.name + " > b->" + sortField.name + "){\n" +
				tabs + "\t return false; // Strictly greater-than retuns false\n" +
				tabs + "} else {\n"
			);
		}
		tabLevel ++;
	}
	ret.append(
		tabs + "\treturn false; // Equals returns false\n"
	);
	while(--tabLevel >= 1){
		tabs.erase(); for(int i = 0; i < tabLevel; i++) tabs.append("\t");
		ret.append(
			tabs + "}\n"
		);
	}
	ret.append(
		"\n"
		"}\n"
		"\n"
	);

	return ret;
}
