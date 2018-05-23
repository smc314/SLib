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
    SECTION("Null string") {
        SECTION("String is NULL") {
            SECTION("Direct assignment"){
                REQUIRE_THROWS([&](){twine t = (char *)NULL;}());
            }

            SECTION("Assignment after construction"){
                REQUIRE_THROWS([&](){twine t; t = (char *)NULL;}());
            }
        }

        SECTION("String is \"\"") {
            SECTION("Direct assignment"){
                REQUIRE_NOTHROW([&](){twine t = "";}());
            }

            SECTION("Assignment after construction"){
                REQUIRE_NOTHROW([&](){twine t; t = "";}());
            }
        }
    }
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

    SECTION("Assign Long xmlChar") {
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

    SECTION("Using << Operator")
    {
        SECTION("NULL xmlChar*"){
            REQUIRE_THROWS([&](){twine t; t << (xmlChar*)NULL;}());
        }

        SECTION("Short xmlChar"){
            xmlChar* xml = (xmlChar*)malloc(20 * sizeof(*xml));
            memcpy(xml, "I am a short string", 20);
            twine t; t << xml;

            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

            REQUIRE(t.compare(xml) == 0);
        }

        SECTION("Long xmlChar") {
            xmlChar* xml = (xmlChar*)malloc(61 * sizeof(*xml));
            memcpy(xml, "I am a longer string that will not fit in optimized storage.", 61);
            twine t; t << xml;

            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);

            REQUIRE(t.compare(xml) == 0);

        }
    }

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

TEST_CASE("Twine - Assign from a single char", "[twine]")
{
    SECTION("Calling constructor by name"){
        twine t('Q');
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 1);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t[0] == 'Q');
    }

    SECTION("Implicitly calling constructor"){
        twine t = 'Q';
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 1);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

        REQUIRE(t[0] == 'Q');
    }
    /*
       SECTION("Calling Assignment after initial creation"){
       twine t; t = 'Q';
       REQUIRE_FALSE(t.empty());
       REQUIRE(t.size() == 1);
       REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

       REQUIRE(t[0] == 'Q');
}
*/
}

// TODO: Add testing of twine::twine(const xmlNodePtr, const char*)
// here. I'm going to have to look into how to do that, though.
TEST_CASE("Twine - Construct from xmlNodePtr attribute", "[twine][xml]")
{
    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlParseFile("test/dummy.xml");
    REQUIRE(doc != NULL);

    cur = xmlDocGetRootElement(doc);
    REQUIRE(cur != NULL);
    REQUIRE(xmlStrcmp(cur->name, (const xmlChar *)"RootNode") == 0);

    while(cur != NULL)
    {
        if(xmlStrcmp(cur->name, (const xmlChar *)"ParentNode") == 0)
            break;
        if(cur->next == NULL)
            cur = cur->children;
        else
            cur = cur->next;
    }
    REQUIRE(cur != NULL);

    twine t(cur, "nodeAttribute");
    REQUIRE_FALSE(t.empty());
    REQUIRE(t.size() == 21);
    REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);

    REQUIRE(t.compare("This is an attribute!") == 0);

}

TEST_CASE("Twine - properly returns a small string", "[twine]")
{

    twine t = "This is a small string.";

    REQUIRE(strcmp(t(), "This is a small string.") == 0);

}

// TODO: Somehow make a test that checks whether the destructor properly frees
// the char* m_data. Not sure how to do this either.
// Didn't notice the const before the return type. :|
TEST_CASE("Twine - free m_data before destroying twine", "[twine][.]")
{
    printf("Line %d\n", __LINE__);
    twine *t = new twine("I am a longer string that will not fit in optimized storage.");

    printf("Line %d\n", __LINE__);
    //free((*t)());
    //This line is stupid. WHY DOES THIS WORK???
    char *str = (char*)(*t)();
    printf("Line %d\n", __LINE__);
    free(str);
    printf("Line %d\n", __LINE__);

    printf("Line %d\n", __LINE__);
    REQUIRE_THROWS(delete t);
    printf("Line %d\n", __LINE__);
}

// TODO: implement tests for operator overloads
TEST_CASE("Twine - assignment from constant twine reference", "[twine]")
{
    SECTION("Assign to self")
    {
        twine t("This will be assigned from itself.");
        twine *p = &t;
        t = t;
        REQUIRE(p == &t);
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 34);
        REQUIRE(t.capacity() >= 34);

        REQUIRE(t.compare("This will be assigned from itself.") == 0);
    }
    SECTION("Assign empty to empty"){
        twine t, s;
        t = s;
        REQUIRE(&t != &s);
        REQUIRE(t.empty());
        REQUIRE(t.size() == 0);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        
        REQUIRE(t.compare("") == 0);
        REQUIRE(t.compare(s) == 0);
    }
    SECTION("Assign empty to long"){
        twine t("I am a longer string that will not fit in optimized storage.");
        twine s;
        t = s;
        REQUIRE(&t != &s);
        REQUIRE(t.empty());
        REQUIRE(t.size() == 0);
        REQUIRE(t.capacity() >= 61);

        REQUIRE(t.compare("") == 0);
        REQUIRE(t.compare(s) == 0);
    }
}

TEST_CASE("Twine - assignment from constant twine pointer", "[twine]")
{

    SECTION("Assign to self")
    {
        twine t("This will be assigned from itself.");
        twine *p = &t;
        t = p;
        REQUIRE(p == &t);
        REQUIRE_FALSE(t.empty());
        REQUIRE(t.size() == 34);
        REQUIRE(t.capacity() >= 34);

        REQUIRE(t.compare("This will be assigned from itself.") == 0);
    }
    SECTION("Assign empty to empty"){
        twine t, s, *p;
        p = &s;
        t = p;
        REQUIRE(&t != p);
        REQUIRE(&t != &s);
        REQUIRE(t.empty());
        REQUIRE(t.size() == 0);
        REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
        
        REQUIRE(t.compare("") == 0);
        REQUIRE(t.compare(s) == 0);
    }
    SECTION("Assign empty to long"){
        twine t("I am a longer string that will not fit in optimized storage.");
        twine s, *p;
        p = &s;
        t = p;
        REQUIRE(&t != p);
        REQUIRE(&t != &s);
        REQUIRE(t.empty());
        REQUIRE(t.size() == 0);
        REQUIRE(t.capacity() >= 61);

        REQUIRE(t.compare("") == 0);
        REQUIRE(t.compare(s) == 0);
    }

}

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
        if((int)log10(-(i+1)) + 1 > TWINE_SMALL_STRING - 1)
            REQUIRE(t.capacity() >= (int)log10(-(i+1)) + 1);
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

TEST_CASE("Twine - += another twine", "[twine]")
{
    SECTION("Append an empty twine")
    {
        twine s = "";
        SECTION("Append an empty twine to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE(t.empty());
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare("") == 0);
        }

        SECTION("Append an empty twine to a small twine")
        {
            twine t = "I am a small string";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare("I am a small string") == 0);
        }

        SECTION("Append an empty twine to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.") == 0);
        }

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
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(s) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 * 2);
            REQUIRE(t.capacity() >= 19 * 2);
            REQUIRE(t.compare("I am a small stringI am a small string") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 19);
            REQUIRE(t.capacity() >= 60 + 19);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.I am a small string")
                    == 0);
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

    SECTION("Append a long twine")
    {
        twine s = "I am a longer string that will not fit in optimized storage.";
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);
            REQUIRE(t.compare(s) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + 60);
            REQUIRE(t.capacity() >= 19 + 60);
            REQUIRE(t.compare("I am a small stringI am a longer string that will not fit in optimized storage.")
                    == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 60);
            REQUIRE(t.capacity() >= 60 + 60);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.I am a longer string that will not fit in optimized storage.")
                    == 0);
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

TEST_CASE("Twine - += a twine pointer", "[twine]")
{
    SECTION("Append an empty twine")
    {
        twine *s = new twine("");
        SECTION("Append an empty twine to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE(t.empty());
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare("") == 0);
        }

        SECTION("Append an empty twine to a small twine")
        {
            twine t = "I am a small string";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare("I am a small string") == 0);
        }

        SECTION("Append an empty twine to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.") == 0);
        }

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
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare(*s) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 * 2);
            REQUIRE(t.capacity() >= 19 * 2);
            REQUIRE(t.compare("I am a small stringI am a small string") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 19);
            REQUIRE(t.capacity() >= 60 + 19);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.I am a small string")
                    == 0);
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

    SECTION("Append a long twine")
    {
        twine *s = new twine("I am a longer string that will not fit in optimized storage.");
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);
            REQUIRE(t.compare(*s) == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 19 + 60);
            REQUIRE(t.capacity() >= 19 + 60);
            REQUIRE(t.compare("I am a small stringI am a longer string that will not fit in optimized storage.")
                    == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += s;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == 60 + 60);
            REQUIRE(t.capacity() >= 60 + 60);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.I am a longer string that will not fit in optimized storage.")
                    == 0);
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

// TODO: Deal with this.
// IMPORTANT! This test case MUST be dealt with!
// However, leaving it in a normal run is too dangerous.
// It causes a Bus Error:10 when it calls the insert function.
// This is undesired behavior in the insert function itself,
// but because it's a hardware fault, rather than a software exception,
// it completely aborts all testing, so we lose all test results after this one.
TEST_CASE("Twine - Insert into Extremely Long String", "[twine][stress][.]")
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

TEST_CASE("Twine - Append onto Extremely Long String", "[twine][stress][.][append]")
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
