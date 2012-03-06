#ifndef MSGQUEUE_H
#define MSGQUEUE_H
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

#include <list>
#include <utility>
using namespace std;

#include "Lock.h"

namespace SLib
{

/**
  * This is a fifo queue that is based around the list stl structure.
  * list is a doubly-linked list that we will use as the storage for
  * our fifo processes.  Since this is intended to exist in a threaded
  * environment, we implement mutex exclusion to all activities on the
  * queue.
  * <P>
  * The only restrictions on the data type that should be used with this
  * queue template is that they should be pointer types.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
template < class MsgData > 
class MsgQueue {

	public:
		/**
		  * The standard constructor intializes our mutex,
		  * and list structures and gets everything ready for
		  * use.
		  */
		MsgQueue() {
			m_queue = new list < MsgData > ;
			m_mut = new Mutex();
		}

		/**
		  * The destructor will lock the queue, and then clean
		  * up everything in the queue, ulock the queue, and exit.
		  */
		virtual ~MsgQueue() {
			{ // for scope
				Lock alock(m_mut);
				while(!m_queue->empty()){
					MsgData tb = *(m_queue->begin());
					delete tb;
					m_queue->pop_front();
				}
				delete m_queue;
			}
			delete m_mut;
		}
		 
		/**
		  * This is how a new element is added to the end of
		  * the queue.
		  */
		void AddMsg(MsgData b) {
			Lock alock(m_mut);
			m_queue->push_back(b);
		}

		/**
		  * This is how to get the first element in the queue.
		  * Note: The element returned may be NULL.  If it is 
		  * NULL, then that indicates that the queue is empty.
		  */
		MsgData GetMsg(void) {
			Lock alock(m_mut);
			if(m_queue->empty()){
				return (MsgData)NULL;
			}
			MsgData tb = *(m_queue->begin());
			m_queue->pop_front();
			return tb;
		}

		/**
		  * This method allows you to look at the first element on
		  * the queue without removing it from the queue.  If there
		  * is nothing in the queue, this will return NULL.
		  */
		MsgData Peek(void){
			Lock alock(m_mut);
			if(m_queue->empty()){
				return (MsgData)NULL;
			}
			MsgData tb = *(m_queue->begin());
			return tb;
		}

		/// Returns the current size of the queue
		int Size(void) {
			Lock alock(m_mut);
			return m_queue->size();
		}

	private:

		/**
		  * The queue structure is just a doubly linked list.
		  */
		list < MsgData > *m_queue;

		/**
		  * The mutex that we use to lock access to the queue.
		  */
		Mutex *m_mut;

};

} // End Namespace

#endif // MSGQUEUE Defined.
		
