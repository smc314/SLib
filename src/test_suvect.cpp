
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
#include <string.h>

#include "twine.h"
using namespace SLib;

#include "suvector.h"
using namespace std;

int main (void)
{
	suvector < twine > my_vect;

	my_vect.push_back("one");
	my_vect.push_back("one");
	my_vect.push_back("three");
	my_vect.push_back("three");
	my_vect.push_back("five");
	my_vect.push_back("five");
	my_vect.push_back("seven");
	my_vect.push_back("seven");
	my_vect.push_back("nine");
	my_vect.push_back("nine");
	my_vect.push_back("two");
	my_vect.push_back("two");
	my_vect.push_back("four");
	my_vect.push_back("four");
	my_vect.push_back("six");
	my_vect.push_back("six");
	my_vect.push_back("eight");
	my_vect.push_back("eight");

	for(int i = 0; i < (int)my_vect.size(); i++){
		printf("my_vect[%d] = (%s)\n", i, my_vect[i].c_str());
	}
}


