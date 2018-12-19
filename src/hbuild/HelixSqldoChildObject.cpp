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

#include "HelixSqldoChildObject.h"
using namespace Helix::Build;


HelixSqldoChildObject::HelixSqldoChildObject() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoChildObject::HelixSqldoChildObject(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	type.getAttribute( elem, "type" );
}

twine HelixSqldoChildObject::SimpleType() 
{
	vector<twine> splits = type.split( "::" );
	if(splits.size() > 1){
		return splits[splits.size() - 1];
	} else {
		return type;
	}
}

twine HelixSqldoChildObject::SimplePackage() 
{
	vector<twine> splits = type.split( "::" );
	if(splits.size() > 1){
		return splits[splits.size() - 2];
	} else {
		return "${shortPackage}";
	}
}

twine HelixSqldoChildObject::IncludeLine() 
{
	if(type.find("::") != TWINE_NOT_FOUND){
		return "#include \"" + SimpleType() + ".h\" // " + type + " \n";
	} else {
		return "#include \"" + SimpleType() + ".h\"\n";
	}
}

twine HelixSqldoChildObject::CPPXmlGet() 
{
	return "\txmlNodePtr " + name + "_node = XmlHelpers::FindChild( node, \"" + name + "\");\n"
		"\tif( " + name + "_node != nullptr ){\n"
		"\t\t" + name + ".readXmlNode( " + name + "_node );\n"
		"\t}\n";
}

twine HelixSqldoChildObject::CPPXmlSet() 
{
	return 
		"\txmlNodePtr " + name + "_node = " + name + ".createXmlNode( child );\n"
		"\txmlNodeSetName(" + name + "_node, (const xmlChar*)\"" + name + "\" );\n"
	;
}

twine HelixSqldoChildObject::CPPJsonGet() 
{
	return "\tcJSON* " + name + "_obj = cJSON_GetObjectItem( jsonObj, \"" + name + "\");\n"
		"\tif( " + name + "_obj != nullptr ){\n"
		"\t\t" + name + ".readJSON( " + name + "_obj );\n"
		"\t}\n";
}

twine HelixSqldoChildObject::CPPJsonSet() 
{
	return 
		"\tcJSON* " + name + "_obj = " + name + ".createJSON();\n"
		"\tcJSON_AddItemToObject(child, \"" + name + "\", " + name + "_obj);\n"
	;
}

twine HelixSqldoChildObject::CSXmlGet() 
{
	return "\t\t\tXmlElement " + name + "_node = XmlHelpers.FindChild( node, \"" + name + "\");\n"
		"\t\t\tif( " + name + "_node != null ){\n"
		"\t\t\t\t" + name + ".readXmlNode( " + name + "_node );\n"
		"\t\t\t}\n";
}

twine HelixSqldoChildObject::CSXmlSet() 
{
	return 
		"\t\t\tXmlElement " + name + "_node = " + name + ".createXmlNode( child );\n"
		"\t\t\t// FIXME : xmlNodeSetName(" + name + "_node, (const xmlChar*)\"" + name + "\" );\n"
	;
}

twine HelixSqldoChildObject::JSInit(const twine& app) 
{
	twine ucase( name );
	ucase.ucase(0);
	twine targetPackage( SimplePackage() );
	return 
		"\t\tthis.set" + ucase + "( new " + app + "." + targetPackage + ".sqldo." + SimpleType() + "() );\n"
	;
}

twine HelixSqldoChildObject::JSPropDef() 
{
	return ",\n\t\t" + name + ": {init: null, check: \"Object\"}";
}

twine HelixSqldoChildObject::JSXmlGet(const twine& app) 
{
	twine ucase( name );
	ucase.ucase(0);
	return 
		"\t\t\tvar " + name + "Elem = " + app + ".Statics.xmlFindChild( elem, \"" + name + "\");\n"
		"\t\t\tif( " + name + "Elem ){\n"
		"\t\t\t\tthis.get" + ucase + "().readFromXML( " + name + "Elem );\n"
		"\t\t\t}\n";
}

twine HelixSqldoChildObject::JSXmlSet(const twine& app) 
{
	twine ucase( name );
	ucase.ucase(0);
	return 
		"\t\t\tthis.get" + ucase + "().createXMLElement(subElem, \"" + name + "\");\n"
	;
}

twine HelixSqldoChildObject::JSClone(const twine& app) 
{
	twine ucase( name );
	ucase.ucase(0);
	return 
		"\t\t\tnewObj.set" + ucase + "( this.get" + ucase + "() );\n"
	;
}

