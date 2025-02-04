#ifndef TIMER_H
#define TIMER_H
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
#include <chrono>

namespace SLib
{

/**
  * This object allows you to time the durations of pieces of code.
  * Instantiate this object, and then call the Start method when you
  * want the timer to start.  Then call the Finish method when you want
  * the timer to end, and then use the Duration method to see how much
  * time elapsed between the calls to Start and Finish.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT Timer
{
	public:

		/// Standard constructor
		Timer();

		/// Copy constructor
		Timer(const Timer& other) = default;

		/// Move constructor
		Timer(Timer&& other) noexcept = default;

		/// Copy assignment operator
		Timer& operator=(const Timer& other) = default;

		/// Move assignment operator
		Timer& operator=(Timer&& other) noexcept = default;

		/// Standard Destructor
		virtual ~Timer();
	
		/// Use this to start the timer clock.
		void Start(void);

		/// Use this to end the timer clock.
		void Finish(void);

		/**
		  * Use this to retrieve the duration between the start
		  * and finish times for this timer.  The result is returned
		  * as a float count of seconds and fractions of seconds.
		  */
		double Duration(void) const;

		/** Use this method to retrieve a microsecond resolution clock value.
		  * We use gettimeofday on linux and ftime on windows to implement this. Note,
		  * ftime on windows only provides millisecond resolution, not microsecond
		  * resolution.
		  */
		static uint64_t GetCycleCount(void);

	private:

		std::chrono::high_resolution_clock::time_point m_start_time;
		std::chrono::high_resolution_clock::time_point m_end_time;

};

} // End namespace
#endif // TIMER_H