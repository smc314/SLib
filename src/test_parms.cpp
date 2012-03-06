
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

#include "Parms.h"
#include "twine.h"
using namespace SLib;

#include <vector>
using namespace std;

int main(int argc, char** argv)
{
	Parms our_parms;
	twine value;

	our_parms.SetShortFlags("-a,-b,-c,-d");
	our_parms.SetShortValues("-e,-f,-g,-h,-i");

	our_parms.SetLongFlags("--one,--two,--three");
	our_parms.SetLongValues("--four,--five,--six,--seven");

	our_parms.Parse(argc, argv);

	if(our_parms.ShortFlag("-a")){
		printf("-a flag was found\n");
	} else {
		printf("-a flag was not found\n");
	}

	if(our_parms.ShortFlag("-b")){
		printf("-b flag was found\n");
	} else {
		printf("-b flag was not found\n");
	}

	if(our_parms.ShortFlag("-c")){
		printf("-c flag was found\n");
	} else {
		printf("-c flag was not found\n");
	}

	if(our_parms.ShortFlag("-d")){
		printf("-d flag was found\n");
	} else {
		printf("-d flag was not found\n");
	}

	value = our_parms.ShortValue("-e");
	printf("-e value is (%s)\n", value());

	value = our_parms.ShortValue("-f");
	printf("-f value is (%s)\n", value());

	value = our_parms.ShortValue("-g");
	printf("-g value is (%s)\n", value());

	value = our_parms.ShortValue("-h");
	printf("-h value is (%s)\n", value());

	value = our_parms.ShortValue("-i");
	printf("-i value is (%s)\n", value());

	if(our_parms.LongFlag("--one")){
		printf("--one flag was found\n");
	} else {
		printf("--one flag was not found\n");
	}

	if(our_parms.LongFlag("--two")){
		printf("--two flag was found\n");
	} else {
		printf("--two flag was not found\n");
	}

	if(our_parms.LongFlag("--three")){
		printf("--three flag was found\n");
	} else {
		printf("--three flag was not found\n");
	}

	value = our_parms.LongValue("--four");
	printf("--four value is (%s)\n", value());

	value = our_parms.LongValue("--five");
	printf("--five value is (%s)\n", value());

	value = our_parms.LongValue("--six");
	printf("--six value is (%s)\n", value());

	value = our_parms.LongValue("--seven");
	printf("--seven value is (%s)\n", value());

	vector < twine > &simples = our_parms.Simples();
	for(int i = 0; i < (int)simples.size(); i++){
		printf("Simple[%d] = (%s)\n", i, simples[i].c_str());
	}

	return 0;

}



















