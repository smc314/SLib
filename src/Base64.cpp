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

/* $Id: Base64.cpp,v 1.1.1.1 2002/07/12 14:39:13 smc Exp $
 
   Copyright (c) 1997,1998 Gisle Aas
 
   The tables and some of the code is borrowed from metamail, which comes
   with this message:
 
   Copyright (c) 1991 Bell Communications Research, Inc. (Bellcore)
 
   Permission to use, copy, modify, and distribute this material
   for any purpose and without fee is hereby granted, provided
   that the above copyright notice and this permission notice
   appear in all copies, and that the name of Bellcore not be
   used in advertising or publicity pertaining to this
   material without the specific, prior written permission
   of an authorized representative of Bellcore.  BELLCORE
   MAKES NO REPRESENTATIONS ABOUT THE ACCURACY OR SUITABILITY
   OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED "AS IS",
   WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES.
 
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "Base64.h"
#include "Log.h"
using namespace SLib;

static char basis_64[64] =
    {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
    };


#define XX 255			/* illegal base64 char */
#define EQ 254			/* padding */
static unsigned char index_64[256] =
    {
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, 62, XX, XX, XX, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, XX, XX, XX, EQ, XX, XX,
        XX, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, XX, XX, XX, XX, XX,
        XX, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, XX, XX, XX, XX, XX,

        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
        XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX,
    };

#define MAX_LINE       76
#define GETC(str,len)  (len > 0 ? len--,*str++ : EOF)
#define INVALID_B64(c) (index_64[(unsigned char)c] == XX)

char *Base64::encode(const char *sv)
{
	char* r;
	size_t len, rlen;

	if(sv == NULL){
		return NULL; // nothing to encode.
	}

	len = strlen(sv);
	rlen = (len + 2) / 3 * 4;
	/* add space for null */
	rlen++;

	if ((r = (char *) malloc(rlen)) == NULL)
		return NULL;
	Base64::encode(sv, r);
	return r;
}
	
void Base64::encode(const char* sv, char* r)
{
	const char *s;   		/* string to encode */
	size_t len;   		/* length of the string */
	char *p;   		/* result string */
	size_t i;

	if(sv == NULL){
		return; // Nothing to encode.
	}

	s = sv;
	len = strlen(s);

	p = r;

	/* Transform the 3x8 bits to 4x6 bits, as required by base64.  */
	for (i = 0; i < (int)len; i += 3) {
		*p++ = basis_64[s[0] >> 2];
		*p++ = basis_64[((s[0] & 3) << 4) + (s[1] >> 4)];
		*p++ = basis_64[((s[1] & 0xf) << 2) + (s[2] >> 6)];
		*p++ = basis_64[s[2] & 0x3f];
		s += 3;
	}
	/* Pad the result if necessary...  */
	if (i == (int)len + 1)
		*(p - 1) = '=';
	else if (i == (int)len + 2)
		*(p - 1) = *(p - 2) = '=';
	/* ...and zero-terminate it.  */
	*p = '\0';
}

char *Base64::decode(const char *sv)
{
	char* r;
	size_t len, rlen;

	if(sv == NULL){
		return NULL; // nothing to decode
	}

	len = strlen(sv);
	rlen = len * 3 / 4;
	if ((r = (char *) malloc(rlen)) == NULL)
		return NULL;

	Base64::decode(sv, r);
	return r;
}

void Base64::decode(const char* sv, char* ret)
{
	unsigned char *str;
	size_t len;
	char *r;
	size_t c1, c2, c3, c4;

	if(sv == NULL){
		return; // nothing to decode
	}

	len = strlen(sv);
	str = (unsigned char *)sv;

	r = ret;

	while ((c1 = GETC(str, len)) != EOF) {
		if (INVALID_B64(c1))
			continue;
		do {
			c2 = GETC(str, len);
		} while (c2 != EOF && INVALID_B64(c2));
		do {
			c3 = GETC(str, len);
		} while (c3 != EOF && INVALID_B64(c3));
		do {
			c4 = GETC(str, len);
		} while (c4 != EOF && INVALID_B64(c4));

		if (c2 == EOF || c3 == EOF || c4 == EOF) {
			ERRORL(FL, "Premature end of base64 data");
			if (c2 == EOF)
				break;
			if (c3 == EOF)
				c3 = '=';
			c4 = '=';
		} else if (c1 == '=' || c2 == '=') {
			ERRORL(FL, "Premature padding of base64 data");
			break;
		}

		c1 = index_64[c1];
		c2 = index_64[c2];
		*r++ = (char)((c1 << 2) | ((c2 & 0x30) >> 4));

		if (c3 == '=') {
			break;
		} else {
			c3 = index_64[c3];
			*r++ = ((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2);
		}
		if (c4 == '=') {
			break;
		} else {
			c4 = index_64[c4];
			*r++ = (char)(((c3 & 0x03) << 6) | c4);
		}
	}
	*r = '\0';
}

void Base64::Free(char* c)
{
	free(c);
}
