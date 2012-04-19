#ifndef AUTOXMLCHAR_H
#define AUTOXMLCHAR_H
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

#include <stdlib.h>
#include <string.h>


#include "xmlinc.h"


#include "AnException.h"
using namespace SLib;

/**
  * Simple class to handle the memory that is constantly given to us
  * from gnome-xml.  This will handle the casting to/from xmlChar/char as
  * necessary and also knows the proper way to destroy an xmlChar from
  * gnome-xml.
  */
class AutoXMLChar
{
	public:
		/// Standard constructor doesn't do much.
		AutoXMLChar(){ m_xml_char_ptr = NULL; }

		/**
		  * Standard destructor will invoke the appropriate function
		  * to free the memory we are holding (if any).
		  */
		virtual ~AutoXMLChar() {
			if(m_xml_char_ptr != NULL) {
				xmlFree(m_xml_char_ptr);
			}
		}

		/// Cast to a char*
		virtual operator char*() const {
			return (char*)m_xml_char_ptr;
		}

		/// Cast to an xmlChar*
		virtual operator xmlChar*() const {
			return m_xml_char_ptr;
		}

		/** 
		  * Assignment from an xmlChar*.  Frees any existing memory
		  * held by this object.
		  */
		virtual AutoXMLChar& operator=(xmlChar* a) {
			if(a == m_xml_char_ptr){
				return *this;
			} else {
				if(m_xml_char_ptr != NULL){
					xmlFree(m_xml_char_ptr);
				}
				m_xml_char_ptr = a;
			}
			return *this;
		}

		/// Function operator returns the xmlChar* 
		virtual xmlChar* operator()() const {
			return m_xml_char_ptr;
		}

		/**
		  * Duplicates the xmlChar that we hold and returns the
		  * new memory.  You are responsible for proper cleanup
		  * of this memory.  This function is in-lined so should
		  * not have any problems with cross-dll boundaries.
		  */
		virtual char* duplicate(void) const {
			char* ret;
			size_t len = strlen( (char*)m_xml_char_ptr ) + 1;
			ret = (char*)malloc(len);
			if(ret == NULL){
				throw AnException(0, FL, "Error duplicating "
					"xmlChar.  Lack of memory");
			}
			memset(ret, 0, len);
			memcpy(ret, m_xml_char_ptr, len-1);
			return ret;
		}

	private:

		xmlChar* m_xml_char_ptr;
};

#endif // AUTOXMLCHAR_H Defined		
		

