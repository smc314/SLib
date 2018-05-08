
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

#include "Thread.h"
#include "Socket.h"
#include "Date.h"
#include "Interval.h"
using namespace SLib;

#include "catch.hpp"

TEST_CASE( "Date - GetValue 1", "[date]" )
{
	Date ad; ad.SetValue( "07/13/2001 01:02:03.456", "%m/%d/%Y %H:%M:%S" );
	REQUIRE( ad.GetValue() == "2001/07/13 01:02:03" );
	REQUIRE( ad.GetValue("%Y-%m-%d-%H-%M-%S") == "2001-07-13-01-02-03" );
	REQUIRE( ad.GetValue("%m/%d/%Y %H:%M:%S") == "07/13/2001 01:02:03" );
	REQUIRE( ad.GetValue("%Y%m%d%H%M%S") == "20010713010203" );
	REQUIRE( ad.EDate() == "Fri, 13 Jul 2001 01:02:03 -0500" );

	SECTION( "Flooring the date works" ){
		ad.Floor();
		REQUIRE( ad.Hour() == 0 );
		REQUIRE( ad.Min() == 0 );
		REQUIRE( ad.Sec() == 0 );
		REQUIRE( ad.GetValue() == "2001/07/13 00:00:00" );
	}

	SECTION( "Ceiling the date works" ){
		ad.Ceil();
		REQUIRE( ad.Hour() == 23 );
		REQUIRE( ad.Min() == 59 );
		REQUIRE( ad.Sec() == 59 );
		REQUIRE( ad.GetValue() == "2001/07/13 23:59:59" );
	}
}

TEST_CASE( "Date - time_t conversions", "[date]" )
{
	Date ad;
	time_t t;

	t = time(NULL);
	ad.SetCurrent();
	REQUIRE( t == ad );

	ad.SetValue( t );
	REQUIRE( t == ad );
}

TEST_CASE( "Date - Compare1", "[date]" )
{
	Date ad; 
	Date bd;

	ad.SetValue("2001/08/09 12:00:00");
	bd.SetValue("2001/08/09 12:30:00");

	REQUIRE( bd - ad > SLib::Interval(10, MINUTES) );
	REQUIRE( bd - ad > SLib::Interval(20, MINUTES) );
	REQUIRE_FALSE( bd - ad > SLib::Interval(1, DAY) );
	REQUIRE_FALSE( ad == bd );

	bd.AddMin( -30 );
	REQUIRE( ad == bd );

}

/*
void test_conversions(void)
{
	printf("test_conversions - start");

	Date d;

	printf("test_conversions - date <--> time_t");

	// Date <--> time_t
	time_t t;

	t = time(NULL);
	d.SetValue(t);

	d.SetCurrent();
	t = d;

	printf("test_conversions - date <--> struct tm*");
	// Date <--> struct tm*
	struct tm* stm;

	t = time(NULL);
	stm = localtime(&t);
	d.SetValue(stm);

	d.SetCurrent();
	const struct tm* cstm = d;

	printf("test_conversions - date <--> xmlChar*");
	// Date <--> xmlChar*
	xmlChar* xmlc = (xmlChar*)xmlMalloc(24);	
	memcpy(xmlc, "2002/10/30 10:33:00", 19);
	d.SetValue(xmlc);

	const xmlChar* cxmlc = d;

	printf("test_conversions - date <--> char*");
	// Date <--> char*
	char* c = "2002/10/30 10:33:00";
	d.SetValue(c);

	const char* cc = d;

	printf("test_conversions - done");
}
*/
