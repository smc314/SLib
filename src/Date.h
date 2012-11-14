#ifndef DATE_H
#define DATE_H
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


#include "Interval.h"
#include "twine.h"
#include "xmlinc.h"
using namespace SLib;

#include <time.h>

namespace SLib
{

/**
  * The Date class encapsulates all date and time handling for
  * us.  This is how everyone will represent date and time from within
  * the server code.  It has all of the standard date and time features
  * as well as support for operator overloading that allows for date
  * arithemetic.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.2 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT Date
{
	public:
		
		/**
		  * A Simple constructor for the date class.  This
		  * will initialize the date and time structures to
		  * the current date/time.
		  */
		Date();

		/**
		  * Copy constructor for the Date class.
		  */
		Date(const Date& d);

		/**
		  * The standard destructor for the date class.
		  */
		virtual ~Date();

		/// Assignment operator.
		Date& operator=(const Date& d);

		/**
		  * Use this to update the date and time stamp in this
		  * object to right now.
		  */
		virtual void SetCurrent(void);

		/**
		  * Use this to set the date value by using a character
		  * string.  The string is expected to be in the form
		  * <pre>
		    YYYY/MM/DD HH:MM:SS
		    </pre>
		  * Hours are 24 based.
		  */
		virtual void SetValue(const char *date);
		virtual void SetValue(const twine& date);
		virtual void SetValue(const twine* date);

		/**
		  * Use this to set the date value and specify a specific
		  * format for the input date.  Formatting characters allowed
		  * are as follows:
		  * <ul>
		  *  <li>%Y = 4 digit year value</li>
		  *  <li>%m = 2 digit month value (1-12)</li>
		  *  <li>%d = 2 digit day value (1-31)</li>
		  *  <li>%H = 2 digit hour value (0-23)</li>
		  *  <li>%M = 2 digit minute value (0-59)</li>
		  *  <li>%S = 2 digit second value (0-59)</li>
		  * </ul>
		  * Anything else found in the string will be taken as a
		  * literal value or whitespace and ignored.
		  * <P>
		  * You must supply at least %Y, %m, and %d.  If any of %H,
		  * %M, or %S are missing, their values will be set to 0.
		  */
		virtual void SetValue(const char* date, const char* format);
		virtual void SetValue(const twine& date, const twine& format);
		virtual void SetValue(const twine* date, const twine* format);
		
		/**
		  * Use this to set the date value by using the character
		  * string retrieved from a previous call to GetValue.
		  * GetValue returns a writable string so that you can set
		  * the value of the date field externally.  Once you have
		  * writen to the string that GetValue returns, you must
		  * call this function to have the Date object parse the
		  * date that you wrote to the string, and set all of the
		  * internal structures based on that date.
		  * <P>
		  * Note, your format still has to obey the standard format
		  * when you use this function.
		  */
		virtual void SetValue(void);

		/**
		  * Use this to set the date value by using the character
		  * string retrieved from a previous call to GetValue,
		  * and specify the format that the date string is using.
		  * The formatting rules follow the same pattern as those
		  * described above for SetValue().
		  */
		virtual void SetValuef(twine& format);


		/* *************************************************** */
		/* What follows is a series of SetValue overloads and  */
		/* casting operators that allow us to convert to/from  */
		/* a number of other popular date/time formats.        */
		/* *************************************************** */

		/**
		  * Convert from a time_t value.  e.g.
		  * <pre>
		         time_t t;
		         SLib::Date d;
		         t = time(NULL);
			 d.SetValue(t);
		    </pre>
		  */
		virtual void SetValue(const time_t t);

		/**
		  * Convert from a struct tm* value.  e.g.
		  * <pre>
		         struct tm* t;
		         SLib::Date d;
		         ...
			 d.SetValue(t);
		    </pre>
		  */
		virtual void SetValue(const struct tm* t);

		/**
		  * Convert from an xmlChar* value.  e.g.
		  * <pre>
		         SLib::Date d;
			 d.SetValue(xmlGetProp(node, (const xmlChar*)"date");
		    </pre>
		  * <P>
		  * We will take ownership of the given xmlChar* memory
		  * and ensure that it is deleted properly.  The date format
		  * is assumed to be "YYYY/MM/DD HH:MM:SS".
		  */
		virtual void SetValue(xmlChar* c);

#ifdef _WIN32
#if 0
		/**
		  * Convert from an MFC CTime value.  e.g.
		  * <pre>
		         CTime ct;
		         SLib::Date d;
			 d.SetValue(ct);
		    </pre>
		  * <P>
		  * This conversion is only available on a WIN32 platform.
		  */
		virtual void SetValue(const CTime& ct);

		/**
		  * Convert from a COleDateTime value.  e.g.
		  * <pre>
		         COleDateTime codt;
		         SLib::Date d;
			 d.SetValue(codt);
		    </pre>
		  * <P>
		  * This conversion is only available on a WIN32 platform.
		  */
		virtual void SetValue(const COleDateTime& codt);

		/**
		  * Convert from a windows DATE value.  e.g.
		  * <pre>
		         DATE D;
		         SLib::Date d;
			 d.SetValue(D);
		    </pre>
		  * <P>
		  * This conversion is only available on a WIN32 platform.
		  */
		virtual void SetValue(const DATE d);
#endif
#endif

		/**
		  * Cast to a time_t value.  e.g.
		  * <pre>
		         time_t t;
		         SLib::Date d;
		         t = d;
		    </pre>
		  */
		virtual operator time_t() const;

		/**
		  * Cast to a struct tm* value.  e.g.
		  * <pre>
		         SLib::Date d;
		         const struct tm* t = d;
		    </pre>
		  */
		virtual operator const struct tm*() const;

		/**
		  * Cast to a const char* value.  e.g.
		  * <pre>
		         SLib::Date d;
		         const char* c = d;
		    </pre>
		  * <P>
		  * The format will be "YYYY/MM/DD HH:MM:SS"
		  */
		virtual operator const char*();

		/**
		  * Cast to a const xmlChar* value.  e.g.
		  * <pre>
		         SLib::Date d;
		         xmlSetProp(node, (const xmlChar*)"date", d);
		    </pre>
		  * <P>
		  * The format will be "YYYY/MM/DD HH:MM:SS"
		  */
		virtual operator const xmlChar*();

#ifdef _WIN32
#if 0
		/**
		  * Cast to an MFC CTime value.  e.g.
		  * <pre>
		         CTime ct;
		         SLib::Date d;
			 ct = d;
		    </pre>
		  * <P>
		  * This conversion is only available on a WIN32 platform.
		  */
		virtual operator CTime() const;

		/**
		  * Cast to an COleDateTime value.  e.g.
		  * <pre>
		         COleDateTime codt;
		         SLib::Date d;
			 ct = d;
		    </pre>
		  * <P>
		  * This conversion is only available on a WIN32 platform.
		  */
		virtual operator COleDateTime() const;

		/**
		  * Cast to a windows DATE value.  e.g.
		  * <pre>
		         DATE D;
		         SLib::Date d;
			 D = d;
		    </pre>
		  * <P>
		  * This conversion is only available on a WIN32 platform.
		  */
		virtual operator DATE() const;
#endif
#endif
	

		/**
		  * Use this to retrieve the current value of this date
		  * in the standard form.  The character string will
		  * be of the form:
		  * <pre>
		    YYYY/MM/DD HH:MM:SS
		    </pre>
		  * Hours are 24 based.
		  */
		virtual char *GetValue(void);

		/**
		  * Use this to retrive the current value of this date
		  * and specify the format that you would like to receive
		  * the information in.  Formatting characters are:
		  * <ul>
		  *  <li>%Y = 4 digit year value</li>
		  *  <li>%m = 2 digit month value (1-12)</li>
		  *  <li>%d = 2 digit day value (1-31)</li>
		  *  <li>%H = 2 digit hour value (0-23)</li>
		  *  <li>%M = 2 digit minute value (0-59)</li>
		  *  <li>%S = 2 digit second value (0-59)</li>
		  * </ul>
		  * Anything else in the format is considered a character
		  * literal and is copied to the output date string directly.
		  */
		virtual char *GetValue(const char* format);
		virtual char *GetValue(const twine& format);
		virtual char *GetValue(const twine* format);

		/**
		  * Use this function to set the hour, minute, and second
		  * value of a date to 00:00:00.
		  */
		virtual void Floor(void);

		/**
		  * Use this function to set the hour, minute, and second
		  * value of a date to 23:59:59.
		  */
		virtual void Ceil(void);

		/**
		  * Use this to query the year value by number.  
		  * This will return a full integer year.     
		  */
		virtual int Year(void) const;
		virtual void Year(int);

		/**
		  * Use this to query the month value by number.  Month's
		  * start at 1 = January, ... , 12 = December.
		  */
		virtual int Month(void) const;
		virtual void Month(int);

		/**
		  * Use this to query the day value by number. 
		  */
		virtual int Day(void) const;
		virtual void Day(int);

		/// Returns the 0-based day of week
		virtual int DayW(void) const;

		/**
		  * Use this to query the hour value by number.
		  * Hours start at 0 = Midnight , ... , 23.
		  */
		virtual int Hour(void) const;
		virtual void Hour(int);

		/**
		  * Use this to query the minute value by number.
		  * Minutes start at 0 = top of hour, ... , 59.
		  */
		virtual int Min(void) const;
		virtual void Min(int);

		/**
		  * Use this to query the second value by number.
		  * Seconds start at 0 = top of minute, ... , 59.
		  */
		virtual int Sec(void) const;
		virtual void Sec(int);

		/**
		  * Use this to adjust the year value of this date
		  * object.  Both positive and negative adjustments
		  * are handled.
		  */
		virtual void AddYear(int i);

		/**
		  * Use this to adjust the month value of this date
		  * object.  Both positive and negative adjustments
		  * are handled.  Adjustments may carry over and 
		  * affect year values.  e.g. +14 months = +1year 
		  * and +2months.
		  */
		virtual void AddMonth(int i);

		/**
		  * Use this to adjust the day value of this date
		  * object.  Both positive and negative adjustments
		  * are handled.  Admustments may carry over and
		  * affect month and year values.
		  */
		virtual void AddDay(int i);

		/**
		  * Use this to adjust the hour value of this date
		  * object.  Both positive and negative adjustments
		  * are handled.  Adjustments may carry over and 
		  * affect day, month, and year values.
		  */
		virtual void AddHour(int i);

		/**
		  * Use this to adjust the minute value of this date
		  * object.  Both positive and negative adjustments
		  * are handled.  Adjustments may carry over and
		  * affect hour, day, month, and year values.
		  */
		virtual void AddMin(int i);

		/**
		  * Use this to adjust the second value of this date
		  * object.  Both positive and negative adjustments
		  * are handled.  Adjustments may carry over and
		  * affect minut, hour, day, month, and year values.
		  */
		virtual void AddSec(int i);

		/**
		  * Use this to add an interval value to this date.
		  */
		virtual void Add(Interval& i);

		/** 
		  * Returns the number of seconds since the epoch that this
		  * date represents.  This is equivalent to mktime(3).
		  */
		virtual time_t Epoch(void);

		/**
		  * Returns the date formatted correctly for inclusion in
		  * an e-mail Date: field.  It will return the format:
		  * <pre>
		    "Wed Jun 30 21:49:08 1993"		    
		    </pre>
		  */
		virtual char* EDate(void);

		/// equivalence operator
		bool operator==(const Date& d) const;

		/// Less than comparison
		bool operator<(const Date& d) const;

		/// Greater than comparison
		bool operator>(const Date& d) const;

		/// Less than or equal comparison
		bool operator<=(const Date& d) const;

		/// Greater than or equal comparison
		bool operator>=(const Date& d) const;

		/// Subtraction of dates produces an interval
		SLib::Interval operator-(Date& d);

		/// Addition of an interval produces a new date
		Date operator+(SLib::Interval& i) const;

		/// Addition of an interval adjusts this date
		Date& operator+=(SLib::Interval& i);

		/// Subtraction of an interval produces a new date
		Date operator-(SLib::Interval& i) const;

		/// Subtraction of an interval adjusts this date
		Date& operator-=(SLib::Interval& i);

		/**
		  * This is a public member variable that allows the
		  * DB interfaces to see the length of the date string.
		  */
		size_t m_len;

	protected:

		/**
		  * This will handle the round over bits of functionality
		  * that we will need.  e.g. month = 15 really means
		  * year += 1, month += 3.
		  */
		virtual void Normalize(void);

		time_t m_TimeVal;
		struct tm *m_TimeStruct;
		char *m_picture;

};

#if 0
/** Less Than operation for Date.
  */
inline bool operator<(const Date& lhs, const Date& rhs)
{
	return lhs.operator<(rhs);
}

/** Greater Than operation for Date.
  */
inline bool operator>(const Date& lhs, const Date& rhs)
{
	return lhs.operator>(rhs);
}

/** Less Than or Equals operation for Date.
  */
inline bool operator<=(const Date& lhs, const Date& rhs)
{
	return lhs.operator<=(rhs);
}

/** Greater Than or Equals operation for Date.
  */
inline bool operator>=(const Date& lhs, const Date& rhs)
{
	return lhs.operator>=(rhs);
}
#endif


} // End namespace.

#endif // DATE_H Defined		
		
		

		
