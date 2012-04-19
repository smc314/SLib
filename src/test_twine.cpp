
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
#include <algorithm>


#include <vector>
using namespace std;

#include "twine.h"
#include "sptr.h"
#include "EnEx.h"
#include "Thread.h"
#include "Timer.h"
#include "Log.h"
using namespace SLib;

void *run_lots(void *);
twine func1(twine t);
twine func2(twine t);
twine func3(twine t);

class data_class 
{
	public:
		data_class() {}

		virtual ~data_class() {}

		twine field1;
		twine field2;
		twine field3;
		twine field4;
		twine field5;
		twine field6;
		twine field7;
		twine field8;
		twine field9;
		twine fielda;
		twine fieldb;
		twine fieldc;

		int i1;
		int i2;
		int i3;
		int i4;
		int i5;
		int i6;
		int i7;
		int i8;

		twine dat1;
		twine dat2;
		twine dat3;
		twine dat4;
		twine dat5;
		twine dat6;
		twine dat7;
		twine dat8;
};

int main (void)
{
	printf("Testing twine functionality\n");

	vector < Thread * > th_vect;
	int thread_count = 3;
	int i;

	for(i = 0; i < thread_count; i++){
		Thread *t;
		t = new Thread();
		t->start(run_lots, NULL);
		th_vect.push_back(t);
	}

	for(i = 0; i < thread_count; i++){
		th_vect[i]->join();
	}

	for(i = 0; i < thread_count; i++){
		delete th_vect[i];
	}
	
	EnEx::PrintGlobalHitMap();

	// Logging test:
	Log::SetTrace(true);
	{ // for scope
		EnEx ee(FL, "main_end");

	}
}

void *run_lots(void* v)
{
	EnEx ee("run_lots()", true);
	int i, j;
	Timer tt;

	tt.Start();
	for(j = 0; j < 20; j++){
		EnEx eeo("run_lots() - main loop");
		vector < data_class * > *myvect;
		myvect = new vector < data_class * >;
		for(i = 0; i < 60000; i++){
			EnEx eei("run_lots() - inner loop");
			data_class *dat = new data_class();
			dat->field1 = "field 1 value";
			dat->field2 = "field 2 value";
			dat->field3 = dat->field1;
			dat->field3 += dat->field2;
			dat->field4 = 57.89f;
			dat->field5.format("%s %d %f", "A string", 35, 38.99);
			dat->i1 = 18;
			dat->i2 = 19;
			dat->i3 = dat->i1 * dat->i2;
			dat->field6 = (size_t)dat->i1 + dat->i2 + dat->i3;
			dat->field7 = "interesting";
			dat->field8 = (size_t)i * j;
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
			twine ret_tmp;

			ret_tmp = func1("interesting");

		}
	}

	tt.Finish();
	printf("Duration for the func stuff is (%f)\n", tt.Duration());

	return (void*)NULL;
}

twine func1(twine t)
{
	return func2(t);
}

twine func2(twine t)
{
	return func3(t);
}

twine func3(twine t)
{
	t += "you got here :-)";
	return t;
}
