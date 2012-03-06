
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

#include "Interval.h"
#include "AnException.h"
using namespace SLib;

Interval::Interval()
{
	m_interval_length = 0;
}

Interval::Interval(int duration, int duration_type)
{
	switch(duration_type){
	case SECOND:
		m_interval_length = (time_t)duration;
		break;
	case MINUTE:
		m_interval_length = (time_t)(duration * 60);
		break;
	case HOUR:
		m_interval_length = (time_t)(duration * 60 * 60);
		break;
	case DAY:
		m_interval_length = (time_t)(duration * 60 * 60 * 24);
		break;
	case WEEK:
		m_interval_length = (time_t)(duration * 60 * 60 * 24 * 7);
		break;
	default:
		throw AnException(0, FL, "Invalid duration_type");
	}
}

Interval::Interval(const Interval& i)
{
	m_interval_length = i.m_interval_length;
}

Interval& Interval::operator=(const Interval& i)
{
	m_interval_length = i.m_interval_length;
	return *this;
}

Interval& Interval::operator=(const time_t i)
{
	m_interval_length = i;
	return *this;
}

#ifndef AIX
Interval& Interval::operator=(const int i)
{
	m_interval_length = (time_t)i;
	return *this;
}
#endif

Interval::~Interval()
{
	// nothing to do yet.
}

int Interval::Sec(void) const
{
	return (int)m_interval_length;
}

int Interval::operator()(void) const
{
	return (int)m_interval_length;
}

Interval Interval::operator+(const Interval& i) const
{
	Interval newint = *this;
	newint += i;
	return newint;
}

Interval& Interval::operator+=(const Interval& i)
{
	m_interval_length += i.m_interval_length;
	return *this;
}

Interval Interval::operator-(const Interval& i) const
{
	Interval newint = *this;
	newint -= i;
	return newint;
}

Interval& Interval::operator-=(const Interval& i)
{
	m_interval_length -= i.m_interval_length;
	return *this;
}

bool Interval::operator>(const Interval& i) const
{
	return (m_interval_length > i.m_interval_length);
}

bool Interval::operator>=(const Interval& i) const
{
	return (m_interval_length >= i.m_interval_length);
}

bool Interval::operator<(const Interval& i) const
{
	return (m_interval_length < i.m_interval_length);
}

bool Interval::operator<=(const Interval& i) const
{
	return (m_interval_length <= i.m_interval_length);
}

bool Interval::operator==(const Interval& i) const
{
	return (m_interval_length == i.m_interval_length);
}

int Interval::Days(void)
{
	return m_interval_length / (60 * 60 * 24);
}
