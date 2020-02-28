
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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iconv.h>
#include <errno.h>
#include <inttypes.h>

#include "twine.h"

#include "AnException.h"
#include "EnEx.h"
#include "AutoXMLChar.h"
#include "memptr.h"

#ifdef _WIN32
#include <rpc.h> // For GUID creation
#else
#include <uuid/uuid.h> // For GUID generation
#endif

const size_t MAX_INPUT_SIZE = 1024000000;

using namespace SLib;

#ifndef max
#define max(a, b) (a) > (b) ? (a) : (b)
#endif

twine::twine() :
	m_data (m_small_data),
	m_allocated_size ( TWINE_SMALL_STRING ),
	m_data_size (0),
	userIntVal (0)
{
	/* ************************************************************************ */
	/* twine's are used during log processing.  For this reason, do not include */
	/* the ee(FL, ...) version of the EnEx call.                                */
	/* ************************************************************************ */
	//EnEx ee("twine::twine()");
	memset(m_data, 0, m_allocated_size);
}

twine::twine(const twine& t) :
	m_data (m_small_data),
	m_allocated_size ( TWINE_SMALL_STRING ),
	m_data_size (0),
	userIntVal (0)
{
	//EnEx ee("twine::twine(const twine& t)");
	// short circuit for source having nothing in it.
	if((t.m_data_size == 0) ||
		(t.m_data_size < 0) ||
		(t.m_allocated_size < 0)||
		(t.m_allocated_size < t.m_data_size))
	{
		memset(m_data, 0, m_allocated_size);
		return;
	}
	reserve(t.m_data_size);
	m_data_size = t.m_data_size;
	userIntVal = t.userIntVal;
	memcpy(m_data, t.m_data, m_data_size);
	m_data[m_data_size] = '\0';
}

twine::twine(const char* c) :
	m_data ( m_small_data ),
	m_allocated_size ( TWINE_SMALL_STRING ),
	m_data_size (0),
	userIntVal(0)
{
	//EnEx ee("twine::twine(const char* c)");
	if(c == NULL){
		throw AnException(0, FL, "Input is null.");
	}
	size_t csize = strlen(c);
	if(csize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(csize);
	memcpy(m_data, c, csize);
	m_data_size = csize;
	m_data[m_data_size] = '\0';
}

twine::twine(const xmlChar* c) :
	m_data (m_small_data),
	m_allocated_size ( TWINE_SMALL_STRING ),
	m_data_size (0),
	userIntVal(0)
{
	//EnEx ee("twine::twine(const xmlChar* c)");
	if(c == NULL){
		throw AnException(0, FL, "Input is null.");
	}
	size_t csize = strlen((const char*)c);
	if(csize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(csize);
	memcpy(m_data, (const char*)c, csize);
	m_data_size = csize;
	m_data[m_data_size] = '\0';
}

twine::twine(const char c) :
	m_data (m_small_data),
	m_allocated_size ( TWINE_SMALL_STRING ),
	m_data_size (0),
	userIntVal(0)
{
	//EnEx ee("twine::twine(const char c)");
	//reserve(1);
	memset(m_data, 0, m_allocated_size);
	m_data[0] = c;
	m_data_size = 1;
	m_data[m_data_size] = '\0';
}

twine::twine(const xmlNodePtr node, const char* attrName):
	m_data (m_small_data),
	m_allocated_size ( TWINE_SMALL_STRING ),
	m_data_size (0),
	userIntVal(0)
{
	//EnEx ee("twine::twine(const xmlNodePtr node, const char* c)");

	getAttribute(node, attrName);

}

twine::~twine() 
{
	//EnEx ee("twine::~twine()");
	userIntVal = 0;

	if(m_allocated_size < TWINE_SMALL_STRING){
		throw AnException(0, FL, "twine::m_allocated_size < TWINE_SMALL_STRING");
	} else if(m_allocated_size == TWINE_SMALL_STRING){
		// Nothing to do here, our string data is part of our object.
	} else {
		if(m_data == NULL){
			throw AnException(0, FL, "twine::m_allocated_size > TWINE_SMALL_STRING, but m_data == NULL");
		}
		free(m_data);
		m_allocated_size = TWINE_SMALL_STRING;
		m_data = m_small_data;
	}
}

twine& twine::operator=(const twine& t)
{
	//EnEx ee("twine::operator=(const twine& t)");
	
	// Short circuit for self-assignment
	if(&t == this){
		return *this;
	}

	// short circuit for source having nothing in it.
	if(t.m_data_size == 0){
		if(m_data_size > 0){
			memset(m_data, 0, m_allocated_size);
			m_data_size = 0;
		}
		return *this;
	}

	reserve(t.m_data_size);
	m_data_size = t.m_data_size;
	userIntVal = t.userIntVal;
	memcpy(m_data, t.m_data, m_data_size);
	m_data[m_data_size] = '\0';

	return *this;
}

twine& twine::operator=(const twine* t) 
{
	//EnEx ee("twine::operator=(const twine* t)");
	return operator=(t->c_str());
}

twine& twine::operator=(const char* c)
{
	//EnEx ee("twine::operator=(const char* c)");
	if(c == NULL){
		throw AnException(0, FL, "twine = NULL not allowed.");
	}
	size_t tsize = strlen(c);
	if(tsize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(tsize);
	m_data_size = tsize;
	memcpy(m_data, c, m_data_size);
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::operator<< (xmlChar* c)
{
	//EnEx ee("twine::operator<<(const xmlChar* c)");
	if(c == NULL){
		throw AnException(0, FL, "twine << NULL not allowed.");
	}
	size_t tsize = strlen((const char*)c);
	if(tsize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(tsize);
	m_data_size = tsize;
	memcpy(m_data, (const char*)c, m_data_size);
	m_data[m_data_size] = '\0';
	xmlFree(c);
	return *this;
}

twine& twine::operator=(const size_t i)
{
	//EnEx ee("twine::operator=(const size_t i)");
	reserve(15);
	sprintf(m_data, "%zd", i);
	m_data_size = strlen(m_data);
	return *this;
}
	
twine& twine::operator=(const intptr_t i)
{
	//EnEx ee("twine::operator=(const intptr_t i)");
	reserve(15);
	sprintf(m_data, "%" PRIdPTR, i);
	m_data_size = strlen(m_data);
	return *this;
}
	
twine& twine::operator=(const float f)
{
	//EnEx ee("twine::operator=(const float f)");
	reserve(31);
	sprintf(m_data, "%f", f);
	m_data_size = strlen(m_data);
	return *this;
}

twine& twine::operator+=(const twine& t)
{
	//EnEx ee("twine::operator+=(const twine& t)");
	append(t());
	return *this;
}

twine& twine::operator+=(const twine* t)
{
	//EnEx ee("twine::operator+=(const twine* t)");
	append((*t)());
	return *this;
}

twine& twine::operator+=(const char* c)
{
	//EnEx ee("twine::operator+=(const char* c)");
	append(c);
	return *this;
}

twine& twine::operator+=(const char c)
{
	//EnEx ee("twine::operator+=(const char c)");
	reserve(m_data_size + 1);
	m_data[m_data_size] = c;
	m_data_size ++;
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::operator+=(const size_t i)
{
	//EnEx ee("twine::operator+=(const size_t i)");
	char tmp[16];
	memset(tmp, 0, 16);
	sprintf(tmp, "%zd", i);
	append(tmp);
	return *this;
}

twine& twine::operator+=(const intptr_t i)
{
	//EnEx ee("twine::operator+=(const intptr_t i)");
	char tmp[16];
	memset(tmp, 0, 16);
	sprintf(tmp, "%" PRIdPTR, i);
	append(tmp);
	return *this;
}

twine& twine::operator+=(const float i)
{
	//EnEx ee("twine::operator+=(const float i)");
	char tmp[32];
	memset(tmp, 0, 32);
	sprintf(tmp, "%f", i);
	append(tmp);
	return *this;
}

size_t twine::get_int() const 
{
	//EnEx ee("twine::get_int()");
	return atoi(m_data);
}

float twine::get_float() const 
{
	//EnEx ee("twine::get_float()");
	return (float)atof(m_data);
}

double twine::get_double() const 
{
	//EnEx ee("twine::get_double()");
	return atof(m_data);
}

char& twine::operator[](size_t i) const
{
	//EnEx ee("twine::operator[](size_t i)");
	if( (i < 0) || (i >= m_data_size)){
		throw AnException(0,FL,"twine: Out Of Bounds Access");
	}
	return m_data[i];
}

const char* twine::operator()() const
{
	//EnEx ee("twine::operator()()");
	return m_data;
}

const char* twine::c_str(void) const
{
	//EnEx ee("twine::c_str()");
	return m_data;
}

twine::operator const xmlChar*() const
{
	//EnEx ee("twine::operator(xmlChar*)()");
	return (const xmlChar*)m_data;
}

int twine::compare(const twine& t) const
{
	//EnEx ee("twine::compare(twine& t)");
	if((m_data_size == 0) && (t.m_data_size == 0)){
		return 0; // equal if both null
	}
	if((m_data_size == 0) || (t.m_data_size == 0)){
		return -1;
	}
	return strcmp(m_data, t.m_data);
}

int twine::compare(const char* c) const
{
	//EnEx ee("twine::compare(const char* c)");
	if((m_data_size == 0)){
		if(c == NULL){
			return 0; // equal if both null
		} else if( c[0] == '\0' ) {
			return 0; // c is null terminated at first position - counts as empty/null
		} else {
			return -1; // c has something in it, but we don't
		}
	} else {
		if(c == NULL || c[0] == '\0'){
			return -1; // We have data, but c doesn't
		} 
	}

	// We have data, and c has data - do a string compare
	return strcmp(m_data, c);
}

int twine::compare(const twine& t, size_t count) const
{
	//EnEx ee("twine::compare(twine& t, size_t count)");
	if((m_data_size == 0) && (t.m_data_size == 0)){
		return 0; // equal if both null
	}
	if((m_data_size == 0) || (t.m_data_size == 0)){
		return -1;
	}
	return strncmp(m_data, t.m_data, count);
}

int twine::compare(const char* c, size_t count) const
{
	//EnEx ee("twine::compare(const char* c, size_t count)");
	if((m_data_size == 0) && (c == NULL)){
		return 0; // equal if both null
	}
	if(m_data_size == 0 || c == NULL){
		return -1;
	}
	return strncmp(m_data, c, count);
}

int twine::compare(size_t i) const
{
	//EnEx ee("twine::compare(size_t i)");
	if(m_data_size == 0){
		return -1;
	}

	size_t m = atoi(m_data);
	if(m < i) return -1;
	else if(m > i) return 1;
	else return 0;
}

int twine::compare(float f) const
{
	//EnEx ee("twine::compare(float f)");
	if(m_data_size == 0){
		return -1;
	}

	float m = (float)atof(m_data);
	if(m < f) return -1;
	else if(m > f) return 1;
	else return 0;
}

bool twine::startsWith(const twine& t) const
{
	return compare(t, t.size()) == 0;
}

bool twine::endsWith(const twine& t) const
{
	//EnEx ee("twine::endsWith(const twine& t)");
	// If either are empty, or the input is bigger than we are
	// then return false;
	if(m_data_size == 0 || 
		t.empty() ||
		t.size() > m_data_size
	){
		return false;
	}

	// If the input is exactly the same size as we are, just
	// use the normal compare
	if(t.size() == m_data_size){
		return compare(t) == 0;
	}

	// Otherwise, compare the last few characters of our data
	// to the input data:

	// A B C D E F G H I J
	// 0 1 2 3 4 5 6 7 8 9
	// m_data_size = 10
	// t.m_data_size = 3
	// start = m_data_size - t.m_data_size
	// start = 7

	size_t start = m_data_size - t.m_data_size;

	return strncmp(m_data + start, t.m_data, t.m_data_size) == 0;
}

char* twine::data(void)
{
	//EnEx ee("twine::data(void)");
	return m_data;
}

size_t twine::check_size(void)
{
	//EnEx ee("twine::check_size(void)");
	if(m_data != NULL){
		m_data_size = strlen(m_data);
		if(m_data_size >= m_allocated_size){
			ERRORL(FL, "twine::check_size - data size(%d) > allocated_size(%d).  Possible memory corruption.",
				(int)m_data_size, (int)m_allocated_size );
			ERRORL(FL, "twine::check_size - resetting data_size to allocated_size - 1, and null terminating.");
			m_data_size = m_allocated_size - 1;
			m_data[ m_data_size ] = '\0';
			throw AnException(0, FL, "twine::check_size - data_size > allocated_size - you've just corrupted memory, or you forgot to null terminate m_data when you wrote to it!");
		}
	}
	return m_data_size;
}

twine& twine::set(const char* c)
{
	//EnEx ee("twine::set(const char* c)");
	return operator=(c);
}

twine& twine::set(const char* c, size_t n)
{
	//EnEx ee("twine::set(const char* c, size_t n)");
	if(n > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(n);
	memset(m_data, 0, m_allocated_size);
	memcpy(m_data, c, n);
	m_data_size = n;
	m_data[m_data_size] = '\0';
	return *this;
}
	
twine twine::substr(size_t start) const
{
	//EnEx ee("twine::substr(size_t start)");
	bounds_check(start);
	twine ret(&m_data[start]);
	return ret;
}

twine twine::substr(size_t start, size_t count) const
{
	//EnEx ee("twine::substr(size_t start, size_t count)");
	if(count == 0){
		twine blankret;
		return blankret;
	}
	bounds_check(start);
	bounds_check(start+count-1);
	twine ret;
	ret.set(&m_data[start], count);
	return ret;
}

twine* twine::substrp(size_t start) const
{
	//EnEx ee("twine::substrp(size_t start)");
	bounds_check(start);
	twine *ret = new twine(&m_data[start]);
	return ret;
}

twine* twine::substrp(size_t start, size_t count) const
{
	//EnEx ee("twine::substrp(size_t start, size_t count)");
	if(count == 0){
		twine* blankret = new twine();
		return blankret;
	}
	bounds_check(start);
	bounds_check(start+count-1);
	twine *ret = new twine();
	ret->set(&m_data[start], count);
	return ret;
}

twine& twine::format(const char* f, ...)
{
	va_list ap;
	//EnEx ee("twine::format(const char* f, ...)");
	if(f == NULL){
		throw AnException(0, FL, "Null format string.");
	}
	
	va_start(ap, f);
	format(f, ap);
	va_end(ap);

	return *this;
}

twine& twine::format(const char* f, va_list ap) 
{
	//EnEx ee("twine::format(const char* f, va_list ap)");
	int nsize;
	bool success = false;
	if(f == NULL){
		throw AnException(0, FL, "Null format string.");
	}
	//printf("twine here1\n");	
	reserve(256); // make sure we have a minimum amount of space

	//printf("twine here2\n");	
	while(!success){
		// Use a copy of the args list so that we can cycle through
		// them as many times as we need to:
		//printf("twine here3\n");	
		va_list apCopy;
		//printf("twine here4\n");	
#ifdef _WIN32
		memcpy(&apCopy, &ap, sizeof(va_list) );
#else
		va_copy(apCopy, ap);
#endif

		//printf("twine here5\n");	
		memset(m_data, 0, m_allocated_size);
		//printf("twine here6\n");	
#ifdef _WIN32
		nsize = _vsnprintf(m_data, m_allocated_size - 1, f, apCopy);
#else
		nsize = vsnprintf(m_data, m_allocated_size - 1, f, apCopy);
#endif

		//printf("twine here7\n");	
		if(nsize < 0) { // older C libraries
			//printf("twine here8\n");	
			// double twine capacity
			reserve(m_allocated_size * 2);  
			success = false;
		} else if (nsize > (int)(m_allocated_size - 1)){ // newer C lib
			//printf("twine here9\n");	
			// give it requested size
			reserve(nsize + 20);                 
			success = false;
		} else {
			//printf("twine here10\n");	
			success = true;
			m_data_size = nsize;
		}
	}
	//printf("twine here11\n");	
	return *this;
}

size_t twine::find(const char* needle) const
{
	//EnEx ee("twine::find(const char* needle)");
	if(needle == NULL){
		throw AnException(0, FL, "Can't search for NULL input.");
	}
	char *ptr;
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	ptr = strstr(m_data, needle);
	if(ptr == NULL){
		return TWINE_NOT_FOUND;
	} else {
		return (ptr - m_data);
	}
}

size_t twine::find(const char c) const
{
	//EnEx ee("twine::find(const char c)");
	char *ptr;
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	ptr = strchr(m_data, c);
	if(ptr == NULL){
		return TWINE_NOT_FOUND;
	} else {
		return (ptr - m_data);
	}
}

size_t twine::find(const twine& t) const
{
	//EnEx ee("twine::find(const twine& t)");
	return find(t());
}

size_t twine::find(const char* needle, size_t p) const
{
	//EnEx ee("twine::find(const char* needle, size_t p)");
	if(needle == NULL){
		throw AnException(0, FL, "Can't search for NULL input.");
	}
	char *ptr;
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	ptr = strstr(m_data + p, needle);
	if(ptr == NULL){
		return TWINE_NOT_FOUND;
	} else {
		return (ptr - m_data);
	}
}
	
size_t twine::find(const char c, size_t p) const
{
	//EnEx ee("twine::find(const char c, size_t p)");
	char *ptr;
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	ptr = strchr(m_data + p, c);
	if(ptr == NULL){
		return TWINE_NOT_FOUND;
	} else {
		return (ptr - m_data);
	}
}

size_t twine::find(const twine& t, size_t p) const
{
	//EnEx ee("twine::find(const twine& t, size_t p)");
	return find(t(), p);
}

size_t twine::rfind(const char c) const
{
	//EnEx ee("twine::rfind(const char c)");
	char *ptr;
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	ptr = strrchr(m_data, c);
	if(ptr == NULL){
		return TWINE_NOT_FOUND;
	} else {
		return (ptr - m_data);
	}
}

size_t twine::rfind(const char c, size_t p) const
{
	//EnEx ee("twine::rfind(const char c, size_t p)");
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	bounds_check(p);
	while(p >= 0){
		if(m_data[p] == c)
			return p;
		if(p == 0) break;
		p--; // otherwise keep going
	}
	return TWINE_NOT_FOUND;
}
	
size_t twine::rfind(const char* c) const
{
	//EnEx ee("twine::rfind(const char* c)");
	if(c == NULL){
		throw AnException(0, FL, "Can't search for NULL input.");
	}
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	size_t p = m_data_size - 1;
	size_t len = strlen(c);
	while(p >= 0){
		if(strncmp(&m_data[p], c, len) == 0)
			return p;
		if(p == 0) break;
		p--; // otherwise keep going
	}
	return TWINE_NOT_FOUND;
}

size_t twine::rfind(const char* c, size_t p) const
{
	//EnEx ee("twine::rfind(const char* c, size_t p)");
	if(c == NULL){
		throw AnException(0, FL, "Can't search for NULL input.");
	}
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	bounds_check(p);
	size_t len = strlen(c);
	while(p >= 0){
		if(strncmp(&m_data[p], c, len) == 0)
			return p;
		if(p == 0) break;
		p--; // otherwise keep going
	}
	return TWINE_NOT_FOUND;
}

size_t twine::rfind(const twine& t) const
{
	//EnEx ee("twine::rfind(const twine& t)");
	return rfind(t());
}

size_t twine::rfind(const twine& t, size_t p) const
{
	//EnEx ee("twine::rfind(const twine& t, size_t p)");
	return rfind(t(), p);
}

size_t twine::cifind(const char *needle) const
{
	//EnEx ee("twine::cifind(const char* needle)");
	return cifind(needle, 0);
}

size_t twine::cifind(const char c) const
{
	return cifind(c, 0);
}

size_t twine::cifind(const twine &t) const
{
	//EnEx ee("twine::cifind(const twine& t)");
	return cifind(t, 0);
}

size_t twine::cifind(const char *needle, size_t p) const
{
	//EnEx ee("twine::cifind(const char* needle, size_t p)");
	if(needle == NULL){
		throw AnException(0, FL, "Can't search for NULL input.");
	}

	if(m_data_size == 0) // Do this before bounds_check so we don't throw an error if we're empty
		return TWINE_NOT_FOUND;

	bounds_check(p);

	size_t nlen = strlen(needle);
	if(nlen == 0)
		return TWINE_NOT_FOUND;
	if(nlen > m_data_size)
		return TWINE_NOT_FOUND;

	twine upneedle( needle ); upneedle.ucase();
	twine loneedle( needle ); loneedle.lcase();

	// start with i = p to start searching at p
	for(size_t i = p; i <= m_data_size - nlen; ++i){
		bool found = true;
		for(size_t j = 0; j < nlen; ++j){
			if(i + j >= m_data_size){ // past the end
				found = false;
				break;
			}

			if(m_data[i+j] != upneedle[j] && m_data[i+j] != loneedle[j]){
				found = false;
				break;
			}
		}
		if(found){
			return i;
		}
	}
	return TWINE_NOT_FOUND;
}

size_t twine::cifind(const char c, size_t p) const
{
	//EnEx ee("twine::cifind(const char c, size_t p)");

	if(m_data_size == 0) // Do this before bounds_check so we don't throw an error if we're empty
		return TWINE_NOT_FOUND;

	bounds_check(p);
	char *ptrl, *ptru;
	ptrl = strchr( m_data + p, tolower( c ) );
	ptru = strchr( m_data + p, toupper( c ) );
	if(ptrl == NULL && ptru == NULL){ // catch both null
		return TWINE_NOT_FOUND;
	} else if(ptrl == NULL) { // catch lowercase null
		return (ptru - m_data);
	} else if(ptru == NULL) { // catch uppercase null
		return (ptrl - m_data);
	} else { // both not null
		return min((ptru - m_data), (ptrl - m_data)); // return the first instance
	}
}

size_t twine::cifind(const twine &t, size_t p) const
{
	//EnEx ee("twine::cifind(const twine &t, size_t p)");
	return cifind( t(), p );
}

size_t twine::cirfind(const char c) const
{
	//EnEx ee("twine::cirfind(const char c)");
	return cirfind(c, m_data_size - 1);
}

size_t twine::cirfind(const char c, size_t p) const
{
	//EnEx ee("twine::rfind(const char c, size_t p)");
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	size_t i = p;
	bounds_check(i);
	char uc = toupper( c );
	char lc = tolower( c );
	while(i >= 0){
		if(m_data[i] == uc || m_data[i] == lc)
			return i;
		if(i == 0) break; // prevent buffer underrun
		i--;
	}
	return TWINE_NOT_FOUND;
}

size_t twine::cirfind(const char *needle) const
{
	//EnEx ee("twine::cirfind(const char* needle)");
	return cirfind(needle, m_data_size - 1);
}

size_t twine::cirfind(const char *needle, size_t p) const
{
	//EnEx ee("twine::cirfind(const char* needle, size_t p)");
	if(needle == NULL){
		throw AnException(0, FL, "Can't search for NULL input.");
	}

	if(m_data_size == 0)
		return TWINE_NOT_FOUND;

	size_t nlen = strlen(needle);
	if(nlen == 0)
		return TWINE_NOT_FOUND;

	twine upneedle( needle ); upneedle.ucase();
	twine loneedle( needle ); loneedle.lcase();

	for(size_t i = p; i >= 0; --i){
		bool found = true;
		for(size_t j = 0; j < nlen; ++j){
			if(i + j >= m_data_size){ // past the end
				found = false;
				break;
			}

			if(m_data[i+j] != upneedle[j] && m_data[i+j] != loneedle[j]){
				found = false;
				break;
			}
		}
		if(found){
			return i;
		}
		if(i == 0) break;
	}
	return TWINE_NOT_FOUND;
}

size_t twine::cirfind(const twine &needle) const
{
	return cirfind( needle, m_data_size - 1 );
}

size_t twine::cirfind(const twine &needle, size_t p) const
{
	return cirfind( needle(), p );
}

size_t twine::countof(const char needle) const
{
	//EnEx ee("twine::countof(const char needle)");
	size_t count = 0;
	for(size_t i = 0; i < m_data_size; i++){
		if(m_data[i] == needle) count++;
	}
	return count;
}

size_t twine::findSkipNested(size_t start, const char startChar, const char endChar ) const
{
	bounds_check(start);
	int nestCount = 0;
	for(size_t i = start; i < m_data_size; i++){
		if(m_data[i] == endChar){
			if(nestCount == 0) {
				return i; // we're done
			} else {
				nestCount --; // end of a nested section
			}
		} else if(m_data[i] == startChar){
			nestCount ++; // start of a nested section
		}
	}
	// If we get here - we didn't find it
	return TWINE_NOT_FOUND;
}

twine& twine::replace(size_t start, size_t count, const char* rep)
{
	//EnEx ee("twine::replace(size_t start, size_t count, const char* rep)");
	if(rep == NULL){
		throw AnException(0, FL, "Invalid replacement value (NULL).");
	}
	bounds_check(start);
	
	if((start+count) >= m_data_size){
		erase(start);
		append(rep);
	} else {	
		size_t len = strlen(rep);
		size_t i;
		if(len > MAX_INPUT_SIZE){
			throw AnException(0,FL,"twine: Input Too Large");
		}
		if(len == count){
			for(i = start; i < start + count; i++){
				m_data[i] = rep[i - start];	
			}
		} else if (len < count) {
			for(i = start; i < start + len; i++){
				m_data[i] = rep[i - start];
			}
			// erase the leftover
			erase(start+len, count - len);
		} else { // len > count
			for(i = start; i < start + count; i ++){
				m_data[i] = rep[i - start];
			}
			// insert the leftover
			insert(start+count, rep + count);
		}
	}

	return *this;
}	

twine& twine::replace(size_t start, size_t count, const twine* t)
{
	//EnEx ee("twine::replace(size_t start, size_t count, const twine* t)");
	replace(start, count, t->c_str());
	return *this;
}

twine& twine::replace(size_t start, size_t count, const twine& t)
{
	//EnEx ee("twine::replace(size_t start, size_t count, const twine& t)");
	replace(start, count, t());
	return *this;
}

twine& twine::replace(const char c, const char n)
{
	//EnEx ee("twine::replace(const char c, const char n)");

	for(size_t i = 0; i < m_data_size; i++){
		if(m_data[i] == c){
			m_data[i] = n;
		}
	}
	return *this;
}

twine& twine::replaceAll(const twine& target, const twine& replacement)
{
	size_t idx = find(target);
	while(idx != TWINE_NOT_FOUND){
		replace(idx, target.size(), replacement);
		idx = find(target, idx + replacement.size());
	}
	return *this;
}

twine& twine::cireplaceAll(const twine& target, const twine& replacement)
{
	size_t idx = cifind(target);
	while(idx != TWINE_NOT_FOUND){
		replace(idx, target.size(), replacement);
		if(idx + replacement.size() >= m_data_size)
			idx = TWINE_NOT_FOUND; // reached the end of the string
		else
			idx = cifind(target, idx + replacement.size());
	}
	return *this;
}

twine& twine::append(const char* c)
{
	//EnEx ee("twine::append(const char* c)");
	if(c == NULL){
		return *this; // nothing to append
	}
	size_t csize = strlen(c);
	if(csize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(m_data_size + csize);
	memcpy(m_data + m_data_size, c, csize);
	m_data_size += csize;
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::append(const char* c, size_t count)
{
	//EnEx ee("twine::append(const char* c, size_t count)");
	if(c == NULL){
		return *this; // nothing to append
	}
	if(count > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(m_data_size + count);
	memcpy(m_data + m_data_size, c, count);
	m_data_size += count;
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::insert(size_t p, const char* c)
{
	//EnEx ee("twine::insert(size_t p, const char* c)");
	bounds_check(p);
	if(c == NULL){
		return *this; // nothing to insert
	}

	size_t csize = strlen(c);
	if(csize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(m_data_size + csize);
	for(size_t i = m_data_size - 1; i >= p; i--){
		m_data[i+csize] = m_data[i];
	}
	memcpy(m_data + p, c, csize);
	m_data_size += csize;
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::erase(size_t p, size_t n)
{
	//EnEx ee("twine::erase(size_t p, size_t n)");
	bounds_check(p);
	bounds_check(p+n-1);
	for(size_t i = p; i < m_data_size - n; i++){
		m_data[i] = m_data[i+n];
	}
	m_data_size -= n;	
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::erase(size_t p)
{
	//EnEx ee("twine::erase(size_t p)");
	bounds_check(p);
	m_data[p] = '\0';
	m_data_size = p;
	return *this;
}

twine& twine::erase(void)
{
	//EnEx ee("twine::erase(void)");
	memset(m_data, 0, m_allocated_size);
	m_data_size = 0;
	return *this;
}

twine& twine::rtrim(void)
{
	//EnEx ee("twine::rtrim(void)");
	if(m_data_size == 0){
		return *this; // bail out early.
	}
	size_t i = m_data_size - 1;
	while(i >= 0){
		if(isspace((unsigned char)m_data[i]) || m_data[i] == '\r' || m_data[i] == '\n') {
			m_data[i] = '\0';
			m_data_size --;
			i--;
		} else {
			break; // found something that is not a space or newline.  we're done.
		}
	}
	return *this;
}

twine& twine::ltrim(void)
{
	//EnEx ee("twine::ltrim(void)");
	size_t i = 0;
	while( (i < m_data_size) && (isspace((unsigned char)m_data[i])) ) i++;

	if(i > 0){
		erase(0, i);
	}
	return *this;
}
		
	
twine& twine::reserve(size_t min_size) 
{
	//EnEx ee("twine::reserve(size_t min_size)");
	if(m_allocated_size < TWINE_SMALL_STRING){
		// This is an error, we should never have an allocated size less than the size of
		// our internal character buffer.
		throw AnException(0, FL, "twine::reserve m_allocated_size < TWINE_SMALL_STRING");
	} 

	if(min_size < m_allocated_size){
		return *this; // nothing to do, we already have enough allocated
	}
	
	if(m_allocated_size == TWINE_SMALL_STRING){
		// We've been using our internal character buffer, but now we've been asked for
		// more space than the internal buffer can hold.
	
		// Allocate the size requested
		m_data = (char*)malloc(min_size + 10);
		if(m_data == NULL){
			throw AnException(0, FL, "twine::reserve Error Allocating Memory");
		}
		m_allocated_size = min_size + 10;
		memset(m_data, 0, m_allocated_size);

		if(m_data_size > 0){
			// Copy over anything from m_small_data that was in use:
			memcpy(m_data, m_small_data, m_data_size);
		}
		return *this;

	} else if(m_allocated_size > TWINE_SMALL_STRING){

		// We've already been using a malloc'd buffer.  If they are asking for more space,
		// use the usual realloc strategy.
		
		// Use exponential growth to minimize allocations, and character copies.
		// For detailed analysis: http://www.gotw.ca/gotw/043.htm
		size_t dbl = m_allocated_size * 2;
		size_t newlen = 0;
		if(dbl >  min_size+10 ){
			newlen = dbl;
		} else {
			newlen = min_size + 1;
		}

		char *ptr = (char *)realloc(m_data, newlen);
		if(ptr == NULL){
			throw AnException(0, FL,
				"twine: Error reallocating memory.");
		}
		m_data = ptr;
		m_allocated_size = newlen;
		return *this;
	}

	throw AnException(0, FL, "twine::reserve Shouldn't get to here min_size(%d) m_allocated_size(%d)",
		(int)min_size, (int)m_allocated_size );
}

size_t twine::size(void) const 
{ 
	//EnEx ee("twine::size(void)");
	return m_data_size; 
}

void twine::size(size_t s)
{ 
	//EnEx ee("twine::size(size_t)");
	m_data_size = s; 
}

size_t twine::length(void) const 
{ 
	//EnEx ee("twine::length(void)");
	return m_data_size; 
}

size_t twine::max_size(void) const 
{ 
	//EnEx ee("twine::max_size(void)");
	return m_allocated_size - 1; 
}

size_t twine::capacity(void) const 
{ 
	//EnEx ee("twine::capacity(void)");
	return m_allocated_size - 1; 
}

bool twine::empty(void) const 
{ 
	//EnEx ee("twine::empty(void)");
	return (m_data_size == 0); 
}
	
void twine::bounds_check(size_t p) const
{
	//EnEx ee("twine::bounds_check(size_t p)");
	if( (p < 0) || (p >= m_data_size)){
		//strncpy( (char*)0, (char*)100, 1000 ); // force a segfault to trap this error.
		throw AnException(0, FL, "twine: Index out of bounds. p(%d) m_data_size(%d)", p, m_data_size);
	}
}

twine& twine::ucase(void)
{
	//EnEx ee("twine::ucase(void)");
	size_t i;
	for(i = 0; i < m_data_size; i++){
		m_data[i] = toupper(m_data[i]);
	}
	return *this;
}

twine& twine::ucase(size_t i)
{
	//EnEx ee("twine::ucase(size_t i)");
	bounds_check(i);
	m_data[i] = toupper(m_data[i]);
	return *this;
}

twine& twine::lcase(void)
{
	//EnEx ee("twine::lcase(void)");
	size_t i;
	for(i = 0; i < m_data_size; i++){
		m_data[i] = tolower(m_data[i]);
	}
	return *this;
}

twine& twine::lcase(size_t i)
{
	//EnEx ee("twine::lcase(size_t i)");
	bounds_check(i);
	m_data[i] = tolower(m_data[i]);
	return *this;
}

void twine::strupr(char* input)
{
	//EnEx ee("twine::strupr(char* input)");
	if(input == NULL){
		throw AnException(0, FL, "twine::strupr - input is NULL");
	}
	size_t len = strlen(input);
	if(len > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	for(size_t i = 0; i < len; i++){
		input[i] = toupper(input[i]);
	}
}

void twine::strlwr(char* input)
{
	//EnEx ee("twine::strlwr(char* input)");
	if(input == NULL){
		throw AnException(0, FL, "twine::strlwr - input is NULL");
	}
	size_t len = strlen(input);
	if(len > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	for(size_t i = 0; i < len; i++){
		input[i] = tolower(input[i]);
	}
}

vector < twine > twine::split(const twine& spliton) const
{
	//EnEx ee("twine::split(twine spliton)");
	vector < twine > v;
	size_t idx1, idx2;

	idx1 = 0;
	idx2 = find(spliton);
	if(idx2 == TWINE_NOT_FOUND){
		v.push_back(*this);
		return v;
	}
	while(idx2 != TWINE_NOT_FOUND){
		v.push_back(substr(idx1, idx2-idx1));
		idx1 = idx2 + spliton.size();
		idx2 = find(spliton, idx1);
	}
	if(idx1 < m_data_size){
		v.push_back(substr(idx1));
	}
	return v;
}

vector < twine > twine::tokenize(const twine& tokensep) const
{
	//EnEx ee("twine::tokenize(const twine& tokensep)");
	vector < twine > v;
	size_t idx1, idx2;

	idx1 = 0;
	idx2 = 0;

	do {
		// Find the first non-token-separator to start the first token:
		while( idx1 < m_data_size && tokensep.find( m_data[idx1] ) != TWINE_NOT_FOUND ){
			idx1++;
		}
		if(idx1 < m_data_size){
			// Found the beginning of a token

			// Now walk until we find the end of the token
			idx2 = idx1 + 1;
			while( idx2 < m_data_size && tokensep.find( m_data[idx2] ) == TWINE_NOT_FOUND ){
				idx2++;
			}

			if(idx2 < m_data_size){
				// pull out the sub-string
				v.push_back( substr( idx1, idx2-idx1 ) );
			} else {
				// The entire rest of the string is the last token:
				v.push_back( substr( idx1 ) );
				break;
			}

			idx1 = idx2 + 1; // advance past the end of this token to search for the next
		}
	}  while( idx1 < m_data_size );

	return v;
}


twine& twine::getAttribute(xmlNodePtr node, const char* attrName)
{
	//EnEx ee("twine::getAttribute(xmlNodePtr node, const char* attrName)");
	if(node == NULL){
		throw AnException(0, FL, "Invalid xmlNodePtr given (NULL).");
	}
	if(attrName == NULL){
		throw AnException(0, FL, "Invalid attrName given (NULL).");
	}

	AutoXMLChar tmp;
	tmp = xmlGetProp(node, (const xmlChar*)attrName);
	if(tmp() == NULL){
		return erase();
	}
	return set(tmp);
}

twine& twine::encode64()
{
	//EnEx ee("twine::encode64()");
	if(size() == 0){
		return *this; // We're an empty string
	}

	// Run the conversion
	size_t len;
	memptr< char > tmpspace; tmpspace = Base64::encode( (char*)m_data, size(), &len );

	// Now copy the converted data over to ourself:
	reserve(len + 1);
	erase(); // clean out anything we had
	memcpy( m_data, tmpspace, len );
	m_data[ len ] = '\0';
	m_data_size = len;
	return *this;
}

twine& twine::encode64url()
{
	// First do the standard base64 encode:
	encode64();

	// Strip out any newlines in the content
	replaceAll( "\n", "" );

	// Remove any trailing '='s
	replaceAll( "=", "" );

	replace( '+', '-' ); // Plusses convert to dashes
	replace( '/', '_' ); // Slashes become underscores

	// Return ourselves
	return *this;
}

twine& twine::decode64()
{
	//EnEx ee("twine::decode64()");
	if(size() == 0){
		return *this; // We're an empty string
	}

	// Run the conversion
	size_t len;
	memptr< char > tmpspace; tmpspace = Base64::decode( (char*)m_data, size(), &len);

	// Now copy the converted data over to ourself:
	reserve(len); // shouldn't be necessary, but just in case.
	erase(); // clean out anything we had
	memcpy( m_data, tmpspace, len);	
	m_data_size = len;
	m_data[m_data_size] = '\0';

	// Return ourselves.
	return *this;
}

twine& twine::decode64url()
{
	replace('-', '+'); // dashes convert to plusses
	replace('_', '/'); // underscores become slashes
	replace(',', '='); // comma become equals
	switch(length() % 4){ // Pad with trailing '='s
		case 0: break; // nothing to do
		case 2: append( "==" ); break; // Add 2 equals to the end of the string
		case 3: append( "=" ); break; // Add 1 equals to the end of the string
		default:
			throw AnException(0, FL, "Invalid lengthfor base64 url decoding.");
	}

	// Split into 64 character lines
	size_t i = 64;
	while(i < size()){
		insert(i, "\n");
		i++; // for the newline
		i += 64; // next 64 characters
	}

	// Add a linefeed at the end
	append( "\n" );

	//printf("String for decoding --%s--\n", (char*)m_data);

	// Now do the actual base64 decode
	return decode64();
}

twine& twine::to_utf8(const twine& fromEncoding, bool withChatter)
{
	twine useEncoding("UTF-16LE");
	if(!fromEncoding.empty()){
		useEncoding = fromEncoding;
	}

	if(userIntVal <= 0){
		// nothing to do.
		erase();
		m_data_size = 0;
		return *this;
	}

	if(withChatter){
		printf("Twine context at start of to_utf8:\nm_allocated_size(%" PRIdPTR ") m_data_size(%" PRIdPTR ") userIntVal(%d)\n",
			m_allocated_size, m_data_size, userIntVal
		);
		printf("%s\n", Tools::hexDump(m_data, "to_utf8 - before", 16, userIntVal + 16, true, false)() );
	}

	// Setup a temporary twine to use to hold the output
	twine target; target.reserve( m_allocated_size );
	char* inputData = m_data;
	char* targetData = target.data(); // iconv moves this pointer, so make a copy for it to use
	size_t targetSize = target.capacity();
	size_t inRemains = (size_t)userIntVal;

	// Run the conversion
	iconv_t context = iconv_open("UTF-8", useEncoding()); // To, From
	size_t cvtlen = iconv( context, // Conversion context
#ifdef _WIN32
		(const char**)&inputData,   // Pointer to source to read
#else
		(char**)&inputData,   // Pointer to source to read
#endif
		(size_t*)&inRemains,        // How much to read
		(char**)&targetData,        // Pointer to where to write the data
		(size_t*)&targetSize        // How big is target going in and comming out
	);

	if(cvtlen == (size_t)-1){
		printf("error in to_utf8: %s, %d\nm_allocated_size(%" PRIdPTR ") m_data_size(%" PRIdPTR ") userIntVal(%d)\n",
			 strerror(errno), errno,
			m_allocated_size, m_data_size, userIntVal
		);
		printf("%s\n", Tools::hexDump(m_data, "to_utf8 - details", 16, userIntVal + 16, true, false)() );
		iconv_close( context );
		return *this;
	}

	if(withChatter){
		printf("Twine context at in to_utf8 after iconv:\ncvtlen(%zd) inRemains(%zd) targetSize(%zd)\n",
			cvtlen, inRemains, targetSize
		);
		printf("%s\n", Tools::hexDump(target.data(), "to_utf8 - during", 16, targetSize+16, true, false)() );
	}

	// Erase our data and replace it with that from the temporary twine	
	erase();
	// iconv moves the targetData pointer - use it to determine where it stopped
	size_t newLen = targetData - target.data(); 
	memcpy(m_data, target.data(), newLen);
	m_data_size = newLen;

	// Free up the iconv conversion context
	iconv_close( context );

	if(withChatter){
		printf("Twine context at end of to_utf8:\nm_allocated_size(%" PRIdPTR ") m_data_size(%" PRIdPTR ") userIntVal(%d)\n",
			m_allocated_size, m_data_size, userIntVal
		);
		printf("%s\n", Tools::hexDump(m_data, "to_utf8 - after", 16, m_data_size + 16, true, false)() );
	}

	return *this;
}

twine& twine::to_utf16le(const twine& fromEncoding)
{
	twine useEncoding("UTF-8");
	if(!fromEncoding.empty()){
		useEncoding = fromEncoding;
	}

	if(m_data_size == 0){
		return *this; // Nothing to do
	}

	/*
	printf("Twine context at start of to_utf8:\nm_allocated_size(%" PRIdPTR ") m_data_size(%" PRIdPTR ") userIntVal(%d)\n",
		m_allocated_size, m_data_size, userIntVal
	);
	printf("%s\n", Tools::hexDump(m_data, "to_utf8 - before", 16, userIntVal + 16, true, false)() );
	*/

	// Setup a temporary twine to use to hold the output
	twine target; target.reserve( m_data_size * 2 );
	char* inputData = m_data;
	char* targetData = target.data(); // iconv moves this pointer, so make a copy for it to use
	size_t targetSize = target.capacity();
	size_t inRemains = m_data_size;

	// Run the conversion
	iconv_t context = iconv_open("UTF-16LE", useEncoding()); // To, From
	size_t cvtlen = iconv( context, // Conversion context
#ifdef _WIN32
		(const char**)&inputData,   // Pointer to source to read
#else
		(char**)&inputData,   // Pointer to source to read
#endif
		(size_t*)&inRemains,        // How much to read
		(char**)&targetData,        // Pointer to where to write the data
		(size_t*)&targetSize        // How big is target going in and comming out
	);

	if(cvtlen == (size_t)-1){
		printf("error in to_utf16le: %s, %d\nm_allocated_size(%" PRIdPTR ") m_data_size(%" PRIdPTR ") userIntVal(%d)\n",
			 strerror(errno), errno,
			m_allocated_size, m_data_size, userIntVal
		);
		printf("%s\n", Tools::hexDump(m_data, "to_utf16le - details", 16, m_data_size + 16, true, false)() );
		iconv_close( context );
		return *this;
	}

	/*
	printf("Twine context at in to_utf8 after iconv:\ncvtlen(%zd) inRemains(%zd) targetSize(%zd)\n",
		cvtlen, inRemains, targetSize
	);
	printf("%s\n", Tools::hexDump(target.data(), "to_utf8 - during", 16, targetSize+16, true, false)() );
	*/

	// Erase our data and replace it with that from the temporary twine	
	erase();
	// iconv moves the targetData pointer - use it to determine where it stopped
	size_t newLen = targetData - target.data(); 
	reserve( newLen ); // ensure we have enough space
	memcpy(m_data, target.data(), newLen);
	m_data_size = newLen;

	// Free up the iconv conversion context
	iconv_close( context );

	/*
	printf("Twine context at end of to_utf8:\nm_allocated_size(%" PRIdPTR ") m_data_size(%" PRIdPTR ") userIntVal(%d)\n",
		m_allocated_size, m_data_size, userIntVal
	);
	printf("%s\n", Tools::hexDump(m_data, "to_utf8 - after", 16, m_data_size + 16, true, false)() );
	*/

	return *this;
}

bool twine::isZeroGuidOrEmpty() const
{
	// Are we empty or the zero-guid
	if((m_data_size == 0) || (compare("00000000-0000-0000-0000-000000000000") == 0)) return true;

	// Nope
	return false;
}

bool twine::isZeroGuid() const
{
	// Are we the zero-guid?
	if(compare("00000000-0000-0000-0000-000000000000") == 0) return true;

	// Nope
	return false;
}

twine& twine::zeroGuid()
{
	return set("00000000-0000-0000-0000-000000000000");
}

twine& twine::Guid()
{
	erase(); // Clear out any contents we might have
	reserve(36); // Make sure we have enough space to hold the guid

#ifdef _WIN32
	// First generate the uuid
	UUID u;
	UuidCreate( &u );

	// Then convert it to a string that we can use:
	RPC_CSTR uStr;
	UuidToString( &u, &uStr );
	
	set( (char*)uStr );
	RpcStringFree( &uStr );
#else
	uuid_t u;
	uuid_generate(u);
	uuid_unparse(u, m_data); // Write directly to our data
	check_size(); // Get all of our size counters correct
#endif

	return *this;
}

