#include "BlockingQueue.h"
using namespace SLib;

#include <stdio.h>
#include <stdlib.h>

int testBlockingQueue(void)
{
	BlockingQueue<int> testQueue;

	testQueue.push(1);
	testQueue.push(2);
	testQueue.push(3);

	int val = testQueue.pop();
	printf("First value is %d\n", val);
	val = testQueue.pop();
	printf("Second value is %d\n", val);
	val = testQueue.pop();
	printf("Third value is %d\n", val);

	bool isEmpty = testQueue.empty();
	if(isEmpty){
		printf("Queue is empty.\n");
	} else {
		printf("Queue is not empty.\n");
	}

	return 0;
}
