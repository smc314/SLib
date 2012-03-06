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

#include <time.h>
#include <sys/timeb.h>

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
		float Duration(void);

		/** Use this method to retrieve the current CPU cycle count
		 * value on an intel machine.  This works on windows, linux, and mac.
		 */
#ifdef _WIN32
		// Link http://stackoverflow.com/questions/275004/c-timer-function-to-provide-time-in-nano-seconds

		// Here's the actual code to retrieve the number of 80x86 CPU clock ticks passed
		// since the CPU was last started.  It will work on Pentium and above (386/486 not
		// supported).  This function has the advantage of being extremely fast - it usually
		// takes no more than 50 cpu cycles to execute.

		// If you need to translate the clock counts into true elapsed time, divide the results by
		// your chip's clock speed.  Remember that the "rated" GHz is likely to be slightly different
		// from the actual speed of your chip.  To check your chip's true speed, you can use several
		// very good utilities, or the Win32 call: QueryPerformanceFrequency();

		// This code is MS Visual C++ specific
		static inline unsigned long GetCycleCount(void)
		{
			// Counter
			struct { __int32 low, high; } counter;

			// Use RDTSC instruction to get clocks count
			__asm push EAX
			__asm push EDX
			__asm __emit 0fh __asm __emit 031h // RDTSC
			__asm mov counter.low, EAX
			__asm mov counter.high, EDX
			__asm pop EDX
			__asm pop EAX

			// Returned result
			return *(__int64 *)(&counter);

		}
#else

		// This version works on any 80x86 platform targeted by the g++ compiler.
		static __inline__ unsigned long GetCycleCount(void)
		{
			unsigned a, d;
			asm volatile("rdtsc" : "=a" (a), "=d" (d));
			return ((unsigned long)a) | (((unsigned long)d) << 32);
		}
#endif



	private:

		struct timeb m_start_time;
		struct timeb m_end_time;

};

} // End namespace

#endif // TIMER_H Defined
