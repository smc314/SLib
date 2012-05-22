#ifndef FILE_H
#define FILE_H
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

// Special conversion functions includes on windows
#ifdef _WIN32
#	include <windows.h>
#endif


#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <vector>
using namespace std;

#include "twine.h"
#include "MemBuf.h"
#include "Date.h"

namespace SLib
{

/**
  * The File class is a simple wrapper around FILE* handles and some of the
  * most used stdlib functions involving them.  It makes it easy to open
  * read and get information about files.  It will also handle closing the
  * file handle properly during destruction.
  *
  * @author Steven M. Cherry
  * @copyright 2011 Steven M. Cherry
  */
class DLLEXPORT File
{
	public:
		
		/// A Simple constructor for the file class.  
		File();

		/// Pass in a file name and we will open the given file.  
		File(const twine& fileName);

		/// Pass in an existing file pointer.  
		File(FILE* fp);

		/// Copy constructor.
		File(const File& d);

		/// The standard destructor
		virtual ~File();

		/// Assignment operator.
		File& operator=(const File& d);

		/// Assignment operator.
		File& operator=(FILE* fp);

		/// Cast to a FILE*
		operator FILE*(void) const;

		/// equivalence operator
		bool operator==(File& f) const;

		/// Opens the file name given.
		File& open(const twine& fileName);

		/// Returns the file name:
		twine& name();

		/// Returns the current size of the file
		long size();

		/// Returns the time of last access
		Date lastAccess();

		/// Returns the time of last modification
		Date lastModified();

		/// Returns the time of last status change
		Date lastStatusChange();

		/// Reads the whole file and returns the contents in a newly allocated buffer.
		unsigned char* readContents();
		MemBuf& readContents(MemBuf& contents);
		twine readContentsAsTwine();

		/// Reads the whole file and returns each line from the file as an row in a vector.
		vector<twine> readLines();

		/** Reads just a portion of the file, up to the size of the buffer given and returns
		  * the number of bytes read & written to the buffer.
		  */
		size_t read(MemBuf& buffer);


		/// Determines if the file named exists or not.
		static bool Exists(const twine& fileName);

		/// Returns a list of file names that exist in this directory
		static vector<twine> listFiles(const twine& dirName);
		static vector<twine> listFolders(const twine& dirName);

		/// Quick method to write a string out to a file.
		static void writeToFile(const twine& fileName, const twine& contents);
		static void writeToFile(const twine& fileName, const MemBuf& contents);

		/// Utility method to copy from one file location to another
		static void Copy(const twine& from, const twine& to);

		/// Utility method to move from one file location to another
		static void Move(const twine& from, const twine& to);

		/// Utility method to ensure all of the directories in a given path exist.
		static void EnsurePath( const twine& fileName );

		/// Utility method to delete a file
		static void Delete(const twine& fileName) {
#ifdef _WIN32
			_unlink( fileName() );
#else
			unlink( fileName() );
#endif
		}

		/// Recursively deletes the given directory
		static void RmDir(const twine& dirName);

	protected:

		void closeFile();
		void getStat();

		FILE* m_fp;
		twine m_fileName;


#ifdef _WIN32
		struct _stat m_stat;
#else
		struct stat m_stat;
#endif

};

} // End namespace.

#endif // DATE_H Defined		
		
		

		
