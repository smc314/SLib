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

#include "HelixSqldoValidateFunction.h"
using namespace Helix::Build;


HelixSqldoValidateFunction::HelixSqldoValidateFunction() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoValidateFunction::HelixSqldoValidateFunction(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	for(auto n : XmlHelpers::FindChildren( elem, "Field" ) ){
		fields.push_back( HelixSqldoParam( n ) );
	}
}

twine HelixSqldoValidateFunction::GenCPPHeader(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPHeader(const twine& className)");

	if(fields.size() == 0){
		return "";
	}

	return	
		"\t\t/** This is used to validate instances of " + className + " in a standard way.\n"
		"\t\t  */\n"
		"\t\tvoid " + name + "();\n"
		"\n"
	;

}

twine HelixSqldoValidateFunction::GenCPPBody(const twine& className) 
{
	EnEx ee(FL, "HelixSqldo::GenCPPBody(const twine& className)");

	if(fields.size() == 0){
		return "";
	}

	twine ret;

	ret.append(
		"void " + className  + "::" + name + "()\n"
		"{\n"
		"\n"
	);
	for(auto& field : fields){
		if(field.type == "length"){
			if(!field.min.empty()){
				ret.append(
					"\t// Validate minimum length\n"
					"\tif(" + field.name + ".length() < " + field.min + "){\n"
					"\t\tthrow AnException(EXCEPTION_HANDLED_VALIDATION, FL,\n"
					"\t\t\t\"" + className + " - " + field.name + " must be at least " + field.min +
					" characters.\"\n"
					"\t\t);\n"
					"\t}\n"
					"\n"
				);
			}
			if(!field.max.empty()){
				ret.append(
					"\t// Validate maximum length\n"
					"\tif(" + field.name + ".length() > " + field.max + "){\n"
					"\t\tthrow AnException(EXCEPTION_HANDLED_VALIDATION, FL,\n"
					"\t\t\t\"" + className + " - " + field.name + " must be no more than " + field.max +
					" characters.\"\n"
					"\t\t);\n"
					"\t}\n"
					"\n"
				);
			}
		} else if(field.type == "nonzero"){
			ret.append(
				"\t// Validate nonzero integer field\n"
				"\tif(" + field.name + " == 0){\n"
				"\t\tthrow AnException(EXCEPTION_HANDLED_VALIDATION, FL,\n"
				"\t\t\t\"" + className + " - " + field.name + " must not be zero.\"\n"
				"\t\t);\n"
				"\t}\n"
				"\n"
			);
		} else if(field.type == "nonzeroid"){
			ret.append(
				"\t// Validate nonzero guid field\n"
				"\tif(" + field.name + ".isZeroGuidOrEmpty() == true){\n"
				"\t\tthrow AnException(EXCEPTION_HANDLED_VALIDATION, FL,\n"
				"\t\t\t\"" + className + " - " + field.name + " must not be empty.\"\n"
				"\t\t);\n"
				"\t}\n"
				"\n"
			);
		} else if(field.type == "nonnulldate"){
			ret.append(
				"\t// Validate nonnull date field\n"
				"\tif(" + field.name + ".IsMinValue() == true){\n"
				"\t\tthrow AnException(EXCEPTION_HANDLED_VALIDATION, FL,\n"
				"\t\t\t\"" + className + " - " + field.name + " must not be empty.\"\n"
				"\t\t);\n"
				"\t}\n"
				"\n"
			);
		} else if(field.type == "numeric"){
			ret.append(
				"\t// Validate numeric twine field\n"
				"\t" + field.name + " = Statics::VerifyNumeric( " + field.name + " );\n"
				"\n"
			);
		} else if(field.type == "number"){
			if(!field.min.empty()){
				ret.append(
					"\t// Validate minimum value\n"
					"\tif(" + field.name + " < " + field.min + "){\n"
					"\t\tthrow AnException(EXCEPTION_HANDLED_VALIDATION, FL,\n"
					"\t\t\t\"" + className + " - " + field.name + " must be at least " + field.min + ".\"\n"
					"\t\t);\n"
					"\t}\n"
					"\n"
				);
			}
			if(!field.max.empty()){
				ret.append(
					"\t// Validate maximum value\n"
					"\tif(" + field.name + " > " + field.max + "){\n"
					"\t\tthrow AnException(EXCEPTION_HANDLED_VALIDATION, FL,\n"
					"\t\t\t\"" + className + " - " + field.name + " must be no more than " + field.max + ".\"\n"
					"\t\t);\n"
					"\t}\n"
					"\n"
				);
			}
		} else if(field.type == "childArray"){
			ret.append(
				"\t// Validate child array objects\n"
				"\tfor(auto c : *" + field.name + ") c->" + field.function + "();\n"
				"\n"
			);
		}
	}

	ret.append(
		"}\n"
		"\n"
	);
	return ret;
}

