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

#include <openssl/evp.h>

#include "MemBuf.h"
#include "AnException.h"
#include "EnEx.h"
#include "AutoXMLChar.h"
#include "XmlHelpers.h"
#include "memptr.h"
#include "zip.h"

#include "zlib.h"

using namespace SLib;

MemBuf::MemBuf() :
	m_data (NULL),
	m_data_size (0),
	userIntVal (0)
{
	/* ************************************************************************ */
	/* MemBuf's are used during log processing.  For this reason, do not include */
	/* the ee(FL, ...) version of the EnEx call.                                */
	/* ************************************************************************ */
	EnEx ee("MemBuf::MemBuf()");
}

MemBuf::MemBuf(const MemBuf& t) :
	m_data (NULL),
	m_data_size (0),
	userIntVal (0)
{
	EnEx ee("MemBuf::MemBuf(const MemBuf& t)");

	// short circuit for source having nothing in it.
	if(t.m_data_size == 0) {
		return;
	}

	reserve(t.m_data_size);
	m_data_size = t.m_data_size;
	memcpy(m_data, t.m_data, m_data_size);
}

MemBuf::MemBuf(const char* c) :
	m_data (NULL),
	m_data_size (0),
	userIntVal (0)
{
	EnEx ee("MemBuf::MemBuf(const char* c)");
	if(c == NULL){
		throw AnException(0, FL, "Input is null.");
	}
	size_t csize = strlen(c);
	reserve(csize);
	memcpy(m_data, c, csize);
	m_data_size = csize;
}

MemBuf::MemBuf(const xmlChar* c) :
	m_data (NULL),
	m_data_size (0),
	userIntVal (0)
{
	EnEx ee("MemBuf::MemBuf(const xmlChar* c)");
	if(c == NULL){
		throw AnException(0, FL, "Input is null.");
	}
	size_t csize = strlen((const char*)c);
	reserve(csize);
	memcpy(m_data, (const char*)c, csize);
	m_data_size = csize;
}

MemBuf::MemBuf(const size_t s) :
	m_data (NULL),
	m_data_size (0),
	userIntVal (0)
{
	EnEx ee("MemBuf::MemBuf(const size_t s)");
	reserve(s);
	m_data_size = s;
}

MemBuf::MemBuf(const twine& c):
	m_data (NULL),
	m_data_size (0),
	userIntVal (0)
{
	EnEx ee("MemBuf::MemBuf(const twine& c)");

	reserve( c.size() );
	memcpy(m_data, c(), c.size());
	m_data_size = c.size();

}

MemBuf::~MemBuf() 
{
	EnEx ee("MemBuf::~MemBuf()");
	if(m_data_size > 0 || m_data != NULL){
		if(m_data != NULL){
			free(m_data);
		}
		m_data_size = 0;
		m_data = NULL;
		userIntVal = 0;
	}
}

MemBuf& MemBuf::operator=(const MemBuf& t)
{
	EnEx ee("MemBuf::operator=(const MemBuf& t)");
	if(&t == this){
		return *this;
	}

	// Clear out anything that we have.
	clear();

	// short circuit for source having nothing in it.
	if(t.m_data_size == 0){
		return *this;
	}

	reserve(t.m_data_size);
	m_data_size = t.m_data_size;
	memcpy(m_data, t.m_data, m_data_size);
	userIntVal = t.userIntVal;
	return *this;
}

MemBuf& MemBuf::operator=(const char* c)
{
	EnEx ee("MemBuf::operator=(const char* c)");
	if(c == NULL){
		throw AnException(0, FL, "MemBuf = NULL not allowed.");
	}
	
	// Clear out anything that we have.
	clear();

	size_t tsize = strlen(c);

	reserve(tsize);
	m_data_size = tsize;
	memcpy(m_data, c, m_data_size);
	return *this;
}

MemBuf& MemBuf::operator=(const xmlChar* c)
{
	EnEx ee("MemBuf::operator=(const xmlChar* c)");

	// Clear out anything that we have.
	clear();

	size_t tsize = strlen((const char*)c);

	reserve(tsize);
	m_data_size = tsize;
	memcpy(m_data, c, m_data_size);
	return *this;
}
	
MemBuf& MemBuf::operator=(const twine& t)
{
	EnEx ee("MemBuf::operator=(const twine& t)");
	
	// Clear out anything that we have.
	clear();

	reserve(t.size());
	m_data_size = t.size();
	memcpy(m_data, t(), m_data_size);
	return *this;
}

MemBuf& MemBuf::operator+=(const MemBuf& t)
{
	EnEx ee("MemBuf::operator+=(const MemBuf& t)");
	append(t);
	return *this;
}

MemBuf& MemBuf::operator+=(const char* c)
{
	EnEx ee("MemBuf::operator+=(const char* c)");
	append(c);
	return *this;
}

MemBuf& MemBuf::operator+=(const xmlChar* c)
{
	EnEx ee("MemBuf::operator+=(const xmlChar* c)");
	append((const char*)c);
	return *this;
}

MemBuf& MemBuf::operator+=(const twine& t)
{
	EnEx ee("MemBuf::operator+=(const size_t i)");
	append(t());
	return *this;
}

const char MemBuf::operator[](size_t i) const
{
	EnEx ee("MemBuf::operator[](size_t i)");
	if( (i < 0) || (i >= m_data_size)){
		throw AnException(0,FL,"MemBuf: Out Of Bounds Access");
	}
	return ((char*)m_data)[i];
}

const char* MemBuf::operator()() const
{
	return (char*)m_data;
}

int MemBuf::compare( const MemBuf& other) const
{
	EnEx ee("MemBuf::compare( const MemBuf& other)");

	// Are they both empty?
	if(m_data_size == 0 && other.m_data_size == 0){
		return 0;
	}

	// At least one is not empty, compare sizes first
	if(m_data_size < other.m_data_size){
		return -1;
	} else if(m_data_size > other.m_data_size){
		return +1;
	} else {
		// data sizes match, compare contents directly
		return memcmp( m_data, other.m_data, m_data_size );
	}
}

char* MemBuf::data(void)
{
	EnEx ee("MemBuf::data(void)");
	return (char*)m_data;
}

MemBuf& MemBuf::set(const char* c)
{
	EnEx ee("MemBuf::set(const char* c)");
	return operator=(c);
}

MemBuf& MemBuf::set(const char* c, size_t n)
{
	EnEx ee("MemBuf::set(const char* c, size_t n)");

	// Clear out anything that we have
	clear();

	reserve(n);
	memcpy(m_data, c, n);
	m_data_size = n;
	return *this;

}
	
MemBuf& MemBuf::replace(size_t start, const MemBuf& rep, size_t count)
{
	EnEx ee("MemBuf::replace(size_t start, const MemBuf& rep, size_t count)");
	bounds_check(start);
	
	if((start+count) >= m_data_size){
		erase(start);
		append(rep);
	} else {	
		size_t len = rep.size();
		if(len >= count){
			memcpy( (char*)m_data + start, rep(), count );
		} else if (len < count) {
			memcpy( (char*)m_data + start, rep(), len );
		}
	}

	return *this;
}	

MemBuf& MemBuf::replace(size_t start, const twine& rep, size_t count)
{
	EnEx ee("MemBuf::replace(size_t start, const twine& rep, size_t count)");
	bounds_check(start);
	
	if((start+count) >= m_data_size){
		erase(start);
		append(rep);
	} else {	
		size_t len = rep.size();
		if(len >= count){
			memcpy( (char*)m_data + start, rep(), count );
		} else if (len < count) {
			memcpy( (char*)m_data + start, rep(), len );
		}
	}

	return *this;
}	

MemBuf& MemBuf::replace(size_t pos, const char n)
{
	EnEx ee("MemBuf::replace(size_t pos, const char n)");

	bounds_check(pos);
	((char*)m_data)[ pos ] = n;

	return *this;
}

MemBuf& MemBuf::append(const char* c)
{
	EnEx ee("MemBuf::append(const char* c)");
	if(c == NULL){
		return *this; // nothing to append
	}

	size_t oldSize = m_data_size;
	size_t csize = strlen(c);
	reserve(m_data_size + csize);

	memcpy((char*)m_data + oldSize, c, csize);

	return *this;
}

MemBuf& MemBuf::append(const char* c, size_t csize)
{
	EnEx ee("MemBuf::append(const char* c, size_t csize)");
	if(c == NULL || csize == 0){
		return *this; // nothing to append
	}

	size_t oldSize = m_data_size;
	reserve(m_data_size + csize);

	memcpy((char*)m_data + oldSize, c, csize);

	return *this;
}

MemBuf& MemBuf::append(const MemBuf& c)
{
	EnEx ee("MemBuf::append(const MemBuf& c)");
	if(c.size() == 0){
		return *this; // nothing to append
	}

	size_t oldSize = m_data_size;
	reserve(m_data_size + c.size());

	memcpy((char*)m_data + oldSize, c.m_data, c.size());

	return *this;
}

MemBuf& MemBuf::erase(size_t p, size_t n)
{
	EnEx ee("MemBuf::erase(size_t p, size_t n)");

	bounds_check(p);
	bounds_check(p+n-1);

	memset((char*)m_data + p, 0, n);

	return *this;
}

MemBuf& MemBuf::erase(size_t p)
{
	EnEx ee("MemBuf::erase(size_t p)");
	bounds_check(p);

	// 0 1 2 3 4 5 6 7 8 9
	// A B C D E F G H I J
	// m_data_size = 10
	// p = 3
	// count = 7

	memset((char*)m_data + p, 0, m_data_size - p);

	return *this;
}

MemBuf& MemBuf::erase(void)
{
	EnEx ee("MemBuf::erase(void)");
	if(m_data_size == 0){
		return *this; // nothing to do
	}

	memset(m_data, 0, m_data_size);

	return *this;
}

MemBuf& MemBuf::clear(void)
{
	EnEx ee("MemBuf::clear(void)");

	if(m_data_size == 0){
		return *this; // nothing to do
	}
	if(m_data != NULL){
		free(m_data);
	}
	m_data = NULL;
	m_data_size = 0;

	return *this;
}

MemBuf& MemBuf::reserve(size_t min_size) 
{
	EnEx ee("MemBuf::reserve(size_t min_size)");
	if(m_data_size == 0){
		m_data = malloc(min_size + 10);
		if(m_data == NULL){
			throw AnException(0, FL, "MemBuf: Error Allocating Memory");
		}
		m_data_size = min_size;
		memset(m_data, 0, min_size + 10);
		return *this;
	}
	if(min_size < m_data_size) {
		return *this;
	} else {
		char *ptr = (char *)realloc(m_data, min_size+10);
		if(ptr == NULL){
			throw AnException(0, FL, "MemBuf: Error reallocating memory.");
		}
		m_data = ptr;
		size_t oldSize = m_data_size;
		m_data_size = min_size;
		erase( oldSize ); // ensure the new segment of memory is zeroed 

		return *this;
	}
}

size_t MemBuf::size(void) const 
{ 
	return m_data_size; 
}

void MemBuf::size(size_t newSize)
{ 
	EnEx ee("MemBuf::size(size_t newSize)");
	// Make sure we have enough space - if they are asking for a bigger size, this
	// will allocate the required data.
	if(newSize == m_data_size){
		return; // bail out early - sizes already match
	}
	if(newSize > m_data_size){
		reserve(newSize);
	} else {
		erase(newSize); // Clean out anything after the newSize
		m_data_size = newSize; // record the new size.
	}
}

size_t MemBuf::length(void) const 
{ 
	EnEx ee("MemBuf::length(void)");
	return m_data_size; 
}

bool MemBuf::empty(void) const 
{ 
	EnEx ee("MemBuf::empty(void)");
	return (m_data_size == 0); 
}
	
void MemBuf::bounds_check(size_t p) const
{
	EnEx ee("MemBuf::bounds_check(size_t p)");
	if( (p < 0) || (p >= m_data_size)){
		throw AnException(0, FL, "MemBuf: Index out of bounds. p(%d) m_data_size(%d)", p, m_data_size);
	}
}

MemBuf& MemBuf::encode64()
{
	EnEx ee("MemBuf::encode64()");

	// Run the conversion
	size_t len;
	memptr< char > tmpspace; tmpspace = Base64::encode( (char*)m_data, size(), &len);
	if(tmpspace == (char*)NULL){
		throw AnException(0, FL, "Error base64 encoding.");
	}
	
	// Erase what we had in the buffer:
	clear();

	// Re-assign the encoded buffer to ourself:
	m_data = tmpspace.release();
	m_data_size = len;

	return *this;
}

MemBuf& MemBuf::encode64url()
{
	EnEx ee("MemBuf::encode64url()");

	// First do the standard base64 encode:
	encode64();

	// Now convert our contents to a twine so they are easier to manipulate.
	twine tmp; tmp.set( (const char*)m_data, m_data_size );

	// Now adjust the twine a bit:
	// Strip out any newlines in the content
	tmp.replaceAll( "\n", "" );

	// Remove any trailing '='s
	tmp.replaceAll( "=", "" );

	tmp.replace( '+', '-' ); // Plusses convert to dashes
	tmp.replace( '/', '_' ); // Slashes become underscores

	// Re-set our contents from the twine
	set( tmp(), tmp.size() );

	// Return ourselves
	return *this;
}

MemBuf& MemBuf::decode64()
{
	EnEx ee("MemBuf::decode64()");

	// Run the conversion
	size_t len;
	memptr< char > tmpspace; tmpspace = Base64::decode( (char*)m_data, size(), &len);
	if(tmpspace == (char*)NULL){
		throw AnException(0, FL, "Error base64 decoding.");
	}

	// Erase what we had in the buffer:
	clear();

	// Re-assign the decoded buffer to ourself:
	m_data = tmpspace.release();
	m_data_size = len;

	return *this;
}

MemBuf& MemBuf::decode64url()
{
	EnEx ee("MemBuf::decode64url()");

	// First convert our contents to a twine so they are easier to manipulate
	twine tmp; tmp.set( (const char*)m_data, m_data_size );

	tmp.replace( '-', '+' ); // dashes convert to plusses
	tmp.replace( '_', '/' ); // underscores become slashes
	tmp.replace( ',', '=' ); // commas become equals
	switch(tmp.length() % 4){ // Pad with trailing '='s
		case 0: break; // Nothing to do
		case 2: tmp.append( "==" ); // Add 2 equals to the end of the string
		case 3: tmp.append( "=" ); // Add 1 equals to the end of the string
		default:
			throw AnException(0, FL, "Invalid length for base64 url decoding.");
	}

	// Split into 64 character lines
	size_t i = 64;
	while(i < tmp.size()){
		tmp.insert(i, "\n");
		i++; // for the newline
		i += 64; // next 64 characters
	}

	// Add a linefeed at the end
	append( "\n" );

	// Re-set our contents from the twine
	set( tmp(), tmp.size() );

	// Now do the actual decode
	return decode64();
}

MemBuf& MemBuf::zip()
{
	EnEx ee("MemBuf::zip()");

	if(m_data_size == 0){
		return *this;
	}

	int ret;
	z_stream strm;

	/* allocate deflate state *
	 * -- Use default allocation fuctions */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = m_data_size;
	strm.next_in = (Bytef*) m_data;

	ret = deflateInit2(&strm,
			Z_BEST_COMPRESSION, // level:
			Z_DEFLATED, // method: must be Z_DEFLATED in current version of zlib
			15+16, // windowBits: lg(winSize), [8,15];larger:better comp, more mem;+16 gzip
			9, // memLevel: [1,9]; max mem, optimal speed
			Z_DEFAULT_STRATEGY // strategy: don't know data type, so go with default
			);
	if(ret != Z_OK){
		throw AnException(0, FL,"Error initializing zlib deflate: %s", strm.msg);
	}

	size_t destSize = (size_t)deflateBound(&strm, m_data_size);

	// For now, we'll allocate a temp buffer.
	memptr<unsigned char> dest((unsigned char*)malloc( destSize ));
	memset( dest, 0, destSize );

	// set up data pointers
	strm.avail_out = destSize;
	strm.next_out = dest;

	ret = deflate(&strm, Z_FINISH); // do it in one go
	if(ret != Z_STREAM_END){
		deflateEnd(&strm);
		throw AnException(0, FL, "deflate() did not return Z_STREAM_END: %s", strm.msg);
	}

	destSize -= strm.avail_out;

	deflateEnd(&strm);

	size(destSize); // resize our buffer to the compressed size
	memcpy( m_data, dest, destSize );

	return *this;
}

MemBuf& MemBuf::unzip()
{
	EnEx ee("MemBuf::unzip()");

	if(m_data_size == 0){
		return *this;
	}

	int ret;
	z_stream strm;

	/* allocate inflate state             *
	 * -- Use default allocation fuctions */
	strm.zalloc = Z_NULL;
	strm.zfree  = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = m_data_size;
	strm.next_in = (Bytef*) m_data;

	// initialize the inflate stream
	ret = inflateInit2(&strm,
			15+16 // windowBits, corresponds with zip 
			);
	if(ret != Z_OK){
		throw AnException(0, FL, "Error initializing zlib inflate: %s", strm.msg);
	}

	/* Note: zlib has an inflateGetHeader() function that will tell inflate() to put
	 * the gzip header into a gz_header struct. This, however, requires a call to inflate()
	 * to actually decode. It is unclear whether we can catch a malformed data stream
	 * in this way. */

	// Allocate a buffer. Likely, most cases will be at most twice the data size
	size_t destSize = m_data_size * 2;
	memptr<unsigned char> dest((unsigned char*)malloc(destSize));
	memset( dest, 0, destSize );

	strm.avail_out = destSize;
	strm.next_out = dest;

	ret = inflate( &strm, Z_NO_FLUSH ); // Unzip as much as possible
	while(ret == Z_OK){
		dest = (unsigned char*)realloc(dest.release(), destSize * 2);
		memset( dest + destSize, 0, destSize );
		strm.avail_out = destSize; // Because we're doubling each time
		strm.next_out = dest + destSize; // Move to end of saved data
		destSize *= 2;
		ret = inflate( &strm, Z_NO_FLUSH ); // Unzip as much as possible
	}
	if(ret != Z_STREAM_END){
		// At some point, it may make sense to use this if to simply return *this
		// because it'll usually mean a malformed (i.e., not zipped) data stream.
		// Perhaps have multiple return value checks with different results.
		deflateEnd(&strm);
		throw AnException(0, FL, "error while inflating: %s", strm.msg);
	}

	destSize -= strm.avail_out; // get the number of bytes written
	deflateEnd(&strm);

	size(destSize);

	memcpy(m_data, dest, destSize);

	return *this;
}

xmlDocPtr MemBuf::Encrypt(RSA* keypair, bool usePublic)
{
	EnEx ee("MemBuf::Encrypt()");

	if(keypair == NULL){
		throw AnException(0, FL, "Invalid RSA keypair given: NULL");
	}

	int keysize = RSA_size(keypair);
	MemBuf rsaOut( (size_t) keysize );
	MemBuf rsaOutTrimmed;
	int chunkSize = keysize - 42;

	size_t loopCount = m_data_size / chunkSize;
	size_t leftOver = m_data_size % chunkSize;
	int encryptedSize;

	sptr<xmlDoc, xmlFreeDoc> doc = xmlNewDoc((const xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, (const xmlChar*)"Encrypted", NULL);
	xmlNodePtr root = xmlDocGetRootElement(doc);

	for(size_t i = 0; i < loopCount; i++){
		rsaOut.erase(); // clear the output buffer.
		if(usePublic){
			encryptedSize = RSA_public_encrypt( 
				chunkSize,                                // how big is the data
				(unsigned char*)m_data + (i * chunkSize), // where is the data
				(unsigned char*)rsaOut.data(),            // where does the encrypted data go
				keypair,                                  // public/private keypair
				RSA_PKCS1_OAEP_PADDING                    // what type of padding to use
			);
		} else {
			encryptedSize = RSA_private_encrypt( 
				chunkSize,                                // how big is the data
				(unsigned char*)m_data + (i * chunkSize), // where is the data
				(unsigned char*)rsaOut.data(),            // where does the encrypted data go
				keypair,                                  // public/private keypair
				RSA_PKCS1_OAEP_PADDING                    // what type of padding to use
			);
		}
		rsaOutTrimmed.set( rsaOut.data(), encryptedSize );

		// Save to the XML as a base64 CDATA section:
		xmlNodePtr chunk = xmlNewChild(root, NULL, (const xmlChar*)"chunk", NULL);
		XmlHelpers::setBase64( chunk, rsaOutTrimmed );

		// keep going through the whole buffer
	}

	// Now, encrypt anything left at the end of the buffer:
	if(leftOver != 0){
		rsaOut.erase(); // clear the output buffer.
		if(usePublic){
			encryptedSize = RSA_public_encrypt( 
				(int)leftOver,                                    // how big is the data
				(unsigned char*)m_data + (loopCount * chunkSize), // where is the data
				(unsigned char*)rsaOut.data(),                    // where does the encrypted data go
				keypair,                                          // public/private keypair
				RSA_PKCS1_OAEP_PADDING                            // what type of padding to use
			);
		} else {
			encryptedSize = RSA_private_encrypt( 
				(int)leftOver,                                    // how big is the data
				(unsigned char*)m_data + (loopCount * chunkSize), // where is the data
				(unsigned char*)rsaOut.data(),                    // where does the encrypted data go
				keypair,                                          // public/private keypair
				RSA_PKCS1_OAEP_PADDING                            // what type of padding to use
			);
		}
		rsaOutTrimmed.set( rsaOut.data(), encryptedSize );

		// Save to the XML as a base64 CDATA section:
		xmlNodePtr chunk = xmlNewChild(root, NULL, (const xmlChar*)"chunk", NULL);
		XmlHelpers::setBase64( chunk, rsaOutTrimmed );
	}

	// Finally, return the xml document
	return doc.release();
}

MemBuf& MemBuf::Decrypt(xmlDocPtr doc, RSA* keypair, bool usePrivate)
{
	EnEx ee("MemBuf::Decrypt()");

	if(keypair == NULL){
		throw AnException(0, FL, "Invalid RSA keypair given: NULL");
	}

	// Clear out any data that we are already holding, and reset our size to zero.
	clear();

	int keysize = RSA_size(keypair);
	int decryptedSize;
	MemBuf rsaOut( (size_t) keysize );

	xmlNodePtr root = xmlDocGetRootElement(doc);
	vector<xmlNodePtr> chunks = XmlHelpers::FindChildren( root, "chunk" );

	for(size_t i = 0; i < chunks.size(); i++){
		// Pick up the raw base64 data:
		MemBuf chunk = XmlHelpers::getCDATASection( chunks[i] );
		// Decode to binary in place:
		chunk.decode64();

		rsaOut.erase(); // clear the output buffer.
		if(usePrivate){
			decryptedSize = RSA_private_decrypt( 
				(int)chunk.size(),             // how big is the data
				(unsigned char*)chunk.data(),  // where is the data
				(unsigned char*)rsaOut.data(), // where does the decrypted data go
				keypair,                       // public/private keypair
				RSA_PKCS1_OAEP_PADDING         // what type of padding to use
			);
		} else {
			decryptedSize = RSA_public_decrypt( 
				(int)chunk.size(),             // how big is the data
				(unsigned char*)chunk.data(),  // where is the data
				(unsigned char*)rsaOut.data(), // where does the decrypted data go
				keypair,                       // public/private keypair
				RSA_PKCS1_OAEP_PADDING         // what type of padding to use
			);
		}

		// Append the decrypted chunk of data to our buffer.
		append( rsaOut.data(), decryptedSize );

		// keep going through all of the xml nodes.
	}

	// Finally, return ourselves
	return *this;
}

MemBuf MemBuf::Digest(RSA* keypair)
{
	EnEx ee("MemBuf::Digest(RSA* keypair)");

	int rCode = 0;
	MemBuf ret((size_t)EVP_MAX_MD_SIZE);

	const EVP_MD *md = EVP_get_digestbyname( "sha256" );
	if(md == NULL){
		// Try loading all of the digest algorithms - they may not have been loaded yet
		OpenSSL_add_all_digests();
		md = EVP_get_digestbyname( "sha256" );
		if(md == NULL){
			throw AnException(0, FL, "Could not load the sha256 digest routines from OpenSSL");
		}
	}

	EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
	if(keypair == NULL) {
		// No signature required - just a refular digest
		unsigned int md_len = 0;
		EVP_DigestInit_ex(mdctx, md, NULL);
		EVP_DigestUpdate(mdctx, (unsigned char*)m_data, m_data_size );
		EVP_DigestFinal_ex(mdctx, (unsigned char*)ret.data(), &md_len);
		ret.size( (size_t)md_len );
	} else {
		// Convert the RSA keypair into an EVP keypair:
		EVP_PKEY *pkey = EVP_PKEY_new();
		if(pkey == NULL){
			EVP_MD_CTX_destroy(mdctx);
			throw AnException(0, FL, "Could not create an EVP key to hold the RSA keypair for signing.");
		}
		rCode = EVP_PKEY_set1_RSA(pkey, keypair);
		if(rCode == 0){
			EVP_MD_CTX_destroy(mdctx);
			throw AnException(0, FL, "Could not set the RSA keypair given into the EVP key.");
		}

		// Create our signing context
		EVP_PKEY_CTX *pctx = NULL;
		rCode = EVP_DigestSignInit( mdctx, &pctx, md, NULL, pkey );
		if(rCode == 0){
			EVP_MD_CTX_destroy(mdctx);
			throw AnException(0, FL, "Could not initialize the digest signature context.");
		}

		// Pass in the data to be digested and signed
		EVP_DigestSignUpdate(mdctx, (unsigned char*)m_data, m_data_size );

		// Finalize with a NULL output buffer so that we can get the size of the output required
		size_t sd_len = 0;
		EVP_DigestSignFinal(mdctx, NULL, &sd_len);

		ret.reserve( sd_len );

		// Call Finalize again with the real output buffer to get the data
		EVP_DigestSignFinal(mdctx, (unsigned char*)ret.data(), &sd_len );
		ret.size( sd_len );
		
	}
	EVP_MD_CTX_destroy(mdctx);

	//printf("Internal value:\n%s\n", ret.hex()() );

	return ret;
}

twine MemBuf::hex() const
{
	EnEx ee("MemBuf::hex()");

	twine ret;
	for(size_t i = 0; i < size(); i++){
		twine hexChar; hexChar.format("%02x", (unsigned)(unsigned char)(*this)[i]);
		ret += hexChar;
	}

	return ret;
}
