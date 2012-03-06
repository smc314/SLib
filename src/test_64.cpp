
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

/* ******************************************************* */
/* Simple program to test the functions in base64.cpp      */
/* ******************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Base64.h"
using namespace SLib;

int main (void)
{
	char tmp[2048];

	memset(tmp, 0, 2048);
	
	sprintf(tmp, "hithere:yothere");

	printf("Encoded version of (%s) is (%s)\n",
		tmp, Base64::encode(tmp));
}
