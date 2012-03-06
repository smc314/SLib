#ifndef BASE64_H
#define BASE64_H
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

#include "twine.h"

/* ************************************************************ */
/* This is the header file that will define the encoding and    */
/* decoding routines for base64 representation as defined by    */
/* RFC1521.  The RFC can be found at http://www.faq.org/rfcs/rfc1521.html */
/* ************************************************************ */

namespace SLib
{

/**
  * This class contains our base64 encoding/decoding routines.
  * Base64 coding is based on RFC1521.  The RFC can be found at
  * <a href="http://www.faq.org/rfcs/rfc1521.html">
  * http://www.faq.org/rfcs/rfc1521.html</a>
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */ 
class DLLEXPORT Base64 {

	public:

		/**
		  * This method will encode the given character
		  * string into a new character string, using base64
		  * encoding.  You are responsible for freeing the memory
		  * of the new string.
		  */
		static char *encode(const char *src);

		/**
		  * This method will encode the given character
		  * string into a new character string, using base64
		  * encoding.  You are responsible for freeing the memory
		  * of the new string.
		  */
		static twine *encode(twine* src);

		/**
		  * This method will encode the given character
		  * string into a new character string, using base64
		  * encoding.  You are responsible for freeing the memory
		  * of the new string.
		  */
		static twine *encode(twine& src);

		/**
		  * This method will encode the given character
		  * string into the given target string, using base64
		  * encoding.  You are responsible for allocating enough
		  * space and providing the target character string.
		  */
		static void encode(const char* src, char* dst);

		/**
		  * This method will decode the given character
		  * string into a new character string, using base64
		  * decoding.  You are responsible for freeing the memory
		  * of the new string.
		  */
		static char *decode(const char* src);

		/**
		  * This method will decode the given character
		  * string into a new character string, using base64
		  * decoding.  You are responsible for freeing the memory
		  * of the new string.
		  */
		static twine *decode(twine* src);

		/**
		  * This method will decode the given character
		  * string into a new character string, using base64
		  * decoding.  You are responsible for freeing the memory
		  * of the new string.
		  */
		static twine *decode(twine& src);

		/**
		  * This method will decode the given character
		  * string into the given target string, using base64
		  * decoding.  You are responsible for allocating enough
		  * space and providing the target character string.
		  */
		static void decode(const char* src, char* dst);

		/**
		  * Use this to free a twine* returned from this class.
		  */
		static void Free(twine* t);

		/**
		  * Use this to free a char* returned from this class.
		  */
		static void Free(char* c);

};

} // End Namespace.

#endif /* BASE64_H Defined */

