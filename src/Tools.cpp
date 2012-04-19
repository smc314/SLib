
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

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#	include <sys/types.h>
#	include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "Tools.h"

using namespace SLib;

void Tools::sleep(int mills)
{
#ifdef _WIN32
	Sleep(mills / 1000);
#else
	struct timeval tv;
	tv.tv_sec = mills / 1000000;
	tv.tv_usec = mills % 1000000;
	select(0, NULL, NULL, NULL, &tv);
#endif
}

void Tools::rtrim(char *s)
{
	size_t i;
	i = strlen(s) -1;
	while(s[i] == ' '){
		s[i] = '\0';
		i --;
	}
}

void Tools::char_free(char *s)
{
	free(s);
}

int Tools::rand(int max, int min)
{
	int ret;
	ret=min+(int) ( ((float)max-min) * (::rand()/(RAND_MAX+1.0)) );	
	return ret;
}

