
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <time.h>
#ifdef _WIN32
#	include <sys/types.h>
#	include <sys/timeb.h>
#else
#	include <sys/time.h>
#endif

#include "Thread.h"
#include "Log.h"
#include "twine.h"
#include "LogMsg.h"
#include "MsgQueue.h"

using namespace SLib;

// Some variables necessary to handle the logging.
static FILE *logout = stdout;
static int loginit = 0;
static bool panicon = true;
static bool erroron = true;
static bool warnon = false;
static bool infoon = false;
static bool debugon = false;
static bool traceon = false;
static bool sqltraceon = false;
static bool lazy_on = false;

static MsgQueue<LogMsg*>* log_queue = NULL;

void Log::TimeStamp(twine& t)
{
	t.reserve(64);
	t.erase();
	char *tmp_pict = t.data();
	char local_tmp[64];

#ifdef _WIN32
	struct timeb tmp_tv;
	ftime(&tmp_tv);

	memset(local_tmp, 0, 32);
	strftime(local_tmp, 32, "%Y/%m/%d %H:%M:%S", localtime(&(tmp_tv.time)));
	memset(tmp_pict, 0, 64);
	sprintf(tmp_pict, "%s.%.3d", local_tmp, (int)tmp_tv.millitm);
#else
	struct timeval tmp_tv;
	gettimeofday(&tmp_tv, NULL);
	memset(local_tmp, 0, 64);
	strftime(local_tmp, 64, "%Y/%m/%d %H:%M:%S", localtime(&(tmp_tv.tv_sec)));
	sprintf(tmp_pict, "%s.%.6d", local_tmp, (int)tmp_tv.tv_usec);
#endif

	t.check_size();
}

void Log::Init(const char *filename)
{
	FILE *tmp;

	if(loginit){
		// switch streams here so that the logout pointer is
		// never undefined.
		tmp = logout;
		logout = stdout;

		fflush(tmp);
		fclose(tmp);
	}

	if(strcmp(filename, "stdout") == 0){
		logout = stdout;
		loginit = 0;
		return;
	}

	if(strcmp(filename, "stderr") == 0){
		logout = stderr;
		loginit = 0;
		return;
	}		

	tmp = fopen(filename, "w");
	if(tmp == NULL){
		ERRORL(FL, "Error opening log file (%s) for output", filename);
	} else {
		logout = tmp;
		INFO(FL, "New logfile (%s) opened", filename);
		loginit = 1;
	}
}
	
void Log::Fini(void)
{
	Init("stdout");
}

FILE *Log::FileHandle(void)
{
	return logout;
}

void Log::SetLazy(bool onoff)
{
	lazy_on = onoff;
}

bool Log::LazyOn(void)
{
	return lazy_on;
}

MsgQueue<LogMsg*>& Log::GetLogQueue(void)
{
	if(log_queue == NULL){
		log_queue = new MsgQueue<LogMsg*>();
	}
	return *log_queue;
}

void Log::Persist(LogMsg* lm)
{
	if(lazy_on){
		GetLogQueue().AddMsg(lm);
	} else {
		char local_tmp[32];
		memset(local_tmp, 0, 32);

#ifdef _WIN32
		strftime(local_tmp, 32, "%Y/%m/%d %H:%M:%S",
			localtime(&(lm->timestamp.time)));
		fprintf(logout, "%s.%.3d|%ld|%s|%d|%d|%s\n", 
			local_tmp, (int)lm->timestamp.millitm,
			lm->tid,
			lm->file(),
			lm->line,
			lm->channel,
			lm->msg());
#else
		strftime(local_tmp, 32, "%Y/%m/%d %H:%M:%S",
			localtime(&(lm->timestamp.tv_sec)));
		fprintf(logout, "%s.%.6d|%ld|%s|%d|%d|%s\n", 
			local_tmp, (int)lm->timestamp.tv_usec,
			(intptr_t)lm->tid,
			lm->file(),
			lm->line,
			lm->channel,
			lm->msg());
#endif
		delete lm;
	}
}
		


void Log::SetPanic(bool	onoff)
{
	panicon = onoff;
}

bool Log::PanicOn(void)
{
	return panicon;
}

void Log::Panic(const char *file, int line, const char *msg, ...)
{
	if(!panicon) return;

	LogMsg* lm = new LogMsg(file, line);

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 0; // Panic
	Persist(lm);
}	

void Log::Panic(const twine& appSession, const char *file, int line, const char *msg, ...)
{
	if(!panicon) return;

	LogMsg* lm = new LogMsg(file, line);
	lm->appSession = appSession;

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 0; // Panic
	Persist(lm);
}	

void Log::SetError(bool	onoff)
{
	erroron = onoff;
}

bool Log::ErrorOn(void)
{
	return erroron;
}

void Log::Error(const char *file, int line, const char *msg, ...)
{
	if(!erroron) return;

	LogMsg* lm = new LogMsg(file, line);

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 1; // Error
	Persist(lm);
}	

void Log::Error(const twine& appSession, const char *file, int line, const char *msg, ...)
{
	if(!erroron) return;

	LogMsg* lm = new LogMsg(file, line);
	lm->appSession = appSession;

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 1; // Error
	Persist(lm);
}	

void Log::SetWarn(bool	onoff)
{
	warnon = onoff;
}

bool Log::WarnOn(void)
{
	return warnon;
}

void Log::Warn(const char *file, int line, const char *msg, ...)
{
	if(!warnon) return;

	LogMsg* lm = new LogMsg(file, line);

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 2; // Warn
	Persist(lm);
}	

void Log::Warn(const twine& appSession, const char *file, int line, const char *msg, ...)
{
	if(!warnon) return;

	LogMsg* lm = new LogMsg(file, line);
	lm->appSession = appSession;

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 2; // Warn
	Persist(lm);
}	

void Log::SetInfo(bool	onoff)
{
	infoon = onoff;
}

bool Log::InfoOn(void)
{
	return infoon;
}

void Log::Info(const char *file, int line, const char *msg, ...)
{
	if(!infoon) return;

	LogMsg* lm = new LogMsg(file, line);

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 3; // Info
	Persist(lm);
}	

void Log::Info(const twine& appSession, const char *file, int line, const char *msg, ...)
{
	if(!infoon) return;

	LogMsg* lm = new LogMsg(file, line);
	lm->appSession = appSession;

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 3; // Info
	Persist(lm);
}	

void Log::SetDebug(bool	onoff)
{
	debugon = onoff;
}

bool Log::DebugOn(void)
{
	return debugon;
}

void Log::Debug(const char *file, int line, const char *msg, ...)
{
	if(!debugon) return;

	LogMsg* lm = new LogMsg(file, line);

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 4; // Debug
	Persist(lm);
}	

void Log::Debug(const twine& appSession, const char *file, int line, const char *msg, ...)
{
	if(!debugon) return;

	LogMsg* lm = new LogMsg(file, line);
	lm->appSession = appSession;

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 4; // Debug
	Persist(lm);
}	

void Log::SetTrace(bool	onoff)
{
	traceon = onoff;
}

bool Log::TraceOn(void)
{
	return traceon;
}

void Log::Trace(const char *file, int line, const char *msg, ...)
{
	if(!traceon) return;

	LogMsg* lm = new LogMsg(file, line);

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 5; // Trace

	Persist(lm);
}	

void Log::Trace(const twine& appSession, const char *file, int line, const char *msg, ...)
{
	if(!traceon) return;

	LogMsg* lm = new LogMsg(file, line);
	lm->appSession = appSession;

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 5; // Trace

	Persist(lm);
}	

void Log::SetSqlTrace(bool onoff)
{
	sqltraceon = onoff;
}

bool Log::SqlTraceOn(void)
{
	return sqltraceon;
}

void Log::SqlTrace(const char *file, int line, const char *msg, ...)
{
	if(!sqltraceon) return;

	LogMsg* lm = new LogMsg(file, line);

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 6; // SqlTrace
	Persist(lm);
}	

void Log::SqlTrace(const twine& appSession, const char *file, int line, const char *msg, ...)
{
	if(!sqltraceon) return;

	LogMsg* lm = new LogMsg(file, line);
	lm->appSession = appSession;

	va_list ap;
	va_start(ap, msg);
	lm->msg.format(msg, ap);
	if(lm->msg.length() == strlen(msg)){
		lm->msg_static = true;
	}
	va_end(ap);
	
	lm->channel = 6; // SqlTrace
	Persist(lm);
}	
