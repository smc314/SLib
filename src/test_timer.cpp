
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

#include "Timer.h"
using namespace SLib;

int main (void)
{
	int i;
	float f;

	Timer t;

	t.Start();
	for(i = 0; i < 1000000; i++){
		f = i * 2.3;
	}
	t.Finish();
	printf("Time for 100 float mults is (%f)\n", t.Duration());

	t.Start();
	for(i = 0; i < 100000000; i++){
		f = i * 2.3;
	}
	t.Finish();
	printf("Time for 10000 float mults is (%f)\n", t.Duration());

}

