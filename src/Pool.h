#ifndef POOL_H
#define POOL_H
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

#include <vector>
#include <utility>
using namespace std;

#include "Mutex.h"

namespace SLib {

/**
  * @memo This is the template class for all pools.  It implements all of the
  *       fundamental pooling mechanisms without being specific to the type
  *       of object that is pooled.  It's template arguments specify the
  *       data type that the pool will hold, a class type that is used to
  *       initialize the pool and every object that the pool owns, a
  *       function object that does the work of creating the new pooled
  *       objects, and a function object that is capable of deleteing the
  *       pooled objects.
  * @doc  This is the template class for all pools.  It implements all of the
  *       fundamental pooling mechanisms without being specific to the type
  *       of object that is pooled.  It's template arguments specify the
  *       data type that the pool will hold, a class type that is used to
  *       initialize the pool and every object that the pool owns, a
  *       function object that does the work of creating the new pooled
  *       objects, and a function object that is capable of deleteing the
  *       pooled objects.
  *       <P>
  *       To use this pool template, you must do the following:
  *       <ul>
  *         <li>First decide on the object that you need to have pools of.
  *             This object must at a minimum support the comparison == 
  *             operator.  This is what is used to find the object when
  *             it is returned to the pool.  The pool is designed to
  *             store pointers to these objects.  So when the comparison
  *             is applied, it is done with the code:
  *             <pre>
  *                   (*obj1) == (*obj2)
  *             </pre>
  *             </li>
  *         <li>Second decide on the data that is necessary to initialize
  *             those objects.  Encapsulate this data in a single object with
  *             the capability of providing that data during the object
  *             initialization.  </li>
  *         <li>Design a function object whose application () operator will
  *             accept an object of the initialization data and return an
  *             object that will be stored in the pool.</li>
  *         <li>Design a function object whose application () operator will
  *             accept a pooled object and handle the destruction of that
  *             object.</li>
  *       </ul>
  *       Take these four objects and pass them in as parameters to the
  *       instantiation of this pool template and everything else will be
  *       handled for you.
  *       <P>
  *       Here is an example.  We want to pool objects of type PooledObject.
  *       PooledObject requires two int's and a twine to be initialized 
  *       correctly.  You would define the following objects prior to 
  *       creating the pool:
          <pre>
	// This is the object that we want to pool.  It already exists.
	class PooledObject {
		public:
			// Normal constructor
			PooledObject(int var1, int var2, twine var3);

			// Must support the == operator
			bool operator==(PooledObject& po){
				return this == &po;   // or whatever is best.
			}

			// the rest of this object
			....
	};

	// Define an initialization data object.
	class PooledObject_init {
		public:
			int var1;
			int var2;
			twine var3;
	};

	// Define a construction object.  AKA a Factory object.
	class PooledObject_create {
		public:
			PooledObject* operator()(PooledObject_init* i){
				PooledObject* po;
				po = new PooledObject(i->var1, i->var2, 
					i->var3);
				return po;
			}
	};

	// Define a deletion object.
	class PooledObject_destroy {
		public:
			void operator()(PooledObject *po) {
				delete po;  // or whatever is necessary
			}
	};
          </pre>
  *       Once you have done all of that, then create the pool, and set
  *       the initialization data in it.
          <pre>
	PooledObject_init* poi;
	poi = new PooledObject_init();
	poi->var1 = 5;
	poi->var2 = 13;
	poi->var3 = "hello there everyone :-)";

	Pool&lt;PooledObject*,               // The object type to pool
		PooledObject_init*,       // The object with init data
		PooledObject_create,      // The factory object
		PooledObject_destroy&gt;     // The destruction object
		global_pool;              // The variable name of the pool
	
	// All the objects in a pool are initialized with the same data:
	global_pool.SetInitInfo(poi);

	// Set the size of the pool:
	global_pool.SetMaxSize(10);
	global_pool.SetGrowthFactor(1);
	global_pool.SetMinSize(1);        // This forces the first object to be
	                                  // created.
		
          </pre>
  *       And that's it.  You now have a pool that will automatically 
  *       create and delete PooledObjects.  It will hand them out one at
  *       a time and accept them back when the caller is finished with them.
  *       Using this approach, any type of object may be pooled for use.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
template <class DataType, class PoolInfo, class NewDataType, class FreeDataType>
class Pool
{
	public:
		/**
		  * @memo The standard constructor provides all initialization
		  *       and construction of all of the pool data structures.
		  * @doc  The standard constructor provides all initialization
		  *       and construction of all of the pool data structures.
		  */
		Pool();

		/**
		  * @memo The standard destructor cleans up all the pool data
		  *       and all of the tracking and accounting structures.
		  * @doc  The standard destructor cleans up all the pool data
		  *       and all of the tracking and accounting structures.
		  */
		virtual ~Pool();

		void SetInitInfo(PoolInfo init_info) {
			m_init_info = init_info;
		}

		/**
		  * @memo This is the method that programmers will use to
		  *       request a new object from the pool.
		  * @doc  This is the method that programmers will use to
		  *       request a new object from the pool.
		  */
		virtual DataType Acquire(void);


		/**
		  * @memo This is the method that programmers will use to
		  *       give pooled objects back to us.
		  * @doc  This is the method that programmers will use to
		  *       give pooled objects back to us.
		  */
		virtual void Release(DataType dt);

		/**
		  * @memo This method allows removal from the pool of an 
		  *       object directly.
		  * @doc  This method allows removal from the pool of an 
		  *       object directly.  The object is destroyed and removed
		  *       from the pool.
		  */
		virtual void Remove(DataType dt);

		/**
		  * @memo This method allows others to see how many total
		  *       objects we are holding.
		  * @doc  This method allows others to see how many total
		  *       objects we are holding.  
		  */
		virtual int GetPoolSize(void);

		/**
		  * @memo This method allows other to see how many pooled
		  *       objects are in use at this point.
		  * @doc  This method allows other to see how many pooled
		  *       objects are in use at this point.
		  */
		virtual int GetInUseSize(void);

		/**
		  * @memo Allows the user to set the minimum size of the
		  *       pool.  If this is larger than the current size of
		  *       the pool, then the pool size will be grown to this
		  *       new minimum.
		  * @doc  Allows the user to set the minimum size of the
		  *       pool.  If this is larger than the current size of
		  *       the pool, then the pool size will be grown to this
		  *       new minimum.
		  */
		virtual void SetMinSize(int newmin);

		/**
		  * @memo Allows the user to set the growth factor of the
		  *       pool.  This has no immediate affect.  However, the
		  *       next time the pool is grown, this value will be 
		  *       used.
		  * @doc  Allows the user to set the growth factor of the
		  *       pool.  This has no immediate affect.  However, the
		  *       next time the pool is grown, this value will be 
		  *       used.
		  */
		virtual void SetGrowthFactor(int newgrowth);

		/**
		  * @memo Allows the user to set the maximum size of the
		  *       pool.  If this size is smaller than the current
		  *       size of the pool, then an Exception will be thrown.
		  * @doc  Allows the user to set the maximum size of the
		  *       pool.  If this size is smaller than the current
		  *       size of the pool, then an Exception will be thrown.
		  */
		virtual void SetMaxSize(int newmax);
		

	protected:

		/**
		  * @memo This is our internal function used for creating
		  *       a new pool object and adding it
		  *       to our avail list.
		  * @doc  This is our internal function used for creating
		  *       a new pool object and adding it
		  *       to our avail list.  
		  */
		void CreateNewObj(int locked=0);

		/**
		  * @memo This is the minimum number of pooled objects.  This
		  *       many objects will be initialized at startup.
		  * @doc  This is the minimum number of pooled objects.  This
		  *       many objects will be initialized at startup.
		  */
		int m_min_size;

		/**
		  * @memo This is the growth increment that we will use to
		  *       add objects when necessary.  
		  * @doc  A new batch of objects
		  *       is added to the available pool when
		  *       the code detects that it is handing out the last
		  *       object available.
		  */
		int m_grow_by;

		/**
		  * @memo This is the maximum number of objects (in use
		  *       or available) that we will grow to.
		  * @doc  This is the maximum number of objects (in use
		  *       or available) that we will grow to.
		  */
		int m_max_size;

		/**
		  * @memo This is the mutex that we use to ensure that we
		  *       have exclusive access to the pools when we edit
		  *       them.
		  * @doc  This is the mutex that we use to ensure that we
		  *       have exclusive access to the pools when we edit
		  *       them.
		  */
		Mutex *m_mut;

		/**
		  * @memo This is the list of pooled objects.
		  * @doc  This is the list of pooled objects.  The integer
		  *       member of the pair indicates whether the obj is in
		  *       use by someone or not.  
		  *       0 = available.
		  *       1 = in use.
		  *       2 = Destroyed object.  Will be re-built,
		  *           but do not use when in this status.
		  */
		vector < pair < DataType , int> * > *m_pool;

		/**
		  * @memo This is the initialization data that is given
		  *       to every object that is created for this pool.
		  * @doc  This is the initialization data that is given
		  *       to every object that is created for this pool.
		  */
		PoolInfo m_init_info;		

		/**
		  * @memo This is our instance of the creator function object.
		  * @doc  This is our instance of the creator function object.
		  */
		NewDataType m_new;	

		/**
		  * @memo This is our instance of the delete function object.
		  * @doc  This is our instance of the delete function object.
		  */
		FreeDataType m_free;	

};

} // End namespace

#include "Pool.cpp"

#endif // POOL_H defined.


