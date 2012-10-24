#ifndef LOGFILE2_H
#define LOGFILE2_H
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

#include <vector>
#include <utility>
using namespace std;

#include "sqlite3.h"
#include "twine.h"
#include "sptr.h"
#include "Mutex.h"
#include "LogMsg.h"

namespace SLib {

/**
  * This class is responsible for handling the storage and retrieval of mainframe log 
  * messages.  We do this by wrapping a sqlite3 database and using it to store all of
  * the log messages and provide retrieval functions for us.  This class will automatically
  * create the log file if it doesn't exist, and it will manage the size of the log file
  * and create new ones as required when the size exceeds the given threshold.
  *
  * @author Steven M. Cherry
  */
class LogFile2
{
	private:
		/// copy constructor is private to prevent use
		LogFile2(const LogFile2& c) {}

		/// assignmet operator is private to prevent use
		LogFile2& operator=(const LogFile2& c) { return *this;}

	public:
		/**
		  * Our standard constructor with defaults for the logFileName, and maxFileSize
		  * parameters.
		  */
		LogFile2(const twine& logFileName = "IvoryHubDirector.log", size_t maxFileSize = 100 * 1024 * 1024);

		/// Standard destructor
		virtual ~LogFile2();

		/** This method will add a new log message to our log file.  If the new message
		 * causes the log file to exceed the maxFileSize threshold, a new log file will be
		 * created.
		 */
		void writeMsg(LogMsg& msg);

		/** This method allows you to write multiple log messages to our file.  This is the
		 * preferred way to write messages to our file as this takes advantage of including
		 * multiple inserts within a single commit.  This maximizes file/io of the database.
		 */
		void writeMsg(vector<LogMsg*>* messages);

		/** Returns the number of messages in our file.
		 */
		int messageCount();
		int messageCount(const twine& whereClause);

		/** Retrieves a single log message by message ID */
		LogMsg* getMessage(int id);

		/** Retrieves log messages by free-form query - pass us the where clause. */
		vector<LogMsg*>* getMessages( const twine& whereClause, int limit = 0, int offset = 0);

		/** Returns the ID of the oldest message in our log */
		int getOldestMessageID();

		/** Returns the ID of the newest message in our log */
		int getNewestMessageID();

		/** This will close our log file and move it to a new name so that we can
		 * re-open a new log file.
		 */
		void createNewFile();

		/** Use this to properly close the log file.
		 */
		void close();

	protected:

		/// This method initializes our log file and ensures everything is properly setup
		void Setup();

		/// Our common error checking routine for the SQLite calls - converts errors into exceptions
		int check_err(int rc);

		/// Begins a transaction
		void begin_transaction();

		/// Commits an outstanding transaction
		void commit_transaction();

		/// Does the work of writing a single message to the logs
		void writeOneMsg(LogMsg& msg);

		/// Checks the current size of the database, and calls createNewFile if necessary.
		void CheckSize();

	private:

		/// Our mutex to ensure single-threadded access to our database
		Mutex* m_mutex;

		/// Our SQLite database interface
		sqlite3* m_db;

		/// Our SQLite generic statement handle
		sqlite3_stmt* m_stmt;

		/// Our SQLite statement handle for inserting messages
		sqlite3_stmt* m_insert_stmt;

		/// The Log file name
		twine m_logFileName;

		/// The max log file size
		size_t m_maxFileSize;

};

} // End Namespace SLib

#endif // LOGFILE2_H Defined
