#ifndef INTERVAL_H
#define INTERVAL_H
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

#include <time.h>

#ifdef _WIN32
#       ifndef DLLEXPORT
#               define DLLEXPORT __declspec(dllexport)
#       endif
#else
#       define DLLEXPORT 
#endif

/* ************************************************************* */
/* These are named constants used to initialize intervals using  */
/* the constructor with parameters.                              */
/* ************************************************************* */
const int SECOND = 1;
const int SECONDS = SECOND;
const int MINUTE = 2;
const int MINUTES = MINUTE;
const int HOUR = 3;
const int HOURS = HOUR;
const int DAY = 4;
const int DAYS = DAY;
const int WEEK = 5;
const int WEEKS = WEEK;


namespace SLib {

/**
  * This class defines a time interval.  It is used when doing
  * arithemetic on dates.  Internally it is stored as a number
  * of seconds.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT Interval
{
	public:
		/// Standard Constructor
		Interval();

		/**
		  * Constructor that allows setting the duration of the 
		  * interval based on a duration type.  The duration type
		  * must be one of the named constants:
		  * <ul>
		  *   <li>SECOND</li>
		  *   <li>MINUTE</li>
		  *   <li>HOUR</li>
		  *   <li>DAY</li>
		  *   <li>WEEK</li>
		  * </ul>
		  */
		Interval(int duration, int duration_type);

		/// Copy Constructor
		Interval(const Interval& i);

		/// Assignment operator
		Interval& operator=(const Interval& i);

		/// Assignment operator
		Interval& operator=(const time_t i);

#ifndef AIX
		/// Assignment operator
		Interval& operator=(const int i);
#endif

		/// Standard destructor
		virtual ~Interval();

		/// Returns the number of seconds in this interval
		int Sec(void) const;

		/// Easier way to return the number of seconds.
		int operator()(void) const;

		/// Addition on intervals
		Interval operator+(const Interval& i) const;

		/// Addition on intervals
		Interval& operator+=(const Interval& i);

		/// Subtraction on intervals
		Interval operator-(const Interval& i) const;

		/// Subtraction on intervals
		Interval& operator-=(const Interval& i);

		/// Comparison of intervals
		bool operator>(const Interval& i) const;

		/// Comparison of intervals
		bool operator>=(const Interval& i) const;

		/// Comparison of intervals
		bool operator<(const Interval& i) const;

		/// Comparison of intervals
		bool operator<=(const Interval& i) const;

		/// Comparison of intervals
		bool operator==(const Interval& i) const;

		/// Returns the number of days that this interval represents
		int Days(void);

	private:

		time_t m_interval_length;
};

} // End namespace

#endif // INTERVAL_H Defined







