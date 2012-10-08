#ifndef AnException_h
#define AnException_h
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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <string>
using namespace std;

#include "Log.h"
#include "Tools.h"
#include "Thread.h"
#include "EnEx.h"


#ifndef FL
#define FL __FILE__, __LINE__
#endif

/**
  * @memo The AnException class.
  * @doc  This is the class that encapsulates all of our exceptions.  It
  *       has many features for tracking what exception happened, where, and
  *       why.  It also contains some basic requirements for translation of
  *       exceptions.
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  */

class AnException {
	public:
		/**
		  * @memo The standard way to instantiate an Exception class.
		  * @doc  This is the standard way to create and then throw
		  *       AnException.  This allows for a custom message, 
		  *       a standard message, as well as file and line info
		  *       for debugging purposes.
		  * @param id  The exception id for lookup purposes.
		  * @param file The name of the file that this occurred in.  
		  *             This is truncated at 255 bytes.
		  * @param line The line number in the file where this happened.
		  * @param msg The custom message for this exception. This 
		  *            message is truncated at 255 bytes.  For a longer
		  *            and more detailed message, use the exception id
		  *            and look up the message in the list.
		  * @param ... This is the variable list of parameters that
		  *            can be used to fill out the message in a printf
		  *            style of formatting.
		  */
		AnException(int id, const char *file, int line, const char *msg, ...)
		{
			va_list ap;
			char buffer[1024];
			
			va_start(ap, msg);

			memset(buffer, 0, 1024);
			m_file.erase();
			m_file = file;

#ifdef _WIN32
			_vsnprintf(buffer, 1023, msg, ap);
#else
			vsnprintf(buffer, 1023, msg, ap);
#endif
			m_message.erase();
			m_message = buffer;
			m_id = id;
			m_line = line;
			va_end(ap);

			m_stack = EnEx::GetStackTrace()();

			DEBUG(FL, 
				"Generating exception:\n"
				"=====================\n" 
				"%s\n"
				"=====================\n"
				"In File: %s\n"
				"On Line: %d\n"
				"With ID: %d\n"
				"=====================\n"
				"%s\n"
				"=====================\n",
				m_message.c_str(),
				m_file.c_str(),
				m_line,
				m_id,
				m_stack.c_str()
			);

			
		}	

		/// The destructor.  It doesn't have to do much.
		virtual ~AnException()
		{

		}

		/// Use this function to retrieve the exception ID.
		int ID(void)
		{
			return m_id;
		}

		/// Use this function to retrieve the line number.
		int Line(void)
		{
			return m_line;
		}

		/// Use this function to retrieve the file name.
		const char *File(void)
		{
			return m_file.c_str();	
		}

		/// Use this function to retrieve the message.
		const char *Msg(void)
		{
			return m_message.c_str();
		}	

		/// Use this function to add more to the message string.
		void AddMsg(const char *new_msg)
		{
			m_message.append(new_msg);
		}

		/// Use this function to retrieve the stack trace captured at the time of the exception.
		const char* Stack(void){
			return m_stack.c_str();
		}

	protected:

		string m_message;
		string m_file;
		int  m_id;
		int  m_line;
		string m_stack;

};


#endif //AnException_h defined.
