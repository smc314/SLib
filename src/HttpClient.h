#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
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

#include <curl/curl.h>

#include "xmlinc.h"
#include "twine.h"
using namespace SLib;

namespace SLib {

/** 
  * This class defines a simple interface to using the Curl library.
  *
  * @author Steven M. Cherry
  */
class DLLEXPORT HttpClient
{
	public:
		
		/** Default constructor */
		HttpClient();

		/// Standard Destructor
		virtual ~HttpClient();

		/// Use this to download a page using GET with no cookie or state management.
		static char* GetPage( const twine& url );

		/// Use this do download a page using GET and have it returned as an XML document.
		static xmlDocPtr GetPageXml( const twine& url );

		/// Use this to post a message to a URL and retrieve the response.
		static xmlDocPtr PostPage( const twine& url, const char* msg, size_t msgLen);

		/// Use this to post an xml document to a URL and retrieve the response.
		static xmlDocPtr PostXml( const twine& url, xmlDocPtr doc);
};

} // End Namespace SLib

#endif // HTTPCLIENT_H defined
