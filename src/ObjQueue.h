#ifndef OBJQUEUE_H
#define OBJQUEUE_H
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
#include "AnException.h"

namespace SLib
{

/**
  * This is a fifo queue that is based around the list stl structure.
  * list is a doubly-linked list that we will use as the storage for
  * our fifo processes.  Since this is intended to exist in a threaded
  * environment, we implement mutex exclusion to all activities on the
  * queue.
  * <P>
  * This queue differs from MsgQueue in that it expects it's queue
  * elements to be non-pointer types.  When deleteing the queue, it
  * will not delete any elements on the queue.  This will be fine for
  * non-pointer objects, but pointer objects will be leaked by this
  * queue.  Use MsgQueue for pointer objects.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
template < class MsgData > 
class ObjQueue {

	public:
		/**
		  * The standard constructor intializes our mutex,
		  * and list structures and gets everything ready for
		  * use.
		  */
		ObjQueue() {
			m_queue = new list < MsgData > ;
			m_mut = new Mutex();
		}

		/**
		  * The destructor will lock the queue, and then clean
		  * up everything in the queue, ulock the queue, and exit.
		  */
		virtual ~ObjQueue() {
			{ // for scope
				Lock alock(m_mut);
				while(!m_queue->empty()){
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
		  * Note: If the queue is empty, this will throw an
		  * exception.  Check to see if the queue has data before
		  * using this call.
		  */
		MsgData GetMsg(void) {
			Lock alock(m_mut);
			if(m_queue->empty()){
				throw AnException(0, FL, "Queue has no data");
			}
			MsgData tb = *(m_queue->begin());
			m_queue->pop_front();
			return tb;
		}

		/**
		  * Returns true if the queue is empty, or false if the
		  * queue has data.
		  */
		bool Empty(void){
			Lock alock(m_mut);
			return m_queue->empty();
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

#endif // OBJQUEUE Defined.
		
