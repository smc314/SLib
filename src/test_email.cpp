
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
#include <stdio.h>

#include "smtp.h"
#include "EMail.h"
#include "twine.h"
#include "AnException.h"
using namespace SLib;

int main (void)
{
	smtp the_smtp;
	EMail the_email("smtp and EMail test, what a pain");
	twine machine = "localhost";

	twine tmp;

	tmp = "stevenc@flashcom.net";
	the_email.From(tmp);
	tmp = "smc@cherrys.org";
	the_email.ReplyTo(tmp);

	tmp = "smc@cherrys.org";
	the_email.AddTO(tmp);

	tmp = "quadra@mail.ptd.net";
	the_email.AddTO(tmp);

	tmp = "gb@cherrys.org";
	the_email.AddCC(tmp);


	twine message;


	message.format(
		"Oh, and now I find out that all the dns functions\n"
		"aren't thread safe either.  <hmph>  At least that\n"
		"is simple to fix.\n"
		"\n"
		"Man, what a pain in the ass.  In order to correctly\n"
		"send out an e-mail, I have to take the portion of the\n"
		"e-mail address after the @ sign, use that in a dns\n"
		"lookup for MX (mail exchange) host information, find\n"
		"the MX host with the highest priority, and then send\n"
		"the e-mail there.  Geeze.\n"
		"\n"
		"And the code to do this stuff is no walk in the park\n"
		"either.  Totally undocumented, zero info in the man\n"
		"pages.  Good thing people post code to the internet\n"
		"or our e-mailng days would be over.\n"
		"\n"
		"Bah.  Anyway, this last test should prove that everything\n"
		"is working the way that it should be.  This is a direct\n"
		"connect e-mail that was delivered to your host e-mail\n"
		"system by the smtp class.\n");
	the_email.Body(message);

	try {

		the_smtp.Send(the_email);

	} catch (AnException e){
		printf("Caught Exception:\n%s\n", e.Msg());
	}
}

