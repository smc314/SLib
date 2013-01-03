#include "Log.h"
#include "LogMsg.h"
#include "LogFile2.h"
#include "AnException.h"
#include "dptr.h"
#include "Timer.h"
using namespace SLib;

#include <stdarg.h>

void runTest1();
void runTest2();
void runTest3();
LogMsg* buildMessage(const char* file, int line, const char* msg, ...);

int main(void)
{

	try {
		runTest1();

		runTest2();

		runTest3();

	} catch (AnException& e){
		printf("Exception caught: %s\n", e.Msg() );
		printf("Aborting tests.\n" );
		return -1;
	}

}

void runTest1()
{
	Timer tt;
	tt.Start();
	// Open a new log file and write some messages to it.
	printf("Opening a new log file testLogFile2.log - writing 10,000 messages.\n");
	twine fileName = "testLogFile2.log";
	LogFile2 lf(fileName, (size_t)(1024 * 1024 * 10)); // 10M max size
	printf("log file opened\n");

	// Create 10,000 messages:
	for(int i = 0; i < 10000; i ++){
		dptr<LogMsg> lm = buildMessage(FL, "Test Message #%d", i);
		lm->id = i;
		lf.writeMsg(*lm);
	}
	lf.flush(); // ensure everything goes to the disk.

	printf("Closing log:\n");
	lf.close();
	tt.Finish();
	printf("Duration for runTest1 is (%f)\n", tt.Duration() );
}

void runTest2()
{
	Timer tt;
	tt.Start();
	// Open a new log file and write some messages to it.
	printf("Opening a new log file testLogFile3.log - writing 10,000 messages.\n");
	twine fileName = "testLogFile3.log";
	LogFile2 lf(fileName, (size_t)(1024 * 1024 * 10)); // 10M max size
	printf("log file opened\n");
	lf.setCacheSize( 1000 );

	// Create 10,000 messages:
	for(int i = 0; i < 10000; i ++){
		dptr<LogMsg> lm = buildMessage(FL, "Test Message #%d", i);
		lm->id = i;
		lf.writeMsg(*lm);
	}
	lf.flush(); // ensure everything goes to the disk.

	printf("Closing log:\n");
	lf.close();
	tt.Finish();
	printf("Duration for runTest2 is (%f)\n", tt.Duration() );
}

void runTest3()
{
	Timer tt;
	tt.Start();
	// Open a new log file and write some messages to it.
	printf("Opening a new log file testLogFile4.log - writing 10,000 messages.\n");
	twine fileName = "testLogFile4.log";
	LogFile2 lf(fileName, (size_t)(1024 * 1024 * 10)); // 10M max size
	printf("log file opened\n");
	lf.setCacheSize( 3000 );

	// Create 10,000 messages:
	for(int i = 0; i < 10000; i ++){
		dptr<LogMsg> lm = buildMessage(FL, "Test Message #%d", i);
		lm->id = i;
		lf.writeMsg(*lm);
	}
	lf.flush(); // ensure everything goes to the disk.

	printf("Closing log:\n");
	lf.close();
	tt.Finish();
	printf("Duration for runTest3 is (%f)\n", tt.Duration() );
}

LogMsg* buildMessage(const char* file, int line, const char* msg, ...)
{
	LogMsg* lm = new LogMsg(file, line);

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	va_end(ap);

	lm->channel = 0; // Panic
	return lm;
}
