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

#include "LogFile2.h"
#include "sqlite3.h"
#include "AnException.h"
#include "dptr.h"
#include "XmlHelpers.h"
#include "EnEx.h"
#include "twine.h"
#include "ZipFile.h"
#include "File.h"
using namespace SLib;

LogFile2::LogFile2(const twine& logFileName, size_t maxFileSize)
{
	//printf("LogFile2::LogFile2(const twine& logFileName, size_t maxFileSize)\n");

	m_logFileName = logFileName;
	m_maxFileSize = maxFileSize;
	m_readOnly = false;
	m_db = NULL;
	m_stmt = NULL;
	m_insert_stmt = NULL;
	m_mutex = new Mutex();
	m_cacheSize = 100;
	m_cacheTime = 100;

	try {
		Setup();
	} catch (AnException& e){
		//printf("Caught exception calling Setup\n");
		//printf("createNewFile\n");
		createNewFile(); // try to get it out of the way so we can create it from scratch.
	}
}

LogFile2::LogFile2(bool readOnly, const twine& logFileName)
{
	//printf("LogFile2::LogFile2(const twine& logFileName, size_t maxFileSize)\n");

	m_logFileName = logFileName;
	m_maxFileSize = 50 * 1024 * 1024;
	m_readOnly = readOnly;
	m_db = NULL;
	m_stmt = NULL;
	m_insert_stmt = NULL;
	m_mutex = new Mutex();
	m_cacheSize = 100;
	m_cacheTime = 100;

	Setup(); // any exception trying to open the file is passed back in read-only mode.
}

LogFile2::~LogFile2()
{
	//printf("LogFile2::~LogFile2()\n");
	{ // used for mutex scope
		Lock theLock(m_mutex);

		flushInternal(); // anything left in the cache goes to the disk

		// Close off our log file
		if(m_stmt != NULL){
			sqlite3_finalize( m_stmt );
		}
		if(m_insert_stmt != NULL){
			sqlite3_finalize( m_insert_stmt );
		}
		if(m_db != NULL){
			sqlite3_close(m_db);
		}

	} // mutex unlocked here

	delete m_mutex;

	// that's it.
}

void LogFile2::close()
{
	//printf("LogFile2::close()\n");
	Lock theLock(m_mutex);

	flushInternal(); // anything left in the cache goes to the disk

	// Close off our log file
	if(m_stmt != NULL){
		sqlite3_finalize( m_stmt );
		m_stmt = NULL;
	}
	if(m_insert_stmt != NULL){
		sqlite3_finalize( m_insert_stmt );
		m_insert_stmt = NULL;
	}
	if(m_db != NULL){
		sqlite3_close(m_db);
		m_db = NULL;
	}

}

void LogFile2::Setup(void)
{
	//printf("LogFile2::Setup()\n");
	int rc = 0;

	// Initialize sqlite3
	sqlite3_initialize();

	// Open/Create the database:
	if(m_readOnly){
		check_err(
			sqlite3_open_v2( m_logFileName(), &m_db, SQLITE_OPEN_READONLY, NULL)
		);
	} else {
		check_err(
			sqlite3_open( m_logFileName(), &m_db)
		);
	}
	
	// Check for our mainframe log table:
	twine stmt = "select name from sqlite_master where type='table' and name='logtable'";
	check_err(
		sqlite3_prepare( m_db, stmt(), (int)stmt.length(), &m_stmt, NULL)
	);

	rc = check_err( sqlite3_step( m_stmt ));
	if(rc == 0){
		if(m_readOnly){
			throw AnException(0, FL, "logtable does not exist - not creating in readonly mode.");
		}
		// table doesn't exist, create it:
		sqlite3_finalize( m_stmt );
		m_stmt = NULL;
		
		stmt.erase();
		stmt.append( "create table logtable ( "
			"id integer primary key autoincrement, "
			"file varchar(20), "
			"line int, "
			"tid int, "
			"timestamp_a int, "
			"timestamp_b int, "
			"channel int, "
			"appName varchar(10), "
			"machineName varchar(10), "
			"appSession varchar(10), "
			"msg varchar(10) "
			");"
		);
		check_err(
			sqlite3_prepare( m_db, stmt(), (int)stmt.length(), &m_stmt, NULL)
		);
		check_err( sqlite3_step( m_stmt ));

	} else {
		// table exists, find out stuff about it
		sqlite3_finalize( m_stmt );
		m_stmt = NULL;
	}
}

int LogFile2::check_err(int rc)
{
	//printf("LogFile2::check_err(int rc)\n");

	if(rc == SQLITE_OK){
		return 0; // everything is fine.
	} else if(rc == SQLITE_ROW){
		return 1; // everything is fine - data available
	} else if(rc == SQLITE_DONE){
		return 0; // everything is fine - no more data
	} else if(rc == SQLITE_MISUSE){
		throw AnException(0, FL, "Programming problem using the sqlite3 interface.");
	}

	// Otherwise, look up the error message and convert to an exception:
	throw AnException(0, FL, "Sqlite3 Error: %s", sqlite3_errmsg( m_db ) );
}

void LogFile2::flush()
{
	Lock theLock(m_mutex);
	flushInternal();
}

void LogFile2::setCacheSize( size_t cacheSize )
{
	if(cacheSize >= 0 && cacheSize <= 10000){
		m_cacheSize = cacheSize;
	} 
}

void LogFile2::setCacheTime( size_t cacheTime )
{
	if(cacheTime >= 0 && cacheTime <= 1000){
		m_cacheTime = cacheTime;
	}
}

void LogFile2::flushInternal()
{
	if(m_cache.size() != 0 && m_db != NULL){
		begin_transaction();
		for(size_t i = 0; i < m_cache.size(); i++){
			writeOneMsg( m_cache[i] );
		}
		commit_transaction();
		m_cache.clear();
		CheckSize();
	}
}

void LogFile2::checkFlushCache()
{
	if(m_cache.size() >= m_cacheSize){ // Is the cache large enough to be written?
		flushInternal();
	} else {
		if(m_cache.size() > 1) { // more than one message
			// Check the time difference between the newest and oldest messages in the cache
			LogMsg& lm1 = m_cache[ m_cache.size() - 1 ]; // most recent log message.
			LogMsg& lm2 = m_cache[ 0 ];                  // oldest log message.
#ifdef _WIN32
			if(lm1.timestamp.time > lm2.timestamp.time){
				// If the seconds are greater at all, then flush the cache.
				flushInternal();
			} else if(lm1.timestamp.millitm >= (lm2.timestamp.millitm + m_cacheTime)){
				// seconds are the same, but milliseconds have elapsed more than m_cacheTime
				flushInternal();
			}
#else
			if(lm1.timestamp.tv_sec > lm2.timestamp.tv_sec){
				// If the seconds are greater at all, then flush the cache.
				flushInternal();
			} else if(lm1.timestamp.tv_usec >= (lm2.timestamp.tv_usec + (m_cacheTime * 1000) )){
				// seconds are the same, but milliseconds have elapsed more than m_cacheTime
				flushInternal();
			}
#endif
		}
	}
}

void LogFile2::writeMsg(LogMsg& msg)
{
	//printf("LogFile2::writeMsg()\n");
	if(m_readOnly){
		throw AnException(0, FL, "writeMsg is not allowed in readonly mode.");
	}
	Lock theLock(m_mutex);

	LogMsg msgCopy( msg );
	m_cache.push_back( msgCopy );
	checkFlushCache();
}

void LogFile2::writeMsg(vector<LogMsg*>* messages)
{
	//printf("LogFile2::writeMsg(vector<LogMsg*>* messages)\n");
	if(m_readOnly){
		throw AnException(0, FL, "writeMsg is not allowed in readonly mode.");
	}
	Lock theLock(m_mutex);

	for(size_t i = 0; i < messages->size(); i++){
		LogMsg msgCopy( *messages->at( i ) );
		m_cache.push_back( msgCopy );
	}

	checkFlushCache();
}

void LogFile2::writeOneMsg(LogMsg& msg)
{
	//printf("LogFile2::writeOneMsg()\n");
	if(m_readOnly){
		throw AnException(0, FL, "writeMsg is not allowed in readonly mode.");
	}

	if(m_insert_stmt == NULL){
		twine sql = 
			"insert into logtable (file, line, tid, timestamp_a, timestamp_b, channel, "
			" appName, machineName, appSession, msg ) "
			" values ( ?, ?, ?, ?, ?, ?, "
			" ?, ?, ?, ? ) "
		;
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &m_insert_stmt, NULL)
		);
	} else {
		sqlite3_reset(m_insert_stmt);
	}

	check_err( 
		sqlite3_bind_text(m_insert_stmt, 1, msg.file(), (int)msg.file.length(), SQLITE_STATIC)
	);
	check_err( 
		sqlite3_bind_int(m_insert_stmt, 2, msg.line )
	);
	check_err( 
		sqlite3_bind_int(m_insert_stmt, 3, (int)msg.tid)
	);
#ifdef _WIN32
	check_err( 
		sqlite3_bind_int(m_insert_stmt, 4, (int)msg.timestamp.time)
	);
	check_err( 
		sqlite3_bind_int(m_insert_stmt, 5, (int)msg.timestamp.millitm)
	);
#else
	check_err( 
		sqlite3_bind_int(m_insert_stmt, 4, (int)msg.timestamp.tv_sec)
	);
	check_err( 
		sqlite3_bind_int(m_insert_stmt, 5, (int)msg.timestamp.tv_usec)
	);
#endif
	check_err( 
		sqlite3_bind_int(m_insert_stmt, 6, msg.channel )
	);
	check_err( 
		sqlite3_bind_text(m_insert_stmt, 7, msg.appName(), (int)msg.appName.length(), SQLITE_STATIC)
	);
	check_err( 
		sqlite3_bind_text(m_insert_stmt, 8, msg.machineName(), (int)msg.machineName.length(), SQLITE_STATIC)
	);
	check_err( 
		sqlite3_bind_text(m_insert_stmt, 9, msg.appSession(), (int)msg.appSession.length(), SQLITE_STATIC)
	);
	check_err( 
		sqlite3_bind_text(m_insert_stmt, 10, msg.msg(), (int)msg.msg.length(), SQLITE_STATIC)
	);

	// Run the insert.
	check_err( sqlite3_step( m_insert_stmt ));

	// Update the Message to indicate what the new ID is:
	msg.id = (int)sqlite3_last_insert_rowid( m_db );
}

void LogFile2::begin_transaction()
{
	//printf("LogFile2::begin_transaction()\n");
	if(m_readOnly){
		throw AnException(0, FL, "begin_transaction is not allowed in readonly mode.");
	}
	
	sqlite3_stmt* stmt;
	try {
		twine sql = "begin transaction;";
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL)
		);
		check_err( sqlite3_step( stmt ));
		sqlite3_finalize(stmt);
	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		throw e;
	}
}

void LogFile2::commit_transaction()
{
	//printf("LogFile2::commit_transaction()\n");
	if(m_readOnly){
		throw AnException(0, FL, "commit_transaction is not allowed in readonly mode.");
	}
	
	sqlite3_stmt* stmt;
	try {
		twine sql = "commit transaction;";
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL)
		);
		check_err( sqlite3_step( stmt ));
		sqlite3_finalize(stmt);
	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		throw e;
	}
}

void LogFile2::CheckSize()
{
	//printf("LogFile2::messageCount()\n");
	
	size_t page_size = 0, page_count = 0;
	sqlite3_stmt* stmt;
	try {
		twine sql = "pragma page_size;";
		check_err( sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL));
		check_err( sqlite3_step( stmt ));
		page_size = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize(stmt);

		sql = "pragma page_count;";
		check_err( sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL));
		check_err( sqlite3_step( stmt ));
		page_count = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize(stmt);

		size_t total_size = page_size * page_count;
		if(total_size > m_maxFileSize){
			createNewFile();
		}

	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		throw e;
	}
}


int LogFile2::messageCount()
{
	//printf("LogFile2::messageCount()\n");
	Lock theLock(m_mutex);
	
	sqlite3_stmt* stmt;
	try {
		twine sql = "select count(1) from logtable;";
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL)
		);
		check_err( sqlite3_step( stmt ));
		int rc = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize(stmt);
		return rc;
	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		throw e;
	}
}

int LogFile2::messageCount(const twine& whereClause)
{
	//printf("LogFile2::messageCount()\n");
	Lock theLock(m_mutex);
	
	sqlite3_stmt* stmt;
	try {
		twine sql = "select count(1) from logtable " + whereClause + ";";
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL)
		);
		check_err( sqlite3_step( stmt ));
		int rc = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize(stmt);
		return rc;
	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		throw e;
	}
}

LogMsg* LogFile2::getMessage(int id)
{
	//printf("LogFile2::getMessage()\n");
	Lock theLock(m_mutex);

	sqlite3_stmt* stmt;
	try {
		twine sql; sql.format(
			"select id, file, line, tid, timestamp_a, timestamp_b, channel, "
			"       appName, machineName, appSession, msg "
			"from logtable "
			"where id = %d;", id);
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL)
		);
		int rc = check_err( sqlite3_step( stmt ));
		if(rc == 0){
			// Didn't find the log message:
			sqlite3_finalize( stmt );
			return NULL;
		} else {
			// Ensure we have a sane list of columns:
			int colCount = sqlite3_column_count(stmt);
			if(colCount != 11){
				WARN(FL, "We don't understand the layout of logtable table in the current log file.");
				sqlite3_finalize( stmt );
				return NULL;
			}

			// Pick up all of the columns:
			dptr<LogMsg> ret = new LogMsg();
			ret->id = sqlite3_column_int( stmt, 0 );
			ret->file.set( 
				(const char*)sqlite3_column_text(stmt, 1), (size_t)sqlite3_column_bytes(stmt, 1) );
			ret->line = sqlite3_column_int( stmt, 2 );
			ret->tid = sqlite3_column_int( stmt, 3 );
#ifdef _WIN32
			ret->timestamp.time = sqlite3_column_int( stmt, 4 );
			ret->timestamp.millitm = (unsigned short)sqlite3_column_int( stmt, 5 );
#else
			ret->timestamp.tv_sec = sqlite3_column_int( stmt, 4 );
			ret->timestamp.tv_usec = sqlite3_column_int( stmt, 5 );
#endif
			ret->channel = sqlite3_column_int( stmt, 6 );
			ret->appName.set( 
				(const char*)sqlite3_column_text(stmt, 7), (size_t)sqlite3_column_bytes(stmt, 7) );
			ret->machineName.set( 
				(const char*)sqlite3_column_text(stmt, 8), (size_t)sqlite3_column_bytes(stmt, 8) );
			ret->appSession.set( 
				(const char*)sqlite3_column_text(stmt, 9), (size_t)sqlite3_column_bytes(stmt, 9) );
			ret->msg.set( 
				(const char*)sqlite3_column_text(stmt, 10), (size_t)sqlite3_column_bytes(stmt, 10) );

			sqlite3_finalize( stmt );
			return ret.release();
		}
	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		throw e;
	}
}

vector<LogMsg*>* LogFile2::getMessages(const twine& whereClause, int limit, int offset)
{
	//printf("LogFile2::getMessage()\n");
	Lock theLock(m_mutex);

	vector<LogMsg*>* ret = new vector<LogMsg*>();
	sqlite3_stmt* stmt;
	try {
		twine sql; sql.format(
			"select id, file, line, tid, timestamp_a, timestamp_b, channel, appName, machineName, appSession, msg "
			"from logtable "
			" %s ",
			whereClause()
		);
		if(limit != 0){
			twine limitClause; limitClause.format(" limit %d offset %d ", limit, offset);
			sql.append( limitClause );
		}
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL)
		);
		int rc = check_err( sqlite3_step( stmt ));
		if(rc == 0){
			// Didn't find the log message:
			sqlite3_finalize( stmt );
			return ret;
		} else {
			// Ensure we have a sane list of columns:
			int colCount = sqlite3_column_count(stmt);
			if(colCount != 11){
				WARN(FL, "We don't understand the layout of logtable table in the current log file.");
				sqlite3_finalize( stmt );
				return ret;
			}

			while(rc != 0){
				// Pick up all of the columns:
				dptr<LogMsg> msg = new LogMsg();
				msg->id = sqlite3_column_int( stmt, 0 );
				msg->file.set( 
					(const char*)sqlite3_column_text(stmt, 1), (size_t)sqlite3_column_bytes(stmt, 1) );
				msg->line = sqlite3_column_int( stmt, 2 );
				msg->tid = sqlite3_column_int( stmt, 3 );
#ifdef _WIN32
				msg->timestamp.time = sqlite3_column_int( stmt, 4 );
				msg->timestamp.millitm = (unsigned short)sqlite3_column_int( stmt, 5 );
#else
				msg->timestamp.tv_sec = sqlite3_column_int( stmt, 4 );
				msg->timestamp.tv_usec = sqlite3_column_int( stmt, 5 );
#endif
				msg->channel = sqlite3_column_int( stmt, 6 );
				msg->appName.set( 
					(const char*)sqlite3_column_text(stmt, 7), (size_t)sqlite3_column_bytes(stmt, 7) );
				msg->machineName.set( 
					(const char*)sqlite3_column_text(stmt, 8), (size_t)sqlite3_column_bytes(stmt, 8) );
				msg->appSession.set( 
					(const char*)sqlite3_column_text(stmt, 9), (size_t)sqlite3_column_bytes(stmt, 9) );
				msg->msg.set( 
					(const char*)sqlite3_column_text(stmt, 10), (size_t)sqlite3_column_bytes(stmt, 10) );

				ret->push_back( msg.release() );

				// Fetch the next row:
				rc = check_err( sqlite3_step ( stmt ) );
			}

			sqlite3_finalize( stmt );
			return ret;
		}
	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		if(ret != NULL){
			for(size_t i = 0; i < ret->size(); i++){
				delete ret->at( i );
			}
			delete ret;
		}
		throw e;
	}
}

int LogFile2::getOldestMessageID()
{
	//printf("LogFile2::getOldestMessageID()\n");
	Lock theLock(m_mutex);

	sqlite3_stmt* stmt;
	try {

		twine sql = "select min(id) from logtable;";
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL)
		);
		check_err( sqlite3_step( stmt ));
		int rc = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize( stmt );
		return rc;
	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		throw e;
	}
}

int LogFile2::getNewestMessageID()
{
	//printf("LogFile2::getNewestMessageID()\n");
	Lock theLock(m_mutex);

	sqlite3_stmt* stmt;
	try {

		twine sql = "select max(id) from logtable;";
		check_err(
			sqlite3_prepare( m_db, sql(), (int)sql.length(), &stmt, NULL)
		);
		check_err( sqlite3_step( stmt ));
		int rc = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize( stmt );
		return rc;
	} catch (AnException& e){
		if(stmt != NULL){
			sqlite3_finalize( stmt );
			stmt = NULL;
		}
		throw e;
	}
}


void LogFile2::createNewFile()
{
	//printf("LogFile2::createNewFile()\n");
	if(m_readOnly){
		throw AnException(0, FL, "createNewFile is not allowed in readonly mode.");
	}

	// Close off our log file
	if(m_stmt != NULL){
		sqlite3_finalize( m_stmt );
		m_stmt = NULL;
	}
	if(m_insert_stmt != NULL){
		sqlite3_finalize( m_insert_stmt );
		m_insert_stmt = NULL;
	}
	if(m_db != NULL){
		sqlite3_close(m_db);
		m_db = NULL;
	}

	// Then move it to a new name:
	Date d;
	twine newName = m_logFileName + "." + d.GetValue("%Y%m%d%H%M%S");
	int res = rename( m_logFileName(), newName() );
	if(res){
		Setup(); // don't leave us without a log file
		throw AnException(0, FL, "Error renaming existing log file %s to %s",
			m_logFileName(), newName() );
	}
	// Then zip it up to save space
	ZipFile zf( newName + ".zip" );
	zf.AddFile( newName );
	zf.Close();

	// Remove the unzipped version
	File::Delete( newName );

	// Then create our new log file:
	Setup();
}

