#include <stdlib.h>
#include <stdio.h>

#include "twine.h"
#include "Timer.h"
using namespace SLib;

int main(void)
{
	int i, count;
	Timer t;

	count = 1000000;

	t.Start();
	for(i = 0; i < count; i++){
		twine tmp;
		tmp.format("This is (%d) a message (%s) with several (%f) "
			"replacement (%d) parameters (%s)",
			i, "hithere everyone", 3.14159f, i*2,
			"interesting isn't it? ;-)");
	}
	t.Finish();

	printf("Time for %d twine::format calls is (%f)\n", 
		count, t.Duration());

	return 0;
}

