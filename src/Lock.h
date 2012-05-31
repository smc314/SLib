#ifndef LOCK_H
#define LOCK_H
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

#include "Mutex.h"

namespace SLib {

/**
  * @memo Very simple class to make locking and unlocking mutexes very simple,
  *       even in the presence of exceptions and other non-linear control
  *       mechanisms.
  * @doc  Very simple class to make locking and unlocking mutexes very simple,
  *       even in the presence of exceptions and other non-linear control
  *       mechanisms.
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class Lock
{
	public:
		/**
		  * @memo This version of the constructor expects to be given
		  *       a mutex to be locked immediately.
		  * @doc  This version of the constructor expects to be given
		  *       a mutex to be locked immediately.
		  */
		Lock(SLib::Mutex *the_mutex) {
			m_has_mut = 1;
			m_mut = the_mutex;
			m_mut->lock();
		}

		/**
		  * @memo This version of the constructor allows for delayed
		  *       acceptance and locking of the mutex.
		  * @doc  This version of the constructor allows for delayed
		  *       acceptance and locking of the mutex.
		  */
		Lock() {
			m_has_mut = 0;
			m_mut = 0;
		}

		/**
		  * @memo The destructor handles unlocking of the mutex.
		  * @doc  The destructor handles unlocking of the mutex.  This
		  *       is done so that this object will unlock the mutex
		  *       when destroyed.  This allows automatic versions
		  *       of this class to be used even in the case of 
		  *       exceptions.
		  */
		virtual ~Lock() {
			if(m_has_mut == 1){
				m_mut->unlock();
			}
		}

		/**
		  * @memo This function is used in conjunction with the delayed
		  *       version of the constructor to set the mutex.
		  * @doc  This function is used in conjunction with the delayed
		  *       version of the constructor to set the mutex.
		  */
		void SetMutex(SLib::Mutex *the_mutex) {
			m_has_mut = 1;
			m_mut = the_mutex;
			m_mut->lock();
		}

		/** This allows for manual unlocking of the mutex.
		  */
		void UnLock() {
			if(m_has_mut == 1){
				m_mut->unlock();
				m_has_mut = 0;
			}
		}

	private:

		int m_has_mut;
		SLib::Mutex *m_mut;
};

} // End Namespace

#endif // LOCK_H Defined.

