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

#include "twine.h"
using namespace SLib;

void print_tokens( const twine& test );

int main (void)
{
	twine t1 = "select  something interesting\t\t\tfrom (table one two)\n\n\nand elsewhere";
	print_tokens( t1 );

	twine t2 = "    sentence with spaces at beginning and end   ";
	print_tokens( t2 );

	twine t3 = "  CSASRV  SET  TRACE  IXF  ";
	print_tokens( t3 );

	return 0;
}

void print_tokens( const twine& test )
{
	printf("Input string: %s\n", test() );

	vector < twine > tokens = test.tokenize(TWINE_WS);
	for(int i = 0; i < (int)tokens.size(); i++){
		printf("Token[%d] = (%s)\n", i, tokens[i].c_str());
	}

	printf("\n");
}