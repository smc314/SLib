#include "LogFile.h"
#include "Date.h"
using namespace SLib;

/// The size of our LogFile signature = 8.
static int SIGNATURE_SIZE = 8;

/// The size of our LogFile index header = 16.
static int INDEX_HEADER_SIZE = 16;

/// The size of our LogFile index entry = 12.
static int INDEX_ENTRY_SIZE = 12;

/// The size of our LogFile string table header = 12.
static int STRINGTAB_HEADER_SIZE = 12;

/// The size of our LogFile string table entry = 8.
static int STRINGTAB_INDEX_ENTRY_SIZE = 8;

static int MESSAGE_ENTRY_EYE_CATCHER = 0x0BACADAB;



LogMsgStripped::LogMsgStripped(const LogMsg& the_msg, LogFile* lf)
{
	LogMsg::operator=( the_msg );

	msg_id = -1;

	file_id = lf->addStringTableEntry(file);
	app_id = lf->addStringTableEntry(appName);
	machine_id = lf->addStringTableEntry(machineName);

	// Only do this for static messages:
	// msg_id = lf->addStringTableEntry(msg);
}

int LogMsgStripped::length() 
{
	int ret;
	ret =
		// -- sizes for the message header info
		4 + // for the eyecatcher
		4 + // for the id
		4 + // for the index
		// -- then the actual message content
		8 + // for the date part 1
		8 + // for the date part 2
		4 + // for the line
		4 + // for the channel
		4 + // for the threaid
		4; // for the 4 flags indicating string indexes or not.

	if (file_id != -1) {
		ret += 4; // string id for the file name
	} else {
		ret += 4; // length of byte array.
		ret += file.length();
	}

	if (app_id != -1) {
		ret += 4; // string id for the application name
	} else {
		ret += 4; // length of byte array.
		ret += appName.length();
	}

	if (machine_id != -1) {
		ret += 4; // string id for the machine name
	} else {
		ret += 4; // length of byte array.
		ret += machineName.length();
	}

	if (msg_id != -1) {
		ret += 4; // string id for the message id
	} else {
		ret += 4; // length of byte array.
		ret += msg.length();
	}

	return ret;
}










LogFile::LogFile(twine FileName, int max_size, bool reuse, bool clear_at_startup)
{
	m_signature = (char*)"3141ZEDL";
	m_mutex = new Mutex();
	m_file_name = FileName;
	m_max_size = max_size;
	m_reuse = reuse;
	m_log = NULL;
	m_clear_at_startup = clear_at_startup;

	m_indexes = NULL;
	m_log_ids = NULL;
	m_string_indexes = NULL;
	m_string_table = NULL;
	m_string_table_reverse = NULL;

	// Automatically calculate the max entries, string table, and max strings based
	// on the max size, with the desire to optimize the number of log messages we
	// can get into the file.
	
	// String table size should be 10% of log file size, but not more than 1M
	m_string_table_size = m_max_size / 10;
	if(m_string_table_size > 1024000){
		m_string_table_size = 1024000;
	}
	// Strings, on average, are about 40 characters long.  Find how many will fit into our
	// string table area based on its size.
	m_max_strings = m_string_table_size / 40;

	// Check to ensure we haven't filled up our whole string table with just indexes:
	if( (m_max_strings * STRINGTAB_INDEX_ENTRY_SIZE) > (m_string_table_size / 5) ){
		// Should not be greater than 20% of our string table size:
		m_max_strings = (m_string_table_size / 5) / STRINGTAB_INDEX_ENTRY_SIZE;
	}

	int data_size = m_max_size - m_string_table_size;

	// Logs, on average are about 80 characters long.  Find how many will fit into our
	// data area based on its size.
	m_max_entries = data_size / 80;

	// Check to ensure we haven't filled up our whole log file with just indexes:
	if( (m_max_entries * INDEX_ENTRY_SIZE) > (data_size / 5) ){
		// Should not be greater than 20% of our data area:
		m_max_entries = (data_size / 5) / INDEX_ENTRY_SIZE;
	}
	
	// Find the file if it exists and open it
	openFile();

	// Create the file brand new if not and initialize it
	if (m_log == NULL) {
		createFile();
	}
}

LogFile::LogFile(twine FileName, int max_size, int max_entries,
		int string_table_size, int max_strings, bool reuse,
		bool clear_at_startup)
{
	m_signature = (char*)"3141ZEDL";
	m_mutex = new Mutex();
	m_file_name = FileName;
	m_max_size = max_size;
	m_max_entries = max_entries;
	m_string_table_size = string_table_size;
	m_max_strings = max_strings;
	m_reuse = reuse;
	m_log = NULL;
	m_clear_at_startup = clear_at_startup;

	m_indexes = NULL;
	m_log_ids = NULL;
	m_string_indexes = NULL;
	m_string_table = NULL;
	m_string_table_reverse = NULL;

	// Find the file if it exists and open it
	openFile();

	// Create the file brand new if not and initialize it
	if (m_log == NULL) {
		createFile();
	}
}

LogFile::~LogFile()
{
	// Ensure that the log file is closed
	close();
	m_log = NULL;

	delete m_mutex;
	m_mutex = NULL;

	clearIndexes();
	clearLogIds();
	clearStringIndexes();
	clearStringTable();
	clearStringTableReverse();

	delete m_indexes;
	delete m_log_ids;
	delete m_string_indexes;
	delete m_string_table;
	delete m_string_table_reverse;
}

void LogFile::writeMsg(LogMsg& msg)
{
	Lock theLock(m_mutex);

	int start_of_messages = 
		SIGNATURE_SIZE + 
		INDEX_HEADER_SIZE + 
		(m_index_header.index_count * INDEX_ENTRY_SIZE) + 
		m_string_table_header.total_size;

	// First we need to stringify this message by replacing all of
	// it's static strings with references to our string table.
	LogMsgStripped msg2(msg, this);

	// How big is the message:
	int msg_len = msg2.length();
	if (msg_len > (m_max_size - start_of_messages)) {
		throw AnException(0, FL, "Message size greater than max log file size.");
	}

	// Get the next index:
	IndexEntry* oldest = (*m_indexes)[m_index_header.oldest_entry];
	IndexEntry* newest = (*m_indexes)[m_index_header.newest_entry];

	// easy case: first one in
	if (m_index_header.record_count == 0) {
		IndexEntry* our_index = (*m_indexes)[0];
		our_index->id = msg.id;
		our_index->offset = start_of_messages;
		our_index->length = msg_len;

		m_index_header.record_count++;
		m_index_header.oldest_entry = 0;
		m_index_header.newest_entry = 0;
		(*m_log_ids)[our_index->id] = our_index;

		writeIndexHeader(); // Write the index header:
		writeIndexEntry(0); // Write the new index entry
		writeMessageEntry(0, msg2); // Write the new data
		fflush(m_log);

	} else if (m_index_header.record_count < m_index_header.index_count) {
		// Still available indexes to be used.
		int space_left;
		if (oldest->offset <= newest->offset) {
			// oldest still before newest in the physical layout.
			// Haven't wrapped the log yet.
			space_left = m_max_size - (newest->offset + newest->length);
			if (msg_len < space_left) {
				int which_index = m_index_header.record_count;
				IndexEntry* our_index = (*m_indexes)[which_index];
				our_index->id = msg.id;
				our_index->offset = newest->offset + newest->length;
				our_index->length = msg_len;

				m_index_header.newest_entry = which_index;
				m_index_header.record_count++;
				(*m_log_ids)[our_index->id] = our_index;

				writeIndexHeader(); // Write the index header:
				writeIndexEntry(which_index); // Write the new index entry
				writeMessageEntry(which_index, msg2); // Write the new data
				fflush(m_log);

			} else {
				// Not enough space at the end of the file.
				// If we are reusing the file, then wrap.
				// If not, shut the file down, open another and then
				// write the message again.
				if(m_reuse){
					throw AnException(0, FL, "Out of space.  Reuse not Implemented Yet.");
				} else {
					createNewFile();
				}
					
			}
		} else {
			// We've wrapped the log in terms of physical layout.
			// calculate how many messages need to be removed to open up
			// a gap big enough for us to write to.
			//throw AnException(0, FL, "Out of log space! Not Implemented Yet.");
			createNewFile();
		}
	} else {
		// We've run out of indexes. Start a new file for logging.
		createNewFile();
	}
	
}

int LogFile::messageCount() 
{
	Lock theLock(m_mutex);	
	return m_index_header.record_count;
}

vector<LogMsg*>* LogFile::getAllMessages() 
{
	Lock theLock(m_mutex);

	vector<LogMsg*>* ret = new vector<LogMsg*>();

	if (m_index_header.oldest_entry < m_index_header.newest_entry) {
		// run a straight loop to get them
		for (int i = m_index_header.oldest_entry; i <= m_index_header.newest_entry; i++)
		{
			try {
				ret->push_back(readMessageEntry( (*m_indexes)[i] ));
			} catch (AnException&) {
			}
		}
	} else {
		// if oldest is bigger than newest, we've looped the table.
		// first go from oldest to end of table:
		for (int i = m_index_header.oldest_entry; i < m_index_header.index_count; i++)
		{
			try {
				ret->push_back(readMessageEntry( (*m_indexes)[i] ));
			} catch (AnException&) {
			}
		}
		// Then go from beginning of table to newest
		for (int i = 0; i <= m_index_header.newest_entry; i++) {
			try {
				ret->push_back(readMessageEntry( (*m_indexes)[i] ));
			} catch (AnException&) {
			}
		}
	}

	return ret;
}

LogMsg* LogFile::getMessage(int id) 
{
	Lock theLock(m_mutex);
	
	if(m_log_ids->count(id) == 0){
		return NULL;
	}

	try {
		return readMessageEntry( (*m_log_ids)[id] );
	} catch (AnException&) {
		return NULL;
	}
}

int LogFile::getOldestMessageID() 
{
	Lock theLock(m_mutex);
	return (*m_indexes)[m_index_header.oldest_entry]->id;
}

int LogFile::getNewestMessageID() 
{
	Lock theLock(m_mutex);
	return (*m_indexes)[m_index_header.newest_entry]->id;
}

void LogFile::getStats(xmlNodePtr node)
{
	Lock theLock(m_mutex);
/*
	Document doc = node.getOwnerDocument();
	Element our_stats = doc.createElement("LogStats");
	our_stats.setAttribute("LogFile", m_file_name);
	Xml.setIntAttr(our_stats, "MaxSize", m_max_size);
	Xml.setIntAttr(our_stats, "IndexHeaderSize", INDEX_HEADER_SIZE);
	Xml.setIntAttr(our_stats, "IndexEntriesSize", m_index_header.index_count * INDEX_ENTRY_SIZE);
	Xml.setIntAttr(our_stats, "StringTableSize", m_string_table_size);
	Xml.setIntAttr(our_stats, "StringTableHeaderSize", STRINGTAB_HEADER_SIZE);
	Xml.setIntAttr(our_stats, "StringTableIndexSize", m_string_table_header.total_indexes * STRINGTAB_INDEX_ENTRY_SIZE);
	Xml.setIntAttr(our_stats, "StringTableDataArea", (m_string_table_header.total_size -
			STRINGTAB_HEADER_SIZE -
			(m_string_table_header.total_indexes * STRINGTAB_INDEX_ENTRY_SIZE)));
	Xml.setIntAttr(our_stats, "MessageDataArea",
			(m_max_size - (SIGNATURE_SIZE + INDEX_HEADER_SIZE + (m_index_header.index_count * INDEX_ENTRY_SIZE) +
					m_string_table_size)
			) );
	
	node.appendChild(our_stats);
	
	Element index_stats = doc.createElement("IndexStats");
	Xml.setIntAttr(index_stats, "RecordCount", m_index_header.record_count);
	Xml.setIntAttr(index_stats, "IndexCount", m_index_header.index_count);
	Xml.setIntAttr(index_stats, "OldestEntry", m_index_header.oldest_entry);
	Xml.setIntAttr(index_stats, "NewestEntry", m_index_header.newest_entry);
	our_stats.appendChild(index_stats);
	
	Element oldest = doc.createElement("OldestEntry");
	IndexEntry old = m_indexes->get(m_index_header.oldest_entry);
	Xml.setIntAttr(oldest, "id", old.id);
	Xml.setIntAttr(oldest, "length", old.length);
	Xml.setIntAttr(oldest, "offset", old.offset);
	index_stats.appendChild(oldest);
	
	Element newest = doc.createElement("NewestEntry");
	IndexEntry nw = m_indexes->get(m_index_header.newest_entry);
	Xml.setIntAttr(newest, "id", nw.id);
	Xml.setIntAttr(newest, "length", nw.length);
	Xml.setIntAttr(newest, "offset", nw.offset);
	index_stats.appendChild(newest);
	
	Element strings = doc.createElement("StringTable");
	Xml.setIntAttr(strings, "TotalSize", m_string_table_header.total_size);
	Xml.setIntAttr(strings, "TotalEntries", m_string_table_header.total_indexes);
	Xml.setIntAttr(strings, "EntriesInUse", m_string_table_header.index_in_use);
	int string_table_start = SIGNATURE_SIZE + INDEX_HEADER_SIZE + (m_index_header.index_count * INDEX_ENTRY_SIZE);
	int stringTableIndexSize = m_string_table_header.total_indexes * STRINGTAB_INDEX_ENTRY_SIZE;
	
	int space_used = 0;
	if(m_string_table_header.index_in_use != 0){
		StringTableIndex sti = m_string_indexes->get(m_string_table_header.index_in_use-1);
		space_used = (sti.offset + sti.length) - string_table_start - STRINGTAB_HEADER_SIZE - stringTableIndexSize;
	}
	
	Xml.setIntAttr(strings, "SpaceUsed", space_used);
	our_stats.appendChild(strings);
	*/
}

void LogFile::dumpLog() 
{
	printf("=========================== LOG DUMP =============================\n");
	dumpIndexAndStrings();
	dumpMessageData();
	printf("=========================== END LOG DUMP =========================\n");
}

void LogFile::dumpIndexAndStrings()
{
	Lock theLock(m_mutex);
	
	printf("=========================== LOG Index And Strings ================\n");
	printf("Total Log File Size     = %d\n", m_max_size);
	printf("Index Header Size       = %d\n", INDEX_HEADER_SIZE);
	printf("Index Entries Size      = %d\n", m_index_header.index_count * INDEX_ENTRY_SIZE);
	printf("Total String Table Size = %d\n", m_string_table_size);
	printf("String Table Header Siz = %d\n", STRINGTAB_HEADER_SIZE);
	printf("String Table Index Size = %d\n", m_string_table_header.total_indexes * STRINGTAB_INDEX_ENTRY_SIZE);
	printf("String Table Data Area  = %d\n", (m_string_table_header.total_size -
		STRINGTAB_HEADER_SIZE -
		(m_string_table_header.total_indexes * STRINGTAB_INDEX_ENTRY_SIZE)) );
	fflush(stdout);
	printf("Message Data Area       = %d\n",
		(m_max_size - 
		 (SIGNATURE_SIZE + INDEX_HEADER_SIZE + (m_index_header.index_count * INDEX_ENTRY_SIZE) +
				m_string_table_size)
		) );
	fflush(stdout);
	
	printf("=========================== Index Header =========================\n");
	printf("Record Count = %d\n", m_index_header.record_count);
	printf("Index Count  = %d\n", m_index_header.index_count);
	printf("Oldest Entry = %d\n", m_index_header.oldest_entry);
	printf("Newest Entry = %d\n", m_index_header.newest_entry);
	fflush(stdout);

	printf("=========================== String Table Header ==================\n");
	printf("Total Size     = %d\n", m_string_table_header.total_size);
	printf("Total Entries  = %d\n", m_string_table_header.total_indexes);
	printf("Entries In Use = %d\n", m_string_table_header.index_in_use);
	printf("=========================== String Table Entries =================\n");
	fflush(stdout);
	for(int i = 0; i < m_string_table_header.total_indexes; i++){
		StringTableIndex* sti = (*m_string_indexes)[i];
		if(sti->offset != 0){
			printf("String Table Index (%d) Offset (%d) Length (%d) String (%s)\n",
				i, sti->offset, sti->length, (*m_string_table_reverse)[sti]() );
		}
	}
	printf("=========================== End LOG Index And Strings =============\n");
	fflush(stdout);
}

void LogFile::dumpMessageData()
{
	Lock theLock(m_mutex);
	
	printf("=========================== Log Messages =========================\n");
	for(int i = 0; i < m_index_header.index_count; i++){
		IndexEntry* ie = (*m_indexes)[i];
		if(ie->offset != 0){
			try {
				dptr<LogMsg> lm = readMessageEntry(ie);
				char local_tmp[32];
				memset(local_tmp, 0, 32);

#ifdef _WIN32
				strftime(local_tmp, 32, "%Y/%m/%d %H:%M:%S", localtime(&(lm->timestamp.time)));
				printf("%d|%s.%.3d|%s|%s|%d|%s|%d|%d|%s\n",
					lm->id,
					local_tmp, (int)lm->timestamp.millitm,
					lm->machineName(),
					lm->appName(),
					lm->tid,
					lm->file(),
					lm->line,
					lm->channel,
					lm->msg()
				);
#else
				strftime(local_tmp, 32, "%Y/%m/%d %H:%M:%S", localtime(&(lm->timestamp.tv_sec)));
				printf("%d|%s.%.3d|%s|%s|%d|%s|%d|%d|%s\n",
					lm->id,
					local_tmp, (int)lm->timestamp.tv_usec,
					lm->machineName(),
					lm->appName(),
					lm->tid,
					lm->file(),
					lm->line,
					lm->channel,
					lm->msg()
				);
#endif

			} catch (AnException&){
				printf("Message ID(%d) offset(%d) length(%d)\n", ie->id, ie->offset, ie->length);
				printf("Error reading message from log file!\n");
			}
		}
	}
}

void LogFile::recoverLog(twine FileName) 
{
}

void LogFile::close() 
{
	Lock theLock(m_mutex);	

	if(m_log != NULL){
		fclose(m_log);
	}
	m_log = NULL;
}

void LogFile::createNewFile()
{
	// First close the log file
	close();
	
	// Then move it to a new name:
	Date d;
	twine newName = m_file_name + "." + d.GetValue("%Y%m%d%H%M%S"); 
	int res = rename( m_file_name(), newName() );
	if(res){
		throw AnException(0, FL, "Error renaming existing log file %s to %s",
			m_file_name(), newName() );
	}
	
	// Then create our new log file:
	createFile();
}

void LogFile::openFile()
{

	// Does the file exist?
	m_log = fopen(m_file_name(), "rb+"); // read and write anywhere in the file.
	if (m_log == NULL) {
		// File does not exist.
		m_log = NULL;
		return;
	}
	
	if( m_clear_at_startup ){
		// zero out the file.
		fclose(m_log);
		m_log = fopen(m_file_name(), "wb+"); // read and write anywhere after clearing the file.
		fclose(m_log);
		m_log = NULL;
		return;
	}

	// Check the signature:
	char test_signature[9];
	memset(test_signature, 0, 9);
	fread(test_signature, 8, 1, m_log);
	for (int i = 0; i < 8; i++) {
		if (test_signature[i] != m_signature[i]) {
			fclose(m_log);
			m_log = NULL;
			throw AnException(0, FL, "Not a Proper log file.  Invalid Signature");
		}
	}

	// Read our structures from it
	readLogHeaders();
}

void LogFile::readLogHeaders()
{
	//printf("reading log headers...\n");
	if (m_log == NULL) {
		return; // sanity check
	}

	try {
		// reset the FD back to the beginning of the file
		seek(8); // just past the signature

		// Read the Index Header information
		//printf("reading index headers...\n");
		m_index_header.record_count = readInt();
		m_index_header.index_count = readInt();
		m_index_header.oldest_entry = readInt();
		m_index_header.newest_entry = readInt();

		// Read all of the indexes
		clearIndexes();
		clearLogIds();
		//printf("Index Headers:\n");
		//printf("Record Count: %d\n", m_index_header.record_count);
		//printf("Index Count: %d\n", m_index_header.index_count);
		//printf("Oldest Entry: %d\n", m_index_header.oldest_entry);
		//printf("Newest Entry: %d\n", m_index_header.newest_entry);
		//printf("loading index entries...\n");
		for (int i = 0; i < m_index_header.index_count; i++) {
			IndexEntry* ie = new IndexEntry();
			ie->offset = readInt();
			ie->length = readInt();
			ie->id = readInt();

			(*m_log_ids)[ie->id] = ie;
			m_indexes->push_back(ie);
		}

		// Read our String table
		//printf("reading string table headers...\n");
		m_string_table_header.total_size = readInt();
		m_string_table_header.total_indexes = readInt();
		m_string_table_header.index_in_use = readInt();
		if(m_string_table_header.total_size == 0 ||
			m_string_table_header.total_indexes == 0
		){
			// Something is wrong with this log file. There is no string table, and nothing
			// in use.  Set the total indexes and index in use to 1 so that we'll avoid trying
			// to add anything else to this string table:
			m_string_table_header.total_indexes = 1;
			m_string_table_header.index_in_use = 1;
		}
		
		clearStringIndexes();
		clearStringTable();
		clearStringTableReverse();
		for (int i = 0; i < m_string_table_header.total_indexes; i++) {
			StringTableIndex* sti = new StringTableIndex();
			sti->offset = readInt();
			sti->length = readInt();

			m_string_indexes->push_back(sti);
		}
		
		for (int i = 0; i < m_string_table_header.index_in_use; i++) {
			StringTableIndex* sti = (*m_string_indexes)[i];
			seek(sti->offset);
			twine tmp = readTwine(sti->length);

			(*m_string_table)[tmp] = sti;
			(*m_string_table_reverse)[sti] = tmp;
		}

	} catch (AnException& e) {
		try {
			fclose(m_log);
		} catch (...) {
			throw;
		}

		m_log = NULL;
		throw e;
	}

}

void LogFile::createFile()
{
	// Try to open it.
	m_log = fopen(m_file_name(), "wb+"); // read and write anywhere after clearing the file.
	if(m_log == NULL){
		// Somethine went wrong trying to open it.
		throw AnException(0, FL, "Error opening our new log file.");
	}

	// Write our signature to the file:
	fwrite(m_signature, 8, 1, m_log);

	// Figure out how big everything should be
	// 10M max means:
	// index header = 16
	// index entries = 12 * 42,000 (max_indexes)
	// String table header = 12
	// String table indexes = 8 * 10,000 (max_strings)
	// String table size = 1M
	// Message size (on average) 12 + 200

	// Write the Index Header information
	m_index_header.record_count = 0;
	m_index_header.index_count = m_max_entries;
	m_index_header.oldest_entry = 0;
	m_index_header.newest_entry = 0;
	writeIndexHeader();

	// Write all of the indexes
	clearIndexes();
	clearLogIds();
	for (int i = 0; i < m_index_header.index_count; i++) {
		IndexEntry* ie = new IndexEntry();
		ie->offset = 0;
		ie->length = 0;
		ie->id = 0;
		m_indexes->push_back(ie);
	}
	int len = m_index_header.index_count * INDEX_ENTRY_SIZE ;
	void* bytes = malloc( len );
	if(bytes == NULL){
		throw AnException(0, FL, "Error allocating memory for the write.");
	}
	memset(bytes, 0, len );
	fwrite( bytes, len, 1, m_log);
	free(bytes);

	// Write our String table
	m_string_table_header.total_size = m_string_table_size;
	m_string_table_header.total_indexes = m_max_strings;
	m_string_table_header.index_in_use = 0;

	write( m_string_table_header.total_size );
	write( m_string_table_header.total_indexes );
	write( m_string_table_header.index_in_use );
	
	clearStringIndexes();
	clearStringTable();
	clearStringTableReverse();
	for (int i = 0; i < m_string_table_header.total_indexes; i++) {
		StringTableIndex* sti = new StringTableIndex();
		sti->offset = 0;
		sti->length = 0;
		m_string_indexes->push_back(sti);
	}
	len = m_string_table_header.total_indexes * STRINGTAB_INDEX_ENTRY_SIZE;
	bytes = malloc( len );
	if(bytes == NULL){
		throw AnException(0, FL, "Error allocating memory for the write.");
	}
	memset(bytes, 0, len );
	fwrite( bytes, len, 1, m_log);
	free(bytes);
	
	// Zero the rest of the string table.
	len = m_string_table_header.total_size - len;
	bytes = malloc( len );
	if(bytes == NULL){
		throw AnException(0, FL, "Error allocating memory for the write.");
	}
	memset(bytes, 0, len );
	fwrite( bytes, len, 1, m_log);
	free(bytes);

}

int LogFile::addStringTableEntry(twine str)
{
	// check to see if it's already in there.
	if (m_string_table->count(str) > 0) {
		StringTableIndex* sti = (*m_string_table)[str];
		// Find the actual index:
		for(int i = 0; i < (int)m_string_indexes->size(); i++){
			if((*m_string_indexes)[i] == sti){
				return i;
			}
		}
		throw AnException(0, FL, "Could not find our StringTableIndex in the m_string_indexes vector!");
	}

	int string_table_start = SIGNATURE_SIZE + INDEX_HEADER_SIZE
			+ (m_index_header.index_count * INDEX_ENTRY_SIZE);

	// If we get to here, we have to add it.
	StringTableIndex* ret;
	if (m_string_table_header.index_in_use != 0) {
		if (m_string_table_header.index_in_use == m_string_table_header.total_indexes)
		{
			// String table is full.
			return -1;
		}
		StringTableIndex* last = (*m_string_indexes)[m_string_table_header.index_in_use - 1];
		ret = (*m_string_indexes)[m_string_table_header.index_in_use];
		ret->offset = last->offset + last->length;
		ret->length = str.length();
		m_string_table_header.index_in_use++;
	} else {
		// First one in
		ret = (*m_string_indexes)[m_string_table_header.index_in_use];
		ret->offset = string_table_start
				+ STRINGTAB_HEADER_SIZE
				+ (m_string_table_header.total_indexes * STRINGTAB_INDEX_ENTRY_SIZE);
		ret->length = str.length();
		m_string_table_header.index_in_use = 1;
	}

	// Is there enough room for it to fit?
	int end_of_table = string_table_start + m_string_table_header.total_size;

	if (ret->offset + ret->length > end_of_table) {
		// String is too big. Don't save it in our table.
		ret->offset = 0;
		ret->length = 0;
		m_string_table_header.index_in_use--;
		return -1;
	}

	// Write out the updated string table header
	seek(string_table_start);
	write(m_string_table_header.total_size);
	write(m_string_table_header.total_indexes);
	write(m_string_table_header.index_in_use);

	// write out the updated string index
	seek(string_table_start
			+ STRINGTAB_HEADER_SIZE
			+ ((m_string_table_header.index_in_use - 1) * STRINGTAB_INDEX_ENTRY_SIZE));
	
	write(ret->offset);
	write(ret->length);

	// Write out the new string itself
	seek(ret->offset);
	write(str);

	// Add the new string to our string table
	(*m_string_table)[str] = ret;
	(*m_string_table_reverse)[ret] = str;

	// return it's index entry
	return m_string_table_header.index_in_use - 1;
}

void LogFile::writeIndexHeader()
{
	seek(SIGNATURE_SIZE);
	
	write(m_index_header.record_count);
	write(m_index_header.index_count);
	write(m_index_header.oldest_entry);
	write(m_index_header.newest_entry);
	
}

void LogFile::writeIndexEntry(int which_index)
{
	seek(SIGNATURE_SIZE + INDEX_HEADER_SIZE + (which_index * INDEX_ENTRY_SIZE));

	IndexEntry* ie = (*m_indexes)[which_index];
	
	write(ie->offset);
	write(ie->length);
	write(ie->id);
}

void LogFile::write(int32_t value)
{
	if(m_log == NULL){
		throw AnException(0, FL, "Trying to write to a log file that has not been opened.");
	}
	fwrite( &value, sizeof(int32_t), 1, m_log);
}

int32_t LogFile::readInt()
{
	if(m_log == NULL){
		throw AnException(0, FL, "Trying to write to a log file that has not been opened.");
	}
	int ret = 0;
	size_t count = fread ( &ret, sizeof(int32_t), 1, m_log);
	if(count != 1){
		throw AnException(0, FL, "Error reading an int from our log file.");
	}
	return ret;
}

void LogFile::write(twine& value)
{
	if(m_log == NULL){
		throw AnException(0, FL, "Trying to write to a log file that has not been opened.");
	}
	fwrite( value.data(), value.length(), 1, m_log);
}

twine LogFile::readTwine(size_t length)
{
	if(m_log == NULL){
		throw AnException(0, FL, "Trying to write to a log file that has not been opened.");
	}
	twine ret;
	ret.reserve(length);
	size_t count = fread ( ret.data(), length, 1, m_log);
	if(count != 1){
		throw AnException(0, FL, "Error reading a twine from our log file.");
	}
	ret.check_size();
	return ret;
}

void LogFile::write(twine& value, int stringTableIndex)
{
	if(m_log == NULL){
		throw AnException(0, FL, "Trying to write to a log file that has not been opened.");
	}

	// If it's a string ID, just write the id. Otherwise write the whole string.
	if (stringTableIndex != -1) {
		write(stringTableIndex);
	} else {
		write((int)value.length());
		write(value);
	}
}

void LogFile::seek(long offsetFromStart)
{
	if(m_log == NULL){
		throw AnException(0, FL, "Trying to write to a log file that has not been opened.");
	}
	fseek(m_log, offsetFromStart, SEEK_SET);
}


void LogFile::writeMessageEntry(int which_index, LogMsgStripped& msg)
{
	IndexEntry* ie = (*m_indexes)[which_index];
	
	seek(ie->offset);

	write(MESSAGE_ENTRY_EYE_CATCHER);
	write(msg.id);
	write(which_index);
#ifdef _WIN32
	write((long)msg.timestamp.time);
	write((long)msg.timestamp.millitm);
#else
	write((long)msg.timestamp.tv_sec);
	write((long)msg.timestamp.tv_usec);
#endif
	write(msg.line);
	write(msg.channel);
#ifdef _WIN32
	write((int)msg.tid);
#else
	write((intptr_t)msg.tid);
#endif

	int flags = 0;
	if (msg.app_id != -1) {
		flags += 1;
	}
	if (msg.file_id != -1) {
		flags += 2;
	}
	if (msg.msg_id != -1) {
		flags += 4;
	}
	if (msg.machine_id != -1) {
		flags += 8;
	}
	
	write(flags);
	write(msg.appName, msg.app_id);
	write(msg.file, msg.file_id);
	write(msg.msg, msg.msg_id);
	write(msg.machineName, msg.machine_id);

}

LogMsg* LogFile::readMessageEntry(IndexEntry* ie)
{
	int test, string_id;
	dptr<LogMsg> msg; msg = new LogMsg(); // don't leak memory
	if(ie->offset == 0){
		throw AnException(0, FL, "%d is not a valid index entry", ie->offset);
	}
	if(m_log == NULL){
		throw AnException(0, FL, "log file is closed");
	}
	
	seek(ie->offset);
	test = readInt();
	if (test != MESSAGE_ENTRY_EYE_CATCHER ) {
		throw AnException(0, FL, "Read of message based on index entry did not succeed!");
	}

	msg->id = readInt();
	test = readInt(); // index
#ifdef _WIN32
	msg->timestamp.time = readInt();
	msg->timestamp.millitm = (unsigned short)readInt();
#else
	msg->timestamp.tv_sec = readInt();
	msg->timestamp.tv_usec = readInt();
#endif
	msg->line = readInt();
	msg->channel = readInt();
	msg->tid = readInt();

	test = readInt();
	if ((test & 1) == 1) { // first bit is for stringified app_id.
		// app_id is a string index
		string_id = readInt();
		msg->appName = (*m_string_table_reverse)[(*m_string_indexes)[string_id]];
	} else {
		string_id = readInt(); // this is string length
		msg->appName = readTwine( string_id );
	}

	if ((test & 2) == 2) { // second bit is for stringified file.
		// File is a string index
		string_id = readInt();
		msg->file = (*m_string_table_reverse)[(*m_string_indexes)[string_id]];
	} else {
		string_id = readInt(); // this is string length
		msg->file = readTwine( string_id );
	}

	if ((test & 4) == 4) { // third bit is for stringified message
		// message is a string index
		string_id = readInt();
		msg->msg = (*m_string_table_reverse)[(*m_string_indexes)[string_id]];
		msg->msg_static = true;
	} else {
		string_id = readInt(); // this is string length
		msg->msg = readTwine( string_id );
		msg->msg_static = false;
	}

	if ((test & 8) == 8) { // fourth bit is for stringified machine.
		// File is a string index
		string_id = readInt();
		msg->machineName = (*m_string_table_reverse)[(*m_string_indexes)[string_id]];
	} else {
		string_id = readInt(); // this is string length
		msg->machineName = readTwine( string_id );
	}

	return msg.release(); // up to the caller to handle it now.
}

void LogFile::clearIndexes()
{
	if(m_indexes != NULL){
		for(int i = 0; i < (int)m_indexes->size(); i++){
			delete (*m_indexes)[i];
		}
		delete m_indexes;
		m_indexes = NULL;
	}
	m_indexes = new vector<IndexEntry*>();
}

void LogFile::clearLogIds()
{
	// m_indexes owns the IndexEntry pointers.  Don't double-delete
	// them here.  Just clear the lookup table.
	if(m_log_ids != NULL){
		delete m_log_ids;
		m_log_ids = NULL;
	}
	m_log_ids = new map<int, IndexEntry*>();
}

void LogFile::clearStringIndexes()
{
	if(m_string_indexes != NULL){
		for(int i = 0; i < (int)m_string_indexes->size(); i++){
			delete (*m_string_indexes)[i];
		}
		delete m_string_indexes;
		m_string_indexes = NULL;
	}
	m_string_indexes = new vector<StringTableIndex*>();
}

void LogFile::clearStringTable()
{
	// m_string_indexes owns the StringTableIndex pointers.  Don't double-delete
	// them here.  Just clear the lookup table.
	if(m_string_table != NULL){
		delete m_string_table;
		m_string_table = NULL;
	}
	m_string_table = new map<twine, StringTableIndex*>();
}

void LogFile::clearStringTableReverse()
{
	// m_string_indexes owns the StringTableIndex pointers.  Don't double-delete
	// them here.  Just clear the lookup table.
	if(m_string_table_reverse != NULL){
		delete m_string_table_reverse;
		m_string_table_reverse = NULL;
	}
	m_string_table_reverse = new map<StringTableIndex*, twine>();
}
