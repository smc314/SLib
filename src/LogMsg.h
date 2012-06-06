#ifndef LOGMSG_H
#define LOGMSG_H
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

#include "twine.h"
#include "Thread.h"
#include "Date.h"

#include <stdint.h>

#ifdef _WIN32
#	include <sys/types.h>
#	include <sys/timeb.h>
#else
#	include <sys/time.h>
#endif

namespace SLib {

/**
  * This class represents a single log message.  All of the fields that
  * we log are here as member variables.  This allows us to store log messages
  * before writing them out to disk if that is what we want to do.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT LogMsg {
	
	public:
		/// Standard Constructor
		LogMsg();

		/// Standard copy constructor
		LogMsg(const LogMsg& c);

		/// Standard assignment operator
		LogMsg& operator=(const LogMsg& c);

		/// Standard destructor
		virtual ~LogMsg();

		/// Construct a log message with no app or trans
		LogMsg(const char* f, int l, twine& msg);

		/// Construct a log message with just file and line
		LogMsg(const char* f, int l);

		/// a unique id for this log message
		int id;

		/// Source File name of the log message
		twine file;

		/// Source File line of the log message
		int line;

		/// Thread ID of the creator of this log message (constrained to only the lower 32-bits)
		uint32_t tid;

		/// Time stamp of the time of this log message
#ifdef _WIN32
		struct timeb timestamp;
#else
		struct timeval timestamp;
#endif

		/// A specific log channel
		int channel;

		/// An Application name
		twine appName;

		/// A Machine name
		twine machineName;

		/// The actual log message
		twine msg;

		/// Whether this message is a static string or not.
		bool msg_static;

		/// Sets our timestamp value to now
		void SetTimestamp(void);

		/// Handles determining our machine name and application name
		void SetAppMachine(void);

		/// Formats our timestamp in a standard way and returns it
		twine GetTimestamp(void);
};

} // End namespace

#endif // LOGMSG_H Defined
