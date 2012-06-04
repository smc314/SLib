
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "BlockingQueue.h"
#include "twine.h"
#include "AnException.h"
#include "Thread.h"
using namespace SLib;

void* consumer1(void*);
void* producer1(void*);
void* producer2(void*);
void* producer3(void*);
void* producer4(void*);

bool stopConsumer = false;
BlockingQueue<twine*> testQueue;

int main (void)
{
	Thread c1;
	Thread p1;
	Thread p2;
	Thread p3;
	Thread p4;

	printf("Starting consumer and all producers...\n");
	c1.start( consumer1, NULL );
	p1.start( producer1, NULL );
	p2.start( producer2, NULL );
	p3.start( producer3, NULL );
	p4.start( producer4, NULL );

	p1.join();
	p2.join();
	p3.join();
	p4.join();
	printf("Producers all done.  Shutting down consumer...\n");

	stopConsumer = true;
	c1.join();

	printf("Consumer all done.  Test exiting.\n");
}

void* consumer1(void*)
{
	while(!stopConsumer){
		dptr<twine > msg = testQueue.pop();
		
		printf("Consumer found message: %s\n", msg->c_str() );
	}
	return NULL;
}

void* producer1(void*)
{
	for(int i = 0; i < 10; i ++){
		twine* msg = new twine();
		msg->format("P1 - msg %d", i);

		printf("P1 - sending message %d\n", i);
		testQueue.push( msg );
	}
	return NULL;
}

void* producer2(void*)
{
	for(int i = 0; i < 10; i ++){
		twine* msg = new twine();
		msg->format("P2 - msg %d", i);

		printf("P2 - sending message %d\n", i);
		testQueue.push( msg );
	}
	return NULL;
}


void* producer3(void*)
{
	for(int i = 0; i < 10; i ++){
		twine* msg = new twine();
		msg->format("P3 - msg %d", i);

		printf("P3 - sending message %d\n", i);
		testQueue.push( msg );
	}
	return NULL;
}


void* producer4(void*)
{
	for(int i = 0; i < 10; i ++){
		twine* msg = new twine();
		msg->format("P4 - msg %d", i);

		printf("P4 - sending message %d\n", i);
		testQueue.push( msg );
	}
	return NULL;
}



