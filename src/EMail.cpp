
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


#include "EMail.h"

using namespace SLib;

EMail::EMail(twine subject)
{
	m_subject = subject;
}

EMail::EMail(const EMail& e)
{
	m_subject = e.m_subject;
	m_body = e.m_body;
	m_from = e.m_from;
	m_reply_to = e.m_reply_to;
	m_to_list = e.m_to_list;
	m_cc_list = e.m_cc_list;
	m_create_date = e.m_create_date;
}

EMail& EMail::operator=(const EMail& e)
{
	m_subject = e.m_subject;
	m_body = e.m_body;
	m_from = e.m_from;
	m_reply_to = e.m_reply_to;
	m_to_list = e.m_to_list;
	m_cc_list = e.m_cc_list;
	m_create_date = e.m_create_date;
	return *this;
}

EMail::~EMail()
{
	// nothing yet.
}

void EMail::Subject(twine& subj)
{
	m_subject = subj;
}

twine& EMail::Subject(void)
{
	return m_subject;
}

void EMail::Body(twine& msg)
{
	int ptr1;
	m_body = msg;

	ptr1 = m_body.find('\n');
	while(ptr1 != TWINE_NOT_FOUND){
		if(ptr1 == 0){
			m_body.insert(0, "\r");
		} else {
			if(m_body[ptr1-1] != '\r'){
				m_body.insert(ptr1, "\r");
			}
		}
		ptr1 = m_body.find('\n', ptr1+1);
	}
}

twine& EMail::Body(void)
{
	return m_body;
}

void EMail::From(twine& addr)
{
	m_from = addr;
}

twine& EMail::From(void)
{
	return m_from;
}

void EMail::ReplyTo(twine& addr)
{
	m_reply_to = addr;
}

twine& EMail::ReplyTo(void)
{
	return m_reply_to;
}

void EMail::AddTO(twine& addr)
{
	m_to_list.push_back(addr);
}

vector < twine >& EMail::TOList(void)
{
	return m_to_list;
}

void EMail::AddCC(twine& addr)
{
	m_cc_list.push_back(addr);
}

vector < twine >& EMail::CCList(void)
{
	return m_cc_list;
}

Date& EMail::CreateDate(void)
{
	return m_create_date;
}
