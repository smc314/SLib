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

// TODO: Add tests for copying of twines with embedded nulls

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>

#include "twine.h"
#include "xmlinc.h"
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
        delete s;
    }
}

TEST_CASE("Twine - += a constant C string", "[twine]")
{
    SECTION("Append an empty string")
    {
        char *s = "";
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
    }

    SECTION("Append a short string")
    {
        char *s = "I am a small string";
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
    }

    SECTION("Append a long string")
    {
        char *s = "I am a longer string that will not fit in optimized storage.";
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

    }
    
}

TEST_CASE("Twine - += a single char", "[twine]")
{
    SECTION("Append a null character")
    {
        char c = '\0';
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += c;
            REQUIRE(t.empty());
            REQUIRE(t.size() == strlen(t()));
            REQUIRE(t.size() == 0);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare("") == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += c;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == strlen(t()));
            REQUIRE(t.size() == 19);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare("I am a small string") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += c;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == strlen(t()));
            REQUIRE(t.size() == 60);
            REQUIRE(t.capacity() >= 60);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.") == 0);
        }
    }

    SECTION("Append a normal character")
    {
        char c = 'Q';
        SECTION("Append to an empty twine")
        {
            twine t = "";
            t += c;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == strlen(t()));
            REQUIRE(t.size() == 1);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare("Q") == 0);
        }

        SECTION("Append to a small twine")
        {
            twine t = "I am a small string";
            t += c;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == strlen(t()));
            REQUIRE(t.size() == 20);
            REQUIRE(t.capacity() == TWINE_SMALL_STRING - 1);
            REQUIRE(t.compare("I am a small stringQ") == 0);
        }

        SECTION("Append to a large twine")
        {
            twine t = "I am a longer string that will not fit in optimized storage.";
            t += c;
            REQUIRE_FALSE(t.empty());
            REQUIRE(t.size() == strlen(t()));
            REQUIRE(t.size() == 61);
            REQUIRE(t.capacity() >= 61);
            REQUIRE(t.compare("I am a longer string that will not fit in optimized storage.Q") == 0);
        }

    }
    // TODO: Add tests for strange characters to see what happens.
}

TEST_CASE("Twine - array indexing", "[twine]")
{
    SECTION("Normal use cases")
    {
        twine t = "Stuff Stuff Stuff";
        REQUIRE_NOTHROW(t[5]);
    }

    SECTION("Out of bounds")
    {
        twine t = "Stuff";
        REQUIRE_THROWS(t[5]);
        REQUIRE_THROWS(t[-1]);
        REQUIRE_THROWS(t[6]);
    }

    SECTION("Float size allocation failure")
    {
        float f = 123e32;

        twine t;
        t = f;

        int len = strlen(t());

        //REQUIRE_NOTHROW(t[5]);
        REQUIRE_NOTHROW(t[len - 1]);
    }
}

TEST_CASE("Twine - get C-style string", "[twine]")
{
    SECTION("Short String")
    {
        char* s = "I am a short string";
        twine t = s; 

        INFO("s: " << s << "\tt(): " << t() << "\tt.c_str(): " << t.c_str());
        REQUIRE(strcmp(t(), s) == 0);
        REQUIRE(strcmp(t.c_str(), s) == 0);
        REQUIRE(t() == t.c_str());
    }

    SECTION("Long String")
    {
        char* s = "I am a longer string that will not fit in optimized storage.";
        twine t = s; 

        INFO("s: " << s << "\tt(): " << t() << "\tt.c_str(): " << t.c_str());
        REQUIRE(strcmp(t(), s) == 0);
        REQUIRE(strcmp(t.c_str(), s) == 0);
        REQUIRE(t() == t.c_str());
    }
}

TEST_CASE("Twine - compare a twine reference", "[twine]")
{
    SECTION("Normal Comparisons")
    {
        // grep -B1 -A1 ^`perl -e 'open IN, "</usr/share/dict/words";rand($.) < 1 && ($n=$_) while <IN>;print $n'`$ /usr/share/dict/words
        twine b = "agistator";
        twine t = "agistment"; //the act of feeding or pasturing livestock for a fee
        twine a = "agistor"; // one who agists

        INFO("b: " << b() << "\tt: " << t() << "\ta: " << a());

        SECTION("Compare whole twine")
        {
            REQUIRE(b.compare(b) == 0);
            REQUIRE(b.compare(t) < 0);
            REQUIRE(b.compare(a) < 0);

            REQUIRE(t.compare(b) > 0);
            REQUIRE(t.compare(t) == 0);
            REQUIRE(t.compare(a) < 0);

            REQUIRE(a.compare(b) > 0);
            REQUIRE(a.compare(t) > 0);
            REQUIRE(a.compare(a) == 0);
        }

        SECTION("Compare part of twine")
        {
            REQUIRE(t.compare(b, 0) == 0);
            REQUIRE(t.compare(t, 0) == 0);
            REQUIRE(t.compare(a, 0) == 0);

            REQUIRE(t.compare(b, 4) == 0);
            REQUIRE(t.compare(t, 4) == 0);
            REQUIRE(t.compare(a, 4) == 0);

            REQUIRE(t.compare(b, 5) == 0);
            REQUIRE(t.compare(t, 5) == 0);
            REQUIRE(t.compare(a, 5) == 0);

            REQUIRE(t.compare(b, 6) > 0);
            REQUIRE(t.compare(t, 6) == 0);
            REQUIRE(t.compare(a, 6) < 0);

            REQUIRE(t.compare(b, 8) > 0);
            REQUIRE(t.compare(t, 8) == 0);
            REQUIRE(t.compare(a, 8) < 0);
        }
    }

    SECTION("Compare Empty Strings")
    {
        twine e = "";
        twine f = "full";

        INFO("e: " << e() << "\tf: " << f());

        SECTION("Compare whole twine")
        {
            REQUIRE(e.compare(e) == 0);
            REQUIRE(e.compare(f) < 0);

            REQUIRE(f.compare(e) > 0);
            REQUIRE(f.compare(f) == 0);
        }

        SECTION("Compare part of twine")
        {
            REQUIRE(e.compare(e, 0) == 0);
            REQUIRE(e.compare(f, 0) == 0);
            REQUIRE(f.compare(e, 0) == 0);
            REQUIRE(f.compare(f, 0) == 0);

            REQUIRE(e.compare(e, 2) == 0);
            REQUIRE(e.compare(f, 2) < 0);
            REQUIRE(f.compare(e, 2) > 0);
            REQUIRE(f.compare(f, 2) == 0);

            REQUIRE(e.compare(e, 4) == 0);
            REQUIRE(e.compare(f, 4) < 0);
            REQUIRE(f.compare(e, 4) > 0);
            REQUIRE(f.compare(f, 4) == 0);
        }
    }
}

TEST_CASE("Twine - compare a C-style string", "[twine]")
{

    SECTION("Normal Comparisons")
    {
        // grep -B1 -A1 ^`perl -e 'open IN, "</usr/share/dict/words";rand($.) < 1 && ($n=$_) while <IN>;print $n'`$ /usr/share/dict/words
        char *b = "batrachoid";
        char *w = "batrachoididae";
        char *a = "batrachophagous";
        
        twine bt = b;
        twine wt = w;
        twine at = a;

        INFO("bt: " << bt() << "\twt: " << wt() << "\tat: " << at());

        SECTION("Compare whole twine")
        {
            REQUIRE(bt.compare(b) == 0);
            REQUIRE(bt.compare(w) < 0);
            REQUIRE(bt.compare(a) < 0);

            REQUIRE(wt.compare(b) > 0);
            REQUIRE(wt.compare(w) == 0);
            REQUIRE(wt.compare(a) < 0);

            REQUIRE(at.compare(b) > 0);
            REQUIRE(at.compare(w) > 0);
            REQUIRE(at.compare(a) == 0);
        }

        SECTION("Compare part of twine")
        {
            REQUIRE(wt.compare(b, 0) == 0);
            REQUIRE(wt.compare(w, 0) == 0);
            REQUIRE(wt.compare(a, 0) == 0);

            REQUIRE(wt.compare(b, 8) == 0);
            REQUIRE(wt.compare(w, 8) == 0);
            REQUIRE(wt.compare(a, 8) == 0);

            REQUIRE(wt.compare(b, 9) > 0);
            REQUIRE(wt.compare(w, 9) == 0);
            REQUIRE(wt.compare(a, 9) < 0);
        }
    }

    SECTION("Compare NULLs")
    {
        twine et = "";
        twine ft = "full";

        char* nc = NULL;
        char* ec = "";
        char* fc = "full";

        INFO("et: " << et() << "\tft: " << ft()); //<< "\tnc: " << nc);

        SECTION("Compare whole twine")
        {
            REQUIRE(et.compare(nc) == 0);
            REQUIRE(et.compare(ec) == 0);
            REQUIRE(et.compare(fc) < 0);

            REQUIRE(ft.compare(nc) > 0);
            REQUIRE(ft.compare(ec) > 0);
            REQUIRE(ft.compare(fc) == 0);
        }

        SECTION("Compare part of twine")
        {
            REQUIRE(et.compare(nc, 0) == 0);
            REQUIRE(et.compare(ec, 0) == 0);
            REQUIRE(et.compare(fc, 0) == 0);

            REQUIRE(ft.compare(nc, 0) == 0);
            REQUIRE(ft.compare(ec, 0) == 0);
            REQUIRE(ft.compare(fc, 0) == 0);

            REQUIRE(et.compare(nc, 2) == 0);
            REQUIRE(et.compare(ec, 2) == 0);
            REQUIRE(et.compare(fc, 2) < 0);

            REQUIRE(ft.compare(nc, 2) > 0);
            REQUIRE(ft.compare(ec, 2) > 0);
            REQUIRE(ft.compare(fc, 2) == 0);

            REQUIRE(et.compare(nc, 4) == 0);
            REQUIRE(et.compare(ec, 4) == 0);
            REQUIRE(et.compare(fc, 4) < 0);

            REQUIRE(ft.compare(nc, 4) > 0);
            REQUIRE(ft.compare(ec, 4) > 0);
            REQUIRE(ft.compare(fc, 4) == 0);
        }
    }
}

TEST_CASE("Twine - test prefixes", "[twine]")
{
    twine b = "calycanthaceous";
    twine t = "calycanthemous";
    twine a = "calycanthemy";

    twine e = "";
    twine p = "calycanth";
    twine np = "calycanthe";
    twine bi = "calycanthaceous";

    INFO("b: " << b() << "\tt: " << t() << "\ta: " << a() << "\te: " << e() << "\tp: " << p() << "\tnp: " << np() << "\tbi: " << bi());

    REQUIRE(b.startsWith(p));
    REQUIRE_FALSE(b.startsWith(np));
    REQUIRE(b.startsWith(bi));

    REQUIRE(t.startsWith(p));
    REQUIRE(t.startsWith(np));
    REQUIRE_FALSE(t.startsWith(bi));

    REQUIRE(a.startsWith(p));
    REQUIRE(a.startsWith(np));
    REQUIRE_FALSE(a.startsWith(bi));

    REQUIRE(b.startsWith(e));
    REQUIRE(t.startsWith(e));
    REQUIRE(a.startsWith(e));
}

TEST_CASE("Twine - test suffixes", "[twine]")
{
    twine t1 = "ditetragonal";
    twine t2 = "pseudotetragonal";
    twine t3 = "rotundotetragonal";
    twine t4 = "tetragonal";

    twine es = "";
    twine cs = "tetragonal";
    twine hs = "dotetragonal";
    twine ns = "udotetragonal";
    twine ls = "rotundotetragonal";

    INFO("t1: " << t1() << "\tt2: " << t2() << "\tt3: " << t3() << "\tt4: " << t4());

    REQUIRE(t1.endsWith(cs));
    REQUIRE_FALSE(t1.endsWith(hs));
    REQUIRE_FALSE(t1.endsWith(ns));
    REQUIRE_FALSE(t1.endsWith(ls));

    REQUIRE(t2.endsWith(cs));
    REQUIRE(t2.endsWith(hs));
    REQUIRE(t2.endsWith(ns));
    REQUIRE_FALSE(t2.endsWith(ls));

    REQUIRE(t3.endsWith(cs));
    REQUIRE(t3.endsWith(hs));
    REQUIRE_FALSE(t3.endsWith(ns));
    REQUIRE(t3.endsWith(ls));

    REQUIRE(t4.endsWith(cs));
    REQUIRE_FALSE(t4.endsWith(hs));
    REQUIRE_FALSE(t4.endsWith(ns));
    REQUIRE_FALSE(t4.endsWith(ls));

    REQUIRE(t1.endsWith(es));
    REQUIRE(t2.endsWith(es));
    REQUIRE(t3.endsWith(es));
    REQUIRE(t4.endsWith(es));
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
