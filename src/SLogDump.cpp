/* ***************************************************************************

   Copyright (c): 2008 - 2012 Viaserv, Inc.

   License: Restricted

   Authors: Steven M. Cherry, Stephen D. Sager

*************************************************************************** */

#include <stdlib.h>
#include <stdio.h>


#include "Socket.h"
#include "twine.h"
#include "Log.h"
#include "EnEx.h"
#include "dptr.h"
#include "Timer.h"
#include "LogFile2.h"
using namespace SLib;

twine m_machineName;
twine m_appName;
twine m_threadID;
int matchThreadID;
twine m_message;
bool m_panic;
bool m_error;
bool m_warn;
bool m_info;
bool m_debug;
bool m_trace;
bool m_sqltrace;
bool m_display_id;
bool m_display_date;
bool m_display_machine;
bool m_display_app;
bool m_display_thread;
bool m_display_file;
bool m_display_line;
bool m_display_channel;
bool m_display_appsession;
bool m_show_stringtable;
bool m_dump_data;
bool m_watch_mode;

void printUsage(char* appName)
{
	printf( "Usage: %s logFile <options>\n", appName);
	printf(
	"Where options include the following:\n"
	"\t-m MachineName Use this to filter on MachineName\n"
	"\t-a AppName     Use this to filter on Application Name\n"
	"\t-t ThreadID    Use this to filter on a thread ID\n"
	"\t-s Message     Use this to filter on message text\n"
	"\t-c*            Use this to include all log channels (default behaviour)\n"
	"\t-c0            Use this to include the PANIC log channel\n"
	"\t-c1            Use this to include the ERROR log channel\n"
	"\t-c2            Use this to include the WARN log channel\n"
	"\t-c3            Use this to include the INFO log channel\n"
	"\t-c4            Use this to include the DEBUG log channel\n"
	"\t-c5            Use this to include the TRACE log channel\n"
	"\t-c6            Use this to include the SQLTRACE log channel\n"
	"\t-d*            Use this to display all log message components (default behaviour)\n"
	"\t-di            Use this to display the log message ID\n"
	"\t-dd            Use this to display the log message date\n"
	"\t-dm            Use this to display the log message machine\n"
	"\t-da            Use this to display the log message application\n"
	"\t-ds            Use this to display the log message appsession\n"
	"\t-dt            Use this to display the log message thread id\n"
	"\t-df            Use this to display the log message file name\n"
	"\t-dl            Use this to display the log message line number\n"
	"\t-dc            Use this to display the log message channel\n"
	"\t-d1            Shortcut for -di -dd -dt -dc\n"
	"\t-b             Use this to display the string table\n"
	"\t-x             Use this to export a dump of the message data directly\n"
	"\t-w             Use this to Watch for new messages\n"
	"\n");
}

void getArgs(int argc, char** argv)
{
	bool first_c = true;
	bool first_d = true;

	for(int i = 2; i < argc; i++){
		if(argv[i][0] == '-'){
			if(argv[i][1] == 'm'){
				i++;
				m_machineName = argv[i];
				continue;
			} else if(argv[i][1] == 'a'){
				i++;
				m_appName = argv[i];
				continue;
			} else if(argv[i][1] == 't'){
				i++;
				m_threadID = argv[i];
				continue;
			} else if(argv[i][1] == 's'){
				i++;
				m_message = argv[i];
				continue;
			} else if(argv[i][1] == 'w'){
				m_watch_mode = true;
			} else if(argv[i][1] == 'b'){
				m_show_stringtable = true;
			} else if(argv[i][1] == 'x'){
				m_dump_data = true;
			} else if(argv[i][1] == 'c'){
				if(first_c){
					// First one of these we hit, turn everything false:
					m_panic = m_error = m_warn = m_info = m_debug = m_trace = m_sqltrace = false;
					first_c = false;
				}
				if(argv[i][2] == '*'){
					m_panic = m_error = m_warn = m_info = m_debug = m_trace = m_sqltrace = true;
				} else if(argv[i][2] == '0'){
					m_panic = true;
				} else if(argv[i][2] == '1'){
					m_error = true;
				} else if(argv[i][2] == '2'){
					m_warn = true;
				} else if(argv[i][2] == '3'){
					m_info = true;
				} else if(argv[i][2] == '4'){
					m_debug = true;
				} else if(argv[i][2] == '5'){
					m_trace = true;
				} else if(argv[i][2] == '6'){
					m_sqltrace = true;
				}
			} else if(argv[i][1] == 'd'){
				if(first_d){
					// First one of these we hit, turn everything false:
					m_display_id = m_display_date = m_display_machine = m_display_app = 
						m_display_appsession = 
						m_display_thread = m_display_file = m_display_line = m_display_channel = false;
					first_d = false;
				}
				if(argv[i][2] == '*'){
					m_display_id = m_display_date = m_display_machine = m_display_app = 
						m_display_appsession = 
						m_display_thread = m_display_file = m_display_line = m_display_channel = true;
				} else if(argv[i][2] == '1'){
					m_display_id = m_display_date = m_display_thread = m_display_channel = true;
					m_display_machine = m_display_app = m_display_file = m_display_line = false;
				} else if(argv[i][2] == 'i'){
					m_display_id = true;
				} else if(argv[i][2] == 'd'){
					m_display_date = true;
				} else if(argv[i][2] == 'm'){
					m_display_machine = true;
				} else if(argv[i][2] == 'a'){
					m_display_app = true;
				} else if(argv[i][2] == 's'){
					m_display_appsession = true;
				} else if(argv[i][2] == 't'){
					m_display_thread = true;
				} else if(argv[i][2] == 'f'){
					m_display_file = true;
				} else if(argv[i][2] == 'l'){
					m_display_line = true;
				} else if(argv[i][2] == 'c'){
					m_display_channel = true;
				}
			} else {
				printf("invalid argument: %s\n", argv[i]);
				printUsage(argv[0]);
				exit(0);
			}
		} else {
			printf("invalid argument: %s\n", argv[i]);
			printUsage(argv[0]);
			exit(0);
		}
	}
}

void printMessage(LogMsg* lm)
{
	char local_tmp[32];
	memset(local_tmp, 0, 32);

	if(m_display_id) printf("%d|", lm->id);

	if(m_display_date){
#ifdef _WIN32
		strftime(local_tmp, 32, "%Y/%m/%d %H:%M:%S", localtime(&(lm->timestamp.time)));
		printf("%s.%.3d|",
			local_tmp, (int)lm->timestamp.millitm
		);
#else
		strftime(local_tmp, 32, "%Y/%m/%d %H:%M:%S", localtime(&(lm->timestamp.tv_sec)));
		printf("%s.%.3d|",
			local_tmp, (int)lm->timestamp.tv_usec
		);
#endif
	}

	if(m_display_machine) printf("%s|", lm->machineName());
	if(m_display_app) printf("%s|", lm->appName());
	if(m_display_appsession) printf("%s|", lm->appSession());
	if(m_display_thread) printf("%ld|", (intptr_t)lm->tid);
	if(m_display_file) printf("%s|", lm->file());
	if(m_display_line) printf("%d|", lm->line);
	if(m_display_channel) {
		switch(lm->channel){
			case 0: printf("PANIC|"); break;
			case 1: printf("ERROR|"); break;
			case 2: printf("WARN|"); break;
			case 3: printf("INFO|"); break;
			case 4: printf("DEBUG|"); break;
			case 5: printf("TRACE|"); break;
			case 6: printf("SQLTRACE|"); break;
		}
	}
	printf("%s\n", lm->msg() );

}

void filterAndPrint(LogMsg* lm)
{
	bool filtersMatch = true;

	if(filtersMatch && m_panic == false){
		if(lm->channel == 0){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_error == false){
		if(lm->channel == 1){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_warn == false){
		if(lm->channel == 2){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_info == false){
		if(lm->channel == 3){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_debug == false){
		if(lm->channel == 4){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_trace == false){
		if(lm->channel == 5){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_sqltrace == false){
		if(lm->channel == 6){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_machineName.length() != 0){
		if(lm->machineName.find( m_machineName ) == TWINE_NOT_FOUND){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_appName.length() != 0){
		if(lm->appName.find( m_appName ) == TWINE_NOT_FOUND){
			filtersMatch = false;
		}
	}
	if(filtersMatch && matchThreadID != 0){
		if((intptr_t)lm->tid != matchThreadID){
			filtersMatch = false;
		}
	}
	if(filtersMatch && m_message.length() != 0){
		if(lm->msg.find( m_message ) == TWINE_NOT_FOUND){
			filtersMatch = false;
		}
	}

	if(filtersMatch){
		printMessage(lm);
	}

}

int main(int argc, char** argv)
{
	twine logFileName = "viaserv.log";
	m_machineName = "";
	m_appName = "";
	m_threadID = "";
	m_message = "";
	m_panic = m_error = m_warn = m_info = m_debug = m_trace = m_sqltrace = true;
	m_display_id = m_display_date = m_display_machine = m_display_app = 
		m_display_appsession = 
		m_display_thread = m_display_file = m_display_line = m_display_channel = true;
	m_show_stringtable = false;
	m_dump_data = false;
	m_watch_mode = false;

	if(argc == 1){
		printUsage(argv[0]);
		return 0;
	};

	logFileName = argv[1];
	getArgs(argc, argv);
	matchThreadID = (int)m_threadID.get_int();

	printf("=============================================\n");

	if(m_panic) printf("Including Log Channel: PANIC\n");
	if(m_error) printf("Including Log Channel: ERROR\n");
	if(m_warn) printf("Including Log Channel: WARN\n");
	if(m_info) printf("Including Log Channel: INFO\n");
	if(m_debug) printf("Including Log Channel: DEBUG\n");
	if(m_trace) printf("Including Log Channel: TRACE\n");
	if(m_sqltrace) printf("Including Log Channel: SQLTRACE\n");

	if(m_machineName.length() != 0 ||
		m_appName.length() != 0 ||
		matchThreadID != 0 ||
		m_message.length() != 0
	){
		printf("Filtering on:\n");
		if(m_machineName.length() != 0){
			printf("Machine Name contains: %s\n", m_machineName() );
		}
		if(m_appName.length() != 0){
			printf("Application Name contains: %s\n", m_appName() );
		}
		if(matchThreadID != 0){
			printf("Thread = %d\n", matchThreadID );
		}
		if(m_message.length() != 0){
			printf("Log Message contains: %s\n", m_message() );
		}
	} else {
		printf("No filtering applied.\n");
	}

	printf("=============================================\n");

	try {
		//printf("Opening log file: %s\n", logFileName() );
		//printf("=============================================\n");
		LogFile2 lf(true, logFileName); // open in read-only mode

		//printf("Dumping Index stats and String table:\n");
		//printf("=============================================\n");
		if(m_show_stringtable){
		}

		int oldest = lf.getOldestMessageID();
		int newest = lf.getNewestMessageID();
		if(m_watch_mode && ( (newest - oldest) > 20 )){
			oldest = newest - 20; // only print the last 20 messages
			if(oldest < 0){
				oldest = 0;
			}
		}
		//printf("Looping over messages %d to %d:\n", oldest, newest);
		//printf("=============================================\n");
		for(int i = oldest; i <= newest; i++){
			//printf("Looking for message: %d\n", i);
			dptr<LogMsg> lm; lm = lf.getMessage(i);
			if(lm == NULL) {
				//printf("\tmessage %d not found!\n", i);
				continue;
			}
			filterAndPrint( lm );
		}			

		if(m_dump_data){
		}

		if(m_watch_mode){ while(1){
			Tools::msleep( 100 );
			try {
				int new_newest = lf.getNewestMessageID();
				if(new_newest > newest){
					for(int i = newest + 1; i <= new_newest; i++){
						dptr<LogMsg> lm; lm = lf.getMessage(i);
						if(lm == NULL) continue;
						filterAndPrint( lm );
					}

					newest = new_newest;
				}
			} catch (AnException&){
				// These are because of database locking.  ignore them.
			}
		} }

	} catch (AnException& e){
		printf("Exception caught opening log file (%s):\n%s\n", logFileName(),
			e.Msg() );
	}
}
