
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

#include "XmlHelpers.h"
#include "sptr.h"
using namespace SLib;

#include "catch.hpp"

TEST_CASE( "XmlHelpers - Parse from twine and free doc", "[xml]" )
{
	twine xmlString(
		"<XmlRoot>With some Text<XmlChild>With child text<XmlGrandChild>"
		"With grand child text</XmlGrandChild>Child text continues here"
		"</XmlChild>Root text is finished here.</XmlRoot>"
	);

	xmlDocPtr doc = xmlParseDoc( xmlString );
	sptr<xmlDoc, xmlFreeDoc> docSptr = doc; // make sure it gets cleaned up
	REQUIRE_FALSE( doc == NULL );
}

TEST_CASE( "XmlHelpers - Read all text nodes of root - but not children", "[xml]" )
{
	twine xmlString(
		"<XmlRoot>With some Text<XmlChild>With child text<XmlGrandChild>"
		"With grand child text</XmlGrandChild>Child text continues here"
		"</XmlChild>Root text is finished here.</XmlRoot>"
	);

	sptr<xmlDoc, xmlFreeDoc> doc = xmlParseDoc( xmlString );

	xmlNodePtr root = xmlDocGetRootElement( doc );
	REQUIRE_FALSE( root == NULL );

	twine textContents = XmlHelpers::getTextNodeValue( root );
	REQUIRE_FALSE( textContents.empty() );
	printf("Root node contents:\n%s\n", textContents() );
}

TEST_CASE( "XmlHelpers - Read all text nodes of root - and children", "[xml]" )
{
	twine xmlString(
		"<XmlRoot>With some Text<XmlChild>With child text<XmlGrandChild>"
		"With grand child text</XmlGrandChild>Child text continues here"
		"</XmlChild>Root text is finished here.</XmlRoot>"
	);

	sptr<xmlDoc, xmlFreeDoc> doc = xmlParseDoc( xmlString );

	xmlNodePtr root = xmlDocGetRootElement( doc );
	REQUIRE_FALSE( root == NULL );

	twine fullText;
	AutoXMLChar tmp;
	tmp = xmlNodeGetContent( root );
	fullText = tmp;
	REQUIRE_FALSE( fullText.empty() );
	printf("Full contents:\n%s\n", fullText() );
}

