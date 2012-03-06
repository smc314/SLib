#ifndef MEMPTR_H
#define MEMPTR_H
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

/**
  * @doc  This template defines our smart memory buffer object.  This is used
  *       to safely handle memory buffers and ensure that they are free()'d when
  *       they go out of scope.
  *       <P>
  *       The way to use this object is to declare it statically in your
  *       functions that need to use memory buffers.  Then assign
  *       the memory pointers, etc, to this and when this object goes
  *       out of scope, it will handle the deletion of the memory that
  *       was given to it automatically.
  *       <P>
  *       The argument for this template is the type that this
  *       will hold.  This must be a NON-pointer type.  We will handle the
  *       construction of the pointer/non-pointer stuff in this class.
  *       <P>
  *       The buffer will be free()'d using the standard free() call..
  * @author Steven M. Cherry
  * @copyright 2011 Steven M. Cherry
  */
template <class T >
class memptr 
{
	public:

		/**
		  * @memo This constructor simply prepares a blank memptr.
		  * @doc  This constructor simply prepares a blank memptr.
		  */
		memptr() {
			m_ptr = NULL;
		}

		/**
		  * @memo This constructor accepts the pointed to object.
		  * @doc  This constructor accepts the pointed to object.
		  */
		memptr(T* a) {
			m_ptr = a;
		}

		/**
		  * @memo Copy constructor transferrs ownership of the object
		  *       to the destination.
		  * @doc  Copy constructor transferrs ownership of the object
		  *       to the destination.
		  */
		memptr(memptr& a) {
			m_ptr = a.release();
		}	

		/**
		  * @doc  Copy operation from another memptr to this one means
		  *       that we must first free our own object, and then
		  *       accept the transferr of ownership from the target.
		  */
		memptr& operator=(memptr& a) {
			if(&a == this){
				return *this;  // benign action for self copy.
			}
			
			// use the standard free() function on our object:
			if(m_ptr != NULL)
				free(m_ptr);	
			
			// Then transferr ownership and return.
			m_ptr = a.release();
			return *this;
		}

		/**
		  * @doc  Assignment operation from the type that we are to
		  *       own.  We first free any buffer that we currently
		  *       own and then take ownership of the indicated object.
		  */
		memptr& operator=(T* a) {
			if(a == m_ptr){
				return *this;  // benign action for NULL op
			}
			
			// use the supplied free function on our object:
			if(m_ptr != NULL)
				free(m_ptr);	
			
			// Then transferr ownership and return.
			m_ptr = a;
			return *this;
		}

		/** 
		  * @memo The destructor handles deleting our owned object.
		  * @doc  The destructor handles deleting our owned object.
		  */
		virtual ~memptr() {
			if(m_ptr != NULL)
				free(m_ptr);
		}

		/**
		  * @memo Cast Operator
		  * @doc  Cast Operator
		  */
		virtual operator T*() const {
			return m_ptr;
		}

		/**
		  * @memo Operator *
		  * @doc  Operator *
		  */
		virtual T& operator*() const {
			return *m_ptr;
		}

		/**
		  * @memo Operator ->
		  * @doc  Operator ->
		  */
		virtual T* operator->() const {
			return m_ptr;
		}
		
		/**
		  * @memo Operator () This is easier to type than .get()
		  * @doc  Operator () This is easier to type than .get()
		  */
		virtual T* operator()() const {
			return m_ptr;
		}
		
		/**
		  * @memo Allows access directly to the object.
		  * @doc  Allows access directly to the object.
		  */
		virtual T* get(void) {
			return m_ptr;
		}

		/**
		  * @memo Relinquishes ownership of the object.
		  * @doc  Relinquishes ownership of the object.
		  */
		virtual T* release(void){
			T* tmp;
			tmp = m_ptr;	
			m_ptr = NULL;
			return tmp;
		}
	


	protected:

		T* m_ptr;
			
};


#endif // MEMPTR_H Defined
