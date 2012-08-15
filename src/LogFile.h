#ifndef LogFile_H
#define LogFile_H

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <map>
using namespace std;

#include "AnException.h"
#include "LogMsg.h"
using namespace SLib;

namespace SLib {

class LogFile;

/** A typical LogMsg has strings for machine, application, file, and
 * the actual log message itself.  We extend this LogMsg class and use
 * our string table to try and store the strings and replace them with
 * index values.  This keeps static strings in the string table, and allows
 * our log message entries to be as small as possible.
 */
class DLLEXPORT LogMsgStripped : public LogMsg {
	public:
		int file_id;
		int app_id;
		int machine_id;
		int msg_id;

		LogMsgStripped(const LogMsg& the_msg, LogFile* lf);

		int length();
};

/**
 * This class is what we use to manage log messages on disk. The log file layout
 * looks like this:
 * <pre> 
 * -- Signature (8 bytes) 
 * -- Index Area (Fixed size block) 
 * -- 	Record Count (4 byte integer) 
 * -- 	Index Count (4 byte integer) 
 * -- 	Oldest Entry (4 byte integer) 
 * -- 	Newest Entry (4 byte integer) 
 * -- Index Entry 1 
 * -- 	Offset into file (4 byte Integer) 
 * -- 	Message Length (4 byte Integer) 
 * -- 	Message ID (4 byte Integer) 
 * -- Index Entry 2 
 * -- ... 
 * -- String Table Area (fixed size block) 
 * -- 	Strings Area Size (4 byte Integer) 
 * -- 	Max Index Count (4 byte Integer) 
 * -- 	In Use Count (4 byte Integer) 
 * -- 	String 1 Offset into file (4 byte Integer) 
 * -- 	String 1 Length (4 byte Integer) 
 * -- 	String 2 Offset (4 byte Integer) 
 * -- 	String 2 Length (4 byte Integer) 
 * -- 	... 
 * -- 	String 1 (variable Length) 
 * -- 	String 2 (variable Length) 
 * -- 	... 
 * -- Message Entry 
 * -- 	Message Eye Catcher (4 byte Integer, value ABACADAB) 
 * -- 	Message ID (4 byte Integer) 
 * -- 	Index Number (4 byte Integer) 
 * -- 	Message Content (varies) 
 * -- Message Entry 
 * -- ...
 * </pre>
 * 
 * This is all done using a random access file structure on the disk. Each time
 * we write to the file, we ensure that it has been flushed to the disk so that
 * when we return from the writeMsg() call, the file has been saved.
 * 
 * 
 * @author Steven M. Cherry
 */
class DLLEXPORT LogFile {

	/** This is our LogFile Index header, which keeps some simple stats about
	 * our log file and where the first and last records are.
	 */
	struct Index {
		/** number of log records in the file */
		int record_count;

		/** how many total index entries we have (also means max log messages) */
		int index_count;

		/** index of the oldest record */
		int oldest_entry;

		/** index of the newest record */
		int newest_entry;
	};

	/** An individual Index entry consists of a simple offset, length and ID which
	 * allows us to track all entries.
	 */
	struct IndexEntry {
		int offset;

		int length;

		int id;
	};

	/** The string table header tells us how many strings we can hold, how many
	 * we are currently holding and what the size of our string table area is.
	 */
	struct StringTable {
		/** Total size in bytes of the whole string table area */
		int total_size;

		/** How many strings can we hold, maximum */
		int total_indexes;

		/** How many strings are we holding right now. */
		int index_in_use;
	};

	/** Each index in our string table consists of just an offset and length
	 * that allow us to find and read the string table entry.
	 */
	struct StringTableIndex {
		int offset;
		int length;

		bool operator< ( StringTableIndex& rhs) {
			if(offset < rhs.offset) return true;
			if(offset > rhs.offset) return false;
			if(length < rhs.length) return true;
			return false;
		}
		
	};
	
	private:
		/** This is our signature. 3141ZEDL */
		char* m_signature;

		/** Keeps track of our index area */
		Index m_index_header;

		/** Our array of indexes */
		vector<IndexEntry*>* m_indexes;

		/** Fast look-up of log ID to IndexEntry */
		map<int, IndexEntry*>* m_log_ids;

		/** Our String table header */
		StringTable m_string_table_header;

		/** Our String table indexes */
		vector<StringTableIndex*>* m_string_indexes;

		/** A fast look-up version of our string table, in memory */
		map<twine, StringTableIndex*>* m_string_table;

		/** A Fast look-up version of our string table, by ID, then string */
		map<StringTableIndex*, twine>* m_string_table_reverse;

		/** Our maximum size in bytes that we will allow the file to grow to. */
		int m_max_size;

		/** Our max number of message entries. This is the size of the index header */
		int m_max_entries;

		/** Our string table max size */
		int m_string_table_size;

		/** Our max number of strings in the string table */
		int m_max_strings;

		/**
		 * An indication of what to do when we run out of space, or log entries. If
		 * set to true, then we will remove old log entries from the file to create
		 * space for new ones. If set to false, when we run out of space/entries we
		 * will close the old log file, archive it, and open up a new one.
		 */
		bool m_reuse;
		
		/**
		 * An indication of whether we should zero out the log file when we first
		 * open it up.  This is usually only true during a dev/debugging setup.
		 */
		bool m_clear_at_startup;

		/** This is the file that we are logging to. */
		twine m_file_name;

		/** This is the file handle of our open log file */
		FILE* m_log;

		/** This is the mutex that we use to keep log file access exclusive. */
		Mutex* m_mutex;

	public:

		/** Standard constructor to open an existing or create a new log file.
		 */
		LogFile(twine FileName, int max_size, bool reuse, bool clear_at_startup);

		/** Standard constructor to open an existing or create a new log file.
		 */
		LogFile(twine FileName, int max_size, int max_entries,
			int string_table_size, int max_strings, bool reuse,
			bool clear_at_startup);

		/** Standard destructor */
		virtual ~LogFile();

		/**
		 * This allows you to write a message to our log file.
		 * 
		 */
		void writeMsg(LogMsg& msg);

		/**
		 * Returns the number of messages in our log file.
		 * 
		 */
		int messageCount();

		/**
		 * Returns all messages from our log file
		 * 
		 */
		vector<LogMsg*>* getAllMessages();

		/** Retrieves a single log message by message ID */
		LogMsg* getMessage(int id);

		/** Returns the ID of the oldest message in our log */
		int getOldestMessageID();

		/** Returns the ID of the newest message in our log */
		int getNewestMessageID();

		/** This will record a series of our log-file statistics as a new child
		 * node to the given XML document node that you give us.
		 */
		void getStats(xmlNodePtr node);
	
		/**
		 * This will dump the entire contents of our log file out to stdout, using
		 * normal formatting rules.
		 */
		void dumpLog();
	
		/** This will dump our header/index/string table information.
		 * 
		 */
		void dumpIndexAndStrings();

		/** This will dump our header/index/string table information.
		 * 
		 */
		void dumpMessageData();

		/**
		 * This will scan a log file and attempt to recover messages from it, after
		 * the indexes have become corrupt.
		 */
		void recoverLog(twine FileName);
	
		/** This will allow you to properly shut-down our log file.
		 * 
		 */
		void close();

		/**
		 * This adds a string to our string table and returns the entry.
		 * 
		 */
		int addStringTableEntry(twine str);

		/** This will close our log file and move it to a new name so
		 * that we can re-open a new log file.
		 */
		void createNewFile();
	
	private:

		/**
		 * This will look for the file to open as our log file. If it can't be
		 * found, or doesn't match the signature of our log file, we'll set m_log to
		 * null.
		 */
		void openFile();

		/**
		 * This will read our header/index/etc. information from the log file that
		 * we currently have open.
		 */
		void readLogHeaders();

		/**
		 * This creates a new version of our log file
		 * 
		 */
		void createFile();

		void writeIndexHeader();

		void writeIndexEntry(int which_index);

		void writeMessageEntry(int which_index, LogMsgStripped& msg);

		LogMsg* readMessageEntry(IndexEntry* ie) ;

		/** Writes an integer out to the current position of our log file stream */
		void write(uint32_t value);

		/** Writes a twine out to the current position of our log file stream */
		void write(twine& value);

		/** Writes a twine or the string table index out to the current position of our log file.*/
		void write(twine& value, int stringTableIndex);

		/** Reads an integer from the current position of our log file stream */
		uint32_t readInt();

		/** Reads a twine from the current position of our log file stream */
		twine readTwine(size_t length);

		/** Seek's in our log file to the position referenced as an offset from the start of the file */
		void seek(long offsetFromStart);

		/** Clear's our indexes vector */
		void clearIndexes();

		/** Clear's our log ids map */
		void clearLogIds();

		/** Clear's our string indexes vector */
		void clearStringIndexes();

		/** Clear's our string table map */
		void clearStringTable();

		/** Clear's our reverse string table map */
		void clearStringTableReverse();

};

} // End Namespace SLib

#endif // LogFile_H Defined
