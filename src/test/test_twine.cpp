
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

#include "twine.h"
using namespace SLib;

#include "catch.hpp"

TEST_CASE( "Twine - short string", "[twine]" )
{
	twine t( "I am a short string" );

	REQUIRE_FALSE( t.empty() );
	REQUIRE( t.size() == 19 );
	REQUIRE( t.capacity() >= 19 );
}

TEST_CASE( "Twine - assignment from const char", "[twine]" )
{
	SECTION( "Direct assignment of short string" ){
		twine t = "I am a short string";
		REQUIRE_FALSE( t.empty() );
		REQUIRE( t.size() == 19 );
		REQUIRE( t.capacity() >= 19 );
	}

	SECTION( "Assignment after construction of short string" ){
		twine t; t = "I am a short string";
		REQUIRE_FALSE( t.empty() );
		REQUIRE( t.size() == 19 );
		REQUIRE( t.capacity() >= 19 );
	}

	SECTION( "Direct assignment of a long string" ){
		twine t = "I am a longer string that will not fit in optimized storage.";
		REQUIRE_FALSE( t.empty() );
		REQUIRE( t.size() == 60 );
		REQUIRE( t.capacity() >= 60 );
	}

	SECTION( "Assignment after construction of a long string" ){
		twine t; t = "I am a longer string that will not fit in optimized storage.";
		REQUIRE_FALSE( t.empty() );
		REQUIRE( t.size() == 60 );
		REQUIRE( t.capacity() >= 60 );
	}

}
