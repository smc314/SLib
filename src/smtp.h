#ifndef SMTP_H
#define SMTP_H
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
#include "EMail.h"
using namespace SLib;

#include <vector>
using namespace std;

namespace SLib {

/**
  * This class defines our smtp interface.  It handles accepting
  * messages from our application and forwarding them on as e-mail
  * messages to the appropriate server.  It implements a subset of
  * RFC-821 commands.  Just those that are necessary to handle the
  * simple e-mail that we need.
  *
  * @author Steven M. Cherry
  * @version $Revision: 1.1.1.1 $
  * @copyright 2002 Steven M. Cherry
  */
class DLLEXPORT smtp
{
	public:
		/// Standard Constructor
		smtp();

		/// Standard Destructor
		virtual ~smtp();

		/// Send a message using a local forwarding machine
		void Send(EMail& message, twine& local_forwarding_machine);

		/// Send a message directly to the recipiants' mail hosts 
		void Send(EMail& message);

	protected:

		/// Send a message to a given list of people on one machine
		void SendToMachine(EMail& message, vector < twine >& rcpt_list,
			twine& machine_name);

		/// Used to create the complete message body with headers
		twine FormatBody(EMail& message);

		/// Used to look up MX DNS records for the given domain.
		twine FindBestMX(twine& domain);

		twine m_hostname;

};

} // End Namespace

#endif // SMTP_H Defined
