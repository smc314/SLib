#ifndef DPTR_H
#define DPTR_H
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
  *       The argument for this template is the type that this
  *       will hold.  This must be a NON-pointer type.  We will handle the
  *       construction of the pointer/non-pointer stuff in this class.
  *       <P>
  *       The object will be destroyed using the standard delete operator.
  *       if the type of object given does not support the delete operator,
  *       then you will need to use the dptr type instead.
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
template <class T >
class dptr 
{
	public:

		/**
		  * @memo This constructor simply prepares a blank dptr.
		  * @doc  This constructor simply prepares a blank dptr.
		  */
		dptr() {
			m_ptr = NULL;
		}

		/**
		  * @memo This constructor accepts the pointed to object.
		  * @doc  This constructor accepts the pointed to object.
		  */
		dptr(T* a) {
			m_ptr = a;
		}

		/**
		  * @memo Copy constructor transferrs ownership of the object
		  *       to the destination.
		  * @doc  Copy constructor transferrs ownership of the object
		  *       to the destination.
		  */
		dptr(dptr& a) {
			m_ptr = a.release();
		}	

		/**
		  * @memo Copy operation from another dptr to this one means
		  *       that we must first delete our own object, and then
		  *       accept the transferr of ownership from the target.
		  * @doc  Copy operation from another dptr to this one means
		  *       that we must first delete our own object, and then
		  *       accept the transferr of ownership from the target.
		  */
		dptr& operator=(dptr& a) {
			if(&a == this){
				return *this;  // benign action for self copy.
			}
			
			// use the supplied delete function on our object:
			if(m_ptr != NULL)
				delete m_ptr;	
			
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
		dptr& operator=(T* a) {
			if(a == m_ptr){
				return *this;  // benign action for NULL op
			}
			
			// use the supplied delete function on our object:
			if(m_ptr != NULL)
				delete m_ptr;	
			
			// Then transferr ownership and return.
			m_ptr = a;
			return *this;
		}

		/** 
		  * @memo The destructor handles deleting our owned object.
		  * @doc  The destructor handles deleting our owned object.
		  */
		virtual ~dptr() {
			if(m_ptr != NULL)
				delete m_ptr;
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


#endif // DPTR_H Defined
