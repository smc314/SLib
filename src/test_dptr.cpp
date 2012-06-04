
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

#include <vector>
using namespace std;

#include "AnException.h"
#include "dptr.h"
#include "memptr.h"
#include "sptr.h"
#include "xmlinc.h"
using namespace SLib;

int main (void)
{
	try {
		printf("Starting here...\n");
		dptr<vector<int> > v = new vector<int>();
		memptr<char> c = (char*)malloc(256);
		
		sptr<xmlDoc, xmlFreeDoc> doc = xmlParseFile( "testmaster.xml" );

		printf("End of try block...\n");
	} catch (AnException& e){
		printf("test_dptr failed with: %s\n", e.Msg() );
	} catch (...){
		printf("test_dptr failed with unknown error\n");
	}

	printf("We're Done.\n");
	fflush(stdout);
}

