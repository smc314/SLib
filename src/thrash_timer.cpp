#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <sys/timeb.h>

#include "Timer.h"
using namespace SLib;

static __inline__ unsigned long GetCC(void)
{
	unsigned a, d;
	asm volatile("rdtsc" : "=a" (a), "=d" (d));
	return ((unsigned long)a) | (((unsigned long)d) << 32);
}

int main(void)
{
	int i, count;
	Timer t;
	timespec rtclock;
	timeb timerTime;
	clock_t ticks;
	unsigned long cc;


	count = 10000000;

	t.Start();
	for(i = 0; i < count; i++){
		clock_gettime(CLOCK_REALTIME, &rtclock);
	}
	t.Finish();

	printf("Time for %d clock_gettime calls is (%f)\n", 
		count, t.Duration());

	t.Start();
	for(i = 0; i < count; i++){
		ftime(&timerTime);
	}
	t.Finish();

	printf("Time for %d ftime calls is (%f)\n", 
		count, t.Duration());

	t.Start();
	for(i = 0; i < count; i++){
		ticks = times(NULL);
	}
	t.Finish();

	printf("Time for %d times calls is (%f)\n", 
		count, t.Duration());

	t.Start();
	for(i = 0; i < count; i++){
		cc = GetCC();
	}
	t.Finish();

	printf("Time for %d GetCC calls is (%f)\n", 
		count, t.Duration());

	return 0;
}

