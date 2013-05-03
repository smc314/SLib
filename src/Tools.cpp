
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

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#	include <sys/types.h>
#	include <sys/wait.h>
#	include <unistd.h>
#	include <ctype.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "AnException.h"
#include "Tools.h"
#include "twine.h"
#include "Log.h"
#include "MemBuf.h"

using namespace SLib;


static int e2a_hex[] =
{
	0x00,0x01,0x02,0x03,0x9C,0x09,0x86,0x7F,0x97,0x8D,0x8E,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x9D,0x85,0x08,0x87,0x18,0x19,0x92,0x8F,0x1C,0x1D,0x1E,0x1F,
	0x80,0x81,0x82,0x83,0x84,0x0A,0x17,0x1B,0x88,0x89,0x8A,0x8B,0x8C,0x05,0x06,0x07,
	0x90,0x91,0x16,0x93,0x94,0x95,0x96,0x04,0x98,0x99,0x9A,0x9B,0x14,0x15,0x9E,0x1A,
	0x20,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xD5,0x2E,0x3C,0x28,0x2B,0x7C,
	0x26,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0x21,0x24,0x2A,0x29,0x3B,0x5E,
	0x2D,0x2F,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xE5,0x2C,0x25,0x5F,0x3E,0x3F,
	0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0x60,0x3A,0x23,0x40,0x27,0x3D,0x22,
	0xC3,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,
	0xCA,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,
	0xD1,0x7E,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0xD2,0xD3,0xD4,0x5B,0xD6,0xD7,
	0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,0x5D,0xE6,0xE7,
	0x7B,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,
	0x7D,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,
	0x5C,0x9F,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
};

void Tools::sleep(int mills)
{
#ifdef _WIN32
	Sleep(mills / 1000);
#else
	struct timeval tv;
	tv.tv_sec = mills / 1000000;
	tv.tv_usec = mills % 1000000;
	select(0, NULL, NULL, NULL, &tv);
#endif
}

void Tools::rtrim(char *s)
{
	size_t i;
	i = strlen(s) -1;
	while(s[i] == ' '){
		s[i] = '\0';
		i --;
	}
}

void Tools::char_free(char *s)
{
	free(s);
}

int Tools::rand(int max, int min)
{
	int ret;
	ret=min+(int) ( ((float)max-min) * (::rand()/(RAND_MAX+1.0)) );	
	return ret;
}

twine Tools::hexDump(void* ptr, char* name, size_t prior, size_t length, bool asciiPrint, bool ebcdicPrint)
{
	twine tmp;
	twine tmpa;
	twine tmpe;
	twine ret;
	int i;

	if(ptr == NULL){
		return ret; 
	}

	char* bptr = (char*)ptr; // so that indexing works
	char* aptr = bptr - prior;
	char* cptr = bptr + length;
	
	ret += "hexDump: ";	
	ret += name;
	ret += "\n";
	while(aptr < cptr ){
		tmpa = "";
		tmpe = "";
		for(i = 0; i < 16 && aptr < cptr; i++, aptr++ ){
			tmp.format("%.2X ", (unsigned)(unsigned char)aptr[ 0 ] );
			ret += tmp;
			if(asciiPrint){
				if(isalnum(aptr[0]) || ispunct(aptr[0]) || isspace(aptr[0]) ){
					tmpa += aptr[0];
				} else {
					tmpa += '.';
				}
			}
			if(ebcdicPrint){
				char asciiChar = (char)e2a_hex[ (int)(aptr[0]) & 0xFF ];
				if(isalnum(asciiChar) || ispunct(asciiChar) || isspace(asciiChar) ){
					tmpe += asciiChar;
				} else {
					tmpe += '.';
				}
			}
		}
		if(i < 16){ 
			// Padd out the hex display area
			for(; i < 16; i++){
				ret += "   ";
				if(asciiPrint){
					tmpa += " ";
				}
				if(ebcdicPrint){
					tmpe += " ";
				}
			}
		}
		if(asciiPrint){
			ret += " " + tmpa;
		}
		if(ebcdicPrint){
			ret += " " + tmpe;
		}
		ret += "\n";
	}

	return ret;
}

vector<twine> Tools::RunCommand(const twine& cmd, int* exitCode)
{
	return RunCommand( cmd, "", exitCode );
}

vector<twine> Tools::RunCommand(const twine& cmd, const twine& inputString, int* exitCode)
{
	// We parse up the command to create cmd + args.
	vector<twine> tokens = cmd.tokenize( TWINE_WS );

	// First one is the command:
	twine runCommand = tokens[ 0 ];

	// The rest is our array of arguments:
	vector<twine> args;
	for(size_t i = 1; i < tokens.size(); i++){
		args.push_back( tokens[i] );
	}

	return RunCommand( runCommand, args, inputString, exitCode );
}

#ifdef _WIN32
// This is how we do this on windows:
vector<twine> Tools::RunCommand(const twine& cmd, vector<twine> args, const twine& inputString, int* exitCode)
{
	HANDLE proc_writepipe = INVALID_HANDLE_VALUE;
	HANDLE writepipe = INVALID_HANDLE_VALUE;
	HANDLE hRP = INVALID_HANDLE_VALUE;
	PSECURITY_DESCRIPTOR pSD;
	SECURITY_ATTRIBUTES sa;
	DWORD dwCreateFlags;
	DWORD uLen;
	STARTUPINFO stStartupInfo;
	PROCESS_INFORMATION stProcessInfo;
	DWORD lastErr;
	BOOL rc, bRC;
		
	// Open a pipe whose handle will be given as stdout to the invoked program
	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc( LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH );
	if(pSD == NULL){
		throw AnException(0, FL, "Error creating security descriptor.");
	}
	if(!InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION )){
		LocalFree( (HLOCAL)pSD );
		throw AnException(0, FL, "Error initializing security descriptor.");
	}
	if(!SetSecurityDescriptorDacl( pSD, TRUE, (PACL)NULL, TRUE )){
		LocalFree( (HLOCAL)pSD );
		throw AnException(0, FL, "Error setting security descriptor Dacl.");
	}
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD;

	// Create the pipe as non-inheritable, then dup the pipe as inheritable to pass to proc
	sa.bInheritHandle = FALSE;
	rc = CreatePipe( &hRP, &writepipe, &sa, 32000L );
	if(!rc){
		LocalFree( (HLOCAL)pSD );
		throw AnException(0, FL, "Error creating pipe for child process.");
	}
	rc = DuplicateHandle( GetCurrentProcess(), writepipe, GetCurrentProcess(), &proc_writepipe, 0, TRUE,
			DUPLICATE_SAME_ACCESS );
	if(!rc){
		CloseHandle( writepipe );
		LocalFree( (HLOCAL)pSD );
		throw AnException(0, FL, "Error duplicating pipe for child process.");
	}
	CloseHandle( writepipe ); // we no longer need this - it's for the child process.
	writepipe = INVALID_HANDLE_VALUE; // just in case - so we don't use it by accident

	// Prepare Process info for the child process:
	memset( (void*)&stStartupInfo, 0, sizeof(stStartupInfo) );
	stStartupInfo.cb = sizeof( stStartupInfo );
	stStartupInfo.lpReserved = NULL;
	stStartupInfo.lpDesktop = NULL;
	dwCreateFlags = NORMAL_PRIORITY_CLASS;
	stStartupInfo.dwFlags = STARTF_USESTDHANDLES;
	stStartupInfo.hStdInput = NULL; // FIXME - update this so we can write to stdin
	stStartupInfo.hStdOutput = proc_writepipe;
	stStartupInfo.hStdError = GetStdHandle( STD_ERROR_HANDLE );

	// Prepare command line arguments and start the proc
	twine cmdLine = cmd;
	for(size_t i = 0; i < args.size(); i++){
		cmdLine.append( " " );
		cmdLine.append( args[ i ] );
	}
	rc = CreateProcess( cmd(), cmdLine(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL,
		&stStartupInfo, &stProcessInfo );
	if(!rc){
		CloseHandle( proc_writepipe );
		LocalFree( (HLOCAL)pSD );
		throw AnException(0, FL, "Error calling CreateProcess for child process: %d", GetLastError() );
	}

	// Close our handle for the pipe passed to the proc
	CloseHandle( proc_writepipe );
	proc_writepipe = INVALID_HANDLE_VALUE; // just in case - so we don't use it by accident

	// Read the output of the child process here until there is no more.
	twine output;
	MemBuf readbuffer; readbuffer.reserve( 4096 );
	ssize_t readRet = 0;
	while(1){
		bRC = ReadFile( hRP, readbuffer.data(), readbuffer.size(), &uLen, NULL );
		if(uLen == 0) break; // end of the file
		if(!bRC){
			CloseHandle( hRP );
			LocalFree( (HLOCAL)pSD );
			throw AnException(0, FL, "Error reading from child process output" );
		}

		output.append( readbuffer.data(), readRet);
	}

	// Close our handle for the read pipe
	CloseHandle( hRP );
	hRP = INVALID_HANDLE_VALUE; // just in case - so we don't use it by accident

	// Wait for the process to finish
	do {
		bRC = GetExitCodeProcess( stProcessInfo.hProcess, exitCode );
		if(!bRC){
			LocalFree( (HLOCAL)pSD );
			throw AnException(0, FL, "Error getting exit code for child process." );
		}
		if(*exitCode == STILL_ACTIVE){
			Sleep( 200 );
		}
	} while( *exitCode == STILL_ACTIVE );
			

	// Split up the output on newlines:
	vector<twine> ret = output.split('\n');

	// Trim each line
	for(size_t i = 0; i < ret.size(); i++){
		ret[ i ].rtrim(); // trim trailing spaces/tabs/\r/\n chars
		if(ret[ i ].size() == 0){
			// remove any completely empty lines
			ret.erase( ret.begin() + i );
			i --; // keep the loop counter in sync.
		}
	}

	// Free up our pSD
	LocalFree( (HLOCAL)pSD );

	// Return the outputs
	return ret;
}
#else
// this is how we do this on unix:
#define PIPE_READ 0
#define PIPE_WRITE 1
vector<twine> Tools::RunCommand(const twine& cmd, vector<twine> args, const twine& inputString, int* exitCode)
{
	int aStdinPipe[2];
	int aStdoutPipe[2];
	pid_t nChild, w;
	char nChar;
	int nResult;

	if(pipe(aStdinPipe) < 0){
		throw AnException(0, FL, "Error allocating a pipe for child input redirect.");
	}
	if(pipe(aStdoutPipe) < 0){
		// close the stdin pipe first
		close(aStdinPipe[PIPE_READ]);
		close(aStdinPipe[PIPE_WRITE]);
		throw AnException(0, FL, "Error allocating a pipe for child output redirect.");
	}

	nChild = fork();
	if(nChild < 0){
		// Error from the fork function - close pipes and abort.
		close(aStdinPipe[PIPE_READ]);
		close(aStdinPipe[PIPE_WRITE]);
		close(aStdoutPipe[PIPE_READ]);
		close(aStdoutPipe[PIPE_WRITE]);
		throw AnException(0, FL, "Error calling fork() to create a new child process.");

	} else if(0 == nChild){
		// Child process continues here
		
		// redirect stdin
		if(dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1){
			ERRORL(FL, "Error redirecting stdin within the child process - exiting.");
			exit(EXIT_FAILURE); // We are the child process, so just abort at this point.
		}

		// redirect stdout
		if(dup2(aStdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1){
			ERRORL(FL, "Error redirecting stdout within the child process - exiting.");
			exit(EXIT_FAILURE); // we are the child process, so just abort at this point.
		}

		// redirect stderr
		if(dup2(aStdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1){
			ERRORL(FL, "Error redirecting stderr within the child process - exiting.");
			exit(EXIT_FAILURE); // we are the child process, so just abort at this point.
		}

		// All of these are for use by the parent only
		close(aStdinPipe[PIPE_READ]);
		close(aStdinPipe[PIPE_WRITE]);
		close(aStdoutPipe[PIPE_READ]);
		close(aStdoutPipe[PIPE_WRITE]);

		// Prepare the arguments array:
		char* argv[ args.size() + 2];
		argv[ 0 ] = (char*)cmd.c_str(); // first argument is always the command to run
		for(size_t i = 0; i < args.size(); i++){
			argv[ i + 1 ] = args[i].data();
		}
		argv[ args.size() + 1 ] = NULL; // last one is null to signal the end of the array.

		// Run the child process image
		nResult = execvp(cmd(), argv);

		// If we get here at all an error occurred, but we are in the child process,
		// so we just exit.
		ERRORL(FL, "Error executing child process command: %s", cmd() );
		exit( nResult );

	} else if(nChild > 0){
		// Parent process continues here

		// Close unused file descriptors, these are for child only
		close(aStdinPipe[PIPE_READ]);
		close(aStdoutPipe[PIPE_WRITE]);

		// Send the input to the new child process on stdin:
		if(inputString.length() != 0){
			write(aStdinPipe[PIPE_WRITE], inputString(), inputString.length() );
		}

		// Read the output of the child process here until there is no more.
		twine output;
		MemBuf readbuffer; readbuffer.reserve( 4096 );
		ssize_t readRet = 0;
		while((readRet = read(aStdoutPipe[PIPE_READ], readbuffer.data(), readbuffer.size())) != 0){
			output.append( readbuffer.data(), readRet);
		}

		// Now wait for the child exit code:
		do {
			w = waitpid( nChild, &nResult, WUNTRACED | WCONTINUED);
			if(w == -1){
				throw AnException(0, FL, "Error waiting for child process to exit.");
			}
			if( WIFEXITED(nResult) ){
				*exitCode = (int) WEXITSTATUS( nResult );
			} else if( WIFSIGNALED(nResult) ){
				// child process was killed by a signal - use WTERMSIG(nResult) if you want to find out which one.
				*exitCode = (int) EXIT_FAILURE; 
			} else if( WIFSTOPPED(nResult) ){
				// child process has been suspended/stopped by a signal - keep watching until it
				// continues or exits
			} else if( WIFCONTINUED(nResult) ){
				// child process has been continued after being suspended - keep watching
				// until it exits
			}
		} while( !WIFEXITED(nResult) && !WIFSIGNALED(nResult) );

		// When we are done talking with the child we close these
		close( aStdinPipe[PIPE_WRITE] );
		close( aStdoutPipe[PIPE_READ] );

		// Split up the output on newlines:
		vector<twine> ret = output.split('\n');

		// Trim each line
		for(size_t i = 0; i < ret.size(); i++){
			ret[ i ].rtrim(); // trim trailing spaces/tabs/\r/\n chars
			if(ret[ i ].size() == 0){
				// remove any completely empty lines
				ret.erase( ret.begin() + i );
				i --; // keep the loop counter in sync.
			}
		}

		// Return the outputs
		return ret;
	}

}
#endif // _WIN32/Unix for RunCommand
