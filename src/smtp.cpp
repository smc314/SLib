
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

#ifndef _WIN32
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#ifdef AIX
#include <netdb.h>
#endif

#include "smtp.h"
#include "AnException.h"
#include "Socket.h"
#include "Log.h"
#include "Lock.h"
#include "Mutex.h"
using namespace SLib;

// DNS Resolver functions aren't intrinsically thread safe.
static Mutex dns_mut;

smtp::smtp()
{
	twine host;
	host.reserve(64);
	int ret = gethostname(host.data(), 64);
	host.check_size();
	if (ret == -1){
		host = "unavail";
	}
	m_hostname = host;

#ifndef _WIN32
	host.erase();
	ret = getdomainname(host.data(), 64);
	host.check_size();
	if(ret == -1){
		host = "nodomain";
	}
	m_hostname += "." + host;
#endif

}

smtp::~smtp()
{
	// nothing yet.
}

void smtp::Send(EMail& message)
{
	/* *********************************************************** */
	/* We have to figure out which mail hosts to send this message */
	/* to.  Do this by takeing the machine name out of the email   */
	/* address.  Group everyone up by machine name, and then send  */
	/* the e-mail out to the specific machines in batches.         */
	/* *********************************************************** */
#ifdef _WIN32
	throw AnException(0, FL, "Direct SMTP Send not supported on Win32.  You must use a forwarding machine setup.");
#endif

	vector < pair < twine, int > > send_list;
	int i;
	int start = 0;

	vector < twine >& tolist = message.TOList();
	vector < twine >& cclist = message.CCList();

	for(i = 0; i < (int)tolist.size(); i++){
		pair < twine, int > apair;
		apair.first = tolist[i];
		apair.second = 0;
		send_list.push_back(apair);
	}

	for(i = 0; i < (int)cclist.size(); i++){
		pair < twine, int > bpair;
		bpair.first = cclist[i];
		bpair.second = 0;
		send_list.push_back(bpair);
	}

	bool something_to_do = true;
	while(something_to_do){
		something_to_do = false;
		// see if there is an un-mailed address
		for(i = 0; i < (int)send_list.size(); i++){
			if(send_list[i].second == 0){
				something_to_do = true;
				start = i;
				break;
			}
		}
		if(something_to_do){
			// find all other addresses that are un-mailed
			// that have the same host.
			twine host;
			int ptr1;
			ptr1 = send_list[start].first.find("@");
			if(ptr1 == (int)TWINE_NOT_FOUND){
				ERRORL(FL, "EMail address (%s) has no @",
					send_list[start].first());
				send_list[start].second = 1;
				continue;
			}
			host = send_list[start].first.substr(ptr1+1);
			vector < twine > tmp_send_list;
			tmp_send_list.clear();
			tmp_send_list.push_back(send_list[start].first);
			send_list[start].second = 1;

			for(i = start+1; i < (int)send_list.size(); i++){
				twine tmphost;
				ptr1 = send_list[i].first.find("@");
				if(ptr1 == (int)TWINE_NOT_FOUND){
					ERRORL(FL, 
						"EMail address (%s) has no @",
						send_list[i].first());
					send_list[i].second = 1;
					continue;
				}
				tmphost = send_list[i].first.substr(ptr1+1);
				if(host == tmphost){
					tmp_send_list.push_back(
						send_list[i].first);
					send_list[i].second = 1;
				}
			}
			
			twine mxhost = FindBestMX(host);
			SendToMachine(message, tmp_send_list, mxhost);
		}
	}
}

void smtp::Send(EMail& message, twine& local_forwarding_machine)
{
	vector < twine > rcpt_list;

	// Send to everyone regarless of mail host.  the forwarding
	// machine will forward to everyone on the correct host.
	rcpt_list = message.TOList();
	vector < twine >& cclist = message.CCList();
	for(int i = 0; i < (int)cclist.size(); i++){
		rcpt_list.push_back(cclist[i]);
	}

	SendToMachine(message, rcpt_list, local_forwarding_machine);
}


void smtp::SendToMachine(EMail& message, vector < twine >& rcpt_list,
	twine& machine_name)
{

	dptr < Socket > the_sock;

	twine whole_body;
	twine tmp;
	int ret;
	int i;

	if(rcpt_list.size() == 0){
		return;
	}

	ERRORL(FL, "Sending to machine (%s)", machine_name());

	whole_body = FormatBody(message);

	tmp.reserve(2048);

	try {
		the_sock = new Socket((char*)machine_name(), 25);
	} catch (AnException e){
		e.AddMsg("\nError Connecting to target smtp.");
		throw e;
	}

	tmp.erase();
	ret = the_sock->GetRawData(tmp.data(), 2048);
	tmp.check_size();		
	if(ret == 0){
		throw AnException(0, FL, "Remote connection closed.");
	}		
	tmp.erase();
	tmp.format("HELO %s\r\n", m_hostname());
	the_sock->SendData(tmp);
	
	tmp.erase();
	ret = the_sock->GetRawData(tmp.data(), 2048);
	tmp.check_size();
	if(ret == 0){
		throw AnException(0, FL, "Remote connection closed.");
	}		
	if(tmp.find("250") == TWINE_NOT_FOUND){
		throw AnException(0, FL, "Error initiating mail:\n%s", tmp());
	}

	tmp.erase();
	tmp.format("MAIL FROM: %s\r\n", message.From()());
	the_sock->SendData(tmp);
	
	tmp.erase();
	ret = the_sock->GetRawData(tmp.data(), 2048);
	tmp.check_size();
	if(ret == 0){
		throw AnException(0, FL, "Remote connection closed.");
	}		
	if(tmp.find("250") == TWINE_NOT_FOUND){
		throw AnException(0, FL, "Error initiating mail:\n%s", tmp());
	}
	
	for(i=0;i< (int)rcpt_list.size();i++){
		tmp.erase();
		tmp.format("RCPT TO: %s\r\n", rcpt_list[i]());
		the_sock->SendData(tmp);
		
		tmp.erase();
		ret = the_sock->GetRawData(tmp.data(), 2048);
		tmp.check_size();
		if(ret == 0){
			throw AnException(0, FL, "Remote connection closed.");
		}		
		if(tmp.find("250") == TWINE_NOT_FOUND){
			throw AnException(0, FL, "Error initiating mail:\n%s", 
				tmp());
		}
	}

	tmp.erase();
	tmp.format("DATA\r\n");
	the_sock->SendData(tmp);
	
	tmp.erase();
	ret = the_sock->GetRawData(tmp.data(), 2048);
	tmp.check_size();
	if(ret == 0){
		throw AnException(0, FL, "Remote connection closed.");
	}		
	if(tmp.find("354") == TWINE_NOT_FOUND){
		throw AnException(0, FL, "Error initiating mail:\n%s", tmp());
	}

	the_sock->SendData(whole_body);

	tmp.erase();
	ret = the_sock->GetRawData(tmp.data(), 2048);
	tmp.check_size();
	if(ret == 0){
		throw AnException(0, FL, "Remote connection closed.");
	}		
	if(tmp.find("250") == TWINE_NOT_FOUND){
		throw AnException(0, FL, "Error sending mail:\n%s", tmp());
	}

	tmp.erase();
	tmp.format("QUIT\r\n");
	the_sock->SendData(tmp);
	
	tmp.erase();
	ret = the_sock->GetRawData(tmp.data(), 2048);
	tmp.check_size();
	if(ret == 0){
		ERRORL( FL, "Remote connection closed.");
	}		
	if(tmp.find("221") == TWINE_NOT_FOUND){
		ERRORL(FL, "Error closing mail connection:\n%s", tmp());
	}
}
	


twine smtp::FormatBody(EMail& message)
{
	twine whole_body;
	int i;

	if(message.ReplyTo().size() == 0){
		message.ReplyTo(message.From());
	}

	whole_body.format(
		"From: %s" CRLF
		"Subject: %s" CRLF
		"Date: %s" CRLF
		"Reply-To: %s" CRLF
		"To:",
		message.From()(),
		message.Subject()(),
		message.CreateDate().EDate(),
		message.ReplyTo()());

	for(i = 0; i < (int)message.TOList().size()-1; i++){
		whole_body += " " + message.TOList()[i] + "," CRLF;
	}
	i = (int)message.TOList().size()-1;
	whole_body += " " + message.TOList()[i] + CRLF;

	if(message.CCList().size() > 0){
		whole_body += "Cc:";

		for(i = 0; i < (int)message.CCList().size()-1; i++){
			whole_body += " " + message.CCList()[i] + "," CRLF;
		}
		i = (int)message.CCList().size()-1;
		whole_body += " " + message.CCList()[i] + CRLF;
	}

	whole_body += CRLF;

	whole_body += message.Body();	

	whole_body += CRLF "." CRLF;

	return whole_body;
}

twine smtp::FindBestMX(twine& domain)
{
#ifdef _WIN32

	throw AnException(0, FL, "MX Lookup not supported on Win32.  You must use a forwarding host setup.");

#else 
	unsigned char response[4096];
	int len, rlen, count, i;
	unsigned char *ptr1, *ptr2;
	HEADER *head;
	char domain_name[128];
	char target_name[128];
	int type, pref, size;

	int max_pref = 0;
	twine best_target = domain;

	Lock the_lock(&dns_mut);

	if ((_res.options & RES_INIT) == 0){
		res_init();
	}

	rlen = res_search(domain(), C_IN, T_MX, response, 4095);
	if(rlen < 0){
		return best_target;
	}

	/* ****************************************************** */
	/* The answer returned from a dns search has a header, a  */
	/* list of the query information given, a list of answers */
	/* to the query, and a list of other records.             */
	/* ****************************************************** */

	// grab the header.
 	head = (HEADER *)response;

	// point to the query data:
	ptr1 = response + sizeof(HEADER);

	// how many query records do we have?
	count = ntohs(head->qdcount);

	// skip 'em all:
	for(i = 0; i < count; i++){	

		len = dn_expand(response, response + rlen,
			ptr1, domain_name, 128);
		if(len < 0){
			return best_target;
		}
		ptr1 += len + 4;  // skip to end of this record.
	}

	// how many answer records do we have?
	count = ntohs(head->ancount);

	// grab 'em all
	for(i = 0; i < count; i++){

		len = dn_expand(response, response + rlen,
			ptr1, domain_name, 128);
		if(len < 0){
			return best_target;
		}

		ptr1 += len;
		GETSHORT(type, ptr1); // Decode the type.
		ptr1 += 6;
		GETSHORT(size, ptr1); // Decode size of addl data
		ptr2 = ptr1 + size;   // Save the start of the next record

		GETSHORT(pref, ptr1); // Decode MX Preference
		len = dn_expand(response, response + rlen, ptr1, 
			target_name, 128);
		if(len < 0){
			return best_target;
		}

		if(pref > max_pref){
			max_pref = pref;
			best_target = target_name;
		}

		// point to the next record and keep going
		ptr1 = ptr2;

	}

	// we don't care about the other records in the answer.

	return best_target;
#endif
}
