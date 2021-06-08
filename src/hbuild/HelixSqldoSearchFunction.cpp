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

#include "HelixSqldoSearchFunction.h"
#include "HelixSqldoMethod.h"
#include "HelixConfig.h"
using namespace Helix::Build;


HelixSqldoSearchFunction::HelixSqldoSearchFunction() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoSearchFunction::HelixSqldoSearchFunction(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	for(auto n : XmlHelpers::FindChildren( elem, "SearchField" ) ){
		fields.push_back( HelixSqldoSearchField( n ) );
	}
}

twine HelixSqldoSearchFunction::GenCPPHeader(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPHeader(const twine& className)");

	return	
		"\t\t/** This is used to return a list of search fields that " + className + " exposes in a standard way.\n"
		"\t\t  */\n"
		"\t\tstatic SearchField_svect " + name + "();\n"
		"\n"
	;

}

twine HelixSqldoSearchFunction::GenCPPBody(const twine& className, map<twine, HelixSqldoParam>& allParams) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className, map<twine, HelixSqldoParam>& allParams)");

	twine ret;

	ret.append(
		"SearchField_svect " + className  + "::" + name + "()\n"
		"{\n"
		"\tSearchField_svect ret( new vector<SearchField*>() );\n"
		"\n"
	);

	for(auto& sf : fields){
		ret.append(
			"\tSearchField* sf_" + sf.name + " = new SearchField();\n"
			"\tsf_" + sf.name + "->name = \"" + sf.name + "\";\n"
			"\tsf_" + sf.name + "->sql = \"" + sf.sql + "\";\n"
			"\tsf_" + sf.name + "->subSql = \"" + HelixSqldoMethod::FlattenSql( sf.subSql ) + "\";\n"
			"\tsf_" + sf.name + "->type = \"" + sf.type + "\";\n"
			"\tsf_" + sf.name + "->alias1 = \"" + sf.alias1 + "\";\n"
			"\tsf_" + sf.name + "->alias2 = \"" + sf.alias2 + "\";\n"
			"\tsf_" + sf.name + "->alias3 = \"" + sf.alias3 + "\";\n"
			"\tsf_" + sf.name + "->alias4 = \"" + sf.alias4 + "\";\n"
			"\tsf_" + sf.name + "->alias5 = \"" + sf.alias5 + "\";\n"
			"\tret->push_back( sf_" + sf.name + " );\n"
			"\n"
		);
	}

	ret.append(
		"\t// When we return, the unique_ptr semantics make this work correctly\n"
		"\treturn ret;\n"
		"\n"
		"}\n"
		"\n"
	);
	return ret;
}

twine HelixSqldoSearchFunction::GenJSBody(const twine& className, const twine& app) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className)");

	twine ret;

	ret.append(
		"\t\t/** This will return a list of search fields defined by this class.\n"
		"\t\t  */\n"
		"\t\t" + name + " : function (){\n"
		"\t\t\tvar ret = [];\n"
		"\n"
	);

	twine finalApp( app );
	if(HelixConfig::getInstance().UseCore()){
		if( app == "atm" || app == "sspm" ){
			finalApp = "corem";
		} else {
			finalApp = "cored";
		}
	}

	for(auto& sf : fields){
		ret.append(
			"\t\t\tvar sf_" + sf.name + " = new " + finalApp + ".util.sqldo.SearchField();\n"
			"\t\t\tsf_" + sf.name + ".setName( \"" + sf.name + "\" );\n"
			"\t\t\tsf_" + sf.name + ".setSql( \"" + sf.sql + "\") ;\n"
			"\t\t\tsf_" + sf.name + ".setType( \"" + sf.type + "\") ;\n"
			"\t\t\tsf_" + sf.name + ".setAlias1( \"" + sf.alias1 + "\") ;\n"
			"\t\t\tsf_" + sf.name + ".setAlias2( \"" + sf.alias2 + "\") ;\n"
			"\t\t\tsf_" + sf.name + ".setAlias3( \"" + sf.alias3 + "\") ;\n"
			"\t\t\tsf_" + sf.name + ".setAlias4( \"" + sf.alias4 + "\") ;\n"
			"\t\t\tsf_" + sf.name + ".setAlias5( \"" + sf.alias5 + "\") ;\n"
			"\t\t\tret.push( sf_" + sf.name + " );\n"
			"\n"
		);
	}

	ret.append(
		"\t\t\treturn ret;\n"
		"\n"
		"\t\t},\n"
		"\n"
	);
	return ret;
}

