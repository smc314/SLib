#ifndef ZIPFILE_H
#define ZIPFILE_H
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

#include <curl/curl.h>

#include "xmlinc.h"
#include "twine.h"
using namespace SLib;

#include "zip.h"
#include "unzip.h"

namespace SLib {

/** 
  * This class defines a simple interface to creating a zip file from a single file
  * or a single directory.  It is primarily a wrapper around the logic from minizip.
  *
  * @author Steven M. Cherry
  */
class DLLEXPORT ZipFile
{
	public:
		
		/** Default constructor */
		ZipFile(const twine& zipName);

		/// Standard Destructor
		virtual ~ZipFile();

		/// Use this to add a single file
		void AddFile( const twine& infile);

		/// Use this to zip a single folder with all contents recursively included.
		void AddFolder( const twine& infolder);

		/// Use this to finish and close out the zip file.
		void Close();

		/// The methods to unzip a file are all static.  This extracts the current file in the list.
		static void ExtractCurrentFile( unzFile uf, const twine& targetDir);

		/// The methods to unzip a file are all static.  This extracts all files from the given zip file.
		static void Extract(const twine& zipName, const twine& targetDir);

	private:

		/// Copy constructor is private to prevent use
		ZipFile( const ZipFile& c) {}

		/// Assignment operator is private to prevent use
		ZipFile& operator=(const ZipFile& c) { return *this; }

		/// Our zip file
		zipFile m_zf;

};

} // End Namespace SLib

#endif // ZIPFILE_H defined
