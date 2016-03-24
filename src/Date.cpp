
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Date.h"

#include "AnException.h"

using namespace SLib;

Date::Date()
{
	// Set the internal structures to the current time.
	m_TimeVal = time(NULL);
	m_TimeStruct = (struct tm *)malloc(sizeof(struct tm));
	memcpy(m_TimeStruct, localtime(&m_TimeVal), sizeof(struct tm));
	m_picture = (char *)malloc(64);
	m_len = 19;
}	

Date::Date(const Date& d)
{
	//m_TimeVal = d.m_TimeVal;
	m_TimeStruct = (struct tm *)malloc(sizeof(struct tm));
	memcpy(m_TimeStruct, d.m_TimeStruct, sizeof(struct tm));
	m_picture = (char *)malloc(64);
	m_len = 19;
}

Date::~Date()
{
	free(m_TimeStruct);
	free(m_picture);
}

Date& Date::operator=(const Date& d)
{
	if(&d == this){
		return *this;
	}
	//m_TimeVal = d.m_TimeVal;
	memcpy(m_TimeStruct, d.m_TimeStruct, sizeof(struct tm));
	return *this;
}

void Date::SetCurrent(void)
{
	m_TimeVal = time(NULL);
	memcpy(m_TimeStruct, localtime(&m_TimeVal), sizeof(struct tm));
}

void Date::SetValue(const char *date)
{
	int ret;

	// date format is expected as YYYY/MM/DD HH:MM:SS
	if(strlen(date) < 19){
		throw AnException(0, FL, "Format is shorter than 19 chars.  "
			"Format is expected to be YYYY/MM/DD HH:MM:SS. "
			"Your string is (%s)", date);
	}

	ret = sscanf(date, "%d/%d/%d %d:%d:%d", &(m_TimeStruct->tm_year),
		&(m_TimeStruct->tm_mon),
		&(m_TimeStruct->tm_mday),
		&(m_TimeStruct->tm_hour),
		&(m_TimeStruct->tm_min),
		&(m_TimeStruct->tm_sec));

	if(ret != 6){
		throw AnException(0, FL, "Not all conversions happened "
			"successfully for date(%s) in standard format.", date);
	}

	/* ********************************************************* */
	/* The year in this structure represents the number of years */
	/* since 1900.  Adjust for this.                             */
	/* ********************************************************* */
	if(m_TimeStruct->tm_year < 100){
		if(m_TimeStruct->tm_year < 50)
			m_TimeStruct->tm_year += 2000;
		else 
			m_TimeStruct->tm_year += 1900;
	}
	m_TimeStruct->tm_year -= 1900;

	m_TimeStruct->tm_mon -= 1; // months in struct are 0 based.
	
	// Ensure our TimeVal is also in sync.
	//m_TimeVal = mktime(m_TimeStruct);
		
}

void Date::SetValue(const twine& date)
{
	SetValue(date());
}

void Date::SetValue(const twine* date)
{
	SetValue(date->c_str());
}

void Date::SetValue(void)
{
	SetValue(m_picture);
}

void Date::SetValue(const char* date, const char* format)
{
	int* values[6];
	size_t field_count, ptr1, ret;
	bool found_year, found_month, found_day;
	twine format_copy;

	//figure out the order of the format characters

	field_count = 0;
	found_year = found_month = found_day = false;
	format_copy = format;
	ptr1 = format_copy.find('%');
	while(ptr1 != TWINE_NOT_FOUND){
		// skip %%
		if(format_copy[ptr1+1] == '%'){
			format_copy.erase(ptr1,1);
			ptr1 ++;
			ptr1 = format_copy.find('%', ptr1);
			continue;
		}
		switch (format_copy[ptr1+1]){
		case 'Y':
			values[field_count++] = &(m_TimeStruct->tm_year);
			format_copy.replace(ptr1+1, 1, "4d");
			found_year = true;
			break;
		case 'm':
			values[field_count++] = &(m_TimeStruct->tm_mon);
			format_copy.replace(ptr1+1, 1, "2d");
			found_month = true;
			break;
		case 'd':
			values[field_count++] = &(m_TimeStruct->tm_mday);
			format_copy.replace(ptr1+1, 1, "2d");
			found_day = true;
			break;
		case 'H':
			values[field_count++] = &(m_TimeStruct->tm_hour);
			format_copy.replace(ptr1+1, 1, "2d");
			break;
		case 'M':
			values[field_count++] = &(m_TimeStruct->tm_min);
			format_copy.replace(ptr1+1, 1, "2d");
			break;
		case 'S':
			values[field_count++] = &(m_TimeStruct->tm_sec);
			format_copy.replace(ptr1+1, 1, "2d");
			break;
		default:
			throw AnException(0, FL, "Invalid format character %%%c"
				" found in date format string (%s)", 
				format_copy[ptr1+1], format);
		}

		ptr1 += 2;
		ptr1 = format_copy.find('%', ptr1);
	}

	if(field_count < 3){
		throw AnException(0, FL, "Invalid format (%s) %%Y, %%m, and "
			"%%d must be specified.", format);
	}
	if(!found_year){
		throw AnException(0, FL, "Invalid format (%s) %%Y missing.",
			format);
	}
	if(!found_month){
		throw AnException(0, FL, "Invalid format (%s) %%m missing.",
			format);
	}
	if(!found_day){
		throw AnException(0, FL, "Invalid format (%s) %%d missing.",
			format);
	}

	m_TimeStruct->tm_hour = 0;
	m_TimeStruct->tm_min = 0;
	m_TimeStruct->tm_sec = 0;

	switch (field_count){
	case 3:
		ret = sscanf(date, format_copy(), 
			values[0], values[1], values[2]);
		if(ret != 3){
			throw AnException(0, FL, "Not all conversions happend "
				"for date (%s) in format (%s).", date, format);
		}
		break;
	case 4:
		ret = sscanf(date, format_copy(), 
			values[0], values[1], values[2],
			values[3]);
		if(ret != 4){
			throw AnException(0, FL, "Not all conversions happend "
				"for date (%s) in format (%s).", date, format);
		}
		break;
	case 5:
		ret = sscanf(date, format_copy(), 
			values[0], values[1], values[2],
			values[3], values[4]);
		if(ret != 4){
			throw AnException(0, FL, "Not all conversions happend "
				"for date (%s) in format (%s).", date, format);
		}
		break;
	case 6:
		ret = sscanf(date, format_copy(), 
			values[0], values[1], values[2],
			values[3], values[4], values[5]);
		if(ret != 6){
			throw AnException(0, FL, "Not all conversions happend "
				"for date (%s) in format (%s).", date, format);
		}
		break;
	default:
		throw AnException(0, FL, "Invalid number of format fields (%d) "
			"for date (%s) in format (%s)", field_count,
			date, format);
	}

	/* ********************************************************* */
	/* The year in this structure represents the number of years */
	/* since 1900.  Adjust for this.                             */
	/* ********************************************************* */
	if(m_TimeStruct->tm_year < 100){
		if(m_TimeStruct->tm_year < 50)
			m_TimeStruct->tm_year += 2000;
		else 
			m_TimeStruct->tm_year += 1900;
	}
	m_TimeStruct->tm_year -= 1900;
	m_TimeStruct->tm_mon -= 1; // months in struct are 0 based.

	// Ensure our TimeVal is also in sync.
	//m_TimeVal = mktime(m_TimeStruct);
		
}

void Date::SetValue(const twine& date, const twine& format)
{
	SetValue(date(), format());
}

void Date::SetValue(const twine* date, const twine* format)
{
	SetValue(date->c_str(), format->c_str());
}

void Date::SetValuef(twine& format)
{
	SetValue(m_picture, format());
}


void Date::SetValue(const time_t t)
{
	m_TimeVal = t;
	memcpy(m_TimeStruct, localtime(&m_TimeVal), sizeof(struct tm));
}

Date::operator time_t() const
{
	struct tm tmp_tm;

	// use a temporary because mktime modifies it's argument.
	memcpy(&tmp_tm, m_TimeStruct, sizeof(struct tm));
	time_t tmp = mktime(&tmp_tm);	
	return tmp;
}

void Date::SetValue(const struct tm* t)
{
	memcpy(m_TimeStruct, t, sizeof(struct tm));
	//m_TimeVal = mktime(m_TimeStruct);
}

Date::operator const struct tm*() const
{
	return m_TimeStruct;
}

Date::operator const char*()
{
	return GetValue();
}

void Date::SetValue(xmlChar* c)
{
	SetValue( (char*)c);
	xmlFree(c);
}

Date::operator const xmlChar*()
{
	return (const xmlChar*)GetValue();
}

#ifdef _WIN32
#if 0
void Date::SetValue(const CTime& ct)
{
	Year(ct.GetYear());
	Month(ct.GetMonth());
	Day(ct.GetDay());
	Hour(ct.GetHour());
	Min(ct.GetMinute());
	Sec(ct.GetSecond());
}

Date::operator CTime() const
{
	return CTime(Year(), Month(), Day(), Hour(), Min(), Sec());
}

void Date::SetValue(const COleDateTime& codt)
{
	Year(codt.GetYear());
	Month(codt.GetMonth());
	Day(codt.GetDay());
	Hour(codt.GetHour());
	Min(codt.GetMinute());
	Sec(codt.GetSecond());
}

Date::operator COleDateTime() const
{
	return COleDateTime(Year(), Month(), Day(), Hour(), Min(), Sec());
}

void Date::SetValue(const DATE d)
{
	SetValue(COleDateTime(d));
}

Date::operator DATE() const
{
	return (DATE)operator COleDateTime();
}
#endif
#endif // _WIN32


char *Date::GetValue(void)
{
	memset(m_picture, 0, 64);

	strftime(m_picture, 63, "%Y/%m/%d %H:%M:%S", m_TimeStruct);

	m_len = 19;
	
	return m_picture;
}
	
char *Date::GetValue(const char* format)
{
	memset(m_picture, 0, 64);

	strftime(m_picture, 63, format, m_TimeStruct);

	m_len = strlen(m_picture);
	
	return m_picture;
}

char* Date::GetValue(const twine& format)
{
	return GetValue(format());
}

char* Date::GetValue(const twine* format)
{
	return GetValue(format->c_str());
}

void Date::Floor(void)
{
	m_TimeStruct->tm_hour = 0;
	m_TimeStruct->tm_min = 0;
	m_TimeStruct->tm_sec = 0;
}

void Date::Ceil(void)
{
	m_TimeStruct->tm_hour = 23;
	m_TimeStruct->tm_min = 59;
	m_TimeStruct->tm_sec = 59;
}
	
int Date::Year(void) const
{
	return m_TimeStruct->tm_year + 1900;
}

void Date::Year(int y)
{
	m_TimeStruct->tm_year = y - 1900;
}

int Date::Month(void) const
{
	return m_TimeStruct->tm_mon + 1;
}

void Date::Month(int m)
{
	m_TimeStruct->tm_mon = m - 1;
	Normalize();
}

int Date::Day(void) const
{
	return m_TimeStruct->tm_mday;
}

void Date::Day(int d)
{
	m_TimeStruct->tm_mday = d;
	Normalize();
}

int Date::DayW(void) const
{
	return m_TimeStruct->tm_wday;
}

int Date::Hour(void) const
{
	return m_TimeStruct->tm_hour;
}

void Date::Hour(int h)
{
	m_TimeStruct->tm_hour = h;
	Normalize();
}

int Date::Min(void) const
{
	return m_TimeStruct->tm_min;
}

void Date::Min(int m)
{
	m_TimeStruct->tm_min = m;
	Normalize();
}

int Date::Sec(void) const
{
	return m_TimeStruct->tm_sec;
}

void Date::Sec(int s)
{
	m_TimeStruct->tm_sec = s;
	Normalize();
}

void Date::AddYear(int i)
{
	m_TimeStruct->tm_year += i;
}

void Date::AddMonth(int i)
{
	m_TimeStruct->tm_mon += i;
	Normalize();
}

void Date::AddDay(int i)
{
	m_TimeStruct->tm_mday += i;
	Normalize();
}

void Date::AddHour(int i)
{
	m_TimeStruct->tm_hour += i;
	Normalize();
}

void Date::AddMin(int i)
{
	m_TimeStruct->tm_min += i;
	Normalize();
}

void Date::AddSec(int i)
{
	m_TimeStruct->tm_sec += i;
	Normalize();
}

void Date::Add(SLib::Interval& i)
{
	m_TimeStruct->tm_sec += i();
	Normalize();
}

void Date::Normalize(void)
{
	/* ***************************************************** */
	/* Both mktime and gmtime play with time zones.  All we  */
	/* want from this function is a legal time structure.    */
	/* We don't want time zones taken into account.  Thus we */
	/* do our own normalization routine.                     */
	/* ***************************************************** */

	if(m_TimeStruct->tm_sec > 59){
		m_TimeStruct->tm_min += m_TimeStruct->tm_sec / 60;
		m_TimeStruct->tm_sec = m_TimeStruct->tm_sec % 60;
	}
	if(m_TimeStruct->tm_sec < 0){
		m_TimeStruct->tm_sec *= -1;
		m_TimeStruct->tm_min -= 1 + m_TimeStruct->tm_sec / 60;
		if(m_TimeStruct->tm_sec % 60 == 0){
			m_TimeStruct->tm_sec = 0;
			m_TimeStruct->tm_min ++;
		} else {
			m_TimeStruct->tm_sec = 60 - m_TimeStruct->tm_sec % 60;
		}
	}
	if(m_TimeStruct->tm_min > 59){
		m_TimeStruct->tm_hour += m_TimeStruct->tm_min / 60;
		m_TimeStruct->tm_min = m_TimeStruct->tm_min % 60;
	}
	if(m_TimeStruct->tm_min < 0){
		m_TimeStruct->tm_min *= -1;
		m_TimeStruct->tm_hour -= 1 + m_TimeStruct->tm_min / 60;
		if(m_TimeStruct->tm_min % 60 == 0){
			m_TimeStruct->tm_min = 0;
			m_TimeStruct->tm_hour ++;
		} else {
			m_TimeStruct->tm_min = 60 - m_TimeStruct->tm_min % 60;
		}
	}
	if(m_TimeStruct->tm_hour > 23){
		m_TimeStruct->tm_mday += m_TimeStruct->tm_hour / 24;
		m_TimeStruct->tm_hour = m_TimeStruct->tm_hour % 24;
	}
	if(m_TimeStruct->tm_hour < 0){
		m_TimeStruct->tm_hour *= -1;
		m_TimeStruct->tm_mday -= 1 + m_TimeStruct->tm_hour / 24;
		if(m_TimeStruct->tm_hour % 24 == 0){
			m_TimeStruct->tm_hour = 0;
			m_TimeStruct->tm_mday ++;
		} else {
			m_TimeStruct->tm_hour = 24 - m_TimeStruct->tm_hour % 24;
		}
	}
	if(m_TimeStruct->tm_mon > 11){
		m_TimeStruct->tm_year += m_TimeStruct->tm_mon / 12;
		m_TimeStruct->tm_mon = m_TimeStruct->tm_mon % 12;
	}
	if(m_TimeStruct->tm_mon < 0){
		m_TimeStruct->tm_mon *= -1;
		m_TimeStruct->tm_year -= 1 + m_TimeStruct->tm_mon / 12;
		if(m_TimeStruct->tm_mon % 12 == 0){
			m_TimeStruct->tm_mon = 0;
			m_TimeStruct->tm_year ++;
		} else {
			m_TimeStruct->tm_mon = 12 - m_TimeStruct->tm_mon % 12;
		}
	}

	// Handle day's specially.
	bool done = false;
	while (!done){

		if(m_TimeStruct->tm_mday <= 0){
			m_TimeStruct->tm_mon --;
			if(m_TimeStruct->tm_mon == -1){
				m_TimeStruct->tm_mon = 11;
				m_TimeStruct->tm_year --;
			}
		}
		switch(m_TimeStruct->tm_mon){
		case 0: // January has 31 days
		case 2: // March has 31 days
		case 4: // May has 31 days
		case 6: // July has 31 days
		case 7: // August has 31 days
		case 9: // October has 31 days
			if(m_TimeStruct->tm_mday > 31){
				m_TimeStruct->tm_mon ++;
				m_TimeStruct->tm_mday -= 31;
			} else if (m_TimeStruct->tm_mday <= 0){
				m_TimeStruct->tm_mday += 31;
			} else {
				done = true;
			}
			break;
		case 3: // April has 30 days
		case 5: // June has 30 days
		case 8: // September has 30 days
		case 10: // November has 30 days
			if(m_TimeStruct->tm_mday > 30){
				m_TimeStruct->tm_mon ++;
				m_TimeStruct->tm_mday -= 30;
			} else if (m_TimeStruct->tm_mday <= 0){
				m_TimeStruct->tm_mday += 30;
			} else {
				done = true;
			}
			break;
		case 1: // Feb has 28 days unless year % 4 == 0, then 29
			if(m_TimeStruct->tm_year % 4 == 0){
				// leap year.
				if(m_TimeStruct->tm_mday > 29){
					m_TimeStruct->tm_mon ++;
					m_TimeStruct->tm_mday -= 29;
				} else if (m_TimeStruct->tm_mday <= 0){
					m_TimeStruct->tm_mday += 29;
				} else {
					done = true;
				}
			} else {
				if(m_TimeStruct->tm_mday > 28){
					m_TimeStruct->tm_mon ++;
					m_TimeStruct->tm_mday -= 28;
				} else if (m_TimeStruct->tm_mday <= 0){
					m_TimeStruct->tm_mday += 28;
				} else {
					done = true;
				}
			}
			break;
		case 11: // December has 31 days
			if(m_TimeStruct->tm_mday > 31){
				m_TimeStruct->tm_year ++;
				m_TimeStruct->tm_mon = 0;
				m_TimeStruct->tm_mday -= 31;
			} else if (m_TimeStruct->tm_mday <= 0){
				m_TimeStruct->tm_mday += 31;
			} else {
				done = true;
			}
			break;
		}
	}
	
	// Ensure our TimeVal is also in sync.
	//m_TimeVal = mktime(m_TimeStruct);
		
}

time_t Date::Epoch(void)
{
	struct tm tmp_tm;

	// use a temporary because mktime modifies it's argument.
	memcpy(&tmp_tm, m_TimeStruct, sizeof(struct tm));
	m_TimeVal = mktime(&tmp_tm);	
	return m_TimeVal;
}

char* Date::EDate(void)
{
	memset(m_picture, 0, 64);

	strftime(m_picture, 63, "%a, %d %b %Y %H:%M:%S %z", m_TimeStruct);

	m_len = strlen(m_picture);
	
	return m_picture;
}
	

bool Date::operator==(const Date& d) const
{
	if(m_TimeStruct->tm_year != d.m_TimeStruct->tm_year)
		return false;
	if(m_TimeStruct->tm_mon != d.m_TimeStruct->tm_mon)
		return false;
	if(m_TimeStruct->tm_mday != d.m_TimeStruct->tm_mday)
		return false;
	if(m_TimeStruct->tm_hour != d.m_TimeStruct->tm_hour)
		return false;
	if(m_TimeStruct->tm_min != d.m_TimeStruct->tm_min)
		return false;
	if(m_TimeStruct->tm_sec != d.m_TimeStruct->tm_sec)
		return false;
	return true;
}

bool Date::operator<(const Date& d) const
{
	if(m_TimeStruct->tm_year != d.m_TimeStruct->tm_year)
		return m_TimeStruct->tm_year < d.m_TimeStruct->tm_year;
	if(m_TimeStruct->tm_mon != d.m_TimeStruct->tm_mon)
		return m_TimeStruct->tm_mon < d.m_TimeStruct->tm_mon;
	if(m_TimeStruct->tm_mday != d.m_TimeStruct->tm_mday)
		return m_TimeStruct->tm_mday < d.m_TimeStruct->tm_mday;
	if(m_TimeStruct->tm_hour != d.m_TimeStruct->tm_hour)
		return m_TimeStruct->tm_hour < d.m_TimeStruct->tm_hour;
	if(m_TimeStruct->tm_min != d.m_TimeStruct->tm_min)
		return m_TimeStruct->tm_min < d.m_TimeStruct->tm_min;
	return m_TimeStruct->tm_sec < d.m_TimeStruct->tm_sec;
}

bool Date::operator>(const Date& d) const
{
	if(m_TimeStruct->tm_year != d.m_TimeStruct->tm_year)
		return m_TimeStruct->tm_year > d.m_TimeStruct->tm_year;
	if(m_TimeStruct->tm_mon != d.m_TimeStruct->tm_mon)
		return m_TimeStruct->tm_mon > d.m_TimeStruct->tm_mon;
	if(m_TimeStruct->tm_mday != d.m_TimeStruct->tm_mday)
		return m_TimeStruct->tm_mday > d.m_TimeStruct->tm_mday;
	if(m_TimeStruct->tm_hour != d.m_TimeStruct->tm_hour)
		return m_TimeStruct->tm_hour > d.m_TimeStruct->tm_hour;
	if(m_TimeStruct->tm_min != d.m_TimeStruct->tm_min)
		return m_TimeStruct->tm_min > d.m_TimeStruct->tm_min;
	return m_TimeStruct->tm_sec > d.m_TimeStruct->tm_sec;
}

bool Date::operator<=(const Date& d) const
{
	if(m_TimeStruct->tm_year != d.m_TimeStruct->tm_year)
		return m_TimeStruct->tm_year < d.m_TimeStruct->tm_year;
	if(m_TimeStruct->tm_mon != d.m_TimeStruct->tm_mon)
		return m_TimeStruct->tm_mon < d.m_TimeStruct->tm_mon;
	if(m_TimeStruct->tm_mday != d.m_TimeStruct->tm_mday)
		return m_TimeStruct->tm_mday < d.m_TimeStruct->tm_mday;
	if(m_TimeStruct->tm_hour != d.m_TimeStruct->tm_hour)
		return m_TimeStruct->tm_hour < d.m_TimeStruct->tm_hour;
	if(m_TimeStruct->tm_min != d.m_TimeStruct->tm_min)
		return m_TimeStruct->tm_min < d.m_TimeStruct->tm_min;
	return m_TimeStruct->tm_sec <= d.m_TimeStruct->tm_sec;
}

bool Date::operator>=(const Date& d) const
{
	if(m_TimeStruct->tm_year != d.m_TimeStruct->tm_year)
		return m_TimeStruct->tm_year > d.m_TimeStruct->tm_year;
	if(m_TimeStruct->tm_mon != d.m_TimeStruct->tm_mon)
		return m_TimeStruct->tm_mon > d.m_TimeStruct->tm_mon;
	if(m_TimeStruct->tm_mday != d.m_TimeStruct->tm_mday)
		return m_TimeStruct->tm_mday > d.m_TimeStruct->tm_mday;
	if(m_TimeStruct->tm_hour != d.m_TimeStruct->tm_hour)
		return m_TimeStruct->tm_hour > d.m_TimeStruct->tm_hour;
	if(m_TimeStruct->tm_min != d.m_TimeStruct->tm_min)
		return m_TimeStruct->tm_min > d.m_TimeStruct->tm_min;
	return m_TimeStruct->tm_sec >= d.m_TimeStruct->tm_sec;
}

SLib::Interval Date::operator-(Date& d)
{
	SLib::Interval i;

	i = this->Epoch() - d.Epoch();
	return i;
}

Date Date::operator+(SLib::Interval& i) const
{
	Date newdate = *this;

	newdate.Add(i);
	return newdate;
}

Date& Date::operator+=(SLib::Interval& i)
{
	Add(i);
	return *this;
}

Date Date::operator-(SLib::Interval& i) const
{
	Date newdate = *this;

	newdate.m_TimeStruct->tm_sec -= i();
	newdate.Normalize();
	return newdate;
}
	
Date& Date::operator-=(SLib::Interval& i)
{
	m_TimeStruct->tm_sec -= i();
	Normalize();
	return *this;
}
	

