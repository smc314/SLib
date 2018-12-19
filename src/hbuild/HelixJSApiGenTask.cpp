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

#include "HelixFS.h"
#include "HelixJSApiGenTask.h"
#include "HelixWorker.h"
using namespace Helix::Build;


HelixJSApiGenTask::HelixJSApiGenTask() 
{
	EnEx ee(FL, "HelixJSApiGenTask::HelixJSApiGenTask()");

}

HelixJSApiGenTask::HelixJSApiGenTask(const HelixJSApiGenTask& c)
{
	EnEx ee(FL, "HelixJSApiGenTask::HelixJSApiGenTask(const HelixJSApiGenTask& c)");

}

HelixJSApiGenTask& HelixJSApiGenTask::operator=(const HelixJSApiGenTask& c)
{
	EnEx ee(FL, "HelixJSApiGenTask::operator=(const HelixJSApiGenTask& c)");

	return *this;
}

HelixJSApiGenTask::HelixJSApiGenTask(const HelixJSApiGenTask&& c)
{
	EnEx ee(FL, "HelixJSApiGenTask::HelixJSApiGenTask(const HelixJSApiGenTask&& c)");

}

HelixJSApiGenTask& HelixJSApiGenTask::operator=(const HelixJSApiGenTask&& c)
{
	EnEx ee(FL, "HelixJSApiGenTask::operator=(const HelixJSApiGenTask&& c)");


	return *this;
}

HelixJSApiGenTask::~HelixJSApiGenTask()
{
	EnEx ee(FL, "HelixJSApiGenTask::~HelixJSApiGenTask()");

}

void HelixJSApiGenTask::Generate(const twine& app)
{
	EnEx ee(FL, "HelixJSApiGenTask::Generate(const twine& app)");

	twine output;
	try {
		File apiBase("../../qd/" + app + "/source/class/" + app + "/Api.base");
		output = apiBase.readContentsAsTwine();
	} catch (AnException& e){
		WARN(FL, "Error reading Api.base for package %s:\n%s", app(), e.Msg() );
		return;
	}

	// Find all of the cpp files in our filesystem and extract the api generation tags from them
	for(auto file : HelixFS::getInstance().FindFilesByType( ".cpp" )){
		for(auto& api : file->Apis()){
			vector<twine> splits = api.first.split("/");
			twine shortApi = splits[ splits.size() - 1 ];
		
			output.append(
				"\t\t/** This function will call the " + api.first + "\n"
				"\t\t  * server API.\n"
				"\t\t  */\n"
				"\t\t" + shortApi + " : function ( "
			);
			
			size_t howMany = api.second.get_int();
			if(api.second == "NULL"){
				// No inputs.
				output.append("\n");
			} else if(api.second == "ID"){
				output.append("idVal,\n");
			} else if(howMany > 0){
				for(size_t i = 0; i < howMany; i++){
					twine tmp; tmp.format("inputObj%d, ", i);
					output.append(tmp);
				}
				output.append("\n");
			} else {
				output.append("inputObj,\n");
			}

			output.append(
				"\t\t\tcompletionFunction, theThis, errorCallback\n"
				"\t\t){\n"
				"\t\t\t// First build the request XML Document\n"
				"\t\t\tvar requestDoc = qx.xml.Document.create(null, \"" + shortApi + "\");\n"
			);

			if(api.second== "NULL"){
				// No inputs
			} else if(api.second== "ID"){
				output.append(
					"\t\t\tvar requestRoot = requestDoc.documentElement;\n"
					"\t\t\trequestRoot.setAttribute(\"id\", idVal);\n"
				);
			} else if(howMany > 0){
				output.append( "\t\t\tvar requestRoot = requestDoc.documentElement;\n" );
				for(size_t i = 0; i < howMany; i++){
					twine tmp; tmp.format("inputObj%d", i);
					output.append( 
						"\t\t\tif( " + tmp + " !== undefined && " + tmp + " !== null){\n" 
						"\t\t\t\t" + tmp + ".createXMLElement( requestRoot );\n"
						"\t\t\t}\n"
					);
				}
			} else {
				output.append(
					"\t\t\tvar requestRoot = requestDoc.documentElement;\n"
					"\t\t\tinputObj.createXMLElement( requestRoot );\n"
				);
			}

			output.append(
				"\n"
				"\t\t\t// Now send the request to the server.\n"
				"\t\t\t" + app + ".Api.SendRequest(requestDoc, \"" + api.first + "\",\n"
				"\t\t\t\tcompletionFunction, theThis, errorCallback);\n"
				"\t\t},\n"
				"\n"
			);

		}
	}

	output.append(
		"\t\t// Dummy function to end the file\n"
		"\t\tDontUseDummyFunction : function ()\n"
		"\t\t{\n"
		"\n"
		"\t\t}\n"
		"\n"
		"\t}\n"
		"});\n"
	);

	twine outputFile = "../../qd/" + app + "/source/class/" + app + "/Api.js";
	File::writeToFile(outputFile, output );
	
}

