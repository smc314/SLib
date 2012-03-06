
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

int main (void)
{
	twine chars = ",a,b,c,,,d,e,f,g,";
	vector < twine > letters = chars.split(",");
	for(int i = 0; i < (int)letters.size(); i++){
		printf("Letter[%d] = (%s)\n", i, letters[i].c_str());
	}
	return 0;
}
