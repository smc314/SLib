
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

#include "Thread.h"
#ifdef _WIN32
#include "process.h"
#endif

#include <stdlib.h>

#include "Log.h"
#include "AnException.h"
using namespace SLib;


#ifdef _WIN32
UINT __stdcall internal_thread_start_fcn(void *parms)
{
	thread_arg *args;
	TRACE(FL, "Enter UINT __stdcall internal_thread_start_fcn(void *parms)");

	args = (thread_arg *)parms;

	/* ******************************************** */
	/* Then launch the thread by calling the actual */
	/* user-supplied starting function.             */
	/* ******************************************** */
	args->jmp(args->argList);

	free(args);

	TRACE(FL, "Exit UINT __stdcall internal_thread_start_fcn(void *parms)");
	_endthreadex(0);

	return 0;
}
#endif

Thread::Thread()
{
	TRACE(FL, "Enter Thread::Thread()");
	m_status = 0;
	m_arg = (thread_arg *)malloc(sizeof(thread_arg));
	TRACE(FL, "Exit Thread::Thread()");
}

Thread::~Thread()
{
	TRACE(FL, "Enter Thread::~Thread()");

#ifndef _WIN32
	free(m_arg);
#endif

#ifdef _WIN32
	CloseHandle(m_thread);
#endif
	TRACE(FL, "Exit Thread::~Thread()");
}

int Thread::start(void *(*jmpPoint)(void *), void *argList, void *attr)
{
	TRACE(FL, "Enter Thread::start(...)");

#ifdef _WIN32
	m_arg->jmp = jmpPoint;
	m_arg->argList = argList;

	m_thread = (HANDLE)_beginthreadex(NULL, 0, internal_thread_start_fcn,
	                                  m_arg, 0, &m_tid);

	if (m_thread != 0) {
		m_status = 1;
	} else {
		m_status = -1;
		throw AnException(0, FL, "Error starting thread.");
	}
#else

	if (pthread_create(&m_thread, (pthread_attr_t *)attr, jmpPoint, argList) == 0) {
		m_status = 1;

	} else {
		m_status = -1;
		throw AnException(0, FL, "Error starting thread.");
	}
#endif

	TRACE(FL, "Exit Thread::start(...)");
	return 0;

}

int Thread::cancel(void)
{
	TRACE(FL, "Enter Thread::cancel()");
	if (m_status == 1) {
#ifdef _WIN32
		TerminateThread(m_thread, 0);
#else
		TRACE(FL, "Exit Thread::cancel()");
		return pthread_cancel(m_thread);
#endif
	} else {
		DEBUG(FL, "Should not be trying to cancel unstarted thread.");
	}
	TRACE(FL, "Exit Thread::cancel()");
	return 0;
}

int Thread::join(void **ret_status)
{
	TRACE(FL, "Enter Thread::join()");
	if (m_status == 1) {
#ifdef _WIN32
		WaitForSingleObject(m_thread, INFINITE);
#else
		TRACE(FL, "Exit Thread::join()");
		return pthread_join(m_thread, ret_status);
#endif
	} else {
		DEBUG(FL, "Should not be trying to join unstarted thread.");
	}

	TRACE(FL, "Exit Thread::join()");
	return 0;
}

int Thread::detach(void)
{
	TRACE(FL, "Enter Thread::detach()");
	if (m_status == 1) {
#ifndef _WIN32
		m_status = 0;
		TRACE(FL, "Exit Thread::detach()");
		return pthread_detach(m_thread);
#endif
	} else {
		DEBUG(FL, "Should not be trying to detach unstarted thread.");
	}

	TRACE(FL, "Exit Thread::detach()");
	return 0;
}

THREAD_ID_TYPE Thread::ID(void)
{
	TRACE(FL, "Enter Thread::ID()");
	if (m_status == 1) {
		TRACE(FL, "Exit Thread::ID()");
#ifdef _WIN32
		return m_tid;
#else
		return m_thread;
#endif
	} else {
		DEBUG(FL, "Should not be querying ID on unstarted thread.");
	}

	TRACE(FL, "Exit Thread::ID()");
	return (THREAD_ID_TYPE)-1;
}

THREAD_ID_TYPE Thread::CurrentThreadId(void)
{
	return CURRENT_THREAD_ID;
}
