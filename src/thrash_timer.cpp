#include <stdlib.h>
#include <stdio.h>

#include <vector>
using namespace std;

#include "EnEx.h"
#include "Timer.h"
#include "Thread.h"
using namespace SLib;

void *run_lots(void* v);

int main(void)
{
	vector < Thread * > th_vect;
	int thread_count = 30;
	int i;

	for(i = 0; i < thread_count; i++){
		Thread *t;
		t = new Thread();
		t->start(run_lots, NULL);
		th_vect.push_back(t);
	}
	printf("Threads started...\n");

	for(i = 0; i < thread_count; i++){
		th_vect[i]->join();
	}

	for(i = 0; i < thread_count; i++){
		delete th_vect[i];
	}
	
}

void *run_lots(void* v)
{
	EnEx ee("run_lots()", true);
	int i, j;
	Timer tt;
	int count = 20000000;

	tt.Start();
	for(j = 0; j < count; j++){
		uint64_t cc = Timer::GetCycleCount();
	}
	tt.Finish();

	printf("Time for %d calls to GetCycleCount() is: %f\n", count, tt.Duration() );
	return NULL;
}
