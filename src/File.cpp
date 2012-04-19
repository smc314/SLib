#include "File.h"
#include "AnException.h"
#include "EnEx.h"
using namespace SLib;

File::File()
{
	EnEx ee("File::File()");
	m_fp = NULL;
}

File::File(const twine fileName) : m_fileName(fileName)
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

File& File::open(const twine fileName)
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
	int ret;
#ifdef _WIN32
	ret = _fstat(_fileno(m_fp), &m_stat);
#else
	ret = fstat(fileno(m_fp), &m_stat);
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
	size_t ret = fread(buff, size(), 1, m_fp);

	if(ret != 1){
		free(buff);
		throw AnException(0, FL, "Error reading from file (%s)", m_fileName());
	}
	return buff;
}

twine File::readContentsAsTwine()
{
	EnEx ee("File::readContents()");

	twine contents;
	contents.reserve(size());

	// Read the whole file in at once.
	size_t ret = fread(contents.data(), size(), 1, m_fp);

	if(ret != 1){
		throw AnException(0, FL, "Error reading from file (%s)", m_fileName());
	}
	contents.check_size();
	return contents;
}

vector<twine> File::readLines()
{
	EnEx ee("File::readLines()");

	twine contents;
	contents.reserve(size());

	// Read the whole file in at once.
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

vector<twine> File::listFiles(twine dirName)
{
	EnEx ee("File::listFiles()");
	vector<twine> ret;

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

	return ret;
}

vector<twine> File::listFolders(twine dirName)
{
	EnEx ee("File::listFolders()");
	vector<twine> ret;

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

	return ret;
}

void File::writeToFile(twine& fileName, twine& contents)
{

	FILE* fp = fopen(fileName(), "wb");
	if(fp == NULL){
		throw AnException(0, FL, "Error opening file(%s) in write mode!", fileName() );
	}

	size_t ret = fwrite(contents(), contents.size(), 1, fp);
	fclose(fp);
	if(ret != 1){
		throw AnException(0, FL, "Error writing contents to file.");
	}
}

void File::Copy(twine& from, twine& to)
{
	EnEx ee("File::Copy(twine& from, twine& to)");

	File f(from);
	if(f.size() > 1024 * 1024 * 10) {
		throw AnException(0, FL, "File copying limited to 10M or smaller files.");
	}
	dptr<unsigned char> contents = f.readContents();

	FILE* fp = fopen(to(), "wb");
	if(fp == NULL){
		throw AnException(0, FL, "Error opening file(%s) in write mode for copy!", to() );
	}
	size_t ret = fwrite(contents, f.size(), 1, fp);
	fclose(fp);
	if(ret != 1){
		throw AnException(0, FL, "Error writing contents to file copy.");
	}

	// dptr<> contents gets automatically free'd when we leave this function
}
