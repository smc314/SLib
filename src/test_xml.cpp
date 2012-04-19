
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

#include <stdlib.h>
#include <stdio.h>

#include "twine.h"
#include "xmlinc.h"
#include "XmlHelpers.h"
using namespace SLib;

int main (void)
{
	printf("Creating the new document\n");
	xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, (const xmlChar*)"SLibConfig", NULL);

	printf("Setting a property on root.\n");
	xmlNodePtr root = xmlDocGetRootElement(doc);
	xmlSetProp(root, (const xmlChar*)"name", (const xmlChar*)"goes here");

	printf("Creating a new child element of root.\n");
	xmlNodePtr child1 = xmlNewChild(root, NULL, (const xmlChar*)"Child1", NULL);
	xmlSetProp(child1, (const xmlChar*)"prop1", (const xmlChar*)"value1");

	printf("Dumping the doc to a jumbled string\n");
	twine jumbled = XmlHelpers::docToString( doc );

	printf("Dumping the doc to a jumbled string\n");
	twine pretty = XmlHelpers::docToStringPretty( doc );

	printf("Jumbled string: %s\n", jumbled() );
	printf("Pretty string: %s\n", pretty() );

	printf("Reading TestMaster.xml\n");
	xmlDocPtr fileDoc = xmlParseFile( "TestMaster.xml" );

	printf("Formatting TestMaster.xml into a twine\n");
	twine docString = XmlHelpers::docToStringPretty( fileDoc );
	twine format; format.format("Response:\n%s", docString() );

	printf("Formatted twine is:\n%s", format() );
}
