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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>

#include "twine.h"
#include "xmlinc.h"
#include "AnException.h"
using namespace SLib;

#include "catch.hpp"

TEST_CASE( "Twine - Basic Finds", "[twine][twine-find]" )
{
    //        0123456789 123456789 123456789 1234
    twine t( "Search search SEARCH se8arch search" );

    // twine::find(char*)
    REQUIRE( t.find( "Search" ) == 0 );
    REQUIRE( t.find( "search" ) == 7 );
    REQUIRE( t.find( "SEARCH" ) == 14 );
    REQUIRE( t.find( "FOOBAR" ) == TWINE_NOT_FOUND );

    // twine::find(char)
    REQUIRE( t.find( 'S' ) == 0 );
    REQUIRE( t.find( 's' ) == 7 );
    REQUIRE( t.find( 'E' ) == 15 );
    REQUIRE( t.find( '8' ) == 23 );
    REQUIRE( t.find( 'F' ) == TWINE_NOT_FOUND );

    // twine::find(twine&)
    REQUIRE( t.find( twine("Search") ) == 0 );
    REQUIRE( t.find( twine("search") ) == 7 );
    REQUIRE( t.find( twine("SEARCH") ) == 14 );

    // twine::find(char*, size_t)
    REQUIRE( t.find( "Search", 5 ) == TWINE_NOT_FOUND );
    REQUIRE( t.find( "search", 15 ) == 29 );
    REQUIRE( t.find( "SEARCH", 10 ) == 14 );

    // twine::find(char, size_t)
    REQUIRE( t.find( 'S', 10 ) == 14 );
    REQUIRE( t.find( 's', 24 ) == 29 );
    REQUIRE( t.find( 'E', 11 ) == 15 );
    REQUIRE( t.find( '8', 30 ) == TWINE_NOT_FOUND );

    // twine::find(twine&, size_t)
    REQUIRE( t.find( twine("Search"), 5 ) == TWINE_NOT_FOUND );
    REQUIRE( t.find( twine("search"), 15 ) == 29 );
    REQUIRE( t.find( twine("SEARCH"), 10 ) == 14 );
}

// IMPORTANT! In many cases, twine::rfind() will segfault when it should return
// TWINE_NOT_FOUND. This is because we're using size_t, which is unsigned and the
// comparison: p >= 0, which will always be true for unsigned numbers.
TEST_CASE( "Twine - Reverse Finds", "[twine][twine-find]" )
{
    //        0123456789 123456789 123456789 1234
    twine t( "Search search SEARCH se8arch search" );

    // twine::rfind(char*)
    REQUIRE( t.rfind( "Search" ) == 0 );
    REQUIRE( t.rfind( "search" ) == 29 );
    REQUIRE( t.rfind( "SEARCH" ) == 14 );
    REQUIRE( t.rfind( "FOOBAR" ) == TWINE_NOT_FOUND );

    // twine::rfind(char)
    REQUIRE( t.rfind( 'S' ) == 14 );
    REQUIRE( t.rfind( 's' ) == 29 );
    REQUIRE( t.rfind( 'E' ) == 15 );
    REQUIRE( t.rfind( '8' ) == 23 );
    REQUIRE( t.rfind( 'F' ) == TWINE_NOT_FOUND );

    // twine::rfind(twine&)
    REQUIRE( t.rfind( twine("Search") ) == 0 );
    REQUIRE( t.rfind( twine("search") ) == 29 );
    REQUIRE( t.rfind( twine("SEARCH") ) == 14 );

    // twine::rfind(char*, size_t)
    REQUIRE( t.rfind( "Search", 5 ) == 0 );
    REQUIRE( t.rfind( "search", 15 ) == 7 );
    REQUIRE( t.rfind( "SEARCH", 10 ) == TWINE_NOT_FOUND );

    // twine::rfind(char, size_t)
    REQUIRE( t.rfind( 'S', 10 ) == 0 );
    REQUIRE( t.rfind( 's', 24 ) == 21 );
    REQUIRE( t.rfind( 'E', 11 ) == TWINE_NOT_FOUND );
    REQUIRE( t.rfind( '8', 30 ) == 23 );

    // twine::rfind(twine&, size_t)
    REQUIRE( t.rfind( twine("Search"), 5 ) == 0 );
    REQUIRE( t.rfind( twine("search"), 15 ) == 7 );
    REQUIRE( t.rfind( twine("SEARCH"), 10 ) == TWINE_NOT_FOUND );
}

TEST_CASE( "Twine - Basic Finds - Case Insensitive", "[twine][twine-find][ci]" )
{
    //        0123456789 123456789 123456789 1234
    twine t( "Search search SEARCH se8arch search" );

    // twine::cifind(char*)
    REQUIRE( t.cifind( "Search" ) == 0 );
    REQUIRE( t.cifind( "search" ) == 0 );
    REQUIRE( t.cifind( "SEARCH" ) == 0 );
    REQUIRE( t.cifind( "FOOBAR" ) == TWINE_NOT_FOUND );

    // twine::cifind(char)
    REQUIRE( t.cifind( 'S' ) == 0 );
    REQUIRE( t.cifind( 's' ) == 0 );
    REQUIRE( t.cifind( 'E' ) == 1 );
    REQUIRE( t.cifind( '8' ) == 23 );
    REQUIRE( t.cifind( 'F' ) == TWINE_NOT_FOUND );

    // twine::cifind(twine&)
    REQUIRE( t.cifind( twine("Search") ) == 0 );
    REQUIRE( t.cifind( twine("search") ) == 0 );
    REQUIRE( t.cifind( twine("SEARCH") ) == 0 );

    // twine::cifind(char*, size_t)
    REQUIRE( t.cifind( "Search", 5 ) == 7 );
    REQUIRE( t.cifind( "search", 15 ) == 29 );
    REQUIRE( t.cifind( "SEARCH", 10 ) == 14 );

    // twine::cifind(char, size_t)
    REQUIRE( t.cifind( 'S', 10 ) == 14 );
    REQUIRE( t.cifind( 's', 24 ) == 29 );
    REQUIRE( t.cifind( 'E', 11 ) == 15 );
    REQUIRE( t.cifind( '8', 30 ) == TWINE_NOT_FOUND );

    // twine::cifind(twine&, size_t)
    REQUIRE( t.cifind( twine("Search"), 5 ) == 7 );
    REQUIRE( t.cifind( twine("search"), 15 ) == 29 );
    REQUIRE( t.cifind( twine("SEARCH"), 10 ) == 14 );
}

TEST_CASE( "Twine - Reverse Finds - Case Insensitive", "[twine][twine-find][ci]" )
{
    //        0123456789 123456789 123456789 1234
    twine t( "Search search SEARCH se8arch search" );

    // twine::cirfind(char*)
    REQUIRE( t.cirfind( "Search" ) == 29 );
    REQUIRE( t.cirfind( "search" ) == 29 );
    REQUIRE( t.cirfind( "SEARCH" ) == 29 );
    REQUIRE( t.cirfind( "FOOBAR" ) == TWINE_NOT_FOUND );

    // twine::cirfind(char)
    REQUIRE( t.cirfind( 'S' ) == 29 );
    REQUIRE( t.cirfind( 's' ) == 29 );
    REQUIRE( t.cirfind( 'E' ) == 30 );
    REQUIRE( t.cirfind( '8' ) == 23 );
    REQUIRE( t.cirfind( 'F' ) == TWINE_NOT_FOUND );

    // twine::cirfind(twine&)
    REQUIRE( t.cirfind( twine("Search") ) == 29 );
    REQUIRE( t.cirfind( twine("search") ) == 29 );
    REQUIRE( t.cirfind( twine("SEARCH") ) == 29 );

    // twine::cirfind(char*, size_t)
    REQUIRE( t.cirfind( "Search", 5 ) == 0 );
    REQUIRE( t.cirfind( "search", 15 ) == 14 );
    REQUIRE( t.cirfind( "SEARCH", 10 ) == 7 );

    // twine::cirfind(char, size_t)
    REQUIRE( t.cirfind( 'S', 10 ) == 7 );
    REQUIRE( t.cirfind( 's', 24 ) == 21 );
    REQUIRE( t.cirfind( 'E', 11 ) == 8 );
    REQUIRE( t.cirfind( '8', 30 ) == 23 );

    // twine::cirfind(twine&, size_t)
    REQUIRE( t.cirfind( twine("Search"), 5 ) == 0 );
    REQUIRE( t.cirfind( twine("search"), 15 ) == 14 );
    REQUIRE( t.cirfind( twine("SEARCH"), 10 ) == 7 );
}

TEST_CASE( "Twine - Replace", "[twine][twine-find]" )
{
    //        0123456789 123456789 123456789 1234
    twine t( "Search search SEARCH se8arch search" );

    REQUIRE( t.find( "Foo" ) == TWINE_NOT_FOUND );
    REQUIRE( t.find( "FOO" ) == TWINE_NOT_FOUND );
    t.replaceAll( "search", "Foo" );
    t.replaceAll( "Search", "FOO" );
    REQUIRE( t == "FOO Foo SEARCH se8arch Foo" );
}

TEST_CASE( "Twine - Replace - Case Insensitive", "[twine][twine-find][ci]" )
{
    //        0123456789 123456789 123456789 1234
    twine t( "Search search SEARCH se8arch search" );

    REQUIRE( t.cifind( "Foo" ) == TWINE_NOT_FOUND );
    REQUIRE( t.cifind( "FOO" ) == TWINE_NOT_FOUND );
//  try{
    t.cireplaceAll( "search", "Foo" );
    t.cireplaceAll( "Search", "FOO" );
//  }catch(AnException e){printf("%s\n%s\n", e.Msg(), e.Stack());}
    REQUIRE( t == "Foo Foo Foo se8arch Foo" );
}