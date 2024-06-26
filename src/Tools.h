#ifndef TOOLS_H
#define TOOLS_H
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

/* ************************************************************* */
/* This is a set of standard tool functions that everyone can    */
/* use to do interesting stuff.                                  */
/* ************************************************************* */

#ifdef _WIN32
#	ifndef DLLEXPORT
#		define DLLEXPORT __declspec(dllexport)
#       endif
#else
#	define DLLEXPORT 
#endif

#include <vector>
using namespace std;

#include "sptr.h"
#include "twine.h"


namespace SLib
{

/**
  * @memo This class provides a collection of usefull utilities that
  *       are needed here and there.
  * @doc  This class provides a collection of usefull utilities that
  *       are needed here and there.
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT Tools {

	public:
		/**
		  * @memo This function provides an OS-independant 
		  *       high resolution sleep function.
		  * @doc  This function provides an OS-independant 
		  *       high resolution sleep function.  It uses
		  *       the select system call to implement this.
		  *       <P>
		  *       1 second of sleep time can be achieved by
		  *       passing in a value of 1 Million (1,000,000).
		  */
		static void sleep(int mills);

		/**
		  * @memo This function provides an OS-independant 
		  *       second resolution sleep function.
		  * @doc  This function provides an OS-independant 
		  *       second resolution sleep function.
		  *       <P>
		  *       1 second of sleep time can be achieved by
		  *       passing in a value of 1.
		  */
		static void ssleep(int seconds) { 
			Tools::sleep(seconds * 1000000);
		}

		/**
		  * @memo This function provides an OS-independant 
		  *       milisecond resolution sleep function.
		  * @doc  This function provides an OS-independant 
		  *       milisecond resolution sleep function.
		  *       <P>
		  *       1 second of sleep time can be achieved by
		  *       passing in a value of 1000.
		  */
		static void msleep(int thousandths) {
			Tools::sleep(thousandths * 1000);
		}

		/**
		  * @memo This function provides an OS-independant 
		  *       microsecond resolution sleep function.
		  * @doc  This function provides an OS-independant 
		  *       microsecond resolution sleep function.
		  *       <P>
		  *       1 second of sleep time can be achieved by
		  *       passing in a value of 1000000.
		  */
		static void usleep(int millionths) {
			Tools::sleep(millionths);
		}

		/**
		  * @memo This function will trim spaces from the right of 
		  *       a character string.
		  * @doc  This function will trim spaces from the right of 
		  *       a character string.
		  */
		static void rtrim(char *s);

		/// Free a character array.
		static void char_free(char *s);

		/// Generate a random number in a specific range
		static int rand(int max, int min = 0);

		/// Take a given memory pointer and print out the hex memory dump of a given size:
		static twine hexDump(void* ptr, char* name, size_t prior, size_t length, bool asciiPrint = false, bool ebcdicPrint = false);

		/** This will run the given command and collect the standard output and
		  * standard error from the command.  When the command is done we will return
		  * the combined results. Note that we will parse the given command string to split
		  * it up into the command and a list of arguments.  If you want to do this directly,
		  * then call the alternate version of RunCommand and provide your own list of args.
		  */
		static vector<twine> RunCommand( const twine& cmd, int* exitCode );

		/** This is the same as above, only the given input sring will be passed to the child process
		  * on standard input before we start reading the standard output and error of the child
		  * process.
		  */
		static vector<twine> RunCommand( const twine& cmd, const twine& inputString, int* exitCode );

		/** This will run the given command with the given list of arguments and collect
		  * standard output and standard error from the command.  The given input string will
		  * be sent to the command on standard input if it is not empty.  When the command is done
		  * we will return the combined results.
		  */
		static vector<twine> RunCommand( const twine& cmd, vector<twine> args, const twine& inputString, int* exitCode);

}; 

} // End namespace

typedef sptr < char, SLib::Tools::char_free > autochar;

#endif //TOOLS_H Defined.
