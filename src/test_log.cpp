
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

/**
  * @memo this is a simple program that will test logging for us.
  */

#include <stdio.h>
#include <stdlib.h>

#include "Log.h"
using namespace SLib;

int main(void)
{
	printf("Setting all logging levels to true\n");
	Log::SetPanic(true);
	Log::SetError(true);
	Log::SetWarn(true);
	Log::SetInfo(true);
	Log::SetDebug(true);
	Log::SetTrace(true);

	printf("Testing channels:\n");
	TRACE(FL, "Logging initialized and main function started.");
	
	PANIC(FL, "This is a panic message. %d %d %d", 1, 2, 3);

	ERRORL(FL, "This is an error message. %d %d %d", 4, 5, 6);

	WARN(FL, "This is a warning message.");

	INFO(FL, "This is an information message.");

	DEBUG(FL, "This is a debug message.");

	TRACE(FL, "That's it.  Main is done.");

	printf("Log Test Done\n");
}
