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

            // This is a valid size, because the exception is
            // thrown when number of non-zero chars in string
            // is greater than MAX_INPUT_SIZE
            char* monster = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*monster));
         // for(int i = 0; i < MAX_INPUT_SIZE; ++i)
         // {
         //     monster[i] = (i % 94) + 32; // fill it with printable characters! Why? Who cares!
         // }
            memset(monster, 'A', MAX_INPUT_SIZE);
            monster[MAX_INPUT_SIZE] = 0;

            SECTION( "Direct assignment of a long string" ){
                twine* t = NULL;

                REQUIRE_NOTHROW([&](){t = new twine(monster);}());
                REQUIRE_FALSE(t->empty());
                REQUIRE(t->size() == MAX_INPUT_SIZE);
                REQUIRE(t->capacity() >= MAX_INPUT_SIZE);
                INFO("t.capacity() is " << t->capacity());
                INFO("MAX_INPUT_SIZE is " << MAX_INPUT_SIZE);
                
                if(t != NULL)
                    delete t;
            }

            SECTION( "Assignment after construction of a long string" ){
                twine t;
                REQUIRE_NOTHROW(t = monster);

                REQUIRE_FALSE(t.empty());
                REQUIRE(t.size() == MAX_INPUT_SIZE);
                REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
                INFO("t.capacity() is " << t.capacity());
                INFO("MAX_INPUT_SIZE is " << MAX_INPUT_SIZE);
            }

            free(monster);
        }
        
        SECTION("MAX_INPUT_SIZE + 1") {
            // This is an invalid size, because the nonzero portion of the string is of length greater
            // than MAX_INPUT_SIZE
            char* monster = (char*)malloc((MAX_INPUT_SIZE + 2) * sizeof(*monster));
         // for(int i = 0; i <= MAX_INPUT_SIZE; ++i)
         // {
         //     monster[i] = (i % 94) + 32; // fill it with printable characters! Why? Who cares!
         // }
            memset(monster, 'B', MAX_INPUT_SIZE + 1);
            monster[MAX_INPUT_SIZE + 1] = 0;

            SECTION( "Direct assignment of a long string" ){
                REQUIRE_THROWS([&](){twine t = monster;}());
            }

            SECTION( "Assignment after construction of a long string" ){
                twine t;
                REQUIRE_THROWS(t = monster);
            }

            free(monster);
        }
    }


}

TEST_CASE( "Twine - Copying another twine", "[twine]" )
{
    SECTION( "Copy Empty" ) {
        SECTION( "Calling constructor by name" ){
            twine orig;
            twine t(orig);
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

        SECTION("Calling assignment after initial creation"){
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
        twine orig = "I am a short string";

        SECTION( "Calling constructor by name" ){
            twine t(orig);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }

        SECTION("Implicitly calling constructor"){
            twine t = orig;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }

        SECTION("Calling assignment after initial creation"){
            twine t; t = orig;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }
    } 

    SECTION("Copy Long Twine") {
        twine orig = "I am a longer string that will not fit in optimized storage.";

        SECTION( "Calling constructor by name" ){
            twine t(orig);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }

        SECTION("Implicitly calling constructor"){
            twine t = orig;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }

        SECTION("Calling assignment after initial creation"){
            twine t; t = orig;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);

            REQUIRE(t.compare(orig) == 0);
            REQUIRE(&t != &orig);
        }
    }
}

TEST_CASE("Twine - assign from xmlChar", "[twine][xml]")
{
    SECTION( "Assign Empty" ) {
        xmlChar* xml = (xmlChar*)("");

        SECTION( "Calling constructor by name" ){
            twine t(xml);
            REQUIRE(t.empty());
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(xml) == 0);
        }

        SECTION("Implicitly calling constructor"){
            twine t = xml;
            REQUIRE(t.empty());
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(xml) == 0);
        }

        SECTION("Calling assignment after initial creation"){
            twine t; t = xml;
            REQUIRE(t.empty());
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(xml) == 0);
        }
    } 

    SECTION( "Assign Short xmlChar" ) {
        xmlChar* xml = (xmlChar*)("I am a short string");

        SECTION( "Calling constructor by name" ){
            twine t(xml);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(xml) == 0);
        }

        SECTION("Implicitly calling constructor"){
            twine t = xml;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(xml) == 0);
        }

        SECTION("Calling assignment after initial creation"){
            twine t; t = xml;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(xml) == 0);
        }
    } 

    SECTION("Copy Long Twine") {
        xmlChar* xml = (xmlChar*)("I am a longer string that will not fit in optimized storage.");

        SECTION( "Calling constructor by name" ){
            twine t(xml);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);

            REQUIRE(t.compare(xml) == 0);
        }

        SECTION("Implicitly calling constructor"){
            twine t = xml;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);

            REQUIRE(t.compare(xml) == 0);
        }

        SECTION("Calling assignment after initial creation"){
            twine t; t = xml;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);

            REQUIRE(t.compare(xml) == 0);
        }
    }

}

TEST_CASE("Twine - Random Testing", "[twine][random]")
{
    SECTION("Randomized String")
    {
        srand(time(0));

        int length = rand();
        char* randStr = (char*)malloc((length + 1) * sizeof(*randStr));
        for(int i = 0; i < length; ++i)
        {
            randStr[i] = (rand() % (CHAR_MAX - CHAR_MIN + 1)) + CHAR_MIN;
            if(randStr[i] == 0) // this is the null character for end of string
                break; // the rest of the buffer is irrelevant
        }
        randStr[length] = 0; // make sure we have at least one null char in the string

        SECTION("Create from random String"){
            if(strlen(randStr) > MAX_INPUT_SIZE)
                REQUIRE_THROWS([&](){twine t = randStr;});
            else
            {
                twine t = randStr;
                if(randStr[0] == 0)
                    REQUIRE(t.empty());
                else
                    REQUIRE_FALSE(t.empty());

                REQUIRE(t.size() == strlen(randStr));
                REQUIRE(t.capacity() >= strlen(randStr));
                if(strlen(randStr) < TWINE_SMALL_STRING)
                    REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            } 
        }

        SECTION("Copy from random String"){
            twine orig;
            if(strlen(randStr) > MAX_INPUT_SIZE)
                REQUIRE_THROWS([&](){orig = randStr;});
            else
            {
                orig = randStr;

                twine t = orig;
                if(randStr[0] == 0)
                    REQUIRE(t.empty());
                else
                    REQUIRE_FALSE(t.empty());

                REQUIRE(t.size() == strlen(randStr));
                REQUIRE(t.capacity() >= strlen(randStr));
                if(strlen(randStr) < TWINE_SMALL_STRING)
                    REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

                REQUIRE(t.compare(orig) == 0);
                REQUIRE(&t != &orig);
            } 
        }

        free(randStr);
    }

}
