
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

/* ***************************************************************** */
/* Simple program to test the RunCommand method in in Tools.cpp      */
/* ***************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tools.h"
using namespace SLib;

int main (int argc, char** argv)
{
	printf("Argument count is %d\n", argc);
	for(int i = 0; i < argc; i++){
		printf("\tArgument %d: ---%s---\n", i, argv[i] );
	}
}
