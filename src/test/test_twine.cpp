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
/* Simple program to test the functions in twine.cpp       */
/* ******************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "twine.h"
using namespace SLib;

#include "catch.hpp"

const size_t MAX_INPUT_SIZE = 1024000000;

TEST_CASE( "Twine - short string", "[twine]" )
{
    twine t( "I am a short string" );

    REQUIRE_FALSE( t.empty() );
    REQUIRE( t.size() == 19 );
    REQUIRE( t.capacity() >= 19 );
}

// according to twine.h, a "Short string" is <=32 characters, including the final '\0'
TEST_CASE( "Twine - assignment from const char", "[twine]" )
{
    SECTION( "Short string") {
        SECTION( "Direct assignment" ){
            twine t = "I am a short string";
            REQUIRE_FALSE( t.empty() );
            REQUIRE( t.size() == 19 );
            REQUIRE( t.capacity() >= 19 );
        }

        SECTION( "Assignment after construction" ){
            twine t; t = "I am a short string";
            REQUIRE_FALSE( t.empty() );
            REQUIRE( t.size() == 19 );
            REQUIRE( t.capacity() >= 19 );
        }
    }

    SECTION( "Long string") {
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

    SECTION("Extremely long string") {
        SECTION("MAX_INPUT_SIZE") {
            SECTION( "Direct assignment of a long string" ){
                char* monster = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*monster));
                for(int i = 0; i < MAX_INPUT_SIZE; ++i)
                {
                    monster[i] = (i % 94) + 32; // fill it with printable characters! Why? Who cares!
                }
                monster[MAX_INPUT_SIZE] = 0;
                twine* t = NULL;
                // This is a valid size, because the exception is
                // thrown when number of non-zero chars in string
                // is greater than MAX_INPUT_SIZE

                REQUIRE_NOTHROW([&](){twine t_real = monster; t = &t_real;}());
                REQUIRE_FALSE(t->empty());
                REQUIRE(t->size() == MAX_INPUT_SIZE);
                REQUIRE(t->capacity() >= MAX_INPUT_SIZE);
                INFO("t.capacity() is " << t->capacity());
                INFO("MAX_INPUT_SIZE is " << MAX_INPUT_SIZE);
                
            }

            SECTION( "Assignment after construction of a long string" ){
                char* monster = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*monster));
                for(int i = 0; i < MAX_INPUT_SIZE; ++i)
                {
                    monster[i] = (i % 94) + 32; // fill it with printable characters! Why? Who cares!
                }
                monster[MAX_INPUT_SIZE] = 0;

                twine t;
                REQUIRE_NOTHROW(t = monster);

                REQUIRE_FALSE(t.empty());
                REQUIRE(t.size() == MAX_INPUT_SIZE);
                REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
                INFO("t.capacity() is " << t.capacity());
                INFO("MAX_INPUT_SIZE is " << MAX_INPUT_SIZE);
            }
        }
        
        SECTION("MAX_INPUT_SIZE + 1") {
            SECTION( "Direct assignment of a long string" ){
                char* monster = (char*)malloc((MAX_INPUT_SIZE + 2) * sizeof(*monster));
                for(int i = 0; i <= MAX_INPUT_SIZE; ++i)
                {
                    monster[i] = (i % 94) + 32; // fill it with printable characters! Why? Who cares!
                }
                monster[MAX_INPUT_SIZE + 1] = 0;
                REQUIRE_THROWS([&](){twine t = monster;}());
            }

            SECTION( "Assignment after construction of a long string" ){
                char* monster = (char*)malloc((MAX_INPUT_SIZE + 2) * sizeof(*monster));
                for(int i = 0; i <= MAX_INPUT_SIZE; ++i)
                {
                    monster[i] = (i % 94) + 32; // fill it with printable characters! Why? Who cares!
                }
                monster[MAX_INPUT_SIZE + 1] = 0;

                twine t;
                REQUIRE_THROWS(t = monster);
            }
        }
    }

}

TEST_CASE( "Twine - Copy Constructor" )
{
    SECTION( "Copy Empty" ) {
        SECTION( "Calling constructor by name" ){
            twine orig;
            twine t = twine(orig);
            REQUIRE(t.empty());
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }

        SECTION("Implicitly calling constructor"){
            twine orig;
            twine t = orig;
            REQUIRE(t.empty());
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }

        SECTION("Calling constructor after initial creation"){
            twine orig;
            twine t; t = orig;
            REQUIRE(t.empty());
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }
    } 

    SECTION( "Copy Short Twine" ) {
        SECTION( "Calling constructor by name" ){
            twine orig = "I am a short string";
            twine t = twine(orig);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }

        SECTION("Implicitly calling constructor"){
            twine orig = "I am a short string";
            twine t = orig;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }

        SECTION("Calling constructor after initial creation"){
            twine orig = "I am a short string";
            twine t; t = orig;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }
    } 


}
