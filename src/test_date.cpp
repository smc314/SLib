
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

void test_conversions(void);

int main (void)
{
	Date ad;
	Date bd;
	Date cd;

	printf("ad is (%s)\n", ad.GetValue());
	printf("ad is also (%s)\n", ad.GetValue("%Y-%m-%d-%H-%M-%S"));

	bd.SetValue("07/13/2001 01:02:03.456", "%m/%d/%Y %H:%M:%S");
	printf("bd is (%s)\n", bd.GetValue());
	printf("bd is also (%s)\n", bd.GetValue("%m/%d/%Y %H:%M:%S"));

	ad.Floor();
	bd = ad;
	bd.Ceil();

	printf("ad is (%s) bd is (%s)\n", ad.GetValue(), bd.GetValue());

	cd.SetValue("7/4/2001", "%m/%d/%Y");
	printf("cd is (%s)\n", cd.GetValue());

	cd.SetValue("7/4/89", "%m/%d/%Y");
	printf("cd is (%s)\n", cd.GetValue());

	cd.SetValue("7/4/01", "%m/%d/%Y");
	printf("cd is (%s)\n", cd.GetValue());

	cd.SetValue("20010714080910.123", "%Y%m%d%H%M%S");
	printf("cd is (%s)\n", cd.GetValue());

	cd.SetValue("2001/09/01 00:08:05");
	printf("Initial cd is (%s)\n", cd.GetValue());
	cd.AddSec(-65);
	printf("cd is (%s)\n", cd.GetValue());

	ad.SetValue("2001/08/09 12:00:00");
	bd.SetValue("2001/08/09 12:30:00");

	if(bd - ad > SLib::Interval(10, MINUTES)){
		printf("(%s) - (%s) has a difference greater than 10 minutes\n",
			bd.GetValue(), ad.GetValue());
	}

	if(bd - ad > SLib::Interval(20, MINUTES)){
		printf("(%s) - (%s) has a difference greater than 20 minutes\n",
			bd.GetValue(), ad.GetValue());
	}

	if(bd - ad > SLib::Interval(1, DAY)){
		printf("(%s) - (%s) has a difference greater than 1 day\n",
			bd.GetValue(), ad.GetValue());
	}

	test_conversions();

	return 0;
}

void test_conversions(void)
{
	Date d;

	// Date <--> time_t
	time_t t;

	t = time(NULL);
	d.SetValue(t);

	d.SetCurrent();
	t = d;

	// Date <--> struct tm*
	struct tm* stm;

	t = time(NULL);
	stm = localtime(&t);
	d.SetValue(stm);

	d.SetCurrent();
	const struct tm* cstm = d;

	// Date <--> xmlChar*
	xmlChar* xmlc = (xmlChar*)"2002/10/30 10:33:00";
	d.SetValue(xmlc);

	const xmlChar* cxmlc = d;

	// Date <--> char*
	char* c = "2002/10/30 10:33:00";
	d.SetValue(c);

	const char* cc = d;

#ifdef _WIN32
	// Date <--> CTime
	CTime ct;
	d.SetValue(ct);

	ct = (CTime)d;

	// Date <--> COleDateTime
	COleDateTime codt;
	d.SetValue(codt);

	codt = (COleDateTime)d;

	// Date <--> DATE
	DATE D;
	d.SetValue(D);

	D = d;

#endif _WIN32

}
