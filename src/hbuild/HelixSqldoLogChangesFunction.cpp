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

#include "HelixSqldoLogChangesFunction.h"
using namespace Helix::Build;


HelixSqldoLogChangesFunction::HelixSqldoLogChangesFunction() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoLogChangesFunction::HelixSqldoLogChangesFunction(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	if(name.empty()) throw AnException(0, FL, "Missing name for LogChangesFunction");
	tableName.getAttribute( elem, "tableName" );
	if(tableName.empty()) throw AnException(0, FL, "Missing tableName for LogChangesFunction");
	guid.getAttribute( elem, "guid" );
	if(guid.empty()) throw AnException(0, FL, "Missing guid for LogChangesFunction");


	for(auto n : XmlHelpers::FindChildren( elem, "KeyField" ) ){
		keys.push_back( HelixSqldoParam( n ) );
	}
	for(auto n : XmlHelpers::FindChildren( elem, "Field" ) ){
		fields.push_back( HelixSqldoParam( n ) );
	}
}

twine HelixSqldoLogChangesFunction::GenCPPHeader(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPHeader(const twine& className)");

	if(fields.size() == 0){
		return "";
	}

	return	
		"\t\t/** This is used to log changes in instances of " + className + " in a standard way.\n"
		"\t\t  * If any changes are detected between this object and the original passed in, we will\n"
		"\t\t  * log what has changed to the database and return true - indicating that changes were found.\n"
		"\t\t  *\n"
		"\t\t  * If logging is not desired or not required, pass false for logChanges, and we will still do\n"
		"\t\t  * the change detection, but will not log any details to the database.\n"
		"\t\t  */\n"
		"\t\tbool " + name + "(OdbcObj& db, " + className + "* original, const twine& changeReason, const twine& parentKey, bool logChanges = true);\n"
		"\n"
	;

}

twine HelixSqldoLogChangesFunction::GenCPPBody(const twine& className, map< twine, HelixSqldoParam>& allParams) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className)");

	if(fields.size() == 0){
		return "";
	}

	twine ret;

	ret.append(
		"bool " + className  + "::" + name + "(OdbcObj& db, " + className + "* original, const twine& changeReason, const twine& parentKey, bool logChanges)\n"
		"{\n"
		"\tEnEx ee(FL, \"" + className + "::" + name + "(OdbcObj& db, " + className + "* original, const twine& changeReason, const twine& parentKey, bool logChanges)\");\n"
		"\n"
		"\tbool foundChange = false;\n"
		"\n"
		"\ttwine ourKey;\n"
		"\ttwine tmp;\n"
		"\tif(!parentKey.empty()){\n"
		"\t\tourKey = parentKey + \" / \";\n"
		"\t}\n"
		"\tourKey.append( \"(" + className + ")\" );\n"
		"\n"
	);

	for(auto& field : keys){
		// Verify that this is a good field name for the base object
		if(allParams.count( field.name ) == 0){
			throw AnException(0, FL, "LogChangesFunction for %s - keyfield %s not found in base object.",
				className(), field.name()
			);
		}
		auto& param	= allParams[ field.name ];

		if(param.type == "int" || param.type == "autogen" || param.type == "long"){
			ret.append( "\ttmp.format(\"%d\", (int)" + field.name + "); " );
		} else if(param.type == "float"){
			ret.append( "\ttmp.format(\"%f\", (float)" + field.name + "); " );
		} else if(param.type == "bool"){
			ret.append( "\ttmp.format(\"%s\", " + field.name + " ? : \"true\" : \"false\" ); " );
		} else if(param.type == "cdata"){
			ret.append( "\ttmp = " + field.name + "; " );
		} else if(param.type == "base64"){
			ret.append( "\ttmp = " + field.name + "; " );
		} else if(param.type == "guid" || param.type == "Guid"){
			ret.append( "\ttmp = " + field.name + "; " );
		} else if(param.type == "timestamp " || param.type == "Timestamp" || param.type == "Date" || param.type == "DateTime"){
			ret.append( "\ttmp.format(\"%s\", " + field.name + ".GetValue()() ); " );
		} else {
			ret.append( "\ttmp = " + field.name + "; " );
		}
		ret.append( "ourKey.append( \"[\" ).append( tmp ).append( \"]\" );\n" );
	}
	ret.append(
		"\n"
		"\tif(original == nullptr){\n"
		"\t\t// This is used by the caller to indicate a record has been created or deleted\n"
		"\t\t// We log the change with our key, and then return without doing any actual comparisons\n"
		"\t\tif(logChanges){\n"
		"\t\t\tStatics::LogChange(db, \"" + tableName + "\", " + guid + ", ourKey, changeReason);\n"
		"\t\t}\n"
		"\n"
		"\t\treturn true; // change was logged\n"
		"\t};\n"
		"\n"
		"\t// Now check field by field to see if anything has changed\n"
		"\n"
	);

	for(auto& field : fields){
		if(field.type == "childObject"){
			ret.append(
				"\t// Checking " + field.name + " - Log Changes for child object changes\n"
				"\t{ // For scope\n"
				"\t\tbool childObjHasChanged = this->" + field.name + "." + field.function + "(db, &original->" + field.name + ", changeReason, ourKey, logChanges);\n"
				"\t\tif(childObjHasChanged){\n"
				"\t\t\tfoundChange = true; // Remember this\n"
				"\t\t}\n"
				"\t}\n"
				"\n"
			);
		} else if(field.type == "childArray"){
			ret.append(
				"\t// Checking " + field.name + " - Log Changes for child array changes\n"
				"\t// First look for new records, and changes to existing records\n"
				"\tfor(auto c : *" + field.name + ") {\n"
				"\t\t// Look for it in the original object\n"
				"\t\tbool found = false;\n"
				"\t\tfor(auto originalC : *original->" + field.name + "){\n"
				"\t\t\tif(Statics::IdMatch( c->Id, originalC->Id )){\n"
				"\t\t\t\tfound = true;\n"
				"\t\t\t\tbool childHasChanged = c->" + field.function + "(db, originalC, changeReason, ourKey, logChanges);\n"
				"\t\t\t\tif(childHasChanged){\n"
				"\t\t\t\t\tfoundChange = true; // Remember this\n"
				"\t\t\t\t}\n"
				"\t\t\t\tbreak;\n"
				"\t\t\t}\n"
				"\t\t}\n"
				"\t\tif(!found){\n"
				"\t\t\t// This is a brand new record.  Call the child function to log changes and pass a null\n"
				"\t\t\t// value for the original record to indicate it's a new record.\n"
				"\t\t\tc->" + field.function + "(db, nullptr, \"Insert\", ourKey, logChanges);\n"
				"\t\t\tfoundChange = true;\n"
				"\t\t}\n"
				"\t}\n"
				"\n"
				"\t// Then look for any records that have been removed\n"
				"\tfor(auto originalC : *original->" + field.name + "){\n"
				"\t\t// Look for it in the new object\n"
				"\t\tbool found = false;\n"
				"\t\tfor(auto c : *" + field.name + "){\n"
				"\t\t\tif(Statics::IdMatch( c->Id, originalC->Id )){\n"
				"\t\t\t\tfound = true;\n"
				"\t\t\t\tbreak;\n"
				"\t\t\t}\n"
				"\t\t}\n"
				"\t\tif(!found){\n"
				"\t\t\t// This child is not in the newer version of the object - it has been deleted\n"
				"\t\t\toriginalC->" + field.function + "(db, nullptr, \"Delete\", ourKey, logChanges);\n"
				"\t\t\tfoundChange = true;\n"
				"\t\t}\n"
				"\t}\n"
				"\n"
			);
		} else {
			// Verify that this is a good field name for the base object
			if(allParams.count( field.name ) == 0){
				throw AnException(0, FL, "LogChangesFunction for %s - field %s not found in base object.",
					className(), field.name()
				);
			}

			auto& param	= allParams[ field.name ];
			ret.append("\t// Checking " + field.name + " for changes\n");
			if(param.type == "guid" || param.type == "Guid"){
				ret.append(
					"\tif(!Statics::IdMatch(this->" + field.name + ", original->" + field.name + ")){\n"
					"\t\tfoundChange = true;\n"
					"\t\tif(logChanges){\n"
					"\t\t\tStatics::LogChange(\n"
					"\t\t\t\tdb,                           // Which database to use for logging\n"
					"\t\t\t\t\"" + tableName + "\",        // Our table name\n"
					"\t\t\t\t" + guid + ",                 // Our record Id value\n"
					"\t\t\t\tourKey,                       // Our record key that will be used for searching\n"
					"\t\t\t\tchangeReason,                 // A reason for the change - if provided\n"
					"\t\t\t\t\"" + field.name + "\",       // The name of the file that changed\n"
					"\t\t\t\tthis->" + field.name + ",     // The new value of the field\n"
					"\t\t\t\toriginal->" + field.name + "  // The old value of the field\n"
					"\t\t\t);\n"
					"\t\t}\n"
					"\t}\n"
					"\n"
				);
			} else {
				ret.append(
					"\tif(this->" + field.name + " != original->" + field.name + "){\n"
					"\t\tfoundChange = true;\n"
					"\t\tif(logChanges){\n"
					"\t\t\tStatics::LogChange(\n"
					"\t\t\t\tdb,                           // Which database to use for logging\n"
					"\t\t\t\t\"" + tableName + "\",        // Our table name\n"
					"\t\t\t\t" + guid + ",                 // Our record Id value\n"
					"\t\t\t\tourKey,                       // Our record key that will be used for searching\n"
					"\t\t\t\tchangeReason,                 // A reason for the change - if provided\n"
					"\t\t\t\t\"" + field.name + "\",       // The name of the file that changed\n"
					"\t\t\t\tthis->" + field.name + ",     // The new value of the field\n"
					"\t\t\t\toriginal->" + field.name + "  // The old value of the field\n"
					"\t\t\t);\n"
					"\t\t}\n"
					"\t}\n"
					"\n"
				);
			}
		}
	}

	ret.append(
		"\treturn foundChange;\n"
		"}\n"
		"\n"
	);
	return ret;
}

