#include "File.h"
#include "AnException.h"
#include "EnEx.h"
using namespace SLib;

#ifdef _WIN32
#include <direct.h>
#else
#include <dirent.h>
#include <unistd.h>
#include <dlfcn.h> // for dlopen, etc.
#endif
#include <errno.h>

static bool _slib_file_loaded = false;
static char _slib_file_our_binary[1024];
static char _slib_file_our_folder[1024];

File::File()
{
	EnEx ee("File::File()");
	m_fp = NULL;
}

File::File(const twine& fileName) : m_fileName(fileName)
{
	EnEx ee("File::File(const twine fileName)");
	m_fp = fopen(m_fileName(), "rb");
	if(m_fp == NULL){
		throw AnException(0, FL, "Error opening file (%s).", m_fileName() );
	}
	getStat();
}

File::File(FILE* fp) : m_fp(fp)
{
	EnEx ee("File::File(File* fp)");
	getStat();
}

File::File(const File& f)
{
	EnEx ee("File::File(const File& f)");
	m_fp = f.m_fp;
	m_fileName = f.m_fileName;
	getStat();
}

File::~File()
{
	EnEx ee("File::~File()");
	closeFile();
}

File& File::open(const twine& fileName)
{
	EnEx ee("File::open(const twine fileName)");
	closeFile();

	m_fileName = fileName;
	m_fp = fopen(m_fileName(), "rb");
	if(m_fp == NULL){
		throw AnException(0, FL, "Error opening file (%s).", m_fileName() );
	}
	getStat();
	return *this;
}

File& File::create(const twine& fileName)
{
	EnEx ee("File::create(const twine fileName)");
	closeFile();

	m_fileName = fileName;
	m_fp = fopen(m_fileName(), "w+b");
	if(m_fp == NULL){
		throw AnException(0, FL, "Error creating file (%s).", m_fileName() );
	}
	return *this;
}

void File::closeFile()
{
	EnEx ee("File::closeFile()");
	if(m_fp != NULL){
		fclose(m_fp);
	}
	m_fp = NULL;
	m_fileName = "";
}

void File::getStat()
{
	EnEx ee("File::getStat()");
#ifdef _WIN32
	_fstat(_fileno(m_fp), &m_stat);
#else
	fstat(fileno(m_fp), &m_stat);
#endif
}

File& File::operator=(const File& f)
{
	EnEx ee("File::operator=(const File& f)");
	if(&f == this){
		// Don't copy onto ourselves.
		return *this;
	}
	closeFile();
	m_fp = f.m_fp;
	m_fileName = f.m_fileName;
	getStat();
	return *this;
}
		
File& File::operator=(FILE* fp)
{
	EnEx ee("File::operator=(FILE* fp)");
	if(m_fp == fp){
		// Don't copy onto ourselves.
		return *this;
	}
	closeFile();
	m_fp = fp;
	m_fileName = "";
	getStat();
	return *this;
}

File::operator FILE*(void) const
{
	EnEx ee("File::operator FILE*(void)");
	return m_fp;
}

bool File::operator==(File& f) const
{
	EnEx ee("File::operator==(File& f)");
	if(m_fp == f.m_fp){
		return true;
	} else {
		return false;
	}

}

twine& File::name()
{
	EnEx ee("File::name()");
	return m_fileName;
}

long File::size()
{
	EnEx ee("File::size()");
	return m_stat.st_size;
}

Date File::lastAccess()
{
	EnEx ee("File::lastAccess()");
	Date ret;
	ret.SetValue(m_stat.st_atime);
	return ret;
}

Date File::lastModified()
{
	EnEx ee("File::lastModified()");
	Date ret;
	ret.SetValue(m_stat.st_mtime);
	return ret;
}

Date File::lastStatusChange()
{
	EnEx ee("File::lastStatusChange()");
	Date ret;
	ret.SetValue(m_stat.st_ctime);
	return ret;
}

unsigned char* File::readContents()
{
	EnEx ee("File::readContents()");

	unsigned char* buff = (unsigned char*)malloc(size());
	if(buff == NULL){
		throw AnException(0, FL, "Error allocating memory for the read.");
	}

	// Read the whole file in at once.
	getStat();
	rewind( m_fp ); // Ensure we start at the beginning
	size_t ret = fread(buff, size(), 1, m_fp);

	if(ret != 1){
		free(buff);
		throw AnException(0, FL, "Error reading from file (%s)", m_fileName());
	}
	return buff;
}

MemBuf& File::readContents(MemBuf& contents)
{
	EnEx ee("File::readContentsAsMemBuf()");

	getStat();
	contents.reserve( size() );

	if(size() != 0){
		// Read the whole file in at once.
		rewind( m_fp ); // Ensure we start at the beginning
		size_t ret = fread(contents.data(), size(), 1, m_fp);

		if(ret != 1){
			throw AnException(0, FL, "Error reading from file (%s)", m_fileName());
		}
	}
	return contents;
}

twine File::readContentsAsTwine()
{
	EnEx ee("File::readContentsTwine()");

	twine contents;
	getStat();
	size_t content_size = size();
	contents.reserve(content_size);

	// Read the whole file in at once.
	rewind( m_fp ); // Ensure we start at the beginning
	size_t ret = fread(contents.data(), content_size, 1, m_fp);

	if(ret != 1){
		throw AnException(0, FL, "Error reading from file (%s)", m_fileName());
	}
	// Directly set the size instead of using check_size to avoid strlen calls.
	contents.size(content_size);
	return contents;
}

vector<twine> File::readLines()
{
	EnEx ee("File::readLines()");

	twine contents;
	contents.reserve(size());

	// Read the whole file in at once.
	getStat();
	rewind( m_fp ); // Ensure we start at the beginning
	size_t ret = fread(contents.data(), size(), 1, m_fp);

	if(ret != 1){
		throw AnException(0, FL, "Error reading from file (%s)", m_fileName());
	}
	contents.check_size();

	vector<twine> lines = contents.split("\n");
	for(size_t i = 0; i < lines.size(); i++){
		lines[i].rtrim();
	}

	return lines;

}

size_t File::read(MemBuf& buffer)
{
	EnEx ee("File::read(MemBuf& buffer)");

	size_t ret = fread(buffer.data(), 1, buffer.size(), m_fp);
	return ret;
}

size_t File::write(MemBuf& buffer)
{
	EnEx ee("File::write(MemBuf& buffer)");

	size_t ret = fwrite(buffer.data(), 1, buffer.size(), m_fp);
	return ret;
}

void File::flush()
{
	EnEx ee("File::flush()");

	fflush(m_fp);
}

bool File::Exists(const twine& fileName)
{
	EnEx ee("File::Exists(const twine& fileName)");

	FILE* fp = fopen(fileName(), "rb");
	if(fp == NULL){
		return false; // doesn't exist.
	}
	// does exist.
	fclose(fp);
	return true;
}

vector<twine> File::listFiles(const twine& dirName)
{
	EnEx ee("File::listFiles()");
	vector<twine> ret;

#ifdef _WIN32
	// Works on windows:
	twine wildCard = dirName + "\\*";

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(wildCard(), &ffd);
	if(INVALID_HANDLE_VALUE == hFind){
		throw AnException(0, FL, "No files found in directory: (%s)", dirName() );
	}
	do {
		if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			// This is a directory, skip it.
		} else {
			// This is a file.
			ret.push_back( twine(ffd.cFileName) );
		}
	} while( FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

#else
	DIR* dirp = opendir( dirName() );
	if(dirp == NULL){
		throw AnException(0, FL, "Erorr opening directory (%s) to list files.", dirName() );
	}
	struct dirent* ent;
	while( (ent = readdir(dirp)) != NULL){
		if(ent->d_type == DT_REG){
			ret.push_back( twine( ent->d_name ) );
		} else if(ent->d_type == DT_UNKNOWN){
			// If we can't determine the type, go through the expense of using a stat call
			// to determine the type.  it's more expensive, but the above d_type should work
			// on most filesystems.
			twine fullpath = dirName;
			fullpath.append( "/" );
			fullpath.append( ent->d_name );
			struct stat stbuf;
			if(stat( fullpath(), &stbuf ) == 0){
				// stat retured success, check the mode to see if it's a directory.
				if( S_ISREG( stbuf.st_mode ) ){
					ret.push_back( twine( ent->d_name ) ); // it's a directory
				}
			}
		}
	}
	closedir( dirp );
#endif

	return ret;
}

vector<twine> File::listFolders(const twine& dirName)
{
	EnEx ee("File::listFolders()");
	vector<twine> ret;

#ifdef _WIN32
	// Works on windows:
	twine wildCard = dirName + "\\*";

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(wildCard(), &ffd);
	if(INVALID_HANDLE_VALUE == hFind){
		throw AnException(0, FL, "No files found in directory: (%s)", dirName() );
	}
	do {
		if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			// This is a directory
			ret.push_back( twine(ffd.cFileName) );
		} else {
			// This is a file.
		}
	} while( FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

#else
	DIR* dirp = opendir( dirName() );
	if(dirp == NULL){
		throw AnException(0, FL, "Erorr opening directory (%s) to list directories.", dirName() );
	}
	struct dirent* ent;
	while( (ent = readdir(dirp)) != NULL){
		if(ent->d_type == DT_DIR){
			ret.push_back( twine( ent->d_name ) );
		} else if(ent->d_type == DT_UNKNOWN){
			// If we can't determine the type, go through the expense of using a stat call
			// to determine the type.  it's more expensive, but the above d_type should work
			// on most filesystems.
			twine fullpath = dirName;
			fullpath.append( "/" );
			fullpath.append( ent->d_name );
			struct stat stbuf;
			if(stat( fullpath(), &stbuf ) == 0){
				// stat retured success, check the mode to see if it's a directory.
				if( S_ISDIR( stbuf.st_mode ) ){
					ret.push_back( twine( ent->d_name ) ); // it's a directory
				}
			}
		}
	}
	closedir( dirp );
#endif

	return ret;
}

void File::writeToFile(const twine& fileName, const twine& contents)
{

	FILE* fp = fopen(fileName(), "wb");
	if(fp == NULL){
		throw AnException(0, FL, "Error opening file(%s) in write mode!", fileName() );
	}

	if(contents.size() == 0){
		// nothing to write:
		fclose(fp);
		return;
	}
	size_t ret = fwrite(contents(), contents.size(), 1, fp);
	fclose(fp);
	if(ret != 1){
		throw AnException(0, FL, "Error writing contents to file.");
	}
}

void File::writeToFile(const twine& fileName, const MemBuf& contents)
{

	FILE* fp = fopen(fileName(), "wb");
	if(fp == NULL){
		throw AnException(0, FL, "Error opening file(%s) in write mode!", fileName() );
	}

	if(contents.size() == 0){
		// nothing to write:
		fclose(fp);
		return;
	}
	size_t ret = fwrite(contents(), contents.size(), 1, fp);
	fclose(fp);
	if(ret != 1){
		throw AnException(0, FL, "Error writing contents to file.");
	}
}

void File::CopyPermissionsTo(const twine& to)
{
	EnEx ee("File::CopyPermissionsTo(twine& to)");

	// Copy over our permissions to a different file
#ifndef _WIN32
	chmod( to(), m_stat.st_mode );
#endif

	// The windows version is _chmod but it doesn't do u+g+o, and it has no concept of 
	// the executable bit.  So it really only controls the write flag.  Read these
	// for reference:
	// https://stackoverflow.com/questions/592448/c-how-to-set-file-permissions-cross-platform
	// and
	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/chmod-wchmod?view=vs-2017
	//
	// For now, we've chosen to do nothing in this function for windows.

}

void File::Copy(const twine& from, const twine& to)
{
	EnEx ee("File::Copy(twine& from, twine& to)");

	File f(from);
	MemBuf contents;
	f.readContents(contents);
	File::writeToFile( to, contents );

	// Copy the file permissions as well:
	f.CopyPermissionsTo( to );
}

void File::Move(const twine& from, const twine& to)
{
	EnEx ee("File::Move(twine& from, twine& to)");

#ifdef _WIN32
	BOOL ret = MoveFile( from(), to() );
	if(!ret){
		throw AnException(0, FL, "Failed to move file (%s) to (%s)", from(), to() );
	}
#endif

}

void File::EnsurePath(const twine& fileName)
{
	EnEx ee("File::EnsurePath(const twine& fileName)");

	if(fileName.length() == 0) return; // nothing to do

	vector<twine> segments = fileName.tokenize( "/\\" ); // Split up on forward and back slash
	twine startingPath;

	// What kind of path prefix did we have?
	if(fileName[0] == '/') { // Unix style absolute path
		startingPath = "/";
	} else if( fileName.length() >= 2 && fileName[1] == ':' ){ // Windows style absolute path
		startingPath = segments[0];
		segments.erase( segments.begin() ); // remove the first segment - it's in startingPath
	} else if( fileName.length() >= 2 && fileName[0] == '\\' && fileName[1] == '\\'){ // Windows style network path
		startingPath = "\\\\" + segments[0];
		segments.erase( segments.begin() ); // remove the first segment - it's in startingPath
	} else if( fileName.length() >= 2 && fileName[0] == '/' && fileName[1] == '/'){ // Windows style network path
		startingPath = "//" + segments[0];
		segments.erase( segments.begin() ); // remove the first segment - it's in startingPath
	} else {
		startingPath = ".";
	}

	size_t limit;
	if(fileName.endsWith("/") || fileName.endsWith("\\")){
		limit = segments.size(); // go all the way to the end
	} else {
		limit = segments.size() - 1; // Leave off the last one
	}

	for(size_t i = 0; i < limit; i++){
		if(segments[i].length() == 0 ||
			segments[i] == "."
		){
			continue; // skip these
		}

		// Check to see if the current segment exists:
		vector<twine> folders;
		try {
			folders = File::listFolders( startingPath );
		} catch (AnException& e){
			// ignore empty lists.
		}

		bool found = false;
		for(auto& folder : folders){
			if(folder == segments[i]){
				found = true;
				break;
			}
		}
		startingPath = PathCombine(startingPath, segments[i]);
		if(!found){
			// Create the new directory:
#ifdef _WIN32
			_mkdir( startingPath() );
#else
			mkdir( startingPath(), 0777 );
#endif
		}
	}
}

void File::RmDir(const twine& dirName)
{
	EnEx ee("File::RmDir(const twine& dirName)");

	if(dirName.length() == 0){
		throw AnException(0, FL, "Empty directory name passed to File::RmDir()");
	}

	// Ensure it is empty
	vector<twine> files; 
	vector<twine> folders; 
	try {
		files = File::listFiles( dirName );
	} catch (AnException& e){
		// ignore this - it just means there are no files in the folder.
	}
	try {
		folders = File::listFolders( dirName );
	} catch (AnException& e){
		// ignore this - it just means there are no folders in the folder.
	}

	for(size_t i = 0; i < files.size(); i++){
		File::Delete( dirName + "/" + files[i] );
	}

	for(size_t i = 0; i < folders.size(); i++){
		if(folders[i] != "." && folders[i] != ".." ){
			File::RmDir( dirName + "/" + folders[i] );
		}
	}
		
	// Now delete the directory itself:
#ifdef _WIN32
	_rmdir( dirName() );
#else
	rmdir( dirName() );
#endif

}

twine File::PathCombine(const twine& prefix, const twine& suffix)
{
	EnEx ee("File::PathCombine(const twine& prefix, const twine& suffix)");

	twine ret(prefix);
	if(!ret.endsWith("/") && !ret.endsWith("\\")){
		ret.append("/");
	}
	if(suffix.startsWith("/") || suffix.startsWith("\\")){
		if(suffix.length() > 1){
			ret.append( suffix.substr(1) );
		} else {
			// nothing to do
		}
	} else {
		ret.append( suffix );
	}
	return ret;
}

twine File::NormalizePath( const twine& path )
{
	EnEx ee("File::NormalizePath(const twine& path)");

	// We parse up the path to be able to manipulate it and put it back together
	DEBUG(FL, "NormalizePath: input path --%s--", path() );
	//printf("NormalizePath: input path --%s--\n", path() );

	vector<twine> segments = path.tokenize( "/\\" ); // Split up on forward and back slash
	//for(auto& seg : segments) printf("NormalizePath: segment --%s--\n", seg() );

	twine startingPath;

	// What kind of path prefix did we have?
	if(path.length() >= 2 && path[0] == '/' && path[1] != '/' ) { // Unix style absolute path
		startingPath = "/";
	} else if( path.length() >= 2 && path[1] == ':' ){ // Windows style absolute path
		startingPath = segments[0];
		segments.erase( segments.begin() ); // remove the first segment - it's in startingPath
	} else if( path.length() >= 2 && path[0] == '\\' && path[1] == '\\'){ // Windows style network path
		startingPath = "\\\\" + segments[0];
		segments.erase( segments.begin() ); // remove the first segment - it's in startingPath
	} else if( path.length() >= 2 && path[0] == '/' && path[1] == '/'){ // Windows style network path
		startingPath = "//";
	} else {
		startingPath = ".";
	}
	//printf("NormalizePath: startingPath --%s--\n", startingPath() );

	// Walk all of the segments to remove any empty parts, and handle any ../ portions
	for(int i = 0; i < segments.size(); i++){
		if(segments[i].length() == 0 || segments[i] == "."){
			segments.erase( segments.begin() + i ); // remove these
			i -= 1;
			continue;
		}

		if(segments[i] == ".."){
			if(i == 0){ // We're at the beginning of the path - can go backwards from here
				throw AnException(0, FL, "Invalid path: %s", path() );
			}
			segments.erase( segments.begin() + i ); // erase the ..			
			segments.erase( segments.begin() + i - 1); // erase the one before it
			i -= 2; // adjust the loop pointer
			continue;
		}
	}

	// Now combine all of the segments together with the starting path
	for(auto& seg : segments) {
		startingPath = PathCombine( startingPath, seg );
		//printf("NormalizePath: added seg: --%s-- for new startingPath --%s--\n", seg(), startingPath() );
	}

	// Return the full path
	return startingPath;
}

twine File::FileName( const twine& path)
{
	EnEx ee("File::FileName( const twine& path)");

	if(path.empty()){
		return path; // nothing to do
	}

	// First normalize the path:
	twine normalPath( NormalizePath( path ) );

	// Then find the last segment:
	size_t lastSlash = normalPath.rfind( "/" );
	if(lastSlash == TWINE_NOT_FOUND){
		return path; // No path separators found
	}

	if(lastSlash == (normalPath.size() - 1)){
		return ""; // No file name after the last slash
	}

	// Return whatever is after the last slash
	return normalPath.substr( lastSlash + 1 );
}

twine File::Directory( const twine& path)
{
	EnEx ee("File::Directory( const twine& path)");

	if(path.empty()){
		return path; // nothing to do
	}

	// First normalize the path:
	twine normalPath( NormalizePath( path ) );

	// Then find the last segment:
	size_t lastSlash = normalPath.rfind( "/" );
	if(lastSlash == TWINE_NOT_FOUND){
		return ""; // No path separators found
	}

	// erase from the last slash to the end
	normalPath.erase( lastSlash );
	return normalPath;
}

twine File::Pwd()
{
	EnEx ee("File::Pwd()");

	twine ret;
	ret.reserve( 4096 );
	char* checkptr;
#ifdef _WIN32
	checkptr = _getcwd( ret.data(), (int)ret.capacity() );
#else
	checkptr = getcwd( ret.data(), ret.capacity() );
#endif
	if(checkptr == NULL){
		if(errno == EINVAL){
			throw AnException(0, FL, "Error getting cwd - size is zero");
		} else if(errno == ENOENT){
			throw AnException(0, FL, "Error getting cwd - a component of the pathname no longer exists.");
		} else if(errno == ENOMEM){
			throw AnException(0, FL, "Error getting cwd - insufficient memory to store the pathname.");
		} else if(errno == ERANGE){
			throw AnException(0, FL, "Error getting cwd - 4096 is not large enough to store the pathname.");
		} else {
			throw AnException(0, FL, "Error getting cwd - unknown errno");
		}
	}

	ret.check_size();
	return ret;
}

twine File::OurLocation()
{
	EnEx ee("File::OurLocation()");

	//twine log;
	//twine msg;
	// Have we already looked up ourselves:
	if( _slib_file_loaded == true ){
		twine cached( _slib_file_our_folder ); // Skip the first 4 bytes
		//msg.format("Returning cached value: %s\n", cached() ); log.append(msg);
		//return log;
		return cached;
	}

	// Use windows/linux symbol lookup functions to find out where our binary is located.
	// Our config file needs to live where our binary is located.
	memset(_slib_file_our_binary, 0, sizeof(_slib_file_our_binary));
#ifdef _WIN32
	HMODULE hm = NULL;
	BOOL rc = FALSE;
	rc = GetModuleHandleEx( 
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR)_slib_file_our_binary, 
		&hm
	);
	if(rc == 0){
		int err = GetLastError();
		throw AnException(0, FL, "Error looking up our location: %d", err );
	}
	if(GetModuleFileName(hm, _slib_file_our_binary, sizeof(_slib_file_our_binary)) == 0){
		int err = GetLastError();
		throw AnException(0, FL, "Error looking up our location name: %d", err );
	}
#else
	Dl_info dlinfo;
	int ret = dladdr((void*)OurLocation, &dlinfo);
	if(ret == 0){
		throw AnException(0, FL, "Error looking up our location");
	}
	strcpy( _slib_file_our_binary, dlinfo.dli_fname );
	twine binaryPath( _slib_file_our_binary );
	if(binaryPath[0] != '/'){
		// This is not an absolute path - need to dig further to figure it out
		FILE* proc_self_maps = fopen( "/proc/self/maps", "r" );
		if(proc_self_maps == NULL){
			throw AnException(0, FL, "Error reading from /proc/self/maps");
		}
		const size_t BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE] = "";
		char path[BUFFER_SIZE] = "";
		while(fgets(buffer, BUFFER_SIZE, proc_self_maps)){
			if(sscanf( buffer, "%*llx-%*llx %*s %*s %*s %*s %s", path) == 1){
				//msg.format( "Examining path: %s\n", path); log.append(msg);
				twine fileName = File::FileName( path );
				if(fileName == binaryPath){
					//msg.format( "Found our match: %s\n", path); log.append(msg);
					strcpy( _slib_file_our_binary, path );
					break;
				}
			}
		}
		fclose( proc_self_maps );
	}

#endif
	//msg.format("Our binary lives here: %s\n", _slib_file_our_binary); log.append(msg);
	twine tmp( _slib_file_our_binary );
	tmp = File::NormalizePath( tmp );
	//msg.format("After normalizing: %s\n", tmp()); log.append(msg);
	if(tmp.startsWith( "./" )){
		tmp = File::Pwd();
	} else {
		size_t idx = tmp.rfind( "/" );
		if(idx == TWINE_NOT_FOUND){
			throw AnException(0, FL, "Invalid server binary path: %s", tmp() );
		}
		tmp.erase( idx );
	}
	memset(_slib_file_our_folder, 0, sizeof(_slib_file_our_folder));
	memcpy(_slib_file_our_folder, tmp(), tmp.size());
	//msg.format("Our Binary Folder is: %s\n", _slib_file_our_folder); log.append(msg);

	// Flag the fact that we've looked up this value already.
	_slib_file_loaded = true;

	// Return the folder we live in
	//return log;
	return tmp;
}
