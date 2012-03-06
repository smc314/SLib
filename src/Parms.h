#ifndef PARMS_H
#define PARMS_H
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
#       ifndef DLLEXPORT
#               define DLLEXPORT __declspec(dllexport)
#       endif
#else
#       define DLLEXPORT 
#endif

#include "twine.h"
using namespace SLib;

#include <vector>
#include <map>
#include <utility>
using namespace std;


namespace SLib {

/**
  * This class is used to parse out command line arguments.  Command
  * line arguments come in 5 forms.  Short flags and values.  Long flags
  * and values, and finally simple values.  
  * <P>
  * Short flags look like this: -c -f -k -l
  * <P>
  * Short values look like this: -m mvalue -p pvalue -t tvalue
  * <P>
  * Short values can also look like this: -mmvalue -ppvalue -ttvalue
  * <P>
  * Long flags look like this: --code --final --kill --lunch
  * <P>
  * Long values look like this: --makefile mvalue -power pvalue -time tvalue
  * <P>
  * Simple values look like this: simple1 simple2 simple3 simple4
  * <P>
  * This class has methods that allow you to define the list of each type
  * of parameter that you are looking for, and then it will parse out
  * the command line values and make the information available to you.
  * 
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT Parms
{
	public:
		/// Default Constructor
		Parms();

		/// Default Copy Constructor
		Parms(const Parms& c);

		/// Standard Destructor
		virtual ~Parms();

		/// Default assignment operator
		Parms& operator=(const Parms& c);

		/**
		* This method is used to set the list of short flags that
		* we will look for.  The input is a comma seperated list of
		* single character values. "-c,-f,-k,-l" When we parse the input
		* arguments, we will look for those short flags and store
		* the results of whether we found them or not.
		*/
		void SetShortFlags(twine short_flag_list);

		/**
		* This method is used to set the list of short values that
		* we will look for.  The input is a comma seperated list of
		* single character values.  "-m,-p,-t" When we parse the input
		* arguments, we will look for those short values and store
		* the name/value pair if we find it.
		*/
		void SetShortValues(twine short_value_list);

		/**
		* This method is used to set the list of long flags that
		* we will look for.  The input is a comma seperated list of
		* names. "--code,--final,--kill,--lunch" When we parse the input
		* arguments, we will look for those long flags and store
		* the results of whether we found them or not.
		*/
		void SetLongFlags(twine long_flag_list);

		/**
		* This method is used to set the list of long values that
		* we will look for.  The input is a comma seperated list of
		* names.  "--makefile,--power,--time" When we parse the input
		* arguments, we will look for those long values and store
		* the name/value pair if we find it.
		*/
		void SetLongValues(twine long_value_list);

		/**
		* This method sets the argument values that we are going
		* to parse, and it also executes the parsing.  After you
		* call this method, you can start to ask this object for
		* information about the arguments you are interested in.
		*/
		void Parse(int argc, char** argv);

		/**
		* This method allows you to lookup the given short flag.
		* If the flag was found, this will return true.  If not,
		* this will return false.  Short flags should be specified
		* with a single dash.  e.g. "-c" "-f" "-k" "-l".
		*/
		bool ShortFlag(twine flagname);

		/**
		* This method allows you to lookup the given value for
		* a short value.  If the paramater was found, it's value
		* will be returned.  If not, the return will be an empty
		* string.  Short flags should be specified with a single
		* dash.  e.g. "-m" "-p" "-t"
		*/
		twine ShortValue(twine shortname);

		/**
		* This method allows you to lookup the given long flag.
		* If the flag was found, this will return true.  If not,
		* this will return false.  Long flags should be specified
		* with two dashes.  e.g. "--code" "--final" "--kill" "--lunch".
		*/
		bool LongFlag(twine flagname);

		/**
		* This method allows you to lookup the given value for
		* a long value.  If the paramater was found, it's value
		* will be returned.  If not, the return will be an empty
		* string.  Long flags should be specified with two
		* dashs.  e.g. "--makefile" "--power" "--time"
		*/
		twine LongValue(twine longname);

		/**
		* This method returns a vector that has all of the "simple"
		* parameters in it.  They are listed in the order that they
		* were found on the command line.
		*/
		vector < twine >& Simples(void);

	private:

		vector < twine > m_simple_list;
		map < twine, twine > m_values;
		map < twine, bool > m_flags;

		vector < twine > m_short_values;
		vector < twine > m_short_flags;
		vector < twine > m_long_values;
		vector < twine > m_long_flags;

		int m_argc;
		char** m_argv;
};

} // End Namespace

#endif // PARMS_H Defined
