
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

#include <string.h>

#include "Log.h"
using namespace SLib;

#include "Parms.h"

using namespace SLib;


Parms::Parms()
{
	TRACE(FL, "Enter Parms::Parms()");

	TRACE(FL, "Exit  Parms::Parms()");
}

Parms::Parms(const Parms& c)
{
	TRACE(FL, "Enter Parms::Parms(const Parms& c)");

	m_simple_list = c.m_simple_list;
	m_values = c.m_values;
	m_flags = c.m_flags;
	m_short_values = c.m_short_values;
	m_short_flags = c.m_short_flags;
	m_long_values = c.m_long_values;
	m_long_flags = c.m_long_flags;
	m_argc = c.m_argc;
	m_argv = c.m_argv;

	TRACE(FL, "Exit  Parms::Parms(const Parms& c)");
}

Parms::~Parms()
{
	TRACE(FL, "Enter Parms::~Parms()");

	TRACE(FL, "Exit  Parms::~Parms()");
}

Parms& Parms::operator=(const Parms& c)
{
	TRACE(FL, "Enter Parms::operator=()");

	m_simple_list = c.m_simple_list;
	m_values = c.m_values;
	m_flags = c.m_flags;
	m_short_values = c.m_short_values;
	m_short_flags = c.m_short_flags;
	m_long_values = c.m_long_values;
	m_long_flags = c.m_long_flags;
	m_argc = c.m_argc;
	m_argv = c.m_argv;

	TRACE(FL, "Exit  Parms::operator=()");
	return *this;
}

void Parms::SetShortFlags(twine short_flag_list)
{
	TRACE(FL, "Enter Parms::SetShortFlags()");

	m_short_flags = short_flag_list.split(",");	

	TRACE(FL, "Exit  Parms::SetShortFlags()");
}

void Parms::SetShortValues(twine short_value_list)
{
	TRACE(FL, "Enter Parms::SetShortValues()");

	m_short_values = short_value_list.split(",");	

	TRACE(FL, "Exit  Parms::SetShortValues()");
}

void Parms::SetLongFlags(twine long_flag_list)
{
	TRACE(FL, "Enter Parms::SetLongFlags()");

	m_long_flags = long_flag_list.split(",");

	TRACE(FL, "Exit  Parms::SetLongFlags()");
}

void Parms::SetLongValues(twine long_value_list)
{
	TRACE(FL, "Enter Parms::SetLongValues()");

	m_long_values = long_value_list.split(",");

	TRACE(FL, "Exit  Parms::SetLongValues()");
}

void Parms::Parse(int argc, char** argv)
{
	TRACE(FL, "Enter Parms::Parse()");
	
	int i, j;
	bool found;

	m_argc = argc;
	m_argv = argv;
	m_simple_list.clear();

	for(i = 1; i < m_argc; i++){
		if(m_argv[i][0] == '-'){
			if(m_argv[i][1] == '-'){  // Long argument
				found = false;
				for(j = 0; j < (int)m_long_values.size(); j++){
					if(m_long_values[j] == m_argv[i]){
						m_values[m_long_values[j]] =
							m_argv[++i];
						found = true;
					}
				}
				if(found) continue;
				for(j = 0; j < (int)m_long_flags.size(); j++){
					if(m_long_flags[j] == m_argv[i]){
						m_flags[m_long_flags[j]] =
							true;
					}
				}
			} else {  // Short argument
				found = false;
				for(j=0;j<(int)m_short_values.size(); j++){
					if(m_short_values[j].compare(m_argv[i],
						2) == 0)
					{
						if(strlen(argv[i]) > 2){
							m_values[m_short_values[j]] = m_argv[i]+2;
						} else {
							m_values[m_short_values[j]] = m_argv[++i];
						}
						found = true;
					}
				}
				if(found) continue;
				for(j = 0; j < (int)m_short_flags.size(); j++){
					if(m_short_flags[j] == m_argv[i]){
						m_flags[m_short_flags[j]] =
							true;
					}
				}
			}
		} else { // simple value
			m_simple_list.push_back(m_argv[i]);
		}
	}


	TRACE(FL, "Exit  Parms::Parse()");
}

bool Parms::ShortFlag(twine flagname)
{
	TRACE(FL, "Enter Parms::ShortFlag()");

	if(m_flags.count(flagname) > 0){
		return m_flags[flagname];
	} else {
		return false;
	}

	TRACE(FL, "Exit  Parms::ShortFlag()");
}

twine Parms::ShortValue(twine shortname)
{
	TRACE(FL, "Enter Parms::ShortValue()");

	if(m_values.count(shortname) > 0){
		return m_values[shortname];
	} else {
		return "";
	}

	TRACE(FL, "Exit  Parms::ShortValue()");
}

bool Parms::LongFlag(twine flagname)
{
	TRACE(FL, "Enter Parms::LongFlag()");

	if(m_flags.count(flagname) > 0){
		return m_flags[flagname];
	} else {
		return false;
	}

	TRACE(FL, "Exit  Parms::LongFlag()");
}

twine Parms::LongValue(twine longname)
{
	TRACE(FL, "Enter Parms::LongValue()");

	if(m_values.count(longname) > 0){
		return m_values[longname];
	} else {
		return "";
	}

	TRACE(FL, "Exit  Parms::LongValue()");
}


vector < twine >& Parms::Simples(void)
{
	TRACE(FL, "Enter Parms::Simples()");
	TRACE(FL, "Exit  Parms::Simples()");
	return m_simple_list;
}

