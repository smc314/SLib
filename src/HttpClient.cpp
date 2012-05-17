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

#include "HttpClient.h"
#include "Date.h"
#include "EnEx.h"
#include "Log.h"
#include "memptr.h"
#include "dptr.h"
#include "AnException.h"
#include "XmlHelpers.h"
using namespace SLib;

static bool HttpClient_cURL_Initialized = false;

struct MemoryStruct {
	char* memory;
	size_t size;
};

size_t HttpClient_WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)userp;

	if(mem->memory == NULL){
		mem->memory = (char*)malloc( realsize + 1);
		if(mem->memory == NULL){
			throw AnException(0, FL, "Error allocating memory for Http read.");
		}
	} else {
		char* tmp = (char*)realloc( mem->memory, mem->size + realsize + 1);
		if(tmp != NULL){
			mem->memory = tmp;
		} else {
			throw AnException(0, FL, "Error increasing memory for Http read.");
		}
	}
	memcpy( &(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[ mem->size ] = 0;

	return realsize;
}

/** This one is used by the object (non-static) version of the methods below.
  */
size_t HttpClient_WriteMemoryCallback2(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	HttpClient* client = (HttpClient*)userp;

	client->ResponseBuffer.append( (char*)contents, realsize );

	return realsize;
}

HttpClient::HttpClient()
{
	EnEx ee(FL, "HttpClient::HttpClient()");

	if(HttpClient_cURL_Initialized == false){
		curl_global_init(CURL_GLOBAL_ALL);
		HttpClient_cURL_Initialized = true;
	}
	m_curl_handle = curl_easy_init();
}

HttpClient::~HttpClient()
{
	EnEx ee(FL, "HttpClient::~HttpClient()");

	curl_easy_cleanup( m_curl_handle );
}

char* HttpClient::Get(const twine& url )
{
	EnEx ee(FL, "HttpClient::Get(const twine& url)");

	curl_easy_setopt( m_curl_handle, CURLOPT_URL, url() );
	curl_easy_setopt( m_curl_handle, CURLOPT_WRITEFUNCTION, HttpClient_WriteMemoryCallback2 );
	curl_easy_setopt( m_curl_handle, CURLOPT_WRITEDATA, this );
	
	// Ignore SSL cert issues
	curl_easy_setopt( m_curl_handle, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt( m_curl_handle, CURLOPT_SSL_VERIFYHOST, false);

	curl_easy_perform( m_curl_handle );
	curl_easy_reset( m_curl_handle );

	return ResponseBuffer.data();
}

xmlDocPtr HttpClient::GetXml(const twine& url)
{
	EnEx ee(FL, "HttpClient::GetXml(const twine& url)");

	dptr<char> contents = Get( url );
	xmlDocPtr doc = xmlParseDoc( (xmlChar*) contents() );
	return doc;
}

char* HttpClient::PostRaw(const twine& url, const char* msg, size_t msgLen)
{
	EnEx ee(FL, "HttpClient::PostRaw(const twine& url, const char* msg, size_t msgLen)");

	struct curl_slist* slist = NULL;

	{ // for timing scope
		EnEx ee2(FL, "HttpClient::Post - curl_easy_setopt");
		curl_easy_setopt( m_curl_handle, CURLOPT_VERBOSE, 1);
		curl_easy_setopt( m_curl_handle, CURLOPT_URL, url() );
		curl_easy_setopt( m_curl_handle, CURLOPT_POSTFIELDS, msg );
		curl_easy_setopt( m_curl_handle, CURLOPT_POSTFIELDSIZE, msgLen );
		curl_easy_setopt( m_curl_handle, CURLOPT_WRITEFUNCTION, HttpClient_WriteMemoryCallback2 );
		curl_easy_setopt( m_curl_handle, CURLOPT_WRITEDATA, this );
		slist = curl_slist_append(slist, "Expect:");
		curl_easy_setopt( m_curl_handle, CURLOPT_HTTPHEADER, slist);

		// Ignore SSL cert issues
		curl_easy_setopt( m_curl_handle, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt( m_curl_handle, CURLOPT_SSL_VERIFYHOST, false);
	}
	{ // for timing scope
		EnEx ee3(FL, "HttpClient::Post - curl_easy_perform");
		curl_easy_perform( m_curl_handle );
	}
	curl_easy_reset( m_curl_handle );
	curl_slist_free_all(slist);

	return ResponseBuffer.data();
}

xmlDocPtr HttpClient::Post(const twine& url, const char* msg, size_t msgLen)
{
	EnEx ee(FL, "HttpClient::Post(const twine& url, const char* msg, size_t msgLen)");

	PostRaw( url, msg, msgLen );
	xmlDocPtr doc = xmlParseDoc( (xmlChar*) ResponseBuffer() );
	return doc;
}

char* HttpClient::GetPage(const twine& url )
{
	EnEx ee(FL, "HttpClient::GetPage(const twine& url)");

	CURL* curl_handle;
	struct MemoryStruct chunk;

	chunk.memory = NULL;
	chunk.size = 0;

	if(HttpClient_cURL_Initialized == false){
		curl_global_init(CURL_GLOBAL_ALL);
		HttpClient_cURL_Initialized = true;
	}
	curl_handle = curl_easy_init();
	curl_easy_setopt( curl_handle, CURLOPT_URL, url() );
	curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, HttpClient_WriteMemoryCallback );
	curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, (void*)&chunk );
	curl_easy_perform( curl_handle );
	curl_easy_cleanup( curl_handle );

	return chunk.memory;
}

xmlDocPtr HttpClient::GetPageXml(const twine& url)
{
	EnEx ee(FL, "HttpClient::GetPageXml(const twine& url)");

	dptr<char> contents = HttpClient::GetPage( url );
	xmlDocPtr doc = xmlParseDoc( (xmlChar*) contents() );
	return doc;
}

xmlDocPtr HttpClient::PostPage(const twine& url, const char* msg, size_t msgLen)
{
	EnEx ee(FL, "HttpClient::PostPage(const twine& url, const char* msg, size_t msgLen)");

	CURL* curl_handle;
	struct curl_slist* slist = NULL;
	struct MemoryStruct chunk;

	chunk.memory = NULL;
	chunk.size = 0;

	if(HttpClient_cURL_Initialized == false){
		curl_global_init(CURL_GLOBAL_ALL);
		HttpClient_cURL_Initialized = true;
	}
	curl_handle = curl_easy_init();
	curl_easy_setopt( curl_handle, CURLOPT_VERBOSE, 1);
	curl_easy_setopt( curl_handle, CURLOPT_URL, url() );
	curl_easy_setopt( curl_handle, CURLOPT_POSTFIELDS, msg );
	curl_easy_setopt( curl_handle, CURLOPT_POSTFIELDSIZE, msgLen );
	curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, HttpClient_WriteMemoryCallback );
	curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, (void*)&chunk );
	slist = curl_slist_append(slist, "Expect:");
	curl_easy_setopt( curl_handle, CURLOPT_HTTPHEADER, slist);
	curl_easy_perform( curl_handle );
	curl_easy_cleanup( curl_handle );
	curl_slist_free_all(slist);

	dptr<char> contents = chunk.memory;
	xmlDocPtr doc = xmlParseDoc( (xmlChar*) contents() );
	return doc;
}

xmlDocPtr HttpClient::PostXml(const twine& url, xmlDocPtr doc)
{
	EnEx ee(FL, "HttpClient::PostXml(const twine& url, xmlDocPtr doc)");

	twine docStr = XmlHelpers::docToString( doc );
	return HttpClient::PostPage( url, docStr(), docStr.length() );
}

