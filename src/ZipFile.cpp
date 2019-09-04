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

#include "ZipFile.h"
#include "File.h"
#include "Date.h"
#include "EnEx.h"
#include "Log.h"
#include "memptr.h"
#include "dptr.h"
#include "AnException.h"
#include "XmlHelpers.h"
using namespace SLib;

#if (!defined(_WIN32)) && (!defined(WIN32)) && (!defined(__APPLE__))
        #ifndef __USE_FILE_OFFSET64
                #define __USE_FILE_OFFSET64
        #endif
        #ifndef __USE_LARGEFILE64
                #define __USE_LARGEFILE64
        #endif
        #ifndef _LARGEFILE64_SOURCE
                #define _LARGEFILE64_SOURCE
        #endif
        #ifndef _FILE_OFFSET_BIT
                #define _FILE_OFFSET_BIT 64
        #endif
#endif

#ifdef __APPLE__
// In darwin and perhaps other BSD variants off_t is a 64 bit value, hence no need for specific 64 bit functions
#define FOPEN_FUNC(filename, mode) fopen(filename, mode)
#define FTELLO_FUNC(stream) ftello(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko(stream, offset, origin)
#else
#define FOPEN_FUNC(filename, mode) fopen64(filename, mode)
#define FTELLO_FUNC(stream) ftello64(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef _WIN32
# include <direct.h>
# include <io.h>
#else
# include <unistd.h>
# include <utime.h>
# include <sys/types.h>
# include <sys/stat.h>
#endif

#include "zip.h"
#include "unzip.h"

#ifdef _WIN32
        #define USEWIN32IOAPI
        #include "iowin32.h"
#endif

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

int isLargeFile(size_t fileSize)
{
	if(fileSize >= 0xffffffff){
		return 1;
	}
	return 0;
}

uLong filetime(const Date& lastModified, tm_zip* tmzip)
{
	const struct tm* lm = (const struct tm*)lastModified;
	tmzip->tm_sec  = lm->tm_sec;
	tmzip->tm_min  = lm->tm_min;
	tmzip->tm_hour = lm->tm_hour;
	tmzip->tm_mday = lm->tm_mday;
	tmzip->tm_mon  = lm->tm_mon ;
	tmzip->tm_year = lm->tm_year;
	return 1;
}

/* change_file_date : change the date/time of a file
    filename : the filename of the file where date/time must be modified
    dosdate : the new date at the MSDos format (4 bytes)
    tmu_date : the SAME new date at the tm_unz format */
void change_file_date(const char* filename, uLong dosdate, tm_unz tmu_date)
{
#ifdef _WIN32
  HANDLE hFile;
  FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

  hFile = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE,
                      0,NULL,OPEN_EXISTING,0,NULL);
  GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
  LocalFileTimeToFileTime(&ftLocal,&ftm);
  SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
  CloseHandle(hFile);
#else
#ifdef unix || __APPLE__
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min=tmu_date.tm_min;
  newdate.tm_hour=tmu_date.tm_hour;
  newdate.tm_mday=tmu_date.tm_mday;
  newdate.tm_mon=tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
      newdate.tm_year=tmu_date.tm_year - 1900;
  else
      newdate.tm_year=tmu_date.tm_year ;
  newdate.tm_isdst=-1;

  ut.actime=ut.modtime=mktime(&newdate);
  utime(filename,&ut);
#endif
#endif
}

ZipFile::ZipFile(const twine& zipName)
{
	EnEx ee(FL, "ZipFile::ZipFile(const twine& zipName)");

	if(zipName.length() == 0){
		throw AnException(0, FL, "ZipFile File name is empty.");
	}

	twine zipFileName;
	if(zipName.endsWith(".zip")){
		zipFileName = zipName;
	} else {
		zipFileName = zipName + ".zip";
	}

	// We always overwrite the file.  Check to see if it exists, and if so, then delete it:
	if(File::Exists( zipFileName )){
		File::Delete( zipFileName );
	}

	m_zf = NULL;
#ifdef USEWIN32IOAPI
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64A(&ffunc);
	m_zf = zipOpen2_64(zipFileName,0,NULL,&ffunc);
#else
	m_zf = zipOpen64(zipFileName,0);
#endif

	if (m_zf == NULL) {
		throw AnException(0, FL, "Error creating zip file (%s)", zipFileName());
	}

	// Ready to add files to this zipfile.
}

ZipFile::~ZipFile()
{
	EnEx ee(FL, "ZipFile::~ZipFile()");

	zipClose(m_zf,NULL);
	m_zf = NULL;
}

void ZipFile::Close()
{
	EnEx ee(FL, "ZipFile::Close()");

	zipClose(m_zf,NULL);
	m_zf = NULL;
}

void ZipFile::SetRootFolder( const twine& root )
{
	EnEx ee(FL, "ZipFile::SetRootFolder(const twine& infile)");

	m_root = root;
}

void ZipFile::AddFile(const twine& infile)
{
	EnEx ee(FL, "ZipFile::AddFile(const twine& infile)");

    int opt_compress_level=9;
    int err=0;
    const char* password=NULL;

	if(m_zf == NULL){
		throw AnException(0, FL, "This zipfile has been closed.  Adding more files is not allowed.");
	}

	if(infile.startsWith("/") || infile.startsWith("\\") ){
		throw AnException(0, FL, "ZipFile input file may not start with / or \\.");
	}

	zip_fileinfo zi;
	unsigned long crcFile=0;
	int zip64 = 0;

	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
	zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;

	twine fullFileName;
	if(m_root.empty()){
		fullFileName = infile;
	} else {
		if(m_root.endsWith("/") || m_root.endsWith("\\")){
			fullFileName = m_root + infile;
		} else {
			fullFileName = m_root + "/" + infile;
		}
	}

	File inputFile( fullFileName );
	filetime(inputFile.lastModified(), &zi.tmz_date);
	zip64 = isLargeFile( inputFile.size() );

	err = zipOpenNewFileInZip3_64(
		m_zf, 
		infile(), 
		&zi,
		NULL,
		0,
		NULL,
		0,
		NULL /* comment*/,
		Z_DEFLATED,
		opt_compress_level,
		0,
		-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
		password, crcFile, zip64);

	if (err != ZIP_OK) {
		throw AnException(0, FL, "Error creating %s in zipfile", infile());
	} 

	// Read the contents from the file
	MemBuf fullFileContents;
	inputFile.readContents( fullFileContents );

	// Write the file contents to the zip file.
	if( zipWriteInFileInZip (m_zf, fullFileContents(), fullFileContents.size() ) < 0 ){
		throw AnException(0, FL, "Error in writing %s to the zipfile", fullFileName());
	}

	if( zipCloseFileInZip(m_zf) != ZIP_OK){
		throw AnException(0, FL, "Error closing %s in the zipfile.", fullFileName() );
	}
}

void ZipFile::AddFile(const twine& filePath, const MemBuf& fileData)
{
	EnEx ee(FL, "ZipFile::AddFile(const twine& filePath, const MemBuf& fileData)");

    int opt_compress_level=9;
    int err=0;
    const char* password=NULL;

	if(m_zf == NULL){
		throw AnException(0, FL, "This zipfile has been closed.  Adding more files is not allowed.");
	}

	if(filePath.startsWith("/") || filePath.startsWith("\\") ){
		throw AnException(0, FL, "ZipFile input file may not start with / or \\.");
	}

	zip_fileinfo zi;
	unsigned long crcFile=0;
	int zip64 = 0;

	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
	zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;

	Date now;
	filetime(now, &zi.tmz_date);
	zip64 = isLargeFile( fileData.size() );

	err = zipOpenNewFileInZip3_64(
		m_zf, 
		filePath(), 
		&zi,
		NULL,
		0,
		NULL,
		0,
		NULL /* comment*/,
		Z_DEFLATED,
		opt_compress_level,
		0,
		-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
		password, crcFile, zip64);

	if (err != ZIP_OK) {
		throw AnException(0, FL, "Error creating %s in zipfile", filePath());
	} 

	// Write the file contents to the zip file.
	if( zipWriteInFileInZip (m_zf, fileData(), fileData.size() ) < 0 ){
		throw AnException(0, FL, "Error in writing %s to the zipfile", filePath());
	}

	if( zipCloseFileInZip(m_zf) != ZIP_OK){
		throw AnException(0, FL, "Error closing %s in the zipfile.", filePath() );
	}
}

void ZipFile::AddFolder(const twine& infolder)
{
	EnEx ee(FL, "ZipFile::AddFolder(const twine& infolder)");

	twine fullName;
	if(m_root.empty()){
		fullName = infolder;
	} else {
		if(m_root.endsWith("/") || m_root.endsWith("\\")){
			fullName = m_root + infolder;
		} else {
			fullName = m_root + "/" + infolder;
		}
	}

	// Add all of the files in the target folder:
	vector<twine> files = File::listFiles( fullName );
	for(size_t i = 0; i < files.size(); i ++ ){
		AddFile( infolder + "/" + files[i] );
	}

	// Recurse through all sub-folders and do the same:
	vector<twine> folders = File::listFolders( fullName );
	for(size_t i = 0; i < folders.size(); i ++){
		if(folders[i] != "." && folders[i] != ".."){
			AddFolder( infolder + "/" + folders[i] );
		}
	}
}

void ZipFile::ExtractCurrentFile( unzFile uf, const twine& targetDir)
{
	EnEx ee(FL, "ZipFile::ExtractCurrentFile(unzFile uf, const twine& targetDir)");

    char filename_inzip[256];
    int err=UNZ_OK;
    FILE *fout=NULL;
    void* buf;
    uInt size_buf;

    unz_file_info64 file_info;
    
    err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

    if (err!=UNZ_OK)
    {
		throw AnException(0, FL, "Error %d with zipfile in unzGetCurrentFileInfo",err);
    }

    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf==NULL)
    {
        throw AnException(0, FL, "Error allocating memoryn");
    }

	twine fullFileName = targetDir + "/" + filename_inzip;
	//printf("Extracting current file to: %s\n", fullFileName() );
	if(fullFileName.endsWith("/")){
		// This is just a directory entry.  Ensure that it exists:
		File::EnsurePath( fullFileName + "a" ); // add a to the end to get the whole path created.
		return;
	}

	// Make sure the path leading up to the file exists.
	File::EnsurePath( fullFileName );

	// If the file exists, delete it so we can write the new one.
	if(File::Exists( fullFileName )){
		File::Delete( fullFileName );
	}



	//printf("calling unzOpenCurrentFilePassword\n");
	if( unzOpenCurrentFilePassword(uf,NULL) != UNZ_OK){
		throw AnException(0, FL, "Error with zipfile in unzOpenCurrentFilePassword");
	}

	fout=FOPEN_FUNC(fullFileName(),"wb");
	if (fout == NULL) {
		throw AnException(0, FL, "error opening %s", fullFileName());
	}

	while((err = unzReadCurrentFile(uf,buf,size_buf)) > 0){
		if (fwrite(buf,err,1,fout) != 1) {
			fclose(fout);
			throw AnException(0, FL, "Error in writing extracted file: %s", fullFileName());
		}
	}

	fclose(fout);

	change_file_date(fullFileName(),file_info.dosDate, file_info.tmu_date);

	if(unzCloseCurrentFile (uf) != UNZ_OK){
		throw AnException(0, FL, "Error with zipfile in unzCloseCurrentFile");
	}

    free(buf);
}


void ZipFile::Extract( const twine& zipName, const twine& targetDir)
{
	EnEx ee(FL, "ZipFile::Extract(const twine& zipName, const twine& targetDir)");

    unzFile uf=NULL;

    uLong i;
    unz_global_info64 gi;
    


	if(zipName.length() == 0){
		throw AnException(0, FL, "Given zipName is empty.");
	}
	if(!File::Exists( zipName )){
		throw AnException(0, FL, "Given zip file (%s) does not exist.", zipName() );
	}

#ifdef USEWIN32IOAPI
	zlib_filefunc64_def ffunc;
#endif

#ifdef USEWIN32IOAPI
	fill_win32_filefunc64A(&ffunc);
	uf = unzOpen2_64(zipName(),&ffunc);
#else
	uf = unzOpen64(zipName());
#endif

	if (uf==NULL) { // try it as zipName + ".zip"
		twine withZip = zipName + ".zip";
#ifdef USEWIN32IOAPI
		uf = unzOpen2_64(zipName(),&ffunc);
#else
		uf = unzOpen64(zipName());
#endif
	}

	if(uf == NULL){
		throw AnException(0, FL, "Could not open zipfile (%s) or (%s.zip)", zipName(), zipName() );
	}

    if( unzGetGlobalInfo64(uf,&gi) != UNZ_OK){
		unzClose(uf);
        throw AnException(0, FL, "Error with zipfile in unzGetGlobalInfo");
	}

    for (i=0;i<gi.number_entry;i++) {
		try {
        	ZipFile::ExtractCurrentFile(uf, targetDir);
		} catch (AnException&){
			unzClose(uf);
			throw; // re-throw the exception
		}

        if ((i+1)<gi.number_entry) {
            if( unzGoToNextFile(uf) != UNZ_OK){
				unzClose(uf);
				throw AnException(0, FL, "Error with zipfile in unzGoToNextFile");
            }
        }
    }

    unzClose(uf);
}

