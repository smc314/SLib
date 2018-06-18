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
#include <iomanip>

#include "twine.h"
#include "xmlInc.h"
using namespace SLib;

#include "catch.hpp"

const size_t MAX_INPUT_SIZE = 1024000000;

TEST_CASE("Twine - assignment from const char - bigmem", "[twine][bigmem][.]")
{
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

TEST_CASE("Twine - assign from xmlChar - bigmem", "[twine][xml][bigmem][.]")
{
    SECTION("Extremely long xmlChar") {
        SECTION("MAX_INPUT_SIZE") {

            xmlChar* monster = (xmlChar*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*monster));
            memset(monster, 'A', MAX_INPUT_SIZE);
            monster[MAX_INPUT_SIZE] = 0;

            bool freeIt = true;

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

            SECTION( "Assignment using <<" ){
                twine t;
                REQUIRE_NOTHROW(t << monster);

                freeIt = false;

                REQUIRE_FALSE(t.empty());
                REQUIRE(t.size() == MAX_INPUT_SIZE);
                REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
                INFO("t.capacity() is " << t.capacity());
                INFO("MAX_INPUT_SIZE is " << MAX_INPUT_SIZE);
            }

            if(freeIt)
                free(monster);

        }

        SECTION("MAX_INPUT_SIZE + 1") {
            xmlChar* monster = (xmlChar*)malloc((MAX_INPUT_SIZE + 2) * sizeof(*monster));
            memset(monster, 'B', MAX_INPUT_SIZE + 1);
            monster[MAX_INPUT_SIZE + 1] = 0;

            SECTION( "Direct assignment of a long string" ){
                REQUIRE_THROWS([&](){twine t = monster;}());
            }

            SECTION( "Assignment after construction of a long string" ){
                twine t;
                REQUIRE_THROWS(t = monster);
            }

            SECTION( "Assignment using <<" ){
                twine t;
                REQUIRE_THROWS(t << monster);
            }

            free(monster);
        }
    }
}

TEST_CASE("Twine - += another twine - bigmem", "[twine][bigmem][.]")
{
    SECTION("Append an empty twine")
    {
        twine s = "";

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            
            REQUIRE_NOTHROW(t += s);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == MAX_INPUT_SIZE);
            REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
            REQUIRE(t.compare(mon) == 0);
            free(mon);
        }
    }

    SECTION("Append a short twine")
    {
        twine s = "I am a small string";

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
    }

    SECTION("Append a long twine")
    {
        twine s = "I am a longer string that will not fit in optimized storage.";

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
    }
    
    SECTION("Append a max length twine")
    {
        char* monster = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*monster));
        memset(monster, 'B', MAX_INPUT_SIZE);
        monster[MAX_INPUT_SIZE] = 0;
        twine s = monster;
        free(monster);
        
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == MAX_INPUT_SIZE);
            REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
            REQUIRE(t.compare(s) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            REQUIRE_THROWS(t += s);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            REQUIRE_THROWS(t += s);
        }

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
    }
}

TEST_CASE("Twine - += a twine pointer - bigmem", "[twine][bigmem][.]")
{
    SECTION("Append an empty twine")
    {
        twine *s = new twine("");

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            
            REQUIRE_NOTHROW(t += s);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == MAX_INPUT_SIZE);
            REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
            REQUIRE(t.compare(mon) == 0);
            free(mon);
        }
        delete s;
    }

    SECTION("Append a short twine")
    {
        twine *s = new twine("I am a small string");

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
        delete s;
    }

    SECTION("Append a long twine")
    {
        twine *s = new twine("I am a longer string that will not fit in optimized storage.");

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
        delete s;
    }
    
    SECTION("Append a max length twine")
    {
        char* monster = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*monster));
        memset(monster, 'B', MAX_INPUT_SIZE);
        monster[MAX_INPUT_SIZE] = 0;
        twine *s = new twine(monster);
        free(monster);
        
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == MAX_INPUT_SIZE);
            REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
            REQUIRE(t.compare(*s) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            REQUIRE_THROWS(t += s);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            REQUIRE_THROWS(t += s);
        }

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
        delete s;
    }
}

TEST_CASE("Twine - += a constant C string - bigmem", "[twine][bigmem][.]")
{
    SECTION("Append an empty string")
    {
        char *s = "";

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            
            REQUIRE_NOTHROW(t += s);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == MAX_INPUT_SIZE);
            REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
            REQUIRE(t.compare(mon) == 0);
            free(mon);
        }
    }

    SECTION("Append a short string")
    {
        char *s = "I am a small string";

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
    }

    SECTION("Append a long string")
    {
        char *s = "I am a longer string that will not fit in optimized storage.";

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
    }
    
    SECTION("Append a max length string")
    {
        char* s = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*s));
        memset(s, 'B', MAX_INPUT_SIZE);
        s[MAX_INPUT_SIZE] = 0;
        
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == MAX_INPUT_SIZE);
            REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
            REQUIRE(t.compare(s) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            REQUIRE_THROWS(t += s);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            REQUIRE_THROWS(t += s);
        }

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += s);
        }
        free(s);
    }
}

TEST_CASE("Twine - += a single char - bigmem", "[twine][bigmem][.]")
{
    SECTION("Append a null character")
    {
        char c = '\0';

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            
            REQUIRE_NOTHROW(t += c);
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == strlen(t()));
            REQUIRE(t.size() == MAX_INPUT_SIZE);
            REQUIRE(t.capacity() >= MAX_INPUT_SIZE);
            REQUIRE(t.compare(mon) == 0);
            free(mon);
        }
    }

    SECTION("Append a normal character")
    {
        char c = 'Q';

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            
            REQUIRE_THROWS(t += c);
        }
    }
}

TEST_CASE("Twine - += a size_t - bigmem", "[twine][bigmem][.]")
{
    SECTION("Append 0")
    {
        size_t i = 0;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }

    SECTION("Append SIZE_MAX")
    {
        size_t i = SIZE_MAX;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }

    SECTION("Append unsigned value of smallest possible signed value.")
    {
        size_t i = (SIZE_MAX >> 1) + 1; // 0b11111111... > 0b01111111... > 0b10000000...

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }

    SECTION("Append unsigned value of smallest possible signed value - 1")
    {
        size_t i = (SIZE_MAX >> 1); // 0b11111111... > 0b01111111...

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }

    SECTION("Append max value of atoi()")
    {
        size_t i = (size_t)INT_MAX;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }
    
    SECTION("Append max value of atoi() + 1")
    {
        size_t i = (size_t)INT_MAX + 1;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }
}

TEST_CASE("Twine - += an intptr_t - bigmem", "[twine][bigmem][.]")
{
    SECTION("Append 0")
    {
        intptr_t i = 0;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }

    SECTION("Append INTPTR_MAX")
    {
        intptr_t i = INTPTR_MAX;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }
    
    SECTION("Append unsigned value of smallest possible signed value.")
    {
        intptr_t i = INTPTR_MIN; // 0b11111111...

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }

    SECTION("Append smallest possible signed value - 1")
    {
        intptr_t i = INTPTR_MIN - 1;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }

    SECTION("Append max value of atoi()")
    {
        intptr_t i = (intptr_t)INT_MAX;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }

    SECTION("Append max value of atoi() + 1")
    {
        intptr_t i = (intptr_t)INT_MAX + 1;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += i);
        }
    }
}

TEST_CASE("Twine - += a floating point - bigmem", "[twine][bigmem][.]")
{
    SECTION("Assign +0.0")
    {
        float f = +0.0f;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Assign -0.0")
    {
        float f = -0.0f;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Assign +INFINITY")
    {
        float f = INFINITY;
        
        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Assign -INFINITY")
    {
        float f = -INFINITY;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Assign NaN")
    {
        float f = NAN;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }
    
    SECTION("Assign a sane positive value")
    {
        float f = 142.123;
        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Assign a sane negative value")
    {
        float f = -142.123;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    // The default precision of sprintf() for %f is 6
    SECTION("Assign a positive value with more than 6 digits")
    {
        float f = 1.23012582302;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Assign a negative value with more than 6 digits")
    {
        float f = -1.23012582302;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Assign a rather small number")
    {
        float f = 123e-16;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Assign a rather large number")
    {
        float f = 123e16;

        SECTION("Append to a max length twine")
        {
            char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
            memset(mon, 'A', MAX_INPUT_SIZE);
            mon[MAX_INPUT_SIZE] = 0;
            twine t = mon;
            free(mon);
            
            REQUIRE_THROWS(t += f);
        }
    }

    SECTION("Append a number with more that 32 digits")
    {
        float f = 123e32;

     // SECTION("Append to a max length twine")
     // {
     //     char* mon = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*mon));
     //     memset(mon, 'A', MAX_INPUT_SIZE);
     //     mon[MAX_INPUT_SIZE] = 0;
     //     twine t = mon;
     //     free(mon);
     //     
     //     REQUIRE_THROWS(t += f);
     // }
    }
}

// TODO: Deal with this.
// IMPORTANT! This test case MUST be dealt with!
// However, leaving it in a normal run is too dangerous.
// It causes a Bus Error:10 when it calls the insert function.
// This is undesired behavior in the insert function itself,
// but because it's a hardware fault, rather than a software exception,
// it completely aborts all testing, so we lose all test results after this one.
TEST_CASE("Twine - Insert into Extremely Long String", "[twine][bigmem][.]")
{
    printf("Line %d\n", __LINE__);
    char* monster = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*monster));
    printf("Line %d\n", __LINE__);
    memset(monster, 'A', MAX_INPUT_SIZE);
    printf("Line %d\n", __LINE__);
    monster[MAX_INPUT_SIZE] = 0;

    printf("Line %d\n", __LINE__);
    twine t;

    printf("Line %d\n", __LINE__);
    REQUIRE_NOTHROW([&](){t = monster;}());
    printf("Line %d\n", __LINE__);

    SECTION("Insert one extra char"){
        printf("Line %d\n", __LINE__);
        REQUIRE_THROWS([&](){t.insert(0, "B");}());
        printf("Line %d\n", __LINE__);
    }

    SECTION("Insert a few extra chars"){
        printf("Line %d\n", __LINE__);
        int more = t.capacity() - MAX_INPUT_SIZE;
        printf("Line %d\n", __LINE__);
        char* moreChars = (char*)malloc((more + 1) * sizeof(*moreChars));
        printf("Line %d\n", __LINE__);
        memset(moreChars, 'B', more);
        printf("Line %d\n", __LINE__);
        moreChars[more] = 0;
        printf("Line %d\n", __LINE__);

        CHECK_THROWS([&](){t.insert(0, moreChars);}());
        printf("Line %d\n", __LINE__);
        free(moreChars);
        printf("Line %d\n", __LINE__);
    }

    SECTION("Insert many extra chars"){
        printf("Line %d\n", __LINE__);
        int more = t.capacity() - MAX_INPUT_SIZE;
        printf("Line %d\n", __LINE__);
        char* moreChars = (char*)malloc((more + 3) * sizeof(*moreChars));
        printf("Line %d\n", __LINE__);
        memset(moreChars, 'B', more+2);
        printf("Line %d\n", __LINE__);
        moreChars[more] = 0;
        printf("Line %d\n", __LINE__);

        CHECK_THROWS([&](){t.insert(0, moreChars);}());
        printf("Line %d\n", __LINE__);
        free(moreChars);
        printf("Line %d\n", __LINE__);
    }

    printf("Line %d\n", __LINE__);
    free(monster);
    printf("Line %d\n", __LINE__);
}

TEST_CASE("Twine - Append onto Extremely Long String", "[twine][bigmem][.][append]")
{
    printf("Line %d\n", __LINE__);
    char* monster = (char*)malloc((MAX_INPUT_SIZE + 1) * sizeof(*monster));
    printf("Line %d\n", __LINE__);
    memset(monster, 'A', MAX_INPUT_SIZE);
    printf("Line %d\n", __LINE__);
    monster[MAX_INPUT_SIZE] = 0;

    printf("Line %d\n", __LINE__);
    twine t;

    printf("Line %d\n", __LINE__);
    REQUIRE_NOTHROW([&](){t = monster;}());
    printf("Line %d\n", __LINE__);

    SECTION("Append one extra char"){
        printf("Line %d\n", __LINE__);
        REQUIRE_THROWS([&](){t.append("B");}());
        printf("Line %d\n", __LINE__);
    }

    SECTION("Append a few extra chars"){
        printf("Line %d\n", __LINE__);
        int more = t.capacity() - MAX_INPUT_SIZE;
        printf("Line %d\n", __LINE__);
        char* moreChars = (char*)malloc((more + 1) * sizeof(*moreChars));
        printf("Line %d\n", __LINE__);
        memset(moreChars, 'B', more);
        printf("Line %d\n", __LINE__);
        moreChars[more] = 0;
        printf("Line %d\n", __LINE__);

        CHECK_THROWS([&](){t.append(moreChars);}());
        printf("Line %d\n", __LINE__);
        free(moreChars);
        printf("Line %d\n", __LINE__);
    }

    SECTION("Append many extra chars"){
        printf("Line %d\n", __LINE__);
        int more = t.capacity() - MAX_INPUT_SIZE;
        printf("Line %d\n", __LINE__);
        char* moreChars = (char*)malloc((more + 3) * sizeof(*moreChars));
        printf("Line %d\n", __LINE__);
        memset(moreChars, 'B', more+2);
        printf("Line %d\n", __LINE__);
        moreChars[more] = 0;
        printf("Line %d\n", __LINE__);

        CHECK_THROWS([&](){t.append(moreChars);}());
        printf("Line %d\n", __LINE__);
        free(moreChars);
        printf("Line %d\n", __LINE__);
    }

    printf("Line %d\n", __LINE__);
    free(monster);
    printf("Line %d\n", __LINE__);
}

