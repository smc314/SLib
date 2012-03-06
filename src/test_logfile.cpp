#include "Log.h"
#include "LogMsg.h"
#include "LogFile.h"
#include "AnException.h"
#include "dptr.h"
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
	// Open a new log file and write some messages to it.
	printf("Opening a new log file testLogFile1.log\n");
	LogFile lf("testLogFile1.log", 
		1024 * 1024 * 10, // 10M max size
		10000, // entries max
		1024 * 1024 * 1, // 1M string table
		1024 * 10, // 10K strings
		false, // don't re-use
		true   // clear at startup
	);
	printf("log file opened\n");

	printf("Dumping log:\n");
	lf.dumpLog();

	// Create 100 messages:
	for(int i = 0; i < 100; i ++){
		dptr<LogMsg> lm = buildMessage(FL, "Test Message #%d", i);
		lm->id = i;
		lf.writeMsg(*lm);
	}

	printf("Dumping log:\n");
	lf.dumpLog();

	printf("Closing log:\n");
	lf.close();

	printf("Opening the log again:\n");
	LogFile lf2("testLogFile1.log", 
		1024 * 1024 * 10, // 10M max size
		10000, // entries max
		1024 * 1024 * 1, // 1M string table
		1024 * 10, // 10K strings
		false, // don't re-use
		false  // don't clear at startup
	);
	printf("log file re-opened\n");
	lf2.dumpLog();
	
		
	printf("Closing log:\n");
	lf2.close();

}

void runTest2()
{
	// Open a new log file and write some messages to it.
	

}

void runTest3()
{
	// Open a new log file and write some messages to it.
	

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
