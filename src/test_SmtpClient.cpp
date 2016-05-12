
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

#include "SmtpClient.h"
#include "twine.h"
#include "AnException.h"
using namespace SLib;

int main (void)
{
	SmtpClient the_smtp;
	EMail the_email("smtp and EMail test, what a pain");

	the_email.From("smc@hericus.com");
	the_email.ReplyTo("smc@hericus.com");

	the_email.AddTO("sds@hericus.com");
	the_email.AddTO("smc@cherrys.org");
	the_email.AddCC("mrh@hericus.com");

	twine message;

	message.format(
		"Oh, and now I find out that all the dns functions "
		"aren't thread safe either.  <hmph>  At least that "
		"is simple to fix.\n"
		"\n"
		"Man, what a pain in the ass.  In order to correctly "
		"send out an e-mail, I have to take the portion of the "
		"e-mail address after the @ sign, use that in a dns "
		"lookup for MX (mail exchange) host information, find "
		"the MX host with the highest priority, and then send "
		"the e-mail there.  Geeze.\n"
		"\n"
		"And the code to do this stuff is no walk in the park "
		"either.  Totally undocumented, zero info in the man "
		"pages.  Good thing people post code to the internet "
		"or our e-mailng days would be over.\n"
		"\n"
		"Bah.  Anyway, this last test should prove that everything "
		"is working the way that it should be.  This is a direct "
		"connect e-mail that was delivered to your host e-mail "
		"system by the smtp class.\n"
		"\n"
		"Just kidding - this is all done with Curl now :-)"
	);
	the_email.Body(message);

	try {

		the_smtp.Send(the_email, "smtp.gmail.com", "username", "password", 587, true);

	} catch (AnException& e){
		printf("Caught Exception:\n%s\n", e.Msg());
	}
}

