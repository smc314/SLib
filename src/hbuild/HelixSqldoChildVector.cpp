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

#include "HelixSqldoChildVector.h"
#include "HelixConfig.h"
using namespace Helix::Build;


HelixSqldoChildVector::HelixSqldoChildVector() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoChildVector::HelixSqldoChildVector(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	type.getAttribute( elem, "type" );
}

twine HelixSqldoChildVector::SimpleType() 
{
	vector<twine> splits = type.split( "::" );
	if(splits.size() > 1){
		return splits[splits.size() - 1];
	} else {
		return type;
	}
}

twine HelixSqldoChildVector::SimplePackage() 
{
	vector<twine> splits = type.split( "::" );
	if(splits.size() > 1){
		return splits[splits.size() - 2];
	} else {
		return "${shortPackage}";
	}
}

twine HelixSqldoChildVector::IncludeLine() 
{
	if(type.find("::") != TWINE_NOT_FOUND){
		return "#include \"" + SimpleType() + ".h\" // " + type + " \n";
	} else {
		return "#include \"" + SimpleType() + ".h\"\n";
	}
}

twine HelixSqldoChildVector::CPPCopy() 
{
	return "\t" + CPPInit() + 
		"\tfor( auto child : *(c." + name + ") ){\n"
		"\t\t" + name + "->push_back( new " + type + "( *child ) ); // Make a copy\n"
		"\t}\n";
}

twine HelixSqldoChildVector::CPPInit() 
{
	return name + " = " + type + "_svect( new vector< " + type + "* >() );\n";
}

twine HelixSqldoChildVector::CPPXmlGet() 
{
	return "\txmlNodePtr " + name + "_node = XmlHelpers::FindChild( node, \"" + name + "\");\n"
		"\tif( " + name + "_node != nullptr ){\n"
		"\t\t" + name + " = " + type + "::readXmlChildren( " + name + "_node );\n"
		"\t}\n";
}

twine HelixSqldoChildVector::CPPXmlSet() 
{
	return "\txmlNodePtr " + name + "_node = xmlNewChild( child, NULL, (const xmlChar*)\"" + name + "\", NULL);\n"
		"\t" + type + "::createXmlChildren( " + name + "_node, " + name + ".get() );\n"
	;
}

twine HelixSqldoChildVector::CPPJsonGet() 
{
	return "\tcJSON* " + name + "_array = cJSON_GetObjectItem( jsonObj, \"" + name + "\");\n"
		"\tif( " + name + "_array != nullptr ){\n"
		"\t\t" + name + " = " + type + "::readJSONChildren( " + name + "_array );\n"
		"\t}\n";
}

twine HelixSqldoChildVector::CPPJsonSet() 
{
	return "\t" + type + "::createJSONArray( child, \"" + name + "\", " + name + ".get() );\n"
	;
}

twine HelixSqldoChildVector::CSXmlGet() 
{
	return "\t\t\tXmlElement " + name + "_node = XmlHelpers.FindChild( node, \"" + name + "\");\n"
		"\t\t\tif( " + name + "_node != null ){\n"
		"\t\t\t\t" + name + " = " + SimpleType() + ".readXmlChildren( " + name + "_node );\n"
		"\t\t\t}\n";
}

twine HelixSqldoChildVector::CSXmlSet() 
{
	return 
		"\t\t\tXmlElement " + name + "_node = child.OwnerDocument.CreateElement( \"" + name + "\");\n"
		"\t\t\tchild.AppendChild( " + name + "_node);\n"
		"\t\t\t" + SimpleType() + ".createXmlChildren( " + name + "_node, " + name + " );\n"
	;
}

twine HelixSqldoChildVector::JSInit(const twine& app) 
{
	return "\t\tthis." + name + " = []; // Child array of type " + type + "\n";
}

twine HelixSqldoChildVector::JSDispose(const twine& app) 
{
	return "\t\tthis._disposeArray(\"" + name + "\");\n";
}

twine HelixSqldoChildVector::JSPropDef(const twine& app) 
{
	return 
		"\t\t/** This is an array of child data objects of type " + type + ".\n"
		"\t\t  */\n"
		"\t\t" + name + " : [],\n"
		"\n"
	;
}

twine HelixSqldoChildVector::JSXmlGet(const twine& app) 
{
	twine simplePackage( SimplePackage() );
	twine finalApp( app );
	if(HelixConfig::getInstance().UseCore()){
		if(simplePackage == "admin" || simplePackage == "util"){
			if(app == "atm" || app == "sspm"){
				finalApp = "corem";
			} else {
				finalApp = "cored"; // Use admin and util classes from cored
			}
		}
	}
	return "\t\t\tvar " + name + "_subElem = " + app + ".Statics.xmlFindChild( elem, \"" + name + "\");\n"
		"\t\t\tif( " + name + "_subElem ){\n"
		"\t\t\t\tthis." + name + " = " + finalApp + "." + SimplePackage() + ".sqldo." + SimpleType() + ".readElementChildren( " + name + "_subElem );\n"
		"\t\t\t}\n"
	;
}

twine HelixSqldoChildVector::JSXmlSet(const twine& app) 
{
	twine simplePackage( SimplePackage() );
	twine finalApp( app );
	if(HelixConfig::getInstance().UseCore()){
		if(simplePackage == "admin" || simplePackage == "util"){
			if(app == "atm" || app == "sspm"){
				finalApp = "corem";
			} else {
				finalApp = "cored"; // Use admin and util classes from cored
			}
		}
	}
	return 
		"\t\t\tvar " + name + "_subElem = doc.createElement( \"" + name + "\");\n"
		"\t\t\tsubElem.appendChild( " + name + "_subElem);\n"
		"\t\t\t" + finalApp + "." + SimplePackage() + ".sqldo." + SimpleType() + ".writeArray( this." + name + ", " + name + "_subElem );\n"
	;
}

twine HelixSqldoChildVector::JSClone(const twine& app) 
{
	twine simplePackage( SimplePackage() );
	twine finalApp( app );
	if(HelixConfig::getInstance().UseCore()){
		if(simplePackage == "admin" || simplePackage == "util"){
			if(app == "atm" || app == "sspm"){
				finalApp = "corem";
			} else {
				finalApp = "cored"; // Use admin and util classes from cored
			}
		}
	}
	return 
		"\t\t\tnewObj." + name + " = " + finalApp + "." + SimplePackage() + ".sqldo." + SimpleType() + ".cloneArray( this." + name + " );\n"
	;
}


