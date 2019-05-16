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
#include "xmlinc.h"
using namespace SLib;

#include "catch.hpp"

const size_t MAX_INPUT_SIZE = 1024000000;

TEST_CASE("Twine - assignment from a size_t", "[twine]")
{
    SECTION("Assign 0")
    {
        size_t i = 0;
        twine t;
        t = i;
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 1);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare("0") == 0);
    }

    SECTION("Assign SIZE_MAX")
    {
        size_t i = SIZE_MAX;
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(SIZE_MAX) + 1);
        if((int)log10(SIZE_MAX) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(SIZE_MAX) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare((size_t)SIZE_MAX) == 0);

    }
    
    SECTION("Assign unsigned value of smallest possible signed value.")
    {
        size_t i = (SIZE_MAX >> 1) + 1; // 0b11111111... > 0b01111111... > 0b10000000...
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(i) + 1);
        if((int)log10(i) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(i) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare(i) == 0);

    }

    SECTION("Assign unsigned value of smallest possible signed value - 1")
    {
        size_t i = (SIZE_MAX >> 1); // 0b11111111... > 0b01111111...
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(i) + 1);
        if((int)log10(i) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(i) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare(i) == 0);

    }

    SECTION("Assign max value of atoi()")
    {
        size_t i = (size_t)INT_MAX;
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(i) + 1);
        if((int)log10(i) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(i) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare(i) == 0);

    }

    SECTION("Assign max value of atoi() + 1")
    {
        size_t i = (size_t)INT_MAX + 1;
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(i) + 1);
        if((int)log10(i) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(i) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare(i) == 0);
    }
}

TEST_CASE("Twine - assignment from a intptr_t", "[twine]")
{
    SECTION("Assign 0")
    {
        intptr_t i = 0;
        twine t;
        t = i;
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 1);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare((size_t)i) == 0);
    }

    SECTION("Assign INTPTR_MAX")
    {
        intptr_t i = INTPTR_MAX;
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(i) + 1);
        if((int)log10(i) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(i) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare((size_t)i) == 0);

    }
    
    SECTION("Assign smallest possible signed value.")
    {
        intptr_t i = INTPTR_MIN;
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(-(i+1)) + 2); //this is a negative number, also 2's complement edge case
        if((int)log10(-(i+1)) + 2 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(-(i+1)) + 2);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare((size_t)i) == 0);

    }

    SECTION("Assign smallest possible signed value - 1")
    {
        intptr_t i = INTPTR_MIN - 1;
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(i) + 1); // This should be a positive number
        if((int)log10(i) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(i) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare((size_t)i) == 0);

    }

    SECTION("Assign max value of atoi()")
    {
        intptr_t i = (intptr_t)INT_MAX;
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(i) + 1);
        if((int)log10(i) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(i) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare((size_t)i) == 0);

    }

    SECTION("Assign max value of atoi() + 1")
    {
        intptr_t i = (intptr_t)INT_MAX + 1;
        twine t;
        t = i;
        INFO("i = " << i << "\tt = " << t() << "\tatoi(t) = " << atoi(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == (int)log10(i) + 1);
        if((int)log10(i) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(i) + 1);
        else
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t.compare((size_t)i) == 0);
    }
}

TEST_CASE("Twine - Assign from floating point", "[twine]")
{
    SECTION("Assign +0.0")
    {
        float f = +0.0f;
        twine t;
        t = f;
        INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 8);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    SECTION("Assign -0.0")
    {
        float f = -0.0f;
        twine t;
        t = f;
        INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 9);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    SECTION("Assign +INFINITY")
    {
        float f = INFINITY;
        twine t;
        t = f;
        INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() >= 3);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
        
    }

    SECTION("Assign -INFINITY")
    {
        float f = -INFINITY;
        twine t;
        t = f;
        INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() >= 4);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);

    }

    SECTION("Assign NaN")
    {
        // I'm not sure whether I should REQUIRE f and t compare equal or not.
        // After all, NaN == NaN returns false...
        //
        // After discussion with Steven, this should expect f and t to compare equal

        float f = NAN;
        twine t;
        t = f;
        INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 3);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    // TODO: Add tests for normal values and for floating point error
    // Also do stuff with the numeric limits and such.
    
    SECTION("Assign a sane positive value")
    {
        float f = 142.123;
        twine t;
        t = f;
        INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() >= 7);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    SECTION("Assign a sane negative value")
    {
        float f = -142.123;
        twine t;
        t = f;
        INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() >= 8);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    // The default precision of sprintf() for %f is 6
    SECTION("Assign a positive value with more than 6 digits")
    {
        float f = 1.23012582302;
        twine t;
        t = f;
        INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        //REQUIRE(t.size() >= 8); // Not sure about the size
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    SECTION("Assign a negative value with more than 6 digits")
    {
        float f = -1.23012582302;
        twine t;
        t = f;
        INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        //REQUIRE(t.size() >= 8); // Not sure about the size
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    SECTION("Assign a rather small number")
    {
        float f = 123e-16;
        twine t;
        t = f;
        INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        //REQUIRE(t.size() >= 8); // Not sure about the size
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    SECTION("Assign a rather large number")
    {
        float f = 123e16;
        twine t;
        t = f;
        INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        //REQUIRE(t.size() >= 8); // Not sure about the size
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        REQUIRE(t.compare(f) == 0);
    }

    SECTION("Assign a number with more that 32 digits")
    {
        float f = 123e32;
        twine t;
        t = f;
        INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 42);
        REQUIRE(t.capacity() >= 42);
        REQUIRE(t.compare(f) == 0);
    }
}

TEST_CASE("Twine - += a size_t", "[twine]")
{
    SECTION("Append 0")
    {
        size_t i = 0;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 20);
            REQUIRE(t.capacity() >= 20);
            REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 61);
            REQUIRE(t.capacity() >= 61);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
                    == 0);
        }
    }

    SECTION("Append SIZE_MAX")
    {
        size_t i = SIZE_MAX;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
          //Comparing this would require mallocing
          //REQUIRE(t.compare("I am a small string") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 61);
            REQUIRE(t.size() == 61 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 61 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }
    
    SECTION("Append unsigned value of smallest possible signed value.")
    {
        size_t i = (SIZE_MAX >> 1) + 1; // 0b11111111... > 0b01111111... > 0b10000000...
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 60 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }

    }

    SECTION("Append unsigned value of smallest possible signed value - 1")
    {
        size_t i = (SIZE_MAX >> 1); // 0b11111111... > 0b01111111...
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 60 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }

    SECTION("Append max value of atoi()")
    {
        size_t i = (size_t)INT_MAX;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 60 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }

    SECTION("Append max value of atoi() + 1")
    {
        size_t i = (size_t)INT_MAX + 1;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 60 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }
}

TEST_CASE("Twine - += an intptr_t", "[twine]")
{
    SECTION("Append 0")
    {
        intptr_t i = 0;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare((size_t)i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 20);
            REQUIRE(t.capacity() >= 20);
            REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 61);
            REQUIRE(t.capacity() >= 61);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
                    == 0);
        }
    }

    SECTION("Append INTPTR_MAX")
    {
        intptr_t i = INTPTR_MAX;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare((size_t)i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
          //Comparing this would require mallocing
          //REQUIRE(t.compare("I am a small string") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 61 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 61 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }
    
    SECTION("Append unsigned value of smallest possible signed value.")
    {
        intptr_t i = INTPTR_MIN; // 0b11111111...
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(-(i+1)) + 2);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare((size_t)i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(-(i+1)) + 2);
            REQUIRE(t.capacity() >= 19 + (int)log10(-(i+1)) + 2);
         // REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + (int)log10(-(i+1)) + 2);
            REQUIRE(t.capacity() >= 60 + (int)log10(-(i+1)) + 2);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }

    SECTION("Append smallest possible signed value - 1")
    {
        intptr_t i = INTPTR_MIN - 1;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1); // This should be a positive number
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare((size_t)i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 60 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }

    SECTION("Append max value of atoi()")
    {
        intptr_t i = (intptr_t)INT_MAX;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare((size_t)i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 60 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }

    SECTION("Append max value of atoi() + 1")
    {
        intptr_t i = (intptr_t)INT_MAX + 1;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == (int)log10(i) + 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare((size_t)i) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 19 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a small string0") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += i;
            INFO("i = " << i << " t = " << t());
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + (int)log10(i) + 1);
            REQUIRE(t.capacity() >= 60 + (int)log10(i) + 1);
         // REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.0")
         //         == 0);
        }
    }
}

TEST_CASE("Twine - += a floating point", "[twine]")
{
    SECTION("Assign +0.0")
    {
        float f = +0.0f;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 8);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + 8);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 8);
            REQUIRE(t.capacity() >= 60 + 8);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    SECTION("Assign -0.0")
    {
        float f = -0.0f;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 9);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + 9);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 9);
            REQUIRE(t.capacity() >= 60 + 9);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    SECTION("Assign +INFINITY")
    {
        float f = INFINITY;
        
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 3);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + 3);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 3);
            REQUIRE(t.capacity() >= 60 + 3);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    SECTION("Assign -INFINITY")
    {
        float f = -INFINITY;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 4);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + 4);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 4);
            REQUIRE(t.capacity() >= 60 + 4);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    SECTION("Assign NaN")
    {
        // I'm not sure whether I should REQUIRE f and t compare equal or not.
        // After all, NaN == NaN returns false...
        //
        // After discussion with Steven, this should expect f and t to compare equal

        float f = NAN;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 3);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + 3);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 3);
            REQUIRE(t.capacity() >= 60 + 3);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    // TODO: Add tests for normal values and for floating point error
    // Also do stuff with the numeric limits and such.
    
    SECTION("Assign a sane positive value")
    {
        float f = 142.123;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() >= 7);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() >= 19 + 7);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() >= 60 + 7);
            REQUIRE(t.capacity() >= 60 + 7);
            //REQUIRE(t.compare(f) == 0);
        }

    }

    SECTION("Assign a sane negative value")
    {
        float f = -142.123;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() >= 8);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() >= 19 + 8);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() >= 60 + 8);
            REQUIRE(t.capacity() >= 60 + 8);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    // The default precision of sprintf() for %f is 6
    SECTION("Assign a positive value with more than 6 digits")
    {
        float f = 1.23012582302;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            //REQUIRE(t.size() == 4);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            //REQUIRE(t.size() == 19 + 3);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            //REQUIRE(t.size() == 60 + 3);
            REQUIRE(t.capacity() >= 60 + 3);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    SECTION("Assign a negative value with more than 6 digits")
    {
        float f = -1.23012582302;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            // REQUIRE(t.size() == 4);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
            //REQUIRE(t.size() == 19 + 3);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO(setprecision(15) << "f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
          //REQUIRE(t.size() == 60 + 3);
            REQUIRE(t.capacity() >= 60 + 3);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    SECTION("Assign a rather small number")
    {
        float f = 123e-16;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
          //REQUIRE(t.size() == 4);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
          //REQUIRE(t.size() == 19 + 3);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
          //REQUIRE(t.size() == 60 + 3);
            REQUIRE(t.capacity() >= 60 + 3);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    SECTION("Assign a rather large number")
    {
        float f = 123e16;
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
        //  REQUIRE(t.size() == 4);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
         // REQUIRE(t.size() == 19 + 3);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            //REQUIRE(t.compare(f) == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += f;
            INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
            REQUIRE_FALSE(t.empty());
          //REQUIRE(t.size() == 60 + 3);
            REQUIRE(t.capacity() >= 60 + 3);
            //REQUIRE(t.compare(f) == 0);
        }
    }

    SECTION("Append a number with more that 32 digits")
    {
        float f = 123e32;
     // SECTION("Append to an empty twine")
     // {
     //     twine t = "";
     //     t += f;
     //     INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
     //     REQUIRE_FALSE(t.empty());
     //     REQUIRE(t.size() == 42);
     //     REQUIRE(t.capacity() >= 42);
     //     REQUIRE(t.compare(f) == 0);
     // }

     // SECTION("Append to a small twine")
     // {
     //     twine t = "I am a small string";
     //     t += f;
     //     INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
     //     REQUIRE_FALSE(t.empty());
     //     REQUIRE(t.size() == 19 + 42);
     //     REQUIRE(t.capacity() >= 19 + 42);
     //     //REQUIRE(t.compare(f) == 0);
     // }

     // SECTION("Append to a large twine")
     // {
     //     twine t = "I am a longer string that will not fit in optimized storage.";
     //     t += f;
     //     INFO("f = " << f << "\tt = " << t() << "\tatof(t) = " << atof(t()));
     //     REQUIRE_FALSE(t.empty());
     //     REQUIRE(t.size() == 60 + 42);
     //     REQUIRE(t.capacity() >= 60 + 42);
     //     //REQUIRE(t.compare(f) == 0);
     // }
    }

}

TEST_CASE("Twine - Compare string to zero", "[twine]")
{
    INFO("This maps to the belief that string comparison with an integer should not compare a non-numerical"
            << " string as equal to a number. The other possibility is that which is used by perl: " <<
            "generate a number from the first digits, with no digits -> 0.");
    CHECK(twine("stuff").compare((size_t)0) != 0);
    CHECK(twine().compare((size_t)0) != 0);
    CHECK(twine("stuff").compare(0.0f) != 0);
    CHECK(twine().compare(0.0f) != 0);
}

TEST_CASE("Twine - Get Integer from String", "[twine]")
{
    SECTION("Get 0")
    {
        size_t i = 0;
        twine t;
        t = i;

        INFO("i = " << i << "\tt = " << t() << "\tstoll(string(t())) = " << stoll(string(t())));
        REQUIRE(t.get_int() == stoll(string(t())));
        REQUIRE(i == t.get_int());
    }

    SECTION("Get SIZE_MAX")
    {
        size_t i = SIZE_MAX;
        twine t;
        t = i;

        INFO("i = " << i << "\tt = " << t() << "\tstoll(string(t())) = " << stoll(string(t())));
        REQUIRE(t.get_int() == stoll(string(t())));
        REQUIRE(i == t.get_int());
    }
    
    SECTION("Get unsigned value of smallest possible signed value.")
    {
        size_t i = (SIZE_MAX >> 1) + 1;
        twine t;
        t = i;

        INFO("i = " << i << "\tt = " << t() << "\tstoll(string(t())) = " << stoll(string(t())));
        REQUIRE(t.get_int() == stoll(string(t())));
        REQUIRE(i == t.get_int());
    }

    SECTION("Get unsigned value of smallest possible signed value - 1")
    {
        size_t i = SIZE_MAX >> 1;
        twine t;
        t = i;

        INFO("i = " << i << "\tt = " << t() << "\tstoll(string(t())) = " << stoll(string(t())));
        REQUIRE(t.get_int() == stoll(string(t())));
        REQUIRE(i == t.get_int());
    }

    SECTION("Get max value of atoi()")
    {
        size_t i = (size_t)INT_MAX;
        twine t;
        t = i;

        INFO("i = " << i << "\tt = " << t() << "\tstoll(string(t())) = " << stoll(string(t())));
        REQUIRE(t.get_int() == stoll(string(t())));
        REQUIRE(i == t.get_int());
    }

    SECTION("Get max value of atoi() + 1")
    {
        size_t i = (size_t)INT_MAX + 1;
        twine t;
        t = i;

        INFO("i = " << i << "\tt = " << t() << "\tstoll(string(t())) = " << stoll(string(t())));
        REQUIRE(t.get_int() == stoll(string(t())));
        REQUIRE(i == t.get_int());
    }

    SECTION("Get not an integer")
    {
        twine t = "Not an integer.";

        INFO("t = " << t());
        REQUIRE_THROWS(stoll(string(t())));
        REQUIRE(t.get_int() == 0);
    }

}

TEST_CASE("Twine - Get Single-Precision Floating Point from String", "[twine]")
{
    SECTION("Get +0.0")
    {
        float f = +0.0f;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get -0.0")
    {
        float f = -0.0f;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get +INFINITY")
    {
        float f = INFINITY;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get -INFINITY")
    {
        float f = -INFINITY;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get NaN")
    {
        float f = NAN;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
      //REQUIRE(t.get_float() == stof(string(t())));
      //REQUIRE(f == t.get_float());
        REQUIRE(isnan(t.get_double()));
        REQUIRE(isnan(stod(string(t()))));
        REQUIRE(isnan(f));
    }

    SECTION("Get a sane positive value")
    {
        float f = 142.123;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get a sane negative value")
    {
        float f = -142.123;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    // The default precision of sprintf() for %f is 6
    SECTION("Get a positive value with more than 6 digits")
    {
        float f = 1.23012582302;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get a negative value with more than 6 digits")
    {
        float f = -1.23012582302;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get a rather small number")
    {
        float f = 123e-16;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get a rather large number")
    {
        float f = 123e16;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get a number with more that 32 digits")
    {
        float f = 123e32;
        twine t;
        t = f;

        INFO("f = " << f << "\tt = " << t() << "\tstof(string(t())) = " << stof(string(t())));
        REQUIRE(t.get_float() == stof(string(t())));
        REQUIRE(f == t.get_float());
    }

    SECTION("Get not a float")
    {
        twine t = "Not a float.";

        INFO("t = " << t());
        REQUIRE_THROWS(stof(string(t())));
        REQUIRE(t.get_float() == 0);

    }

}

TEST_CASE("Twine - Get Double-Precision Floating Point from String", "[twine]")
{
    SECTION("Get +0.0")
    {
        double f = +0.0f;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get -0.0")
    {
        double f = -0.0f;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get +INFINITY")
    {
        double f = INFINITY;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get -INFINITY")
    {
        double f = -INFINITY;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get NaN")
    {
        double f = NAN;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
      //REQUIRE(t.get_double() == stod(string(t())));
      //REQUIRE(f == t.get_double());
        REQUIRE(isnan(t.get_double()));
        REQUIRE(isnan(stod(string(t()))));
        REQUIRE(isnan(f));
    }

    SECTION("Get a sane positive value")
    {
        double f = 142.123;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get a sane negative value")
    {
        double f = -142.123;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    // The default precision of sprintf() for %f is 6
    SECTION("Get a positive value with more than 6 digits")
    {
        double f = 1.23012582302;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get a negative value with more than 6 digits")
    {
        double f = -1.23012582302;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get a rather small number")
    {
        double f = 123e-16;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get a rather large number")
    {
        double f = 123e16;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get a number with more that 32 digits")
    {
        double f = 123e32;
        twine t;
        t = (float)f;

        INFO("f = " << f << "\tt = " << t() << "\tstod(string(t())) = " << stod(string(t())));
        REQUIRE(t.get_double() == stod(string(t())));
        REQUIRE(f == t.get_double());
    }

    SECTION("Get not a double")
    {
        twine t = "Not a double.";

        INFO("t = " << t());
        REQUIRE_THROWS(stod(string(t())));
        REQUIRE(t.get_double() == 0);

    }
}

