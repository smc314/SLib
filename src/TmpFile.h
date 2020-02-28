#ifndef TMPFILE_H
#define TMPFILE_H
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

#include "File.h"

namespace SLib
{

/**
  * The TmpFile class is an extension of the File class - it will create a temporary
  * file with a unique name in the current folder and will ensure it is removed when
  * the object is destroyed.
  *
  * @author Steven M. Cherry
  * @copyright 2011 Steven M. Cherry
  */
class DLLEXPORT TmpFile : public File
{
	public:
		
		/// A Simple constructor for the file class.  
		TmpFile();

		/// Pass in a file name and we will open the given file.  
		TmpFile(const twine& fileName);

		/// The standard destructor
		virtual ~TmpFile();

};

} // End namespace.

#endif // TMPFILE_H Defined		
		
		

		
