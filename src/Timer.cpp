
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

#include "Timer.h"
#include <stdio.h>

using namespace SLib;

Timer::Timer()
{
	// not much to do.
}

Timer::~Timer()
{
	// not much to do.
}

void Timer::Start(void)
{
	ftime(&m_start_time);
}

void Timer::Finish(void)
{
	ftime(&m_end_time);
}

float Timer::Duration(void)
{
	double f1, f2, f3, f4, f5;

	f1 = m_start_time.time;
	f2 = m_start_time.millitm;
	f2 /= 1000.0;

	f3 = m_end_time.time;
	f4 = m_end_time.millitm;
	f4 /= 1000.0;

	f5 = (f3+f4) - (f1+f2);

	return (float)f5;
}

uint64_t Timer::GetCycleCount(void)
{
#ifdef _WIN32
	struct timeb tb;
	ftime( &tb);
	return (uint64_t)tb.time * (uint64_t)1000000 + (uint64_t)tb.millitm * (uint64_t)1000;
#else
	struct timeval tv;
	gettimeofday( &tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
#endif
}


