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

/* ***************************************************************************
	This is a testing suite that will test the various bits of functionality
	in our SLib library.
*************************************************************************** */

/* ***************************************************************************
   Output log messages look use a series of 2 character prefix symbols to
   indicate what the log messages pertain to.  Each of the different types
   can be enabled/disabled for the test run.

>> TestName - description of test
++ Optional Details about what we are doing
++ as we do it.
!! Any Errors that we see, or failed assertions about the test results
!! will have these lines reported.
<< TestName - Success - Duration - Tests Total (123) Success (123) Failure (0)
** TestCategory Total (123) Success (123) Failure (0)\n",

*************************************************************************** */

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdexcept>

// Our SLib includes
#include <twine.h>
#include <Date.h>
#include <AnException.h>
#include <XmlHelpers.h>
using namespace SLib;

char* usage = 
	"Usage: %s [options]\n"
	"\twhere [options] include the following:\n"
	"--test-file=testfile              Provide the name of a test file that tells us\n"
	"                                  which tables to test on which servers.  We will\n"
	"                                  loop through all tables listed in the test file\n"
	"                                  and run our generated sql for each table.\n"
	"\n"
	"--log-all=[true|false]            Turn on/off all of our logging. False by default.\n"
	"--log-test=[true|false]           Turn on/off test start/finish logging.  True by default.\n"
	"--log-steps=[true|false]          Turn on/off step by step logging.  False by default.\n"
	"--log-err=[true|false]            Turn on/off error logging.  True by default.\n"
	"\n"
	"--log-html=[true|false]           Produces summary output as HTML table rows for our results\n"
	"                                  when turned on.  False by default.\n"
	"--html-gid=GroupID                Produces a slide section with the given group id so that\n"
	"                                  you can categorize our output.\n"
	"--html-group=GroupHeading         Produces a slide section with the given group heading so that\n"
	"                                  you can categorize our output.\n"
	"--html-desc=RowDesc               Uses the given RowDesc label as the first item in the\n"
	"                                  output table row.\n"
	"--html-out=HtmlOutputFile         Uses the given HtmlOutputFile as the name of a file to output\n"
	"                                  all generated Html text.  If not provided, all html output will\n"
	"                                  go to stdout.\n"
	"\n"
	"--do-meta=[true|false]            When this is set to true we will gather data and metadata for\n"
	"                                  each table as we process them.  This is required for any of the\n"
	"                                  do-bugs, do-cols, do-from, do-where tests. The only time to\n"
	"                                  turn this off is when you are just doing ACI testing.\n"
	"                                  This is true by default.\n"
	"\n"
	"--do-api=[true|false]             When this is set to true we will run our api tests as well.\n"
	"                                  This is true by default.\n"
	"\n"
	"--do-aci=[true|false]             When this is set to true we will run our aci tests as well.\n"
	"                                  This is true by default.\n"
	"\n"
	"--do-bugs=[true|false]            When this is set to true we will run our bug tests as well.\n"
	"                                  This is false by default.\n"
	"\n"
	"--do-cols=[true|false]            When this is set to true we will run our select columns tests.\n"
	"                                  This is true by default.\n"
	"\n"
	"--do-from=[true|false]            When this is set to true we will run our from clause tests.\n"
	"                                  This is true by default.\n"
	"\n"
	"--do-where=[true|false]           When this is set to true we will run our where clause tests.\n"
	"                                  This is true by default.\n"
	"\n"
	"--table-creator=TCreator          Use this if you did not supply a test-file, and want to\n"
	"                                  tell us manually which table to test.\n"
	"--table-name=TName                Use this if you did not supply a test-file.\n"
	"--host=HostName                   Use this if you did not supply a test-file.\n"
	"--port=PortNum                    Use this if you did not supply a test-file.\n"
	"--user=UserName                   Use this if you did not supply a test-file.\n"
	"--pass=UserPassword               Use this if you did not supply a test-file.\n"
	"\n"
	"--driver=DriverName               Use this to tell us which ODBC driver to use when executing\n"
	"                                  our tests.  We will use this for all tests that we execute.\n"
	"\n"
	"--dsn=OdbcDsnName                 Use this in place of host/port/user/pass/driver if you would\n"
	"                                  like us to use an existing DSN instead of manually specifying\n"
	"                                  the details on the command line.\n"
	"\n"
	"If you don't supply any options, or the options you do supply don't make sense to us,\n"
	"we'll print out this usage statement and help you understand which options you need\n"
	"to supply for things to work properly.\n";

// Keep track of our test statistics
int m_test_count = 0;
int m_test_success = 0;
int m_test_failure = 0;
int m_overall_count = 0;
int m_overall_success = 0;
int m_overall_failure = 0;

/* Information about the current test that we are running */
string m_test_category;

/* The name of our test file (if any) from which we load our tests */
string m_test_file;

/* Logging parameters to control what we output */
bool m_log_all = false;
bool m_log_test = true;
bool m_log_steps = false;
bool m_log_err = true;

/* Html output control parameters */
bool m_log_html = false;
string m_html_gid;
string m_html_group;
string m_html_desc;
string m_html_out;
FILE* m_html_outfile;

/* which tests should we run? */
bool m_do_twine = true;
bool m_do_date = true;
bool m_do_xml = true;
bool m_do_exception = true;
bool m_do_base64 = true;
bool m_do_blockingqueue = true;
bool m_do_dptr = true;
bool m_do_enex = true;
bool m_do_file = true;
bool m_do_socket = true;
bool m_do_ssocket = true;
bool m_do_hash = true;
bool m_do_http = true;
bool m_do_interval = true;
bool m_do_lock = true;
bool m_do_logfile = true;
bool m_do_logfile2 = true;
bool m_do_log = true;
bool m_do_membuf = true;
bool m_do_memptr = true;
bool m_do_msgqueue = true;
bool m_do_mutex = true;
bool m_do_objqueue = true;
bool m_do_parms = true;
bool m_do_pool = true;
bool m_do_sptr = true;
bool m_do_suvector = true;
bool m_do_thread = true;
bool m_do_timer = true;
bool m_do_tools = true;
bool m_do_zip = true;
bool m_do_bugs = true;

/** This class helps us to track test method entry/exit.
  */
class TestMethodEnEx
{
	public:
		TestMethodEnEx(const char* methodName) : 
			m_method(methodName) 
		{
			if(m_log_test){
				printf(">> %s - %s\n", m_method.c_str(), m_test_category.c_str() );
			}

			// Default to false.
			m_finish_success = false;
		}

		virtual ~TestMethodEnEx(){
			if(m_log_test){
				if(m_finish_success){
					printf("<< %s - Success - Duration - Tests Total (%d) Success (%d) Failure (%d)\n", 
						m_method.c_str(), m_test_count, m_test_success, m_test_failure );	
				} else {
					printf("<< %s - Failure - Duration - Tests Total (%d) Success (%d) Failure (%d)\n", 
						m_method.c_str(), m_test_count, m_test_success, m_test_failure );	
				}
			}
		}

		string m_method;
		bool m_finish_success;
};

/** This is a utility method for dumping sections of memory to the output for various
  * analysis
  */
void dump_mem(void* ptr, char* name, size_t prior, size_t length);

/** This is the method we use to print our total's information.
  */
void PrintTotal( bool doOverall );
void PrintAndReset( bool doOverall );

/** This is the method we use to start a given HTML group
  */
void StartHtmlGroup( void );

/** This is the method we use to end a given HTML group
  */
void EndHtmlGroup( void );

/** This macro is used at the beginning of each test method and handles a number of calls
  * for us that record the entry into a test method.
  */
#define BEGIN_TEST_METHOD( a ) \
	TestMethodEnEx tmee( a ); \
	try { \
		m_test_count ++;

/** This macros is used at the ending of each test method and handles catching exceptions
  * and recording test success or failure.
  */
#define END_TEST_METHOD \
		fflush(stdout); \
		m_test_success ++; \
		tmee.m_finish_success = true; \
	} catch (AnException& e){ \
		if(m_log_err) printf("!! AnException caught during unit test: %s\n", e.Msg() ); \
		m_test_failure ++; \
	} catch (std::exception& stde){ \
		if(m_log_err) printf("!! std::exception caught during unit test: %s\n", stde.what() ); \
		m_test_failure ++; \
	} catch (...){ \
		if(m_log_err) printf("!! Unknown exception caught during unit test\n" ); \
		m_test_failure ++; \
	}

/** This macro lets us bail out of a test early if we need to
  */
#define QUIT_TEST_METHOD \
		m_test_success ++; \
		tmee.m_finish_success = true; \
		return;

/** Tests if two items are equals using C++ == comparison.
  */
#define ASSERT_EQUALS(a,b,c) \
	if( (a) == (b) ) {\
	} else { \
		if(m_log_err) printf( "!! %s\n", c); \
		m_test_failure++; \
		return; \
	}

/** Tests if two items are not equal using C++ != comparison.
  */
#define ASSERT_NOTEQUALS(a,b,c) \
	if( (a) != (b) ) {\
	} else { \
		if(m_log_err) printf( "!! %s\n", c); \
		m_test_failure++; \
		return; \
	}

/** Tests if a given value is true.
  */
#define ASSERT_TRUE(a,b) \
	if( (a) ){\
	} else { \
		if(m_log_err) printf( "!! %s\n", b); \
		m_test_failure++; \
		return; \
	}

/** Tests if a given value is false.
  */
#define ASSERT_FALSE(a,b) \
	if( !(a) ){\
	} else { \
		if(m_log_err) printf( "!! %s\n", b); \
		m_test_failure++; \
		return; \
	}

/** Tests if a given value is not null.
  */
#define ASSERT_NOTNULL(a,b) \
	if( (a) == NULL ){\
		if(m_log_err) printf( "!! %s\n", b); \
		m_test_failure++; \
		return; \
	}

/** Tests if a given value is null.
  */
#define ASSERT_NULL(a,b) \
	if( (a) != NULL ){\
		if(m_log_err) printf( "!! %s\n", b); \
		m_test_failure++; \
		return; \
	}

/** Tests to verify that a given expression will trigger an exception
  */
#define ASSERT_EXCEPTION(a, b) \
	try { \
		(a); \
		if(m_log_err) printf( "!! Exception Expected: %s\n", b); \
		m_test_failure++; \
		return; \
	} catch (AnException& ) { \
	} 

#include "SLibTest.h"

void LoopTestFile();
vector<string> ReadTestFile();
bool isBlankLine(const string& line);
void CheckAndRun();
void RunSLibTests();
void ParseArgs(int argc, char** argv);
void SetLogParm(const string& arg, bool* logParm);
string& rtrim(string& arg);

int main(int argc, char** argv)
{
	/*
	if(argc == 1){
		printf(usage, argv[0]);
		return EXIT_FAILURE;
	}
	*/
	ParseArgs(argc, argv);

	if(m_html_out.length() != 0){
		m_html_outfile = fopen( m_html_out.c_str(), "w" );
	} else {
		m_html_outfile = stdout;
	}

	if(m_test_file.size() != 0){
		LoopTestFile(); // loop through all of the tables in the given test file
	} else {
		if(m_log_html && m_html_group.length() != 0){
			StartHtmlGroup();
		}
		RunSLibTests(); // just run the one table that the user has provided
	}

	PrintTotal( true ); // print the overall totals

	if(m_log_html){
		EndHtmlGroup();
	}

	if(m_html_out.length() != 0){
		fclose( m_html_outfile );
	} else {
		// no reason to close stdout.
	}

	return EXIT_SUCCESS;
}

void ParseArgs(int argc, char** argv)
{
	for(int i = 1; i < argc; i++){
		string arg = argv[i];

		if(arg.find("--test-file=") == 0) {
			m_test_file = arg.substr(12);
		} else if(arg.find("--log-all=") == 0){
			SetLogParm( arg, &m_log_all );			
		} else if(arg.find("--log-test=") == 0){
			SetLogParm( arg, &m_log_test );			
		} else if(arg.find("--log-steps=") == 0){
			SetLogParm( arg, &m_log_steps );			
		} else if(arg.find("--log-err=") == 0){
			SetLogParm( arg, &m_log_err );			
		} else if(arg.find("--log-html=") == 0){
			SetLogParm( arg, &m_log_html );			
		} else if(arg.find("--do-twine=") == 0){
			SetLogParm( arg, &m_do_twine);			
		} else if(arg.find("--do-date=") == 0){
			SetLogParm( arg, &m_do_date);			
		} else if(arg.find("--do-xml=") == 0){
			SetLogParm( arg, &m_do_xml);			
		} else if(arg.find("--do-exception=") == 0){
			SetLogParm( arg, &m_do_exception);			
		} else if(arg.find("--do-base64=") == 0){
			SetLogParm( arg, &m_do_base64);			
		} else if(arg.find("--do-blockingqueue=") == 0){
			SetLogParm( arg, &m_do_blockingqueue);			
		} else if(arg.find("--do-dptr=") == 0){
			SetLogParm( arg, &m_do_dptr);			
		} else if(arg.find("--html-group=") == 0){
			m_html_group = arg.substr(13);
		} else if(arg.find("--html-gid=") == 0){
			m_html_gid = arg.substr(11);
		} else if(arg.find("--html-desc=") == 0){
			m_html_desc = arg.substr(12);
		} else if(arg.find("--html-out=") == 0){
			m_html_out = arg.substr(11);
		} else {
			printf("Unknown argument \"%s\"\n", arg.c_str() );
			printf(usage, argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	// Update logging parms if log-all is true:
	if(m_log_all){
		m_log_test = true;
		m_log_steps = true;
		m_log_err = true;
	}

}

void SetLogParm(const string& arg, bool* logParm)
{
	size_t eq = arg.find("=");
	eq++;
	string val = arg.substr(eq);
	if(val == "true" ||
		val == "True" ||
		val == "TRUE" ||
		val == "yes" ||
		val == "Yes" ||
		val == "YES" ||
		val == "on" ||
		val == "On" ||
		val == "ON" ||
		val == "1"
	){
		*logParm = true;
	} else {
		*logParm = false;
	}
}

void LoopTestFile()
{
	vector<string> lines = ReadTestFile();
	bool readyToRun = false;

	for(size_t i = 0; i < lines.size(); i++){
		if(lines[i][0] == '#' || isBlankLine(lines[i])) continue; // skip comments and blank lines
		if(lines[i][0] == '['){
			// This begins a new test.  Run any test we have been reading first.
			if(readyToRun){
				CheckAndRun();
			}
			
			// Pick up the table creator and table name:
			//string stripped = lines[i].substr(1, lines[i].size() - 2);
			//size_t dot = stripped.find('.');
			//m_test_tcreator = stripped.substr(0, dot);
			//m_test_tname = stripped.substr(dot + 1);
			//readyToRun = true; // ready to run a test when we get to the next def, or EOF

		} else if(lines[i].find("log-all=") == 0) SetLogParm( lines[i], &m_log_all );			
		else if(lines[i].find("log-test=") == 0)  SetLogParm( lines[i], &m_log_test );			
		else if(lines[i].find("log-steps=") == 0) SetLogParm( lines[i], &m_log_steps );			
		else if(lines[i].find("log-err=") == 0)   SetLogParm( lines[i], &m_log_err );			
		else if(lines[i].find("log-html=") == 0)  SetLogParm( lines[i], &m_log_html );			
		else if(lines[i].find("do-twine=") == 0)  SetLogParm( lines[i], &m_do_twine);			
		else if(lines[i].find("do-date=") == 0)   SetLogParm( lines[i], &m_do_date);			
		else if(lines[i].find("do-xml=") == 0)    SetLogParm( lines[i], &m_do_xml);			
		else if(lines[i].find("html-desc=") == 0) m_html_desc = lines[i].substr(10);
		else if(lines[i].find("html-out=") == 0)  m_html_out = lines[i].substr(9);
		else if(lines[i].find("html-gid=") == 0)  m_html_gid = lines[i].substr(9);
		else if(lines[i].find("html-group=") == 0){
			string tmp = lines[i].substr(11);
			if(tmp != m_html_group && m_log_html){
				if(m_html_group.length() == 0){
					// First time through just start the new html group
					m_html_group = tmp;
					StartHtmlGroup();
				} else {
					// If we've been here before, then end the html group first
					EndHtmlGroup();
					// then start a new one
					m_html_group = tmp;
					StartHtmlGroup();
				}
			}
			m_html_group = tmp;
		}
	}
	
	// At the end of the file, also run any test that we have been reading
	if(readyToRun){
		CheckAndRun();
	}
}

void CheckAndRun()
{

	// Update logging parms if log-all is true:
	if(m_log_all){
		m_log_test = true;
		m_log_steps = true;
		m_log_err = true;
	}

	RunSLibTests();
}

vector<string> ReadTestFile()
{
	// Open the file for reading, and read all contents into memory
	FILE* fp = fopen(m_test_file.c_str(), "rb");
	if(fp == NULL){
		printf("!! Error opening test file (%s) - either does not exist or bad perms.\n",
			m_test_file.c_str() );
		exit(EXIT_FAILURE);
	}

#ifdef _WIN32
	struct _stat fpStat;
	_fstat(_fileno(fp), &fpStat);
#else
	struct stat fpStat;
	fstat(fileno(fp), &fpStat);
#endif

	char* contents = (char*)malloc(fpStat.st_size + 10);
	if(contents == NULL){
		printf("!! Error allocating memory to hold/read test file (%s)\n",
			m_test_file.c_str() );
		exit(EXIT_FAILURE);
	}
	memset(contents, 0, fpStat.st_size + 10);

	size_t ret = fread(contents, fpStat.st_size, 1, fp);
	if(ret != 1){
		printf("!! Error reading contents of test file (%s) into memory.\n",
			m_test_file.c_str() );
		exit(EXIT_FAILURE);
	}

	fclose( fp );

	// Now move the contents over into string so we can more easily manipulate them:
	string fContents = contents;

	// Free the buffer, so we don't use too much memory:
	memset(contents, 0, fpStat.st_size + 10);
	free(contents);

	// Split up the contents by lines:
	vector<string> lines;
	size_t idx1, idx2;
	idx1 = 0;
	idx2 = fContents.find('\n');
	if(idx2 == string::npos){
		lines.push_back( rtrim(fContents) ); // just one string, no splits.
		return lines;
	}
	while(idx2 != string::npos){
		string substr = fContents.substr( idx1, idx2-idx1 );
		lines.push_back( rtrim(substr) );
		idx1 = idx2 + 1;
		idx2 = fContents.find('\n', idx1);
	}
	if(idx1 < fContents.size()){
		string substr = fContents.substr( idx1 );
		lines.push_back( rtrim(substr) );
	}
	return lines;
}

bool isBlankLine(const string& line)
{
	for(size_t i = 0; i < line.size(); i++){
		if(!isspace(line[i]) && line[i] != '\n' && line[i] != '\r' ){
			// not a space, newline or carrage return.  It's not blank.
			return false;
		}
	}
	return true; // everything is space, newline or return
}

string& rtrim(string& arg)
{
	while( isspace(arg[arg.size() - 1]) || arg[arg.size() - 1] == '\n' || arg[arg.size() - 1] == '\r' ){
		arg.resize( arg.size() - 1 ); // chop off one character from the end.
	}

	return arg;
}

void StartHtmlGroup()
{
	if(!m_log_html) return; // Bail out if we're not doing html logging
	fprintf(m_html_outfile,
		"<section id=\"%s\">\n"
		"\t<h3>%s</h3>\n"
		"\t<h4><a href=\"#/0\">Jump to Main</a>&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"#/odbc-summary\">Jump to ODBC Summary</a></h4>\n"
		"\t<table style=\"font-size=14pt;\">\n"
		"\t\t<tr>\n"
		"\t\t\t<th>Area</th>\n"
		"\t\t\t<th>Table</th>\n"
		"\t\t\t<th>Total</th>\n"
		"\t\t\t<th>Success</th>\n"
		"\t\t\t<th>Failure</th>\n"
		"\t\t</tr>\n",
		m_html_gid.c_str(),
		m_html_group.c_str()
	);

}

void EndHtmlGroup()
{
	if(!m_log_html) return; // Bail out if we're not doing html logging
	fprintf(m_html_outfile,
		"\t</table>\n"
		"</section>\n"
	);
}

void PrintAndReset( bool doOverall )
{
	PrintTotal( doOverall );
	m_overall_count += m_test_count;
	m_overall_success += m_test_success;
	m_overall_failure += m_test_failure;
	m_test_count = 0;
	m_test_success = 0;
	m_test_failure = 0;

}

void PrintTotal( bool doOverall )
{
	if(doOverall){
		if(m_log_html){
			// When outputting html format, we don't print the overall totals
		} else {
			printf("== OVERALL Total (%d) Success (%d) Failure (%d)\n",
				m_overall_count, m_overall_success, m_overall_failure 
			);	
		}
	} else {
		if(m_log_html){
			// Use an html format for the output
			fprintf(m_html_outfile, "\t\t<tr>\n"
				"\t\t\t<td class=TestDesc>%s</td>\n"
				"\t\t\t<td class=TestName>%s</td>\n"
				"\t\t\t<td class=Total>%d</td>\n"
				"\t\t\t<td class=Success>%d</td>\n"
				"\t\t\t<td class=Failure>%d</td>\n"
				"\t\t</tr>\n",
				m_html_desc.c_str(),
				m_test_category.c_str(),
				m_test_count,
				m_test_success,
				m_test_failure
			);
		} else {
			printf("** %s Total (%d) Success (%d) Failure (%d)\n",
				m_test_category.c_str(), m_test_count, m_test_success, m_test_failure 
			);	
		}
	}

}

void RunSLibTests()
{
	// Invoke all of our test methods here directly:
	if(m_do_twine){
		TestTwine000();
		PrintAndReset( false );
	}

	if(m_do_date){
		TestDate000();
		PrintAndReset( false );
	}

	if(m_do_bugs){
		m_test_category = "SLib::Bugs Testing";
		Bug0001TwineSplit();
		PrintAndReset( false );
	}

	return;
}

/* ************************************************************************************ */
/* Here is where we include all of our testing code.  Include it directly instead of    */
/* compiling separately.                                                                */
/* ************************************************************************************ */

// Twine Tests
#include "twine/TestTwine000.cpp"

// Date Tests
#include "date/TestDate000.cpp"

// Bug Tests
#include "bugs/Bug0001TwineSplit.cpp"

/* ************************************************************************************ */
/* Beyond here are various utility methods used by everyone.                            */
/* ************************************************************************************ */

void dump_mem(void* ptr, char* name, size_t prior, size_t length)
{
	if(!m_log_steps) return; // skip dumping memory if log steps is not on.

	if(ptr == NULL){
		printf("++ dump_mem: %s - NULL Pointer\n", name );
		return;
	}

	char* bptr = (char*)ptr; // so that indexing works.
	char* aptr = bptr - prior;
	char* cptr = bptr + length;
	printf("++ dump_mem: %s - [%p - _%p_ - %p)\n", name, aptr, bptr, cptr );
	printf("++         +00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n" );
	while( aptr < cptr ){
		printf("++ %p ", aptr);
		for(int i = 0; i < 16 && aptr < cptr; i++, aptr++){
			printf("%.2X ", (unsigned)(unsigned char)aptr[ 0 ] );
		}
		printf("\n");
	}

}
