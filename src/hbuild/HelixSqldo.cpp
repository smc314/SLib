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
#include <File.h>
using namespace SLib;

#include "HelixFS.h"
#include "HelixSqldo.h"
using namespace Helix::Build;


HelixSqldo::HelixSqldo(HelixFSFolder folder, HelixFSFile file) 
	: m_folder( folder ), m_file( file )
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

	ReadSqldo();
}

HelixSqldo::HelixSqldo(const HelixSqldo& c) :
	m_folder( c.m_folder ), m_file( c.m_file )
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(const HelixSqldo& c)");

}

HelixSqldo& HelixSqldo::operator=(const HelixSqldo& c)
{
	EnEx ee(FL, "HelixSqldo::operator=(const HelixSqldo& c)");

	return *this;
}

HelixSqldo::HelixSqldo(const HelixSqldo&& c) :
	m_folder( c.m_folder ), m_file( c.m_file )
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(const HelixSqldo&& c)");

}

HelixSqldo& HelixSqldo::operator=(const HelixSqldo&& c)
{
	EnEx ee(FL, "HelixSqldo::operator=(const HelixSqldo&& c)");


	return *this;
}

HelixSqldo::~HelixSqldo()
{
	EnEx ee(FL, "HelixSqldo::~HelixSqldo()");

}

HelixFSFile HelixSqldo::File()
{
	return m_file;
}

HelixFSFolder HelixSqldo::Folder()
{
	return m_folder;
}

twine HelixSqldo::FQName()
{
	twine ret( m_package_dot_name + "." + m_class_name );
	return ret;
}

twine HelixSqldo::LogicFolder()
{
	return m_package_name.split("/")[1];
}

twine HelixSqldo::CPPHeaderFileName()
{
	return "./" + m_package_name + "/sqldo/" + m_class_name + ".h";
}

twine HelixSqldo::CPPBodyFileName()
{
	return "./" + m_package_name + "/sqldo/" + m_class_name + ".cpp";
}

twine HelixSqldo::CSBodyFileName()
{
	return "../c#/HelixPdfGen/HelixPdfGen/DO/" + LogicFolder() + "/" + m_class_name + ".cs";
}

twine HelixSqldo::JSBodyFileName(const twine& app)
{
	return "../../qd/" + app + "/source/class/" + app + "/" + LogicFolder() + "/sqldo/" + m_class_name + ".js";
}

const HelixSqldoParam& HelixSqldo::Param(const twine& paramName)
{
	if(m_all_params.count(paramName) == 0){
		throw AnException(0, FL, "Unknown param: %s::%s", FQName()(), paramName() );
	}
	return m_all_params[ paramName ];
}

const map<twine, HelixSqldoParam>& HelixSqldo::AllParams()
{
	return m_all_params;
}

const vector<HelixSqldoMethod>& HelixSqldo::Methods()
{
	return m_methods;
}

const vector<HelixSqldoChildVector>& HelixSqldo::ChildVectors()
{
	return m_child_vectors;
}

const vector<HelixSqldoChildObject>& HelixSqldo::ChildObjects()
{
	return m_child_objects;
}

const vector<HelixSqldoSortFunction>& HelixSqldo::SortFunctions()
{
	return m_sorts;
}

const vector<HelixSqldoMatchFunction>& HelixSqldo::MatchFunctions()
{
	return m_matches;
}

void HelixSqldo::ReadSqldo()
{
	m_all_params.clear();
	m_methods.clear();
	m_child_vectors.clear();
	m_child_objects.clear();
	m_sorts.clear();
	m_matches.clear();
	m_parms.clear();

	xmlNodePtr root = xmlDocGetRootElement( m_file->Xml() );
	if(root == nullptr) return; // Bail out

	m_class_name.getAttribute(root, "generateClass" );
	m_package_name.getAttribute(root, "package" );
	m_package_dot_name = m_package_name;
	m_package_dot_name.replace( '/', '.' );

	for(auto n : XmlHelpers::FindChildren( root, "SqlStatement" ) ){
		m_methods.push_back( HelixSqldoMethod( n ) );
	}
	for(auto n : XmlHelpers::FindChildren( root, "ChildVector" ) ){
		m_child_vectors.push_back( HelixSqldoChildVector( n ) );
	}
	for(auto n : XmlHelpers::FindChildren( root, "ChildObject" ) ){
		m_child_objects.push_back( HelixSqldoChildObject( n ) );
	}
	for(auto n : XmlHelpers::FindChildren( root, "SortFunction" ) ){
		m_sorts.push_back( HelixSqldoSortFunction( n ) );
	}
	for(auto n : XmlHelpers::FindChildren( root, "MatchFunction" ) ){
		m_matches.push_back( HelixSqldoMatchFunction( n ) );
	}

	// Gather up our list of unique input/output parameters
	for(auto& m : m_methods){
		for(auto& p : m.inputs){
			if(m_all_params.count( p.name ) == 0){
				m_all_params[ p.name ] = p;
			}
		}
		for(auto& p : m.outputs){
			if(m_all_params.count( p.name ) == 0){
				m_all_params[ p.name ] = p;
			}
		}
	}
}

bool HelixSqldo::SanityCheck()
{
	map<twine, HelixSqldoParam> firstDefinedType;
	map<twine, twine> firstDefinedIn;
	bool foundProblem = false;
	for(auto& m : m_methods){
		for(auto& p : m.inputs){
			if(firstDefinedType.count( p.name ) == 0){
				firstDefinedType[ p.name ] = p;
				firstDefinedIn[ p.name ] = m.name;
			} else {
				// verify the type
				if(firstDefinedType[ p.name ].MatchesType( p ) == false){
					foundProblem = true;
					WARN(FL, "SqlDo Error - %s::%s %s input data type is %s - previously defined in %s as %s\n",
						FQName()(), m.name(), p.name(), p.type(), 
						firstDefinedIn[p.name](), firstDefinedType[p.name].type()
					);
				}
			}
		}
		for(auto& p : m.outputs){
			if(firstDefinedType.count( p.name ) == 0){
				firstDefinedType[ p.name ] = p;
				firstDefinedIn[ p.name ] = m.name;
			} else {
				// verify the type
				if(firstDefinedType[ p.name ].MatchesType( p ) == false){
					foundProblem = true;
					WARN(FL, "SqlDo Error - %s::%s %s output data type is %s - previously defined in %s as %s\n",
						FQName()(), m.name(), p.name(), p.type(), 
						firstDefinedIn[p.name](), firstDefinedType[p.name].type()
					);
				}
			}
		}
	}
	return foundProblem;
}

map< twine, twine >& HelixSqldo::BuildObjectParms()
{
	if(m_parms.size() != 0){
		return m_parms; // bail out early
	}
	DEBUG(FL, "%s - Building Parm List - %d", m_class_name(), (int)m_all_params.size() );

	twine ifdefName = LogicFolder() + "_" + m_class_name + "_H";
	ifdefName.ucase();

	m_parms[ "shortName" ] = m_class_name;
	m_parms[ "ifdefName" ] = ifdefName;
	m_parms[ "shortPackage" ] = LogicFolder();
	m_parms[ "m_currentFile" ] = m_file->PhysicalFileName();

	twine childObjectIncludes;
	twine defineDataMembers;
	twine defineCSDataMembers;
	twine memberCopyStatements;
	twine memberInitStatements;
	twine memberCSInitStatements;
	twine memberCheckSizeStatements;
	twine childVectorDefines;
	twine childCSVectorDefines;
	twine xmlReadMembers;
	twine xmlCSReadMembers;
	twine xmlWriteMembers;
	twine xmlCSWriteMembers;
	twine jsonReadMembers;
	twine jsonWriteMembers;
	for(auto& p : m_all_params){
		DEBUG(FL, "%s - Parm %s(%s)", m_class_name(), p.second.name(), p.second.type() );
		defineDataMembers.append("\t\t" + p.second.CPPType() + " " + p.second.name + ";\n");
		defineCSDataMembers.append("\t\tpublic " + p.second.CSType() + " " + p.second.name + ";\n");
		memberCopyStatements.append("\t" + p.second.name + " = c." + p.second.name + ";\n");
		memberInitStatements.append("\t" + p.second.CPPInit() + "\n" );
		memberCSInitStatements.append("\t" + p.second.CSInit() + "\n" );
		if(p.second.CPPType() == "twine"){
			memberCheckSizeStatements.append( "\t" + p.second.name + ".to_utf8(\"\").rtrim();\n" );
		} else if(p.second.CPPType() == "MemBuf"){
			memberCheckSizeStatements.append("\t" + p.second.name + ".size( " + p.second.name + ".userIntVal );\n");
		}
		xmlReadMembers.append("\t" + p.second.CPPXmlGet() + "\n" );
		xmlCSReadMembers.append("\t\t\t" + p.second.CSXmlGet() + "\n" );
		xmlWriteMembers.append("\t" + p.second.CPPXmlSet() + "\n" );
		xmlCSWriteMembers.append("\t\t\t" + p.second.CSXmlSet() + "\n" );
		jsonReadMembers.append("\t" + p.second.CPPJsonGet() + "\n" );
		jsonWriteMembers.append("\t" + p.second.CPPJsonSet() + "\n" );
	}
	for(auto& cv : m_child_vectors){
		DEBUG(FL, "%s - Child Vector %s(%s)", m_class_name(), cv.name(), cv.type() );
		childObjectIncludes.append( cv.IncludeLine() );
		childVectorDefines.append("\t\t" + cv.type + "_svect " + cv.name + ";\n");
		childCSVectorDefines.append("\t\tpublic List<" + cv.SimpleType() + "> " + cv.name + ";\n");
		memberCopyStatements.append( cv.CPPCopy() );
		memberInitStatements.append( "\t" + cv.CPPInit() + "\n");
		memberCSInitStatements.append( "\t\t\t" + cv.name + " = new List<" + cv.SimpleType() + ">();\n");
		xmlReadMembers.append( cv.CPPXmlGet() );
		xmlCSReadMembers.append( cv.CSXmlGet() );
		xmlWriteMembers.append( cv.CPPXmlSet() );
		xmlCSWriteMembers.append( cv.CSXmlSet() );
		jsonReadMembers.append( cv.CPPJsonGet() );
		jsonWriteMembers.append( cv.CPPJsonSet() );
	}
	for(auto& co : m_child_objects){
		DEBUG(FL, "%s - Child Object %s(%s)", m_class_name(), co.name(), co.type() );
		childObjectIncludes.append( co.IncludeLine() );
		childVectorDefines.append("\t\t" + co.type + " " + co.name + ";\n");
		childCSVectorDefines.append("\t\tpublic " + co.SimpleType() + " " + co.name + ";\n");
		memberCopyStatements.append("\t" + co.name + " = c." + co.name + ";\n");
		memberInitStatements.append( "\t" + co.name + ".init();\n" );
		memberCSInitStatements.append( "\t\t\t" + co.name + " = new " + co.SimpleType() + "();\n" );
		xmlReadMembers.append( co.CPPXmlGet() );
		xmlCSReadMembers.append( co.CSXmlGet() );
		xmlWriteMembers.append( co.CPPXmlSet() );
		xmlCSWriteMembers.append( co.CSXmlSet() );
		jsonReadMembers.append( co.CPPJsonGet() );
		jsonWriteMembers.append( co.CPPJsonSet() );
	}
	m_parms[ "ChildObjectIncludes" ] = childObjectIncludes;
	m_parms[ "DefineDataMembers" ] = defineDataMembers;
	m_parms[ "C#DefineDataMembers" ] = defineCSDataMembers;
	m_parms[ "ChildVectorDefines" ] = childVectorDefines;
	m_parms[ "C#ChildVectorDefines" ] = childCSVectorDefines;
	m_parms[ "MemberCopyStatements" ] = memberCopyStatements;
	m_parms[ "MemberInitStatements" ] = memberInitStatements;
	m_parms[ "C#MemberInitStatements" ] = memberCSInitStatements;
	m_parms[ "MemberCheckSizeStatements" ] = memberCheckSizeStatements;
	m_parms[ "XmlReadMembers" ] = xmlReadMembers;
	m_parms[ "C#XmlReadMembers" ] = xmlCSReadMembers;
	m_parms[ "XmlWriteMembers" ] = xmlWriteMembers;
	m_parms[ "C#XmlWriteMembers" ] = xmlCSWriteMembers;
	m_parms[ "JSONReadMembers" ] = jsonReadMembers;
	m_parms[ "JSONWriteMembers" ] = jsonWriteMembers;

	return m_parms;
}

twine HelixSqldo::GenCPPHeader()
{
	twine output;
	output.append( loadTmpl( "CppObjHeader.start.tmpl", BuildObjectParms() ) );
	for(auto& method : m_methods) output.append( method.GenCPPHeader(m_class_name) );
	for(auto& sort : m_sorts) output.append( sort.GenCPPHeader(m_class_name) );
	for(auto& match : m_matches) output.append( match.GenCPPHeader(m_class_name) );
	output.append( loadTmpl( "CppObjHeader99.tmpl", BuildObjectParms() ) );
	return output;
}

twine HelixSqldo::GenCPPBody()
{
	twine output;
	output.append( loadTmpl( "CppObjBody.start.tmpl", BuildObjectParms() ) );
	for(auto& method : m_methods) output.append( method.GenCPPBody(m_class_name) );
	for(auto& sort : m_sorts) output.append( sort.GenCPPBody(m_class_name) );
	for(auto& match : m_matches) output.append( match.GenCPPBody(m_class_name) );
	return output;
}

twine HelixSqldo::GenCSBody()
{
	twine output;
	output.append( loadTmpl( "C#ObjHeader.start.tmpl", BuildObjectParms() ) );
	for(auto& method : m_methods) output.append( method.GenCSBody(m_class_name) );
	//for(auto& sort : m_sorts) output.append( sort.GenCPPBody(m_class_name) );
	//for(auto& match : m_matches) output.append( match.GenCPPBody(m_class_name) );
	output.append( loadTmpl( "C#ObjHeader99.tmpl", BuildObjectParms() ) );
	return output;
}

twine HelixSqldo::GenJSBody(const twine& app)
{
	// Update a few of our object parameters based on the app name that is passed in
	
	twine package( app + "." + LogicFolder() );
	m_parms["package"] = package;
	m_parms["app"] = app;

	twine childArrayInit;
	twine childArrayDispose;
	twine jsProperties;
	twine jsArrayProperties;
	twine jsReadFromXml;
	twine jsWriteToXml;
	twine jsCloneArrays;
	twine jsCloneProperties;
	bool first = true;
	for(auto& p : m_all_params){
		jsProperties.append( p.second.JSPropDef(first) );
		jsReadFromXml.append( p.second.JSXmlGet( app ) );
		jsWriteToXml.append( p.second.JSXmlSet( app ) );
		jsCloneProperties.append( p.second.JSClone( app ) );

		first = false;
	}
	for(auto& cv : m_child_vectors){
		childArrayInit.append( cv.JSInit( app ) );
		childArrayDispose.append( cv.JSDispose( app ) );
		jsArrayProperties.append( cv.JSPropDef(app) );
		jsReadFromXml.append( cv.JSXmlGet( app ) );
		jsWriteToXml.append( cv.JSXmlSet( app ) );
		jsCloneArrays.append( cv.JSClone( app ) );
	}
	for(auto& co : m_child_objects){
		childArrayInit.append( co.JSInit( app ) );
		jsProperties.append( co.JSPropDef() );
		jsReadFromXml.append( co.JSXmlGet( app ) );
		jsWriteToXml.append( co.JSXmlSet( app ) );
		jsCloneProperties.append( co.JSClone( app ) );
	}
	m_parms[ "childArrayInit" ] = childArrayInit;
	m_parms[ "childArrayDispose" ] = childArrayDispose;
	m_parms[ "jsProperties" ] = jsProperties;
	m_parms[ "jsArrayProperties" ] = jsArrayProperties;
	m_parms[ "jsReadFromXml" ] = jsReadFromXml;
	m_parms[ "jsWriteToXml" ] = jsWriteToXml;
	m_parms[ "jsCloneArrays" ] = jsCloneArrays;
	m_parms[ "jsCloneProperties" ] = jsCloneProperties;

	twine output;
	output.append( loadTmpl( "JsObj.full.tmpl", m_parms) );
	return output;
}


twine HelixSqldo::loadTmpl(const twine& tmplName, map<twine, twine>& vars)
{
	if(!File::Exists("../../../3rdParty/bin/" + tmplName)){
		throw AnException(0, FL, "!! Template %s not found !!\n", tmplName() );
	}
	SLib::File tmpl( "../../../3rdParty/bin/" + tmplName );
	vector<twine> lines = tmpl.readLines();
	twine ret;
	for(size_t i = 0; i < lines.size(); i++){
		ret += HelixSqldo::replaceVars( tmplName, i, lines[i], vars );
		ret += "\n";
	}
	return ret;
}

twine HelixSqldo::replaceVars( const twine& tmplName, size_t lineIdx, twine line, map<twine, twine>& vars )
{
	size_t idx1 = 0, idx2 = 0;
	while(idx1 < line.length() ){
		idx1 = line.find("${", idx1);
		if(idx1 == TWINE_NOT_FOUND){
			break; // nothing to do
		}
		idx1 += 2; // just past ${
		idx2 = line.find("}", idx1);
		if(idx2 == TWINE_NOT_FOUND){
			continue; // nothing to do
		}
		twine varName = line.substr(idx1, idx2-idx1);
		idx1 -= 2; // back to where it was - at ${
		if(vars.count( varName ) != 0){
			line.replace(idx1, idx2-idx1+1, vars[ varName ] );
		} else {
			if(varName != "userid" && varName != "userGUID"){
				//printf("Template %s: Unknown variable %s referenced on line %d\n",
				//	tmplName(), varName(), (int)lineIdx );
			}
			idx1 += 2; // past ${
		}
	}

	return line;
}
