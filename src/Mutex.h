#ifndef MUTEX_H
#define MUTEX_H
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

#include "Thread.h"

namespace SLib {
	
/**
  * @memo This is a small and simple wrapper class that provides a 
  *       consistent interface to mutex constructs.  Mutexes are very
  *       different on an OS by OS basis, and this class allows us to
  *       forget about the details and handle mutexes at a higher level.
  * @doc  This is a small and simple wrapper class that provides a 
  *       consistent interface to mutex constructs.  Mutexes are very
  *       different on an OS by OS basis, and this class allows us to
  *       forget about the details and handle mutexes at a higher level.
  *       <P>
  *       As much as possible has been in-lined in this class so that
  *       we get the benefits of a common abstraction interface without
  *       any extra overhead.
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT Mutex
{
	public:
		/**
		  * @memo The constructor creates a new, and unlocked mutex
		  *       structure that is appopriate for the OS that this
		  *       was compiled for.
		  * @doc  The constructor creates a new, and unlocked mutex
		  *       structure that is appopriate for the OS that this
		  *       was compiled for.
		  */
		Mutex() {
#ifdef _WIN32
			m_mut = CreateMutex(NULL, FALSE, NULL);
#else
			pthread_mutex_init(&m_mut, NULL);
#endif
		}

		/**
		  * @memo The destructor cleans up the memory allocated for
		  *       the mutex structure.  Note, we do NOT wait on this
		  *       mutex to be unlocked or released.  This simply
		  *       clears the mutex from memory.
		  * @doc  The destructor cleans up the memory allocated for
		  *       the mutex structure.  Note, we do NOT wait on this
		  *       mutex to be unlocked or released.  This simply
		  *       clears the mutex from memory.
		  */
		virtual ~Mutex() {
#ifdef _WIN32
			CloseHandle(m_mut);
#else
			pthread_mutex_destroy(&m_mut);
#endif
		}

		/**
		  * @memo This function will lock the mutex.
		  * @doc  This function will lock the mutex.
		  */
		void lock(void) {
#ifdef _WIN32
			WaitForSingleObject(m_mut, INFINITE);
#else
			pthread_mutex_lock(&m_mut);
#endif
		}

		/**
		  * @memo This function will unlock the mutex.
		  * @doc  This function will unlock the mutex.
		  */
		void unlock(void) {
#ifdef _WIN32
			ReleaseMutex(m_mut);
#else
			pthread_mutex_unlock(&m_mut);
#endif
		}

		/** Direct access to the mutex itself.
		  */
#ifdef _WIN32
		HANDLE* internalMutex(void) {
#else
		pthread_mutex_t* internalMutex(void) {	
#endif
			return &m_mut;
		}

	private:

#ifdef _WIN32
		HANDLE m_mut;
#else
		pthread_mutex_t m_mut;
#endif

};
				  
} // End namespace

#endif // MUTEX_H Defined
