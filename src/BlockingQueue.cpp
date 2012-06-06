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
	val = testQueue.pop();
	val = testQueue.pop();

	bool isEmpty = testQueue.empty();
	if(isEmpty){
		printf("Queue is empty.\n");
	} else {
		printf("Queue is not empty.\n");
	}

	return 0;
}
