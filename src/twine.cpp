
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
#include <ctype.h>

#include "twine.h"

#include "AnException.h"
#include "EnEx.h"
#include "AutoXMLChar.h"
#include "memptr.h"

const int MAX_INPUT_SIZE = 10240000;

using namespace SLib;

twine::twine() :
	m_data (NULL),
	m_allocated_size (0),
	m_data_size (0)
{
	/* ************************************************************************ */
	/* twine's are used during log processing.  For this reason, do not include */
	/* the ee(FL, ...) version of the EnEx call.                                */
	/* ************************************************************************ */
	EnEx ee("twine::twine()");
}

twine::twine(const twine& t) :
	m_data (NULL),
	m_allocated_size (0),
	m_data_size (0)
{
	EnEx ee("twine::twine(const twine& t)");
	// short circuit for source having nothing in it.
	if((t.m_data_size == 0) ||
		(t.m_data_size < 0) ||
		(t.m_allocated_size < 0)||
		(t.m_allocated_size < t.m_data_size))
	{
		return;
	}
	reserve(t.m_data_size);
	m_data_size = t.m_data_size;
	memset(m_data, 0, m_allocated_size);
	strncpy(m_data, t.m_data, m_data_size);
}

twine::twine(const char* c) :
	m_data (NULL),
	m_allocated_size (0),
	m_data_size (0)
{
	EnEx ee("twine::twine(const char* c)");
	if(c == NULL){
		throw AnException(0, FL, "Input is null.");
	}
	int csize = strlen(c);
	if(csize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(csize);
	strncpy(m_data, c, csize);
	m_data_size = csize;
}

twine::twine(const xmlChar* c) :
	m_data (NULL),
	m_allocated_size (0),
	m_data_size (0)
{
	EnEx ee("twine::twine(const xmlChar* c)");
	if(c == NULL){
		throw AnException(0, FL, "Input is null.");
	}
	int csize = strlen((const char*)c);
	if(csize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(csize);
	strncpy(m_data, (const char*)c, csize);
	m_data_size = csize;
}

twine::twine(const char c) :
	m_data (NULL),
	m_allocated_size (0),
	m_data_size (0)
{
	EnEx ee("twine::twine(const char c)");
	reserve(1);
	m_data[0] = c;
	m_data_size = 1;
}

twine::twine(const xmlNodePtr node, const char* attrName):
	m_data (NULL),
	m_allocated_size (0),
	m_data_size (0)
{
	EnEx ee("twine::twine(const xmlNodePtr node, const char* c)");

	getAttribute(node, attrName);

}

twine::~twine() 
{
	EnEx ee("twine::~twine()");
	if(m_allocated_size > 0 || m_data != NULL){
		free(m_data);
		m_allocated_size = 0;
		m_data = NULL;
	}
}

twine& twine::operator=(const twine& t)
{
	EnEx ee("twine::operator=(const twine& t)");
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
	memset(m_data, 0, m_allocated_size);
	m_data_size = t.m_data_size;
	strncpy(m_data, t.m_data, m_data_size);
	return *this;
}

twine& twine::operator=(const twine* t) 
{
	EnEx ee("twine::operator=(const twine* t)");
	return operator=(t->c_str());
}

twine& twine::operator=(const char* c)
{
	EnEx ee("twine::operator=(const char* c)");
	int tsize = strlen(c);
	if(tsize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(tsize);
	memset(m_data, 0, m_allocated_size);
	m_data_size = tsize;
	strncpy(m_data, c, m_data_size);
	return *this;
}

twine& twine::operator<< (xmlChar* c)
{
	EnEx ee("twine::operator<<(const xmlChar* c)");
	int tsize = strlen((const char*)c);
	if(tsize > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(tsize);
	memset(m_data, 0, m_allocated_size);
	m_data_size = tsize;
	strncpy(m_data, (const char*)c, m_data_size);
	xmlFree(c);
	return *this;
}

twine& twine::operator=(const int i)
{
	EnEx ee("twine::operator=(const int i)");
	reserve(15);
	memset(m_data, 0, m_allocated_size);
	sprintf(m_data, "%d", i);
	m_data_size = strlen(m_data);
	return *this;
}
	
twine& twine::operator=(const float f)
{
	EnEx ee("twine::operator=(const float f)");
	reserve(31);
	memset(m_data, 0, m_allocated_size);
	sprintf(m_data, "%f", f);
	m_data_size = strlen(m_data);
	return *this;
}

twine& twine::operator+=(const twine& t)
{
	EnEx ee("twine::operator+=(const twine& t)");
	append(t());
	return *this;
}

twine& twine::operator+=(const twine* t)
{
	EnEx ee("twine::operator+=(const twine* t)");
	append((*t)());
	return *this;
}

twine& twine::operator+=(const char* c)
{
	EnEx ee("twine::operator+=(const char* c)");
	append(c);
	return *this;
}

twine& twine::operator+=(const char c)
{
	EnEx ee("twine::operator+=(const char c)");
	reserve(m_data_size + 1);
	m_data[m_data_size] = c;
	m_data_size ++;
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::operator+=(const int i)
{
	EnEx ee("twine::operator+=(const int i)");
	char tmp[16];
	memset(tmp, 0, 16);
	sprintf(tmp, "%d", i);
	append(tmp);
	return *this;
}

twine& twine::operator+=(const float i)
{
	EnEx ee("twine::operator+=(const float i)");
	char tmp[32];
	memset(tmp, 0, 32);
	sprintf(tmp, "%f", i);
	append(tmp);
	return *this;
}

int twine::get_int() const 
{
	EnEx ee("twine::get_int()");
	return atoi(m_data);
}

float twine::get_float() const 
{
	EnEx ee("twine::get_float()");
	return (float)atof(m_data);
}

double twine::get_double() const 
{
	EnEx ee("twine::get_double()");
	return atof(m_data);
}

const char twine::operator[](int i) const
{
	EnEx ee("twine::operator[](int i)");
	if( (i < 0) || (i >= m_data_size)){
		throw AnException(0,FL,"twine: Out Of Bounds Access");
	}
	return m_data[i];
}

const char* twine::operator()() const
{
	EnEx ee("twine::operator()()");
	return m_data;
}

const char* twine::c_str(void) const
{
	EnEx ee("twine::c_str()");
	return m_data;
}

twine::operator const xmlChar*() const
{
	EnEx ee("twine::operator(xmlChar*)()");
	return (const xmlChar*)m_data;
}

int twine::compare(const twine& t) const
{
	EnEx ee("twine::compare(twine& t)");
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
	EnEx ee("twine::compare(const char* c)");
	if((m_data_size == 0) && (c == NULL)){
		return 0; // equal if both null
	}
	if(m_data_size == 0 || c == NULL){
		return -1;
	}
	return strcmp(m_data, c);
}

int twine::compare(const twine& t, int count) const
{
	EnEx ee("twine::compare(twine& t, int count)");
	if((m_data_size == 0) && (t.m_data_size == 0)){
		return 0; // equal if both null
	}
	if((m_data_size == 0) || (t.m_data_size == 0)){
		return -1;
	}
	return strncmp(m_data, t.m_data, count);
}

int twine::compare(const char* c, int count) const
{
	EnEx ee("twine::compare(const char* c, int count)");
	if(m_data_size == 0){
		return -1;
	}
	return strncmp(m_data, c, count);
}

int twine::compare(int i) const
{
	EnEx ee("twine::compare(int i)");
	if(m_data_size == 0){
		return -1;
	}

	int m = atoi(m_data);
	if(m < i) return -1;
	else if(m > i) return 1;
	else return 0;
}

int twine::compare(float f) const
{
	EnEx ee("twine::compare(float f)");
	if(m_data_size == 0){
		return -1;
	}

	float m = (float)atof(m_data);
	if(m < f) return -1;
	else if(m > f) return 1;
	else return 0;
}

bool twine::endsWith(const twine& t) const
{
	EnEx ee("twine::endsWith(const twine& t)");
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

	int start = m_data_size - t.m_data_size;

	return strncmp(m_data + start, t.m_data, t.m_data_size) == 0;
}

char* twine::data(void)
{
	EnEx ee("twine::data(void)");
	return m_data;
}

int twine::check_size(void)
{
	EnEx ee("twine::check_size(void)");
	if(m_data != NULL){
		m_data_size = strlen(m_data);
	}
	return m_data_size;
}

twine& twine::set(const char* c)
{
	EnEx ee("twine::set(const char* c)");
	return operator=(c);
}

twine& twine::set(const char* c, int n)
{
	EnEx ee("twine::set(const char* c, int n)");
	if(n > MAX_INPUT_SIZE){
		throw AnException(0,FL,"twine: Input Too Large");
	}
	reserve(n);
	memset(m_data, 0, m_allocated_size);
	strncpy(m_data, c, n);
	m_data_size = n;
	return *this;
}
	
twine twine::substr(int start) const
{
	EnEx ee("twine::substr(int start)");
	bounds_check(start);
	twine ret(&m_data[start]);
	return ret;
}

twine twine::substr(int start, int count) const
{
	EnEx ee("twine::substr(int start, int count)");
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

twine* twine::substrp(int start) const
{
	EnEx ee("twine::substrp(int start)");
	bounds_check(start);
	twine *ret = new twine(&m_data[start]);
	return ret;
}

twine* twine::substrp(int start, int count) const
{
	EnEx ee("twine::substrp(int start, int count)");
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
	EnEx ee("twine::format(const char* f, ...)");
	
	va_start(ap, f);
	format(f, ap);
	va_end(ap);

	return *this;
}

twine& twine::format(const char* f, va_list ap) 
{
	EnEx ee("twine::format(const char* f, va_list ap)");
	int nsize;
	bool success = false;
	
	reserve(256); // make sure we have a minimum amount of space

	while(!success){
		memset(m_data, 0, m_allocated_size);
#ifdef _WIN32
		nsize = _vsnprintf(m_data, m_allocated_size - 1, f, ap);
#else
		nsize = vsnprintf(m_data, m_allocated_size - 1, f, ap);
#endif

		if(nsize < 0) { // older C libraries
			// double twine capacity
			reserve(m_allocated_size * 2);  
			success = false;
		} else if (nsize > (m_allocated_size - 1)){ // newer C lib
			// give it requested size
			reserve(nsize + 20);                 
			success = false;
		} else {
			success = true;
			m_data_size = nsize;
		}
	}
	return *this;
}

int twine::find(const char* needle) const
{
	EnEx ee("twine::find(const char* needle)");
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

int twine::find(const char c) const
{
	EnEx ee("twine::find(const char c)");
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

int twine::find(const twine& t) const
{
	EnEx ee("twine::find(const twine& t)");
	return find(t());
}

int twine::find(const char* needle, int p) const
{
	EnEx ee("twine::find(const char* needle, int p)");
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
	
int twine::find(const char c, int p) const
{
	EnEx ee("twine::find(const char c, int p)");
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

int twine::find(const twine& t, int p) const
{
	EnEx ee("twine::find(const twine& t, int p)");
	return find(t(), p);
}

int twine::rfind(const char c) const
{
	EnEx ee("twine::rfind(const char c)");
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

int twine::rfind(const char c, int p) const
{
	EnEx ee("twine::rfind(const char c, int p)");
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	bounds_check(p);
	while(p >= 0){
		if(m_data[p] == c)
			return p;
		p--;
	}
	return TWINE_NOT_FOUND;
}
	
int twine::rfind(const char* c) const
{
	EnEx ee("twine::rfind(const char* c)");
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	int p = m_data_size - 1;
	int len = strlen(c);
	while(p >= 0){
		if(strncmp(&m_data[p], c, len) == 0)
			return p;
		p--;
	}
	return TWINE_NOT_FOUND;
}

int twine::rfind(const char* c, int p) const
{
	EnEx ee("twine::rfind(const char* c, int p)");
	if(m_data_size == 0)
		return TWINE_NOT_FOUND;
	bounds_check(p);
	int len = strlen(c);
	while(p >= 0){
		if(strncmp(&m_data[p], c, len) == 0)
			return p;
		p--;
	}
	return TWINE_NOT_FOUND;
}

int twine::rfind(const twine& t) const
{
	EnEx ee("twine::rfind(const twine& t)");
	return rfind(t());
}

int twine::rfind(const twine& t, int p) const
{
	EnEx ee("twine::rfind(const twine& t, int p)");
	return rfind(t(), p);
}

int twine::countof(const char needle) const
{
	EnEx ee("twine::countof(const char needle)");
	int count = 0;
	for(int i = 0; i < m_data_size; i++){
		if(m_data[i] == needle) count++;
	}
	return count;
}

twine& twine::replace(int start, int count, const char* rep)
{
	EnEx ee("twine::replace(int start, int count, const char* rep)");
	if(rep == NULL){
		throw AnException(0, FL, "Invalid replacement value (NULL).");
	}
	bounds_check(start);
	
	if((start+count) >= m_data_size){
		erase(start);
		append(rep);
	} else {	
		int len = strlen(rep);
		int i;
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

twine& twine::replace(int start, int count, const twine* t)
{
	EnEx ee("twine::replace(int start, int count, const twine* t)");
	replace(start, count, t->c_str());
	return *this;
}

twine& twine::replace(int start, int count, const twine& t)
{
	EnEx ee("twine::replace(int start, int count, const twine& t)");
	replace(start, count, t());
	return *this;
}

twine& twine::replace(const char c, const char n)
{
	EnEx ee("twine::replace(const char c, const char n)");

	for(int i = 0; i < m_data_size; i++){
		if(m_data[i] == c){
			m_data[i] = n;
		}
	}
	return *this;
}

twine& twine::append(const char* c)
{
	EnEx ee("twine::append(const char* c)");
	int csize = strlen(c);
	reserve(m_data_size + csize);
	strcat(m_data, c);
	m_data_size += csize;
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::insert(int p, const char* c)
{
	EnEx ee("twine::insert(int p, const char* c)");
	bounds_check(p);

	int csize = strlen(c);
	reserve(m_data_size + csize);
	for(int i = m_data_size - 1; i >= p; i--){
		m_data[i+csize] = m_data[i];
	}
	strncpy(m_data + p, c, csize);
	m_data_size += csize;
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::erase(int p, int n)
{
	EnEx ee("twine::erase(int p, int n)");
	bounds_check(p);
	bounds_check(p+n-1);
	for(int i = p; i < m_data_size - n; i++){
		m_data[i] = m_data[i+n];
	}
	m_data_size -= n;	
	m_data[m_data_size] = '\0';
	return *this;
}

twine& twine::erase(int p)
{
	EnEx ee("twine::erase(int p)");
	bounds_check(p);
	m_data[p] = '\0';
	m_data_size = p;
	return *this;
}

twine& twine::erase(void)
{
	EnEx ee("twine::erase(void)");
	memset(m_data, 0, m_allocated_size);
	m_data_size = 0;
	return *this;
}

twine& twine::rtrim(void)
{
	EnEx ee("twine::rtrim(void)");
	int i = m_data_size - 1;
	while(i >= 0 && (isspace((unsigned char)m_data[i]) ||
		m_data[i] == '\r' ||
		m_data[i] == '\n')
	) {
		m_data[i--] = '\0';
		m_data_size --;
	}
	return *this;
}

twine& twine::ltrim(void)
{
	EnEx ee("twine::ltrim(void)");
	int i = 0;
	while( (i < m_data_size) && (isspace((unsigned char)m_data[i])) ) i++;

	if(i > 0){
		erase(0, i);
	}
	return *this;
}
		
	
twine& twine::reserve(int min_size) 
{
	EnEx ee("twine::reserve(int min_size)");
	if(m_allocated_size == 0){
		m_data = (char *)malloc(min_size + 1);
		if(m_data == NULL){
			throw AnException(0, FL,
				"twine: Error Allocating Memory");
		}
		m_allocated_size = min_size + 1;
		memset(m_data, 0, m_allocated_size);
		m_data_size = 0;
		return *this;
	}
	if(min_size < m_allocated_size) {
		return *this;
	} else {
		char *ptr = (char *)realloc(m_data, min_size+1);
		if(ptr == NULL){
			throw AnException(0, FL,
				"twine: Error reallocating memory.");
		}
		m_data = ptr;
		m_allocated_size = min_size+1;
		//for(int i = m_data_size; i < m_allocated_size; i++)
			//m_data[i] = '\0';
		return *this;
	}
}

int twine::size(void) const 
{ 
	EnEx ee("twine::size(void)");
	return m_data_size; 
}

int twine::length(void) const 
{ 
	EnEx ee("twine::length(void)");
	return m_data_size; 
}

int twine::max_size(void) const 
{ 
	EnEx ee("twine::max_size(void)");
	return m_allocated_size - 1; 
}

int twine::capacity(void) const 
{ 
	EnEx ee("twine::capacity(void)");
	return m_allocated_size - 1; 
}

bool twine::empty(void) const 
{ 
	EnEx ee("twine::empty(void)");
	return (m_data_size == 0); 
}
	
void twine::bounds_check(int p) const
{
	EnEx ee("twine::bounds_check(int p)");
	if( (p < 0) || (p >= m_data_size)){
		throw AnException(0, FL, "twine: Index out of bounds.");
	}
}

twine& twine::ucase(void)
{
	EnEx ee("twine::ucase(void)");
	int i;
	for(i = 0; i < m_data_size; i++){
		m_data[i] = toupper(m_data[i]);
	}
	return *this;
}

twine& twine::ucase(int i)
{
	EnEx ee("twine::ucase(int i)");
	bounds_check(i);
	m_data[i] = toupper(m_data[i]);
	return *this;
}

vector < twine > twine::split(twine spliton)
{
	EnEx ee("twine::split(twine spliton)");
	vector < twine > v;
	int idx1, idx2;

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

twine& twine::getAttribute(xmlNodePtr node, const char* attrName)
{
	EnEx ee("twine::getAttribute(xmlNodePtr node, const char* attrName)");

	AutoXMLChar tmp;
	tmp = xmlGetProp(node, (const xmlChar*)attrName);
	if(tmp() == NULL){
		return erase();
	}
	return set(tmp);
}

twine& twine::encode64()
{
	EnEx ee("twine::encode64()");

	// Reserve enough space to handle the conversion:
	size_t len = (size() + 2) * 4 / 3;
	len ++; // add space for the null.
	memptr< char > tmpspace = (char*)malloc( len );
	if(tmpspace == (char*)NULL){
		throw AnException(0, FL, "Error reserving space for base64 encoding.");
	}
	memset(tmpspace, 0, len);

	// Now do the conversion
	Base64::encode( m_data, tmpspace );

	// Now copy the converted data over to ourself:
	return operator=(tmpspace);
}

twine& twine::decode64()
{
	EnEx ee("twine::decode64()");

	// Reserve enough space to handle the conversion:
	size_t len = (size()) * 3 / 4;
	len ++; // add space for the null.
	memptr< char > tmpspace = (char*)malloc( len );
	if(tmpspace == (char*)NULL){
		throw AnException(0, FL, "Error reserving space for base64 decoding.");
	}
	memset(tmpspace, 0, len);

	// Now do the conversion
	Base64::decode( m_data, tmpspace );

	// Now copy the converted data over to ourself:
	return operator=(tmpspace);
}

