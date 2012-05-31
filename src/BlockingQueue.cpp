#include "BlockingQueue.h"
using namespace SLib;

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

	return 0;
}
