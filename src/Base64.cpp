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

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include <string.h>

#include "Base64.h"
#include "Log.h"
using namespace SLib;

char *Base64::encode(const char *sv)
{
	if(sv == NULL){
		return NULL; // nothing to encode.
	}
	size_t output_length;
	return encode(sv, strlen(sv), &output_length);
}
	
void Base64::encode(const char* sv, char* r)
{
	Base64::encode(sv, strlen(sv), r);
}

void Base64::encode(const char* sv, size_t sv_len, char* r)
{
	size_t output_length;
	char* output_data = encode( sv, sv_len, &output_length);
	memcpy( r, output_data, output_length);
	free(output_data);
}

char* Base64::encode(const char* data, size_t input_length, size_t* output_length)
{
	BIO *bmem, *b64;
	BUF_MEM* bptr;

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, data, (int)input_length); // writing to the base64 filter encodes the data
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	*output_length = bptr->length;
	char* output_data = (char*)malloc(*output_length);
	memcpy(output_data, bptr->data, bptr->length);

	BIO_free_all(b64);

	return output_data;
}

char *Base64::decode(const char *sv)
{
	if(sv == NULL){
		return NULL; // nothing to decode
	}
	size_t output_len;
	return Base64::decode(sv, strlen(sv), &output_len);
}

size_t Base64::decode(const char* sv, char* ret)
{
	if(sv == NULL){
		return 0; // nothing to decode
	}
	size_t output_len;
	char* decoded_data = Base64::decode(sv, strlen(sv), &output_len);
	memcpy(ret, decoded_data, output_len);
	return output_len;
}

void Base64::Free(char* c)
{
	free(c);
}

char* Base64::decode(const char* data, size_t input_length, size_t* output_length)
{
	BIO *bmem, *b64;

	// Our output is always smaller than the input.  Allocate a buffer based on the
	// input size, and it will always be big enough to hold the output.
	char* output_data = (char*) malloc(input_length);
	memset(output_data, 0, input_length);

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new_mem_buf((void*)data, (int)input_length);
	bmem = BIO_push(b64, bmem);
	
	// The real output length is returned from the BIO_read function
	*output_length = BIO_read(bmem, output_data, (int)input_length);

	BIO_free_all(bmem);

	return output_data;
}

void Base64::BuildDecodingTable()
{
}
