
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

#include "AnException.h"
#include "File.h"
using namespace SLib;

int main (void)
{
	try {
		// Absolute path on windows:
		File::EnsurePath( "c:\\slib\\file\\ensure\\path" );

		// Tiny local directory
		File::EnsurePath( "a" );

		// Relative path
		File::EnsurePath( "../../slib/file/ensure/path" );

		// Absolute Path for unix
		File::EnsurePath( "/slib_root/file/ensure/path" );

	} catch (AnException& e){
		printf("Exception caught: %s\n", e.Msg() );
	}
}

