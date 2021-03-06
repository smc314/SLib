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
		  * string into the given target string, using base64
		  * encoding.  You are responsible for allocating enough
		  * space and providing the target character string.
		  */
		static void encode(const char* src, char* dst);

		/**
		  * This method will encode the given character
		  * string into the given target string, using base64
		  * encoding.  You are responsible for allocating enough
		  * space and providing the target character string.
		  * This version of the encode does not do a strlen on the input
		  * src buffer.  You must tell us explicitly how much of the buffer
		  * you want read and encoded.
		  */
		static void encode(const char* src, size_t src_len, char* dst);

		static char* encode(const char* src, size_t src_len, size_t* ouput_len);

		/**
		  * This method will decode the given character
		  * string into a new character string, using base64
		  * decoding.  You are responsible for freeing the memory
		  * of the new string.
		  */
		static char *decode(const char* src);

		/**
		  * This method will decode the given character
		  * string into the given target string, using base64
		  * decoding.  You are responsible for allocating enough
		  * space and providing the target character string.
		  * Returns how much data was decoded into dst.
		  */
		static size_t decode(const char* src, char* dst);

		static char* decode(const char* src, size_t src_len, size_t* output_len);

		/**
		  * Use this to free a char* returned from this class.
		  */
		static void Free(char* c);

		static void BuildDecodingTable();

};

} // End Namespace.

#endif /* BASE64_H Defined */

