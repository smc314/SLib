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

#ifdef _WIN32
#	ifndef DLLEXPORT
#		define DLLEXPORT __declspec(dllexport)
#	endif
#else
#	define DLLEXPORT
#endif

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
class DLLEXPORT LogFile2
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
		LogFile2(const twine& logFileName = "SLib.log", size_t maxFileSize = 50 * 1024 * 1024);

		/**
		  * If you would like to (or need to) open the log file in read-only mode, use
		  * this version of the constructor.
		  */
		LogFile2(bool readOnly, const twine& logFileName = "SLib.log");

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

		/** This log file interface will cache incomming log messages so that we can write more
		  * efficiently to the disk in batches of log messages rather than one at a time.  This
		  * can significantly increase disk write performance when there are a lot of log messages
		  * flowing into the file.  By default the cache size is 10.  If you need to increase or
		  * decrease this size, then you can use this setting to adjust it.
		  */
		void setCacheSize( size_t cacheSize );

		/** When we are caching messages, we also check to see how long it has been since we last
		  * accepted a message into the cache.  If the newest message in the cache is older than
		  * the cache timeout, we'll write everything to the disk now - even if there is less than
		  * cacheSize entries in the cache.  This ensures that messages don't linger in the cache
		  * for too long. This value is set in milliseconds.
		  */
		void setCacheTime( size_t cacheTime );

		/** This method allows you to force us to write anything in our cache out to the disk.
		  * If you know that no messages are forthcomming for a while, this is a good thing to 
		  * do.
		  */
		void flush();

	protected:

		/// This method initializes our log file and ensures everything is properly setup
		void Setup();

		/// Our common error checking routine for the SQLite calls - converts errors into exceptions
		int check_err(const twine& doingWhat, int rc);

		/// Begins a transaction
		void begin_transaction();

		/// Commits an outstanding transaction
		void commit_transaction();

		/// Rollsback an outstanding transaction
		void rollback_transaction();

		/// Does the work of writing a single message to the logs
		void writeOneMsg(LogMsg& msg);

		/// Checks the current size of the database, and calls createNewFile if necessary.
		void CheckSize();

		/// Flushes the internal cache.
		void flushInternal();

		/// Checks to see if the internal cache is ready to be written to disk - if so flushes the cache.
		void checkFlushCache();

	private:

		/// Our mutex to ensure single-threadded access to our database
		Mutex* m_mutex;

		/// Are we in read-only mode?
		bool m_readOnly;

		/// Our SQLite database interface
		sqlite3* m_db;

		/// Our SQLite generic statement handle
		sqlite3_stmt* m_stmt;

		/// Our SQLite statement handle for inserting messages
		sqlite3_stmt* m_insert_stmt;

		/// Our SQLite statement handle for starting a transaction
		sqlite3_stmt* m_stmt_begintran;

		/// Our SQLite statement handle for committing a transaction
		sqlite3_stmt* m_stmt_committran;

		/// Our SQLite statement handle for rolling back a transaction
		sqlite3_stmt* m_stmt_rollbacktran;

		/// The Log file name
		twine m_logFileName;

		/// The max log file size
		size_t m_maxFileSize;

		/// The number of log messages we should cache before writing to disk
		size_t m_cacheSize;

		/// The number of milliseconds we should wait on cached records before writing to the disk
		size_t m_cacheTime;

		/// Our log message cache:
		vector<LogMsg> m_cache;

};

} // End Namespace SLib

#endif // LOGFILE2_H Defined
