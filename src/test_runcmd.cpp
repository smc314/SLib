
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
	if(argc < 2){
		printf("usage: %s target_command arg1 arg2...\n", argv[0] );
		exit(EXIT_FAILURE);
	}
	twine cmd = argv[1];
	vector<twine> args;
	for(size_t i = 2; i < argc; i++){
		args.push_back( twine( argv[i] ) );
	}

	printf("running command: %s with args:\n", cmd() );
	for(size_t i = 0; i < args.size(); i++){
		printf("\t%s\n", args[ i ]() );
	}

	int exitCode;
	vector<twine> output = Tools::RunCommand( cmd, args, "", &exitCode );

	printf("Run Completed with exit code %d - output is:\n", exitCode);
	for(size_t i = 0; i < output.size(); i++){
		printf("%s\n", output[ i ]() );
	}
	printf("--- End of Output ---\n");
}
