#ifndef EMAIL_H
#define EMAIL_H
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
#       endif
#else
#	define DLLEXPORT 
#endif

#include "twine.h"
#include "Date.h"
#include "MemBuf.h"
#include "suvector.h"
using namespace SLib;

#include <vector>
using namespace std;

namespace SLib {

/**
  * This class represents an attachment for an e-mail message.  It holds
  * all of the information necessary to get the attachment correctly formated
  * and delivered to all of it's destinations.
  *
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT EMailAttachment
{
	public:
		twine fileName;
		twine mimeType;
		MemBuf* data;
};

/**
  * This class represents an individual e-mail message.  It holds
  * all of the information necessary to get the email correctly formated
  * and delivered to all of it's destinations.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT EMail
{
	public:
		/**
		  * The standard constructor requires a subject only.
		  * The rest of the e-mail attributes can be filled in
		  * afterwards.
		  */
		EMail(const twine& subject);

		/// Copy Constructor
		EMail(const EMail& e);

		/// Assignment operator
		EMail& operator=(const EMail& e);

		/// Standard destructor
		virtual ~EMail();

		/// Set the Subject of the e-mail
		void Subject(const twine& subj);

		/// Get the Subject of the e-mail
		twine& Subject(void);

		/** 
		  * Set the body text of the e-mail.  When you set the body
		  * text, you must ensure that no line is longer than 80
		  * characters.  This is a requirement of the e-mail systems
		  * we will be communicating with.
		  */
		void Body(const twine& msg);

		/// Get the body text of the e-mail
		twine& Body(void);

		/// Set the from e-mail address
		void From(const twine& addr);

		/// Get the from e-mail address
		twine& From(void);

		/// Set the reply-to e-mail address
		void ReplyTo(const twine& addr);
	
		/// Get the reply-to e-mail address
		twine& ReplyTo(void);

		/// Add an e-mail address to the TO list.
		void AddTO(const twine& addr);

		/// Get the full list of TO addresses
		vector < twine >& TOList(void);		

		/// Add an e-mail address to the CC list.
		void AddCC(const twine& addr);

		/// Get the full list of CC addresses
		vector < twine >& CCList(void);

		/// Add an e-mail header to the header list.
		void AddHeader(const twine& name, const twine& value);

		/// Get the full list of headers
		vector < pair< twine, twine> >& HeaderList(void);

		/// Add an e-mail attachment - Note, we'll delete this data when we destruct
		void AddAttachment(const twine& fileName, const twine& mimeType, MemBuf* buf);

		/// Get the full list of Attachments
		vector < EMailAttachment >& AttachmentList(void);

		/// Get the date this e-mail was created
		Date& CreateDate(void);

	private:

		twine m_subject;
		twine m_body;
		twine m_from;
		twine m_reply_to;
		suvector < twine > m_to_list;
		suvector < twine > m_cc_list;
		vector < pair< twine, twine> > m_headers;
		Date m_create_date;
		vector < EMailAttachment > m_attachment_list;

};

} // End namespace

#endif // EMAIL_H Defined
