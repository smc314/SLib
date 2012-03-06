
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
#include <time.h>

#include <vector>
#include <string>
using namespace std;

#include "Thread.h"
#include "sptr.h"
#include "Timer.h"
using namespace SLib;

void *run_lots(void *);
string func1(string t);
string func2(string t);
string func3(string t);

class data_class 
{
	public:
		data_class() {}

		virtual ~data_class() {}

		string field1;
		string field2;
		string field3;
		string field4;
		string field5;
		string field6;
		string field7;
		string field8;
		string field9;
		string fielda;
		string fieldb;
		string fieldc;

		int i1;
		int i2;
		int i3;
		int i4;
		int i5;
		int i6;
		int i7;
		int i8;

		string dat1;
		string dat2;
		string dat3;
		string dat4;
		string dat5;
		string dat6;
		string dat7;
		string dat8;
};

int main (void)
{
	printf("Testing string functionality\n");

	vector < Thread * > th_vect;
	int thread_count = 3;

	for(int i = 0; i < thread_count; i++){
		Thread *t;
		t = new Thread();
		t->start(run_lots, NULL);
		th_vect.push_back(t);
	}

	for(int i = 0; i < thread_count; i++){
		th_vect[i]->join();
	}

	for(int i = 0; i < thread_count; i++){
		delete th_vect[i];
	}
}

void *run_lots(void* v)
{
	int i, j;
	Timer tt;
	
	tt.Start();
	for(j = 0; j < 20; j++){
		vector < data_class * > *myvect;
		myvect = new vector < data_class * >;
		for(i = 0; i < 60000; i++){
			data_class *dat = new data_class();
			dat->field1 = "field 1 value";
			dat->field2 = "field 2 value";
			dat->field3 = dat->field1;
			dat->field3 += dat->field2;
			dat->field4 = "57.89f";
			dat->field5 = "A string ";
			dat->field5 += "35 ";
			dat->field5 += "38.99";
			dat->i1 = 18;
			dat->i2 = 19;
			dat->i3 = dat->i1 * dat->i2;
			dat->field6 = "ANumber";
			dat->field7 = "AnotherNumber";
			dat->field8 = "AThirdNumber";
			dat->field9 = dat->field2.substr(3, 5);
			if(dat->field1 > dat->field2)
				dat->fielda = "true";
			else
				dat->fielda = "false";
			myvect->push_back(dat);	
		}
		for(i = 0; i < (int)myvect->size(); i++){
			delete (*myvect)[i];
		}
		delete myvect;
	}
	tt.Finish();
	printf("Duration for the regular stuff is (%f)\n", tt.Duration());

	tt.Start();

	for(j = 0; j < 20; j++){
		for(i = 0; i < 60000; i++){
			string ret_tmp;

			ret_tmp = func1("interesting");

		}
	}

	tt.Finish();
	printf("Duration for the func stuff is (%f)\n", tt.Duration());

	return (void*)NULL;
}

string func1(string t)
{
	return func2(t);
}

string func2(string t)
{
	return func3(t);
}

string func3(string t)
{
	t += "you got here :-)";
	return t;
}
