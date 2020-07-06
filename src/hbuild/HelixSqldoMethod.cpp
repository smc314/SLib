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

#include "HelixSqldoMethod.h"
#include "HelixSqldo.h"
using namespace Helix::Build;


HelixSqldoMethod::HelixSqldoMethod() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoMethod::HelixSqldoMethod(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "methodName" );
	type.getAttribute( elem, "methodType" );
	outputCol.getAttribute( elem, "outputCol" );
	if(outputCol.empty()){
		outputCol = "Id";
	}
	target.getAttribute( elem, "target" );
	xmlNodePtr child = XmlHelpers::FindChild( elem, "Comment" );
	if(child != nullptr){
		comment = XmlHelpers::getTextNodeValue( child );
	}
	child = XmlHelpers::FindChild( elem, "Sql" );
	if(child != nullptr){
		sql = XmlHelpers::getTextNodeValue( child );
	}

	for(auto n : XmlHelpers::FindChildren( elem, "Input" ) ){
		inputs.push_back( HelixSqldoParam( n ) );
	}
	for(auto n : XmlHelpers::FindChildren( elem, "Output" ) ){
		outputs.push_back( HelixSqldoParam( n ) );
	}
}

map<twine, twine>& HelixSqldoMethod::BuildStatementParms(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::BuildStatementParms(const twine& className)");

	if(m_parms.size() != 0){
		return m_parms; // Bail out early
	}

	m_parms[ "doName" ] = className;
	m_parms[ "comment" ] = comment;
	m_parms[ "sql" ] = sql;
	m_parms[ "methodName" ] = name;
	m_parms[ "outputCol" ] = outputCol;
	m_parms[ "flatSql" ] = FlattenSql();

	twine typedParms;
	twine csTypedParms;
	twine untypedParms;
	twine objParms;
	twine sqlReplaceParms;
	twine sqlReplaceObjParms;
	twine csSqlReplaceParms;
	twine csSqlReplaceObjParms;
	twine inputDOParms;
	for(auto& input : inputs){
		typedParms.append(input.CPPParm());
		csTypedParms.append(input.CSParm());
		untypedParms.append( ", " + input.name );
		objParms.append( ", obj." + input.name );
		sqlReplaceParms.append( input.CPPReplaceInput() );
		sqlReplaceObjParms.append( input.CPPReplaceInputObj() );
		csSqlReplaceParms.append( input.CSReplaceInput() );
		csSqlReplaceObjParms.append( input.CSReplaceInputObj() );
		inputDOParms.append( ", inputDO." + input.name );
	}
	m_parms[ "TypedParms" ] = typedParms;
	m_parms[ "CSTypedParms" ] = csTypedParms;
	m_parms[ "UntypedParms" ] = untypedParms;
	m_parms[ "ObjParms" ] = objParms;
	m_parms[ "SqlReplaceParms" ] = sqlReplaceParms;
	m_parms[ "SqlReplaceObjParms" ] = sqlReplaceObjParms;
	m_parms[ "CSSqlReplaceParms" ] = csSqlReplaceParms;
	m_parms[ "CSSqlReplaceObjParms" ] = csSqlReplaceObjParms;
	m_parms[ "InputDOParms" ] = inputDOParms;
	if(inputs.size() != 0){
		m_parms[ "CPPInputParmsIdx" ] = "\tsize_t idx = 0;";
		m_parms[ "CSInputParmsIdx" ] = "\t\t\tint idx = 0;";
	} else {
		m_parms[ "CPPInputParmsIdx" ] = "\t// No input parms, so no need for idx;";
		m_parms[ "CSInputParmsIdx" ] = "\t\t\t// No input parms, so no need for idx;";
	}
	
	twine cppReadResultOutput;
	twine csReadResultOutput;
	for(size_t i = 0; i < outputs.size(); i++){
		cppReadResultOutput.append( outputs[i].CPPReadDB( i ) );
		csReadResultOutput.append( outputs[i].CSReadDB( i ) );
	}
	m_parms[ "CPPReadResultOutput" ] = cppReadResultOutput;
	m_parms[ "CSReadResultOutput" ] = csReadResultOutput;

	twine outputSize; outputSize.format( "%d", (int)outputs.size() );
	m_parms[ "SqlOutputColCount" ] = outputSize;

	return m_parms;
}

bool HelixSqldoMethod::HasAutoGen() 
{
	for(auto& o : outputs) if(o.type == "autogen") return true;
	return false;
}

bool HelixSqldoMethod::HasIntInput() 
{
	for(auto& i : inputs) if(i.type == "int") return true;
	return false;
}

bool HelixSqldoMethod::HasFloatInput() 
{
	for(auto& i : inputs) if(i.type == "float") return true;
	return false;
}

twine HelixSqldoMethod::FlattenSql() 
{
	EnEx ee(FL, "HelixSqldo::FlattenSql()");

	// Split into lines so that we can handle comment processing
	vector<twine> lines = sql.split('\n');
	twine uncommentedSQL;
	for(size_t i = 0; i < lines.size(); i++){
		size_t cmtIdx = lines[i].find("--");
		if(cmtIdx != TWINE_NOT_FOUND){
			// Pick up only the beginning of the line up to the -- comment start
			uncommentedSQL += lines[i].substr(0, cmtIdx);
		} else {
			// Pick up the whole line
			uncommentedSQL += lines[i];
		}
		uncommentedSQL += " ";
	}

	uncommentedSQL.replace('\n', ' ');
	uncommentedSQL.replace('\r', ' ');
	uncommentedSQL.rtrim().ltrim();
	return uncommentedSQL;

}

twine HelixSqldoMethod::GenCPPHeader(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPHeader(const twine& className)");

	twine tmplName; 
	if(type == "SELECTTOXML"){
		tmplName = "CppObjHeader.select.tmpl";
	} else if(type == "INSERT"){
		tmplName = "CppObjHeader.insert.tmpl";
	} else if(type == "INSERTGUID"){
		tmplName = "CppObjHeader.insertguid.tmpl";
	} else if(type == "UPDATE"){
		tmplName = "CppObjHeader.update.tmpl";
	} else if(type == "DELETE"){
		tmplName = "CppObjHeader.delete.tmpl";
	} else {
		throw AnException(0, FL, "Unknown method type for generation: %s in %s::%s", 
			type(), className(), name()
		);
	}
	return HelixSqldo::loadTmpl( tmplName, BuildStatementParms(className) );
}

twine HelixSqldoMethod::GenCPPBody(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className)");

	twine tmplName; 
	if(type == "SELECTTOXML"){
		tmplName = "CppObjBody.select.tmpl";
	} else if(type == "INSERT"){
		tmplName = "CppObjBody.insert.tmpl";
	} else if(type == "INSERTGUID"){
		tmplName = "CppObjBody.insertguid.tmpl";
	} else if(type == "UPDATE"){
		tmplName = "CppObjBody.update.tmpl";
	} else if(type == "DELETE"){
		tmplName = "CppObjBody.delete.tmpl";
	} else {
		throw AnException(0, FL, "Unknown method type for generation: %s in %s::%s", 
			type(), className(), name()
		);
	}
	return HelixSqldo::loadTmpl( tmplName, BuildStatementParms(className) );
}

twine HelixSqldoMethod::GenCSBody(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCSBody(const twine& className)");

	twine tmplName; 
	if(type == "SELECTTOXML"){
		tmplName = "C#ObjHeader.select.tmpl";
	} else if(type == "INSERT"){
		tmplName = "C#ObjHeader.insert.tmpl";
	} else if(type == "INSERTGUID"){
		tmplName = "C#ObjHeader.insertguid.tmpl";
	} else if(type == "UPDATE"){
		tmplName = "C#ObjHeader.update.tmpl";
	} else if(type == "DELETE"){
		tmplName = "C#ObjHeader.delete.tmpl";
	} else {
		throw AnException(0, FL, "Unknown method type for generation: %s in %s::%s", 
			type(), className(), name()
		);
	}
	return HelixSqldo::loadTmpl( tmplName, BuildStatementParms(className) );
}

bool HelixSqldoMethod::IsIdGuid(std::map<twine, HelixSqldoParam>& params) 
{
	if(params.count("Id") == 0) return false;
	else if(params["Id"].type == "guid") return true;
	else return false;
}

bool HelixSqldoMethod::IsCreatedColumn(const twine& name) 
{
	if(name == "CreatedBy" ||
		name == "CreatedDate" ||
		name == "DateCreated"
	){
		return true;
	}
	return false;
}

twine HelixSqldoMethod::GetValueForName(const twine& logic, const twine& name) 
{
	if(name == "CreatedBy" ||
		name == "ModifiedBy" ||
		name == "LastUpdatedBy"
	){
		if(logic == "project"){
			return "'${userGUID}'";
		} else {
			return "${userid}";
		}
	} else if( name == "CreatedDate" ||
		name == "DateCreated" ||
		name == "LastUpdated" ||
		name == "ModifiedDate"
	){
		return "GETDATE()";
	}
	return "?"; // Default is to just use ? to mark the input

}

twine HelixSqldoMethod::GenInsertSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params) 
{
	EnEx ee(FL, "HelixSqldo::GenInsertSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params)");

	map<twine, twine> tmpl_params;
	if(IsIdGuid(params)){
		tmpl_params["methodAndOutput"] = "methodType=\"INSERTGUID\" outputCol=\"Id\"";
	} else {
		tmpl_params["methodAndOutput"] = "methodType=\"INSERT\"";
	}
	tmpl_params["tableName"] = tableName;

	twine names;
	twine values;
	twine inputs;

	int count = 0;
	for(auto& entry : params){
		if(entry.first == "Id" || entry.first == "ID") continue; // Don't include Id values in the insert
		if(count > 0){
			names.append(", ");
			values.append(", ");
		}
		if( (count + 1) % 6 == 0 ){ // new line every 6 columns
			names.append("\n\t\t\t\t");
			values.append("\n\t\t\t\t");
		}

		names += entry.first;
		twine valueForName( GetValueForName( logic, entry.first ) );
		values += valueForName;

		if(valueForName == "?"){
			twine tmp; 
			tmp.format("\t\t<Input name=\"%s\" type=\"%s\"/>\n", 
				entry.second.NormalizeName()(),
				entry.second.type()
			);
			inputs.append( tmp );
		}

		count ++;
	}
	tmpl_params["insertColNames"] = names;
	tmpl_params["insertColValues"] = values;
	tmpl_params["insertInputs"] = inputs;

	return HelixSqldo::loadTmpl( "SqlDO.insert.tmpl", tmpl_params );

}

twine HelixSqldoMethod::GenInsertWithIdSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params) 
{
	EnEx ee(FL, "HelixSqldo::GenInsertWithIdSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params)");

	map<twine, twine> tmpl_params;
	tmpl_params["tableName"] = tableName;

	twine names;
	twine values;
	twine inputs;

	int count = 0;
	for(auto& entry : params){
		if(count > 0){
			names.append(", ");
			values.append(", ");
		}
		if( (count + 1) % 6 == 0 ){ // new line every 6 columns
			names.append("\n\t\t\t\t");
			values.append("\n\t\t\t\t");
		}

		names += entry.first;
		twine valueForName( GetValueForName( logic, entry.first ) );
		values += valueForName;

		if(valueForName == "?"){
			twine tmp; 
			tmp.format("\t\t<Input name=\"%s\" type=\"%s\"/>\n", 
				entry.second.NormalizeName()(),
				entry.second.type()
			);
			inputs.append( tmp );
		}

		count ++;
	}
	tmpl_params["insertColNames"] = names;
	tmpl_params["insertColValues"] = values;
	tmpl_params["insertInputs"] = inputs;

	return HelixSqldo::loadTmpl( "SqlDO.insertWithId.tmpl", tmpl_params );

}

twine HelixSqldoMethod::GenUpdateSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params) 
{
	EnEx ee(FL, "HelixSqldo::GenUpdateSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params)");

	map<twine, twine> tmpl_params;
	tmpl_params["tableName"] = tableName;
	tmpl_params["updateWhereValues"] = "Id = ?";

	twine names;
	twine inputs;

	int count = 0;
	for(auto& entry : params){
		if(entry.first == "Id" || entry.first == "ID") continue; // Don't include Id values in the insert
		if(IsCreatedColumn( entry.first )) continue; // Don't include created by or created date columns
		if(count > 0){
			names.append(",\n");
		}
		names.append("\t\t\t\t");

		names += entry.first;
		names += " = ";
		twine valueForName( GetValueForName( logic, entry.first ) );
		names += valueForName;

		if(valueForName == "?"){
			twine tmp; 
			tmp.format("\t\t<Input name=\"%s\" type=\"%s\"/>\n", 
				entry.second.NormalizeName()(),
				entry.second.type()
			);
			inputs.append( tmp );
		}

		count ++;
	}
	// Add in the Id field in the Input list
	if(IsIdGuid( params )){
		inputs.append( "\t\t<Input name=\"Id\" type=\"guid\"/>\n" );
	} else {
		inputs.append( "\t\t<Input name=\"Id\" type=\"int\"/>\n" );
	}
		
	tmpl_params["updateColNames"] = names;
	tmpl_params["updateInputs"] = inputs;

	return HelixSqldo::loadTmpl( "SqlDO.update.tmpl", tmpl_params );

}

twine HelixSqldoMethod::GenDeleteSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params) 
{
	EnEx ee(FL, "HelixSqldo::GenDeleteSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params)");

	map<twine, twine> tmpl_params;
	tmpl_params["tableName"] = tableName;

	return HelixSqldo::loadTmpl( "SqlDO.delete.tmpl", tmpl_params );

}

twine HelixSqldoMethod::GenSelectSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params) 
{
	EnEx ee(FL, "HelixSqldo::GenSelectSql(const twine& logic, const twine& tableName, std::map<twine, HelixSqldoParam>& params)");

	map<twine, twine> tmpl_params;
	tmpl_params["tableName"] = tableName;

	twine names;
	twine simpleNames;
	twine outputs;

	int count = 0;
	for(auto& entry : params){
		if(count > 0){
			names.append(", ");
			simpleNames.append(", ");
		}
		if( (count + 1) % 4 == 0 ){ // new line every 3 columns
			names.append("\n\t\t\t\t");
			simpleNames.append("\n\t\t\t\t");
		}

		names += tableName + "." + entry.first;
		simpleNames += entry.first;

		twine tmp; 
		tmp.format("\t\t<Output name=\"%s\" type=\"%s\"/>\n", 
			entry.second.NormalizeName()(),
			entry.second.type()
		);
		outputs.append( tmp );

		count ++;
	}
	tmpl_params["selectColNames"] = names;
	tmpl_params["selectSimpleColNames"] = simpleNames;
	tmpl_params["selectOutputs"] = outputs;

	return HelixSqldo::loadTmpl( "SqlDO.select.tmpl", tmpl_params );

}

