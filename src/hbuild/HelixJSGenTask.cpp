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
#include "HelixJSGenTask.h"
#include "HelixWorker.h"
using namespace Helix::Build;


HelixJSGenTask::HelixJSGenTask() 
{
	EnEx ee(FL, "HelixJSGenTask::HelixJSGenTask()");

}

HelixJSGenTask::HelixJSGenTask(const HelixJSGenTask& c)
{
	EnEx ee(FL, "HelixJSGenTask::HelixJSGenTask(const HelixJSGenTask& c)");

}

HelixJSGenTask& HelixJSGenTask::operator=(const HelixJSGenTask& c)
{
	EnEx ee(FL, "HelixJSGenTask::operator=(const HelixJSGenTask& c)");

	return *this;
}

HelixJSGenTask::HelixJSGenTask(const HelixJSGenTask&& c)
{
	EnEx ee(FL, "HelixJSGenTask::HelixJSGenTask(const HelixJSGenTask&& c)");

}

HelixJSGenTask& HelixJSGenTask::operator=(const HelixJSGenTask&& c)
{
	EnEx ee(FL, "HelixJSGenTask::operator=(const HelixJSGenTask&& c)");


	return *this;
}

HelixJSGenTask::~HelixJSGenTask()
{
	EnEx ee(FL, "HelixJSGenTask::~HelixJSGenTask()");

}

void HelixJSGenTask::Generate(const twine& app)
{
	EnEx ee(FL, "HelixJSGenTask::Generate(const twine& app)");

	twine cmd;

	// First build the default version of the application, which is usually source-hybrid
	cmd = "cd ../../qd/" + app + " && ./generate.py ";
	INFO(FL, "Generating %s - default", app() );
	int ret = std::system( cmd() );
	if(ret != 0){
		throw AnException(0, FL, "Generate task failed!\nCommand is:\n%s", cmd() );
	}

	// Then build the build version of the application
	cmd = "cd ../../qd/" + app + " && ./generate.py build";
	INFO(FL, "Generating %s - build", app() );
	ret = std::system( cmd() );
	if(ret != 0){
		throw AnException(0, FL, "Generate task failed!\nCommand is:\n%s", cmd() );
	}
}

