#ifndef THREAD_H
#define THREAD_H
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
#	ifndef DLLEXPORT
#		define DLLEXPORT __declspec(dllexport)
#       endif
#else
#	define DLLEXPORT 
#endif

#ifdef _WIN32
#	include <windows.h>
#	define CURRENT_THREAD_ID GetCurrentThreadId()
#	define THREAD_ID_TYPE unsigned int
#else
#	include <pthread.h>
#	include <setjmp.h>
#	define CURRENT_THREAD_ID pthread_self()
#       define THREAD_ID_TYPE pthread_t
#endif


namespace SLib
{

class Thread;

/**
  * @memo This is the thread argument structure.
  * @doc  This structure is used internally to make the passing of arguments
  *       and other things to thread starting points a much easier process.
  * @name thread_arg
  * @author Steven M. Cherry
  * @version $Revision: 1.2 $
  * @copyright 2002 Steven M. Cherry
  */
typedef struct
{
	void *(*jmp)(void *);
	void *argList;
}
thread_arg;

} // End Namespace

/* ******************************************************* */
/* This silly end/begin of the same namespace is necessary */
/* because doc++ looses the namespace after the first      */
/* definition inside it.                                   */
/* ******************************************************* */

namespace SLib
{

/**
  * @memo This is the thread class that provides generic threads cross-platform.
  * @doc  This class is used as the common interface for our application to
  *       provide cross-platform thread capabilities.
  * @author Steven M. Cherry
  * @version $Revision: 1.2 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT Thread
{
	public:
		/**
		  * @memo The normal constructor for a thread.
		  * @doc  A thread is easy to create.  There are no special
		  *       arguments or anything.  Just instantiate, and use.
		  */
		Thread();

		/**
		  * @memo The destructor will take care of stopping the thread
		  *       if necessary.
		  */
		virtual ~Thread();

		/**
		  * @memo This is how you start a thread going.
		  * @doc  This is the method that you use to start your thread
		  *       process going.  You must pass in the starting point
		  *       for where your thread will begin life, and any args
		  *       that it might need.
		  * @param jmpPoint This is the function pointer that we will
		  *                 jump to in order to start your thread.
		  * @param argList This is a void pointer that can be anything
		  *                you need to pass into your thread.
		  * @param attr This lets you set thread attributes.  It is
		  *             almost always NULL.
		  */
		int start(void *(*jmpPoint)(void *), void *argList,
		          void *attr = NULL);

		/**
		  * @memo This is how you stop a thread from processing.
		  */
		int cancel(void);

		/**
		  * @memo This is a blocking function that will wait for the
		  *       thread to finish before returning.
		  */
		int join(void **ret_status = NULL);

		/**
		  * @memo This tells the thread that you don't care when it
		  *       finishes, you will not be watching.
		  */
		int detach(void);

		/**
		  * @memo This is how you find out what the current thread ID
		  *       is set to.
		  */
		THREAD_ID_TYPE ID(void);

		/** Use this to retrieve the current thread ID.
		 */
		static THREAD_ID_TYPE CurrentThreadId(void);

	protected:

#ifdef _WIN32
		HANDLE m_thread;
		unsigned int m_tid;
#else
		pthread_t m_thread;
#endif

		int m_status; // 0 = not started, 1 = o.k., -1 = error in start.
		thread_arg *m_arg;

};

} // End namespace.

#endif /* THREAD_H Defined */
