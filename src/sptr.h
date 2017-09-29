#ifndef SPTR_H
#define SPTR_H
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

#include "dptr.h"

/**
  * A Convenience template for deleting a vector of pointers.  All of the
  * pointers in the vector will be deleted, and then the vector itself
  * will be deleted.
  */
template <class T>
class VectorDelete {
	public:
		void operator()(vector<T*>* v) const {
			for(auto item : *v){
				if(item != NULL){
					delete item;
				}
			}
			delete v;
		}
};

/**
  * @memo This template defines our smart pointer object.  This is used
  *       to safely handle pointers to memory and objects with a built-in
  *       knowledge of destruction practices.
  * @doc  This template defines our smart pointer object.  This is used
  *       to safely handle pointers to memory and objects with a built-in
  *       knowledge of destruction practices.
  *       <P>
  *       The way to use this object is to declare it statically in your
  *       functions that need to use pointers to anything.  Then assign
  *       the memory pointers, etc, to this and when this object goes
  *       out of scope, it will handle the deletion of the memory that
  *       was given to it automatically.
  *       <P>
  *       The first argument for this template is the type that this
  *       will hold.  This must be a NON-pointer type.  We will handle the
  *       construction of the pointer/non-pointer stuff in this class.
  *       The second argument is 
  *       a function pointer to the function that is capable of deleteing 
  *       the object or memory that this object is holding.
  *       <P>
  *       Here is an example.  If we have an object A that we want to create
  *       vectors of pointers of, then we can define a delete function
  *       like:
  *       <pre>
          void delete_vect_a(vector < A* > *v)
          {
                  for(int i=0; i < (int)v->size(); i++){
                          delete (*v);
                  }
                  delete v;
          }
          </pre>
  *       This function can handle deleteing any vector given to it as well
  *       as the contents of the vector.  If we want an sptr to automatically
  *       invoke this function when the vector needs to be deleted, we would
  *       define the sptr variable like this:
  *       <pre>
          sptr < vector < A* >, delete_vect_a> a_vect;
          </pre>
  *       Then any time we assign a vector of A*'s to the a_vect variable, 
  *       the vector is guaranteed to be deleted properly when the a_vect
  *       variable is re-assigned to a new vector, or when it goes out of
  *       scope.
  *       <P>
  *       Another thing to keep in mind is that the "delete function" doesn't
  *       actually have to delete the object contained in the sptr.  It just
  *       has to guarantee that the sptr is safely relinquishing ownership
  *       of the object.  Most of the time this is synonymous with deletion.
  *       However, there is a good example for the case of not wanting to
  *       delete the object.  That case is when you get an object for
  *       temporary use from a pool, and then want to ensure that the object
  *       is given back to the pool in all circumstances.
  *       <P>
  *       In this case you would write a function like:
  *       <pre>
          void release_to_pool(PooledObject *po)
          {
                  global_pool->Release(po);
          }
          </pre>
  *       Any object passed to this function is released back to the global
  *       pool.  Your sptr variable would then look like:
  *       <pre>
          sptr < PooledObject, release_to_pool> pool_var;
          </pre>
  *       You would initialize it with something like:
          <pre>
          pool_var = global_pool->Acquire();
          </pre>
  *       And you would be guaranteed that upon pool_var going out of scope,
  *       or being re-assigned to something else, that the PooledObject would
  *       be released back to the pool.
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
template <class T, void (*delete_fcn)(T*) >
class sptr : public dptr<T>
{
	public:
		typedef void(*sptr_delete)(T*);

		/**
		  * @memo This constructor simply prepares a blank sptr.
		  * @doc  This constructor simply prepares a blank sptr.
		  */
		sptr() : dptr<T>() { }

		/**
		  * @memo This constructor accepts the pointed to object.
		  * @doc  This constructor accepts the pointed to object.
		  */
		sptr(T* a) : dptr<T>(a) { }

		/**
		  * @memo Copy constructor transferrs ownership of the object
		  *       to the destination.
		  * @doc  Copy constructor transferrs ownership of the object
		  *       to the destination.
		  */
		sptr(const sptr& a) : dptr<T>(a) { }

		/**
		  * @memo Copy operation from another sptr to this one means
		  *       that we must first delete our own object, and then
		  *       accept the transferr of ownership from the target.
		  * @doc  Copy operation from another sptr to this one means
		  *       that we must first delete our own object, and then
		  *       accept the transferr of ownership from the target.
		  */
		sptr& operator=(const sptr& a) {
			if(&a == this){
				return *this;  // benign action for self copy.
			}
			
			// use the supplied delete function on our object:
			if(m_ptr != NULL)
				delete_fcn(m_ptr);	
			
			// Then transferr ownership and return.
			m_ptr = a.release();
			return *this;
		}

		/**
		  * @memo Assignment operation from the type that we are to
		  *       own.  We first delete any object that we currently
		  *       own and then take ownership of the indicated object.
		  * @doc  Assignment operation from the type that we are to
		  *       own.  We first delete any object that we currently
		  *       own and then take ownership of the indicated object.
		  */
		sptr& operator=(T* a) {
			if(a == m_ptr){
				return *this;  // benign action for NULL op
			}
			
			// use the supplied delete function on our object:
			if(m_ptr != NULL)
				delete_fcn(m_ptr);	
			
			// Then transferr ownership and return.
			m_ptr = a;
			return *this;
		}

		/** 
		  * @memo The destructor handles deleting our owned object.
		  * @doc  The destructor handles deleting our owned object.
		  */
		virtual ~sptr() {
			if(m_ptr != NULL){
				delete_fcn(m_ptr);
				m_ptr = NULL;
			}
		}

	protected:
		using dptr<T>::m_ptr;

};


#endif // SPTR_H Defined
