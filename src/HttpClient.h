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
#include "MemBuf.h"
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

		/** This is the callback that cURL invokes to tell us about up/down-load progress. A return
		  * of anything other than 0 will cause the transfer to abort.  Our implementation of this
		  * does nothing but return 0.  It is expected that sub-classes will override this and do
		  * something more interesting.
		  */
		virtual int Progress(double dltotal, double dlnow, double ultotal, double ulnow);

		/// Use this to download a page using Get
		char* Get( const twine& url);

		/// Use this to download a page as an XML document
		xmlDocPtr GetXml(const twine& url);

		/// Use this to post a message to the server and retrieve the response.
		xmlDocPtr Post(const twine& url, const char* msg, size_t msgLen);

		/// Use this to post a message to the server and retrieve the response as a raw char buffer.
		char* PostRaw(const twine& url, const char* msg, size_t msgLen);

		/// Use this to download a page using GET with no cookie or state management.
		static char* GetPage( const twine& url );

		/// Use this do download a page using GET and have it returned as an XML document.
		static xmlDocPtr GetPageXml( const twine& url );

		/// Use this to post a message to a URL and retrieve the response.
		static xmlDocPtr PostPage( const twine& url, const char* msg, size_t msgLen);

		/// Use this to post an xml document to a URL and retrieve the response.
		static xmlDocPtr PostXml( const twine& url, xmlDocPtr doc);

		/** This is public so that callers may directly access the memory that was used
		  * when receiving the response from the server.
		  */
		MemBuf ResponseBuffer;

		/**
		  * Allows the caller to provide proxy information for us.  We pass this directly
		  * to the CURLOPT_PROXY config setting.
		  */
		void SetProxy( const twine& proxy );

		/**
		  * Allows the caller to provide header information for us.  We pass this directly
		  * to the CURLOPT_HTTPHEADER config setting.
		  */
		void AddPostHeader( const twine& postHeader );

		/**
		  * Clears the list of POST Headers.
		  */
		void ClearPostHeaders();

		/**
		  * Allows the caller to provide header information for us.  We pass this directly
		  * to the CURLOPT_HTTPHEADER config setting.
		  */
		void AddGetHeader( const twine& getHeader );

		/**
		  * Clears the list of GET Headers.
		  */
		void ClearGetHeaders();

		/** When absolute full control is required, use this to obtain the CURL handle itself.
		  * Note that the handle is cleaned up before Get/Post return, so it's main use is to
		  * add extra headers and setup config prior to calling Get/Post.
		  */
		CURL* CurlHandle( );

		/** Use this to retrieve the verbose curl log after a POST.
		  */
		const twine& GetVerboseLog();


	protected:
		/** This is an internal method that is called by PostRaw just before the curl_easy_perform is
		 * called.  This allows you to set any other options you need for curl to do it's work.
		 */
		virtual void PostOptions();

		/** If you need to free up any data that was allocated in PostOptions, do it here.  This will be
		 * called after the curl_easy_perform, and after we have reset the curl handle.
		 */
		virtual void PostFree();

		/** This is an internal method that is called by Get just before the curl_easy_perform is
		 * called.  This allows you to set any other options you need for curl to do it's work.
		 */
		virtual void GetOptions();

		/** If you need to free up any data that was allocated in GetOptions, do it here.  This will be
		 * called after the curl_easy_perform, and after we have reset the curl handle.
		 */
		virtual void GetFree();

		CURL* m_curl_handle;

	private:

		/// Copy constructor is private to prevent use
		HttpClient( const HttpClient& c) {}

		/// Assignment operator is private to prevent use
		HttpClient& operator=(const HttpClient& c) { return *this; }

		/// Our proxy information - if any
		twine m_proxy;

		/// Our list of POST headers
		vector<twine> m_post_headers;
		struct curl_slist* m_post_headerlist = nullptr;

		/// Our list of GET headers
		vector<twine> m_get_headers;
		struct curl_slist* m_get_headerlist = nullptr;

		/// Holds the curl verbose log output after a post
		twine m_verbose_log;


};

} // End Namespace SLib

#endif // HTTPCLIENT_H defined
