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

#include "HelixSqldoParam.h"
using namespace Helix::Build;


HelixSqldoParam::HelixSqldoParam() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoParam::HelixSqldoParam(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	type.getAttribute( elem, "type" );
	if(strcmp((const char*)elem->name, "Input")){
		isInput = true;
	} else {
		isInput = false;
	}
	desc = XmlHelpers::getBoolAttr( elem, "desc" );
	usecdata = XmlHelpers::getBoolAttr( elem, "usecdata" );

	min.getAttribute( elem, "min" );
	max.getAttribute( elem, "max" );
	function.getAttribute( elem, "function" );

	if(type.empty()){
		type = "twine"; // Default to twine
	}
}

bool HelixSqldoParam::MatchesType(const HelixSqldoParam& p) 
{
	if(p.type == type){
		return true; // The easy case
	}

	// autogen and int match
	if( (p.type == "autogen" && type == "int") || (p.type == "int" && type == "autogen") ){
		return true;
	}

	// twine+usecdata matches cdata
	if( (p.type == "twine" && p.usecdata && type == "cdata") ||
		(type == "twine" && usecdata && p.type == "cdata")
	){
		return true;
	}

	return false; // Types don't match
}

twine HelixSqldoParam::CPPType() const
{
	if(type == "int" || type == "autogen") return "intptr_t";
	else if(type == "long") return "long";
	else if(type == "float") return "float";
	else if(type == "bool") return "bool";
	else if(type == "cdata") return "twine";
	else if(type == "base64") return "twine";
	else if(type == "bin") return "MemBuf";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") return "Date";
	else return type;
}

twine HelixSqldoParam::CPPParm() const
{
	if(type == "int" || type == "autogen") return ", intptr_t " + name;
	else if(type == "long") return ", long " + name;
	else if(type == "float") return ", float " + name;
	else if(type == "bool") return ", bool " + name;
	else if(type == "cdata") return ", twine& " + name;
	else if(type == "base64") return ", twine& " + name;
	else if(type == "bin") return ", MemBuf& " + name;
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") return ", const Date& " + name;
	else return ", " + type + "& " + name;
}

twine HelixSqldoParam::CPPInit() const
{
	if(type == "int" || type == "autogen") return name + " = 0;";
	else if(type == "long") return name + " = 0;";
	else if(type == "float") return name + " = 0.0f;";
	else if(type == "bool") return name + " = false;";
	else if(type == "cdata") return name + ".erase();";
	else if(type == "base64") return name + ".erase();";
	else if(type == "bin") return name + ".erase();";
	else if(type == "twine") return name + ".erase();";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") return name + ".SetMinValue();";
	else return "// error initializing type " + type;
}

twine HelixSqldoParam::CPPXmlGet() const
{
	if(CPPType() == "intptr_t") return name + " = XmlHelpers::getIntAttr( node, \"" + name + "\" );";
	else if(type == "long") return name + " = XmlHelpers::getIntAttr( node, \"" + name + "\" );";
	else if(type == "float") return name + " = (float)XmlHelpers::getFloatAttr( node, \"" + name + "\" );";
	else if(type == "bool") return name + " = XmlHelpers::getBoolAttr( node, \"" + name + "\" );";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return name + " = XmlHelpers::getDateAttr( node, \"" + name + "\", OdbcObj::DateFormat() );";
	else if(type == "cdata") 
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) " + name + " = XmlHelpers::getCDATASection(" + name + "_child);";
	else if(type == "base64")
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) " + name + " = XmlHelpers::getBase64(" + name + "_child);";
	else if(type == "bin")
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) { " + name + " = XmlHelpers::getCDATASection(" + name + "_child); " + name + ".decode64(); }";
	else if(type == "twine") return name + ".getAttribute( node, \"" + name + "\");";
	else return "// error reading xml for type " + type;
}

twine HelixSqldoParam::CPPXmlSet() const
{
	if(CPPType() == "intptr_t") 
		return "if( " + name + " != 0) XmlHelpers::setIntAttr( child, \"" + name + "\", " + name + " );";
	else if(type == "long") 
		return "if( " + name + " != 0) XmlHelpers::setIntAttr( child, \"" + name + "\", (int)" + name + "  );";
	else if(type == "float") 
		return "if( " + name + " != 0.0) XmlHelpers::setFloatAttr( child, \"" + name + "\", " + name + " );";
	else if(type == "bool") 
		return "if( " + name + " != false) XmlHelpers::setBoolAttr( child, \"" + name + "\", " + name + " );";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return "XmlHelpers::setDateAttr( child, \"" + name + "\", " + name + ", OdbcObj::DateFormat() );";
	else if(type == "base64")
		return "if( " + name + ".size() != 0) XmlHelpers::setBase64( xmlNewChild( child, NULL, (const xmlChar*)\"" + name + "\", NULL), " + name + ");";
	else if(type == "bin")
		return "if( " + name + ".size() != 0) XmlHelpers::setBase64( xmlNewChild( child, NULL, (const xmlChar*)\"" + name + "\", NULL), " + name + ");";
	else if(type == "twine") 
		return "if( !" + name + ".empty()) xmlSetProp( child, (const xmlChar*)\"" + name + "\", " + name + ");";
	else return "// error setting xml for type " + type;
}

twine HelixSqldoParam::CPPJsonGet() const
{
	if(CPPType() == "intptr_t") return name + " = cJSON_GetIntValueOrZero( jsonObj, \"" + name + "\" );";
	else if(type == "long") return name + " = cJSON_GetIntValueOrZero( jsonObj, \"" + name + "\" );";
	else if(type == "float") return name + " = (float)cJSON_GetDoubleValueOrZero( jsonObj, \"" + name + "\" );";
	else if(type == "bool") return name + " = (bool)cJSON_GetBoolValueOrFalse( jsonObj, \"" + name + "\" );";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return name + " = OdbcObj::ReadDate( cJSON_GetStringValueOrEmpty( jsonObj, \"" + name + "\") );";
	else if(type == "cdata") return name + " = cJSON_GetStringValueOrEmpty( jsonObj, \"" + name + "\");";
	else if(type == "base64") return "// FIXME - base64 not yet supported for JSON";
	else if(type == "bin") return "// FIXME - bin not yet supported for JSON";
	else if(type == "twine") return name + " = cJSON_GetStringValueOrEmpty( jsonObj, \"" + name + "\");";
	else return "// error reading xml for type " + type;
}

twine HelixSqldoParam::CPPJsonSet() const
{
	if(CPPType() == "intptr_t") 
		return "cJSON_SetAddIntValue( child, \"" + name + "\", (int)" + name + " );";
	else if(type == "long") 
		return "cJSON_SetAddIntValue( child, \"" + name + "\", (int)" + name + "  );";
	else if(type == "float") 
		return "cJSON_SetAddDoubleValue( child, \"" + name + "\", " + name + " );";
	else if(type == "bool") 
		return "cJSON_SetAddBoolValue( child, \"" + name + "\", (int)" + name + " );";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return "if(!" + name + ".IsMinValue()) cJSON_SetAddStringValue( child, \"" + name + "\", " + name + ".GetValue( OdbcObj::DateFormat() )() );";
	else if(type == "base64")
		return "// FIXME - base64 not yet supported for JSON";
	else if(type == "bin")
		return "// FIXME - bin not yet supported for JSON;";
	else if(type == "twine") 
		return "if( !" + name + ".empty()) cJSON_SetAddStringValue( child, \"" + name + "\", " + name + "() );";
	else return "// error setting json for type " + type;
}

twine HelixSqldoParam::CPPReplaceInput() const
{
	return "\tOdbcObj::ReplaceInput(stmt, idx, " + name + ");\n";
}

twine HelixSqldoParam::CPPReplaceInputObj() const
{
	return "\tOdbcObj::ReplaceInput(stmt, idx, obj." + name + ");\n";
}

twine HelixSqldoParam::CPPReadDB(int pos) const
{
	twine pos_string; pos_string = (size_t)(pos + 1);
	return "\t\t\todbc.GetData( " + pos_string + ", obj->" + name + " );\n";
}

twine HelixSqldoParam::CSType() const
{
	if(type == "int" || type == "autogen") return "int";
	else if(type == "long") return "int";
	else if(type == "float") return "float";
	else if(type == "bool") return "bool";
	else if(type == "cdata") return "string";
	else if(type == "base64") return "string";
	else if(type == "bin") return "string";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") return "DateTime";
	else if(type == "twine") return "string";
	else return type;
}

twine HelixSqldoParam::CSParm() const
{
	if(type == "int" || type == "autogen") return ", int " + name;
	else if(type == "long") return ", long " + name;
	else if(type == "float") return ", float " + name;
	else if(type == "bool") return ", bool " + name;
	else if(type == "cdata") return ", string " + name;
	else if(type == "base64") return ", string " + name;
	else if(type == "bin") return ", string " + name;
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") return ", DateTime " + name;
	else if(type == "twine") return ", string " + name;
	else return ", " + type + " " + name;
}

twine HelixSqldoParam::CSInit() const
{
	if(type == "int" || type == "autogen") return name + " = 0;";
	else if(type == "long") return name + " = 0;";
	else if(type == "float") return name + " = 0.0f;";
	else if(type == "bool") return name + " = false;";
	else if(type == "cdata") return name + " = \"\";";
	else if(type == "base64") return name + " = \"\";";
	else if(type == "bin") return name + " = \"\";";
	else if(type == "twine") return name + " = \"\";";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") return name + " = new DateTime();";
	else return "// error initializing type " + type;
}

twine HelixSqldoParam::CSXmlGet() const
{
	if(CSType() == "int") return name + " = XmlHelpers.getIntAttr( node, \"" + name + "\" );";
	else if(type == "float") return name + " = XmlHelpers.getFloatAttr( node, \"" + name + "\" );";
	else if(type == "bool") return name + " = XmlHelpers.getBoolAttr( node, \"" + name + "\" );";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return name + " = XmlHelpers.getDateAttr( node, \"" + name + "\" );";
	/*
	else if(type == "cdata") 
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) " + name + " = XmlHelpers::getCDATASection(" + name + "_child);";
	else if(type == "base64") return name + ".erase();";
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) " + name + " = XmlHelpers::getBase64(" + name + "_child);";
	else if(type == "bin") return name + ".erase();";
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) { " + name + " = XmlHelpers::getCDATASection(" + name + "_child); " + name + ".decode64(); }";
	*/
	else if(type == "twine") return name + " = node.GetAttribute( \"" + name + "\");";
	else return "// error reading xml for type " + type;
}

twine HelixSqldoParam::CSXmlSet() const
{
	if(CSType() == "int") 
		return "if(" + name + " != 0) XmlHelpers.setIntAttr( child, \"" + name + "\", " + name + " );";
	else if(type == "float") 
		return "if(" + name + " != 0.0) XmlHelpers.setFloatAttr( child, \"" + name + "\", " + name + " );";
	else if(type == "bool") 
		return "if(" + name + " != false) XmlHelpers.setBoolAttr( child, \"" + name + "\", " + name + " );";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return "if(" + name + " != DateTime.MinValue) XmlHelpers.setDateAttr( child, \"" + name + "\", " + name + " );";
	/*
	else if(type == "cdata") 
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) " + name + " = XmlHelpers::getCDATASection(" + name + "_child);";
	else if(type == "base64") return name + ".erase();";
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) " + name + " = XmlHelpers::getBase64(" + name + "_child);";
	else if(type == "bin") return name + ".erase();";
		return "xmlNodePtr " + name + "_child = XmlHelpers::FindChild( node, \"" + name + "\"); if(" + name + "_child != nullptr) { " + name + " = XmlHelpers::getCDATASection(" + name + "_child); " + name + ".decode64(); }";
	*/
	else if(type == "twine") 
		return "if(!String.IsNullOrEmpty( " + name + " )) child.SetAttribute( \"" + name + "\", " + name + ");";
	else return "// error setting xml for type " + type;
}

twine HelixSqldoParam::CSReadDB(int pos) const
{
	twine pos_string; pos_string = (size_t)pos;
	if(CSType() == "int") 
		return "\t\t\t\tlocal." + name + " = Util.Statics.ReadSqlInt( reader, " + pos_string + ");\n";
	else if(type == "float") 
		return "\t\t\t\tlocal." + name + " = reader.IsDBNull(" + pos_string + ") ? 0.0f : (float)reader.GetDouble(" + pos_string + ");\n";
	else if(type == "bool") 
		return "\t\t\t\tlocal." + name + " = reader.IsDBNull(" + pos_string + ") ? false : reader.GetBool(" + pos_string + ");\n";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return "\t\t\t\tlocal." + name + " = reader.IsDBNull(" + pos_string + ") ? DateTime.MinValue: reader.GetDateTime(" + pos_string + ");\n";
	else if(type == "twine") 
		return "\t\t\t\tlocal." + name + " = reader.IsDBNull(" + pos_string + ") ? \"\": reader.GetValue(" + pos_string + ").ToString();\n";
	else return "// error reading database for type " + type;
}

twine HelixSqldoParam::CSReplaceInput() const
{
	return "\t\t\tstmt = Util.Statics.FindReplaceParm( stmt, ref idx, " + name + " );\n";
}

twine HelixSqldoParam::CSReplaceInputObj() const
{
	return "\t\t\tstmt = Util.Statics.FindReplaceParm( stmt, ref idx, obj." + name + " );\n";
}

twine HelixSqldoParam::JSPropDef(bool first) const
{
	twine prefix;
	if(!first){
		prefix = ",\n\t\t";
	} else {
		prefix = "\t\t";
	}
	if(type == "int" || type == "autogen") 
		return prefix + name + " : {init: 0, event: \"change" + name + "\", check: \"Integer\" }";
	else if(type == "long")
		return prefix + name + " : {init: 0, event: \"change" + name + "\", check: \"Number\" }";
	else if(type == "float")
		return prefix + name + " : {init: 0, event: \"change" + name + "\", check: \"Number\" }";
	else if(type == "bool")
		return prefix + name + " : {init: false, event: \"change" + name + "\", check: \"Boolean\" }";
	else if(type == "cdata")
		return prefix + name + " : {init: \"\", event: \"change" + name + "\", check: \"String\" }";
	else if(type == "base64")
		return prefix + name + " : {init: \"\", event: \"change" + name + "\", check: \"String\" }";
	else if(type == "bin")
		return prefix + name + " : {init: \"\", event: \"change" + name + "\", check: \"String\" }";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime")
		return prefix + name + " : {init: null, nullable: true, event: \"change" + name + "\", check: \"Date\" }";
	else 
		return prefix + name + " : {init: \"\", event: \"change" + name + "\", check: \"String\" }";
}

twine HelixSqldoParam::JSXmlGet(const twine& app) const
{
	twine ucase( name );
	ucase.ucase(0);
	if(CPPType() == "intptr_t") 
		return "\t\t\tif(elem.getAttribute(\"" + name + "\")){ this.set" + ucase + "( Number(elem.getAttribute(\"" + name + "\") ) ); }\n";
	else if(type == "long")
		return "\t\t\tif(elem.getAttribute(\"" + name + "\")){ this.set" + ucase + "( Number(elem.getAttribute(\"" + name + "\") ) ); }\n";
	else if(type == "float")
		return "\t\t\tif(elem.getAttribute(\"" + name + "\")){ this.set" + ucase + "( Number(elem.getAttribute(\"" + name + "\") ) ); }\n";
	else if(type == "bool")
		return "\t\t\tthis.set" + ucase + "( " + app + ".Statics.xmlGetBool(elem, \"" + name + "\") );\n";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return "\t\t\tthis.readDate( elem, \"" + name + "\" );\n";
	else if(type == "base64" || type == "bin")
		return 
			"\t\t\tthis.set" + ucase + "(\n"
			"\t\t\t\t" + app + ".Statics.xmlGetBase64(\n"
			"\t\t\t\t\t" + app + ".Statics.xmlFindChild( elem, \"" + name + "\" )\n"
			"\t\t\t\t)\n"
			"\t\t\t);\n"
		;
	else 
		return "\t\t\tif(elem.getAttribute(\"" + name + "\")){ this.set" + ucase + "( elem.getAttribute(\"" + name + "\") ); }\n";
}

twine HelixSqldoParam::JSXmlSet(const twine& app) const
{
	twine ucase( name );
	ucase.ucase(0);

	if(CPPType() == "intptr_t") 
		return "\t\t\tif( this.get" + ucase + "() != 0) subElem.setAttribute( \"" + name + "\", String( this.get" + ucase + "() ) );\n";
	else if(type == "long") 
		return "\t\t\tif( this.get" + ucase + "() != 0) subElem.setAttribute( \"" + name + "\", String( this.get" + ucase + "() ) );\n";
	else if(type == "float") 
		return "\t\t\tif( this.get" + ucase + "() != 0) subElem.setAttribute( \"" + name + "\", String( this.get" + ucase + "() ) );\n";
	else if(type == "bool") 
		return "\t\t\tif( this.get" + ucase + "() != false) subElem.setAttribute( \"" + name + "\", \"true\" );\n";
	else if(type == "Timestamp" || type == "Date" || type == "DateTime") 
		return "\t\t\tthis.writeDate( subElem, \"" + name + "\" );\n";
	else if(type == "cdata")
		return
			"\t\t\tif(this.get" + ucase + "().length != 0){\n"
			"\t\t\t\tvar sub = doc.createElement(\"" + name + "\");\n"
			"\t\t\t\t" + app + ".Statics.xmlSetCDATASection( sub, this.get" + ucase + "() );\n"
			"\t\t\t\tsubElem.appendChild( sub );\n"
			"\t\t\t}\n"
		;
	else if(type == "base64" || type == "bin")
		return
			"\t\t\tif(this.get" + ucase + "().length != 0){\n"
			"\t\t\t\tvar sub = doc.createElement(\"" + name + "\");\n"
			"\t\t\t\t" + app + ".Statics.xmlSetBase64( sub, this.get" + ucase + "() );\n"
			"\t\t\t\tsubElem.appendChild( sub );\n"
			"\t\t\t}\n"
		;
	else
		return "\t\t\tif( this.get" + ucase + "().length != 0) subElem.setAttribute( \"" + name + "\", this.get" + ucase + "() );\n";
}

twine HelixSqldoParam::JSClone(const twine& app) const
{
	twine ucase( name );
	ucase.ucase(0);

	return "\t\t\tnewObj.set" + ucase + "( this.get" + ucase + "() );\n";
}
