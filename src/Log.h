#ifndef LOG_H
#define LOG_H
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
#	ifndef DLLEXPORT
#		define DLLEXPORT __declspec(dllexport)
#       endif
#else
#	define DLLEXPORT 
#endif

#include <stdio.h>
#include "twine.h"
#include "MsgQueue.h"
#include "LogMsg.h"

namespace SLib {

/**
  * @memo This class encapsulates our logging functions.
  * @doc  This class encapsulates our logging functions.  All of the logging
  *       requests will pass through here for filtering, formatting and
  *       output.
  * @author Steven M. Cherry
  * @version $Revision: 1.3 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT Log {
	
	public:
	
		/**
		  * This method handles the initialization or
		  * or re-initialization of the log output.  If the
		  * logs are currently open, then they will be flushed,
		  * closed, and then re-opened to the new file name.
		  * <P>
		  * If the filename parameter matches either "stdout"
		  * or "stderr" then output will be directed to those
		  * standard streams.
		  */
		static void Init(const char *filename);
		static void Init(twine* filename){
			Init(filename->c_str());
		}
		static void Init(twine& filename){
			Init(filename.c_str());
		}

		/**
		  * This method turns on lazy logging by sending all
		  * log messages received to our log queue.  Nothing
		  * is written to disk.  If you use this, then you must
		  * implement a queue drainer for the log queue.  If you 
		  * don't then the log queue will fill untill your system
		  * runs out of memory.
		  */
		static void SetLazy(bool onoff);

		/**
		  * This indicates whether Lazy logging is on or not.
		  */
		static bool LazyOn(void);

		/**
		  * This returns a reference to our lazy logging queue.
		  * Use this to implement your own log writer that has
		  * more features than what we provide out of the box.
		  */
		static MsgQueue<LogMsg*>& GetLogQueue(void);

		/**
		  * This method allows you to flush the logs and
		  * close the current log file without opening another
		  * one up.  This is equivalent to calling Init with
		  * the filename of "stdout"
		  */
		static void Fini(void);

		/**
		  * This method allows you to request the file pointer
		  * that we are using to write the log file.  This is
		  * not intended for widespread use, and it is not
		  * really safe on Windows where this is compiled into
		  * a DLL.  But various members of this SLib
		  * package use this, so it is here.
		  */
		static FILE *FileHandle(void);
	

		/**
		  * Allows you to set panic logs on or off.  A value
		  * of true will turn the log on, false will turn it
		  * off.  By default Panic logs are on.
		  */
		static void SetPanic(bool onoff);

		/**
		  * Allows you to write a panic log.  Panic messages
		  * indicate severe low-level problems, and typically
		  * indicate a server shutdown is imminent.
		  *
		  * @param file This is the file where the log call occurred
		  * @param line This is the line in the file where the log 
		  *             call occurred
		  * @param msg This is the log message format.  Printf style
		  *            formatting is allowed
		  * @param ... Any arguments to the log message follow.
		  */
		static void Panic(const char *file, int line, const char *msg, ...);

		/// Indicates whether panic is on or not.
		static bool PanicOn(void);

		/**
		  * Allows you to set error logs on or off.  A value
		  * of true will turn the log on, false will turn it
		  * off. By default error logs are on.
		  */
		static void SetError(bool onoff);

		/**
		  * Allows you to write a error log.  Error messages
		  * indicate bad things like exceptions are happening,
		  * but typically indicate bad things that are under
		  * control.
		  */
		static void Error(const char *file, int line, const char *msg, ...);

		/// Indicates whether error is on or not.
		static bool ErrorOn(void);

		/**
		  * Allows you to set warning logs on or off.  A value
		  * of true will turn the log on, false will turn it
		  * off.  By default Warning logs are off.
		  */
		static void SetWarn(bool onoff);

		/**
		  * Allows you to write a warning log.  Warning messages
		  * indicate problems that could happen.  Possible data 
		  * errors, and other strange things are loged with 
		  * warnings.
		  */
		static void Warn(const char *file, int line, const char *msg, ...);

		/// Indicates whethere warn is on or not.
		static bool WarnOn(void);

		/**
		  * Allows you to set info logs on or off.  A value
		  * of true will turn the log on, false will turn it
		  * off.  By default Info logs are off.
		  */
		static void SetInfo(bool onoff);

		/**
		  * Allows you to write a info log.  Info messages
		  * are logged when bits of data need to be placed in
		  * the logs for various reasons.
		  */
		static void Info(const char *file, int line, const char *msg, ...);

		/// Indicates whethere info is on or not.
		static bool InfoOn(void);

		/**
		  * Allows you to set debug logs on or off.  A value
		  * of true will turn the log on, false will turn it
		  * off.  By default Debug logs are off.
		  */
		static void SetDebug(bool onoff);

		/**
		  * Allows you to write a debug log.  Debug messages
		  * are included to give detailed information about
		  * certain pieces of potentially troublesome code.
		  */
		static void Debug(const char *file, int line, const char *msg, ...);

		/// Indicates whethere Debug is on or not.
		static bool DebugOn(void);

		/**
		  * Allows you to set trace logs on or off.  A value
		  * of true will turn the log on, false will turn it
		  * off.  By default Trace logs are off.
		  */
		static void SetTrace(bool onoff);

		/**
		  * Allows you to write a trace log.  Trace messages
		  * are used to log function and method entry/exit.
		  */
		static void Trace(const char *file, int line, const char *msg, ...);

		/// Indicates whethere trace is on or not.
		static bool TraceOn(void);

		/**
		  * Allows you to set SqlTrace logs on or off.  A value
		  * of true will turn the log on, false will turn it
		  * off.  By default SqlTrace logs are off.
		  */
		static void SetSqlTrace(bool onoff);

		/**
		  * Allows you to write a SqlTrace log.  SqlTrace messages
		  * are used to log sql statement executions, and durations.
		  */
		static void SqlTrace(const char *file, int line, const char *msg, ...);

		/// Indicates whethere SqlTrace is on or not.
		static bool SqlTraceOn(void);

		/**
		  * Produces a micro resolution timestamp and puts it
		  * into t.
		  */
		static void TimeStamp(twine& t);

		/**
		  * This function makes the decision to write the log message
		  * out to the disk or just add it to our log queue in memory.
		  */
		static void Persist(LogMsg* lm);

};

} // End namespace

#ifndef FL
#define FL __FILE__, __LINE__
#endif

#ifdef PANIC
#	undef PANIC
#endif
#define PANIC SLib::Log::Panic
#ifdef ERRORL
#	undef ERRORL
#endif
#define ERRORL SLib::Log::Error
#ifdef WARN
#	undef WARN
#endif
#define WARN SLib::Log::Warn
#ifdef INFO
#	undef INFO
#endif
#define INFO SLib::Log::Info
#ifdef DEBUG
#	undef DEBUG
#endif
#define DEBUG SLib::Log::Debug
#ifdef TRACE
#	undef TRACE
#endif
#define TRACE SLib::Log::Trace
#ifdef SQLTRACE
#	undef SQLTRACE
#endif
#define SQLTRACE SLib::Log::SqlTrace

#endif // LOG_H Defined
