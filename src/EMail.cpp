
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
#include "AnException.h"

using namespace SLib;

EMail::EMail(const twine& subject)
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
	// Ensure all attachment data is destroyed
	for(size_t i = 0; i < m_attachment_list.size(); i++){
		if(m_attachment_list[i].data != NULL){
			delete m_attachment_list[i].data;
			m_attachment_list[i].data = NULL;
		}
	}
}

void EMail::Subject(const twine& subj)
{
	m_subject = subj;
}

twine& EMail::Subject(void)
{
	return m_subject;
}

void EMail::Body(const twine& msg)
{
	size_t ptr1;
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

void EMail::From(const twine& addr)
{
	m_from = addr;
}

twine& EMail::From(void)
{
	return m_from;
}

void EMail::ReplyTo(const twine& addr)
{
	m_reply_to = addr;
}

twine& EMail::ReplyTo(void)
{
	return m_reply_to;
}

void EMail::AddTO(const twine& addr)
{
	m_to_list.push_back(addr);
}

vector < twine >& EMail::TOList(void)
{
	return m_to_list;
}

void EMail::AddCC(const twine& addr)
{
	m_cc_list.push_back(addr);
}

vector < twine >& EMail::CCList(void)
{
	return m_cc_list;
}

void EMail::AddHeader(const twine& name, const twine& value)
{
	pair<twine, twine> p( name, value);
	m_headers.push_back( p );
}

vector < pair<twine, twine> >& EMail::HeaderList(void)
{
	return m_headers;
}

void EMail::AddAttachment(const twine& fileName, const twine& mimeType, MemBuf* buf)
{
	if(fileName.empty()){
		throw AnException(0, FL, "Missing attachment fileName");
	}
	if(mimeType.empty()){
		throw AnException(0, FL, "Missing attachment mimeType");
	}
	if(buf == NULL || buf->size() == 0){
		throw AnException(0, FL, "Missing attachment data");
	}
	EMailAttachment a;
	a.fileName = fileName;
	a.mimeType = mimeType;
	a.data = buf;
	m_attachment_list.push_back(a);
}

vector < EMailAttachment >& EMail::AttachmentList(void)
{
	return m_attachment_list;
}

Date& EMail::CreateDate(void)
{
	return m_create_date;
}
