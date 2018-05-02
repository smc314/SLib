
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

/* ******************************************************* */
/* Simple program to test the functions in base64.cpp      */
/* ******************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Base64.h"
#include "Tools.h"
using namespace SLib;

#include "catch.hpp"

TEST_CASE( "Base64 - Encode of NULL returns NULL", "[base64]" )
{
	char* b64 = NULL;
	char* sampleInput = NULL;

	b64 = Base64::encode( sampleInput );
	REQUIRE( b64 == NULL );
}

TEST_CASE( "Base64 - Simple Encode Works", "[base64]" )
{
	char* b64 = NULL;
	const char* sampleInput = "hithere:yothere";
	const char* expectedOutput = "aGl0aGVyZTp5b3RoZXJl\n";

	b64 = Base64::encode( sampleInput );

	REQUIRE( b64 != NULL ); // Did not return a null
	REQUIRE( strcmp( b64, expectedOutput ) == 0);
}

TEST_CASE( "Base64 - Length Matches", "[base64]" )
{
	char* b64 = NULL;
	char* b64_2 = NULL;
	const char* sampleInput = "hithere:yothere";
	const char* expectedOutput = "aGl0aGVyZTp5b3RoZXJl\n";
	size_t resultLength = 0;

	b64 = Base64::encode( sampleInput );
	b64_2 = Base64::encode( sampleInput, strlen(sampleInput), &resultLength );

	REQUIRE( b64 != NULL ); // Did not return a null
	REQUIRE( b64_2 != NULL ); // Did not return a null

	size_t b64_size = strlen( b64 );
	REQUIRE( b64_size == resultLength );

	//printf("%s\n", Tools::hexDump( b64, "Base64::encode - output", 16, b64_size + 16, true, false)() );

	REQUIRE( b64_size == strlen(expectedOutput) ); // Correct size

	REQUIRE( strcmp( b64, expectedOutput ) == 0);
}
