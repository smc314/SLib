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

#include <time.h>

#include "Mutex.h"
#include "Log.h"
#include "AnException.h"
using namespace SLib;


void Mutex::lock(long timeout) {
#ifdef _WIN32
	DWORD waitRC = WaitForSingleObject(m_mut, timeout);
	if(waitRC != WAIT_OBJECT_0 && waitRC != WAIT_ABANDONED){
		if(waitRC == WAIT_FAILED){
			throw AnException(0, FL, "Wait failed on lock.");
		} else {
			throw AnException(0, FL, "Wait timed out on lock.");
		}
	}
#else
	// get the time right now:
	struct timespec abs_time;
	clock_gettime( CLOCK_REALTIME, &abs_time );

	// Move that forward by the timeout value provided
	abs_time.tv_sec += (timeout / 1000);
	// 1 second = 1,000,000,000 nanoseconds
	// 1 millisecond = 1,000,000 nanoseconds
	abs_time.tv_nsec += ( timeout % 1000 ) * 1000000;

	// Wait on the lock until the calculated absolute time
	int ret = pthread_mutex_timedlock(&m_mut, &abs_time);
	if(ret != 0){
		throw AnException(0, FL, "Failed to acquire lock!" );
	}
#endif
}
