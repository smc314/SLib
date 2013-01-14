
/*
 * Copyright (c) 2001,2002 Steven M. Cherry. All rights reserved.
 *
 * This file is a part of slib - a c++ utility library
 *
 * The slib project, including all files needed to compile 
 * it, is free software; you can redistribute it and/or use it and/or modify 
 * it under the terms of the GNU Lesser General Public License as published by 
 * the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this program.  See file COPYING for details.
 */

#include <stdlib.h>
#include <stdio.h>

#include "Socket.h"
#include "Log.h"
#include "Tools.h"
using namespace SLib;

int main (void)
{
	// Turn all logging on for this test
	Log::SetPanic(true);
	Log::SetError(true);
	Log::SetWarn(true);
	Log::SetInfo(true);
	Log::SetDebug(true);
	Log::SetTrace(true);

	try {
		INFO(FL, "Creating a server socket.");
		Socket* s = new Socket(8092);

		INFO(FL, "Waiting 5 seconds.");
		Tools::ssleep( 5 );

		INFO(FL, "Shutting down socket.");
		delete s;

	} catch (AnException& e){
		ERRORL(FL, "Exception caught: %s", e.Msg() );
	}

	INFO(FL, "Test Complete.");
}

