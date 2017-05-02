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

#include "SmtpClient.h"
#include "Date.h"
#include "EnEx.h"
#include "Log.h"
#include "memptr.h"
#include "dptr.h"
#include "AnException.h"
#include "XmlHelpers.h"
using namespace SLib;

static bool SmtpClient_cURL_Initialized = false;

/** This one is used by the object (non-static) version of the methods below.
  */
size_t SmtpClient_ReadMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	SmtpClient* client = (SmtpClient*)userp;

	//printf("SmtpClient_ReadMemoryCallback(contents, %ld, %ld, userp) SendIndex = %ld\n", size, nmemb, client->SendIndex);

	// If they don't want us to ready any memory, bail out quickly.
	if( (size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
		//printf("SmtpClient_ReadMemoryCallback - size/nmemb is zero\n");
		return 0;
	}

	// If we've sent all of our memory already, bail out quickly
	if(client->SendIndex >= client->SendLines.size()){
		//printf("SmtpClient_ReadMemoryCallback - nothing more to send.\n");
		return 0; // nothing more to send
	}

	size_t len = client->SendLines[ client->SendIndex ].size();
	memcpy(contents, client->SendLines[ client->SendIndex ](), len );
	client->SendIndex ++;
	return len;

	/*
	// How much are they asking for?
	size_t realsize = size * nmemb;

	if( realsize > (client->SendBuffer.size() - client->SendIndex) ){
		// They are asking for more data than we have.  Send it all:
		memcpy( contents,  // Where to write the memory
			client->SendBuffer() + client->SendIndex,  // Where to read the memory
			client->SendBuffer.size() - client->SendIndex // How much to write
		);

		printf("Smtp_ReadMemoryCallback - wrote %ld bytes to buffer\n", 
			client->SendBuffer.size() - client->SendIndex
		);
			
		client->SendIndex = client->SendBuffer.size();
		return (client->SendBuffer.size() - client->SendIndex); // how much did we write
	} else {
		// They are asking for less than what we have.  Send only what they
		// ask for
		memcpy( contents, // where to write the memory
			client->SendBuffer() + client->SendIndex, // Where to read the memory
			realsize // How much to write
		);

		printf("Smtp_ReadMemoryCallback - wrote %ld bytes to buffer\n", 
			realsize
		);
			
		client->SendIndex += realsize; // Keep track of where we are in the buffer
		return realsize; // Hom much did we write
	}
	*/
}

/** This one is used to report progress of downloads
  */
int SmtpClient_ProgressCallback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	SmtpClient* client = (SmtpClient*)clientp;
	return client->Progress(dltotal, dlnow, ultotal, ulnow);
}

SmtpClient::SmtpClient()
{
	EnEx ee(FL, "SmtpClient::SmtpClient()");

	if(SmtpClient_cURL_Initialized == false){
		curl_global_init(CURL_GLOBAL_ALL);
		SmtpClient_cURL_Initialized = true;
	}
	m_curl_handle = curl_easy_init();
}

SmtpClient::~SmtpClient()
{
	EnEx ee(FL, "SmtpClient::~SmtpClient()");

	curl_easy_cleanup( m_curl_handle );
}

int SmtpClient::Progress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	// If we return a non-zero value, it will abort the transfer.
	return 0;
}

void SmtpClient::Send(EMail& message, const twine& smtpServer, const twine& user, const twine& pass, 
	int port, bool useSsl)
{
	EnEx ee(FL, "SmtpClient::Send(EMail& message, const twine& smtpServer, const twine& user, const twine& pass, int port, bool useSsl)");

	SendLines.clear();
	SendIndex = 0;
	FormatMessage( message );

	//printf("Here's what we're sending:\n%s\n", SendLines() );

	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;

	// Set the user name and password
	curl_easy_setopt( m_curl_handle, CURLOPT_USERNAME, user() );
	curl_easy_setopt( m_curl_handle, CURLOPT_PASSWORD, pass() );

	// Set up the URL for the mail server.  Default port is 25, 587 is commonly used for 
	// secure mail submission.
	twine url; url.format("smtp://%s:%d", smtpServer(), port);
	curl_easy_setopt( m_curl_handle, CURLOPT_URL, url() );

	// If they have asked for a secure connection, ensure that it's setup.
	if(useSsl){
		curl_easy_setopt( m_curl_handle, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL );
		// Ignore SSL cert issues
		curl_easy_setopt( m_curl_handle, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt( m_curl_handle, CURLOPT_SSL_VERIFYHOST, false);
	}

	curl_easy_setopt( m_curl_handle, CURLOPT_MAIL_FROM, message.From()() );

	for(size_t i = 0; i < message.TOList().size(); i++){
		recipients = curl_slist_append(recipients, message.TOList()[ i ]() );
	}
	for(size_t i = 0; i < message.CCList().size(); i++){
		recipients = curl_slist_append(recipients, message.CCList()[ i ]() );
	}
	curl_easy_setopt( m_curl_handle, CURLOPT_MAIL_RCPT, recipients );


	curl_easy_setopt( m_curl_handle, CURLOPT_READFUNCTION, SmtpClient_ReadMemoryCallback );
	curl_easy_setopt( m_curl_handle, CURLOPT_READDATA, this );
	curl_easy_setopt( m_curl_handle, CURLOPT_UPLOAD, 1L );

	// For progress information
	curl_easy_setopt( m_curl_handle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt( m_curl_handle, CURLOPT_PROGRESSFUNCTION, SmtpClient_ProgressCallback );
	curl_easy_setopt( m_curl_handle, CURLOPT_PROGRESSDATA, this );

	curl_easy_setopt( m_curl_handle, CURLOPT_VERBOSE, 0L );

	// Add in proxy information if given
	if(!m_proxy.empty()){
		DEBUG(FL, "Using proxy of (%s)", m_proxy() );
		curl_easy_setopt( m_curl_handle, CURLOPT_PROXY, m_proxy() );
	}

	PostOptions();

	res = curl_easy_perform( m_curl_handle );
	twine errmsg; 
	if(res != CURLE_OK){
		errmsg.format("Sending SMTP Message failed: %s", curl_easy_strerror(res) );
		//printf("%s", errmsg() );
	}

	// Always clean up and free lists
	curl_slist_free_all(recipients);
	curl_easy_reset( m_curl_handle );

	PostFree();

	// Now check to see if we need to throw an exception
	if(res != CURLE_OK){
		throw AnException(0, FL, errmsg() );
	}

}

void SmtpClient::SetProxy(const twine& proxyUrl )
{
	EnEx ee(FL, "SmtpClient::SetProxy(const twine& proxyUrl)");

	m_proxy = proxyUrl;
}

void SmtpClient::PostOptions()
{
	// Our implementation does nothing with this method.  Child classes can override this as necessary.
}

void SmtpClient::PostFree()
{
	// Our implementation does nothing with this method.  Child classes can override this as necessary.
}

void SmtpClient::FormatMessage(EMail& message )
{
	EnEx ee(FL, "SmtpClient::FormatMessage(EMail& message)" );

	//printf("here0\n");
	SendLines.clear();
	SendIndex = 0;

	twine tmp;

	if(message.ReplyTo().size() == 0){
		message.ReplyTo(message.From());
	}
	twine boundary;  boundary.format("SLibSmtpClient=%s", message.CreateDate().GetValue("%Y%m%d%H%M%S")() );

	tmp.format("From: %s\r\n", message.From()() ); SendLines.push_back( tmp );
	tmp.format("Subject: %s\r\n", message.Subject()() ); SendLines.push_back( tmp );
	tmp.format("Date: %s\r\n", message.CreateDate().EDate()() ); SendLines.push_back( tmp );
	tmp.format("Reply-To: %s\r\n", message.ReplyTo()() ); SendLines.push_back( tmp );

	// Add in any custom headers the user has specified
	for(size_t i = 0; i < message.HeaderList().size(); i++){
		tmp.format("%s: %s\r\n", message.HeaderList()[i].first(), message.HeaderList()[i].second() );
		SendLines.push_back( tmp );
	}

	tmp.format("MIME-Version: 1.0\r\n" ); SendLines.push_back(tmp);
	tmp.format("Content-Type: multipart/mixed; boundary=\"%s\"\r\n", boundary() ); SendLines.push_back(tmp);
	tmp = "To:";
	for(size_t i = 0; i < message.TOList().size()-1; i++){
		tmp += " " + message.TOList()[i] + ",";
	}
	tmp += " " + message.TOList()[ message.TOList().size() - 1] + "\r\n";
	SendLines.push_back( tmp );

	if(message.CCList().size() > 0){
		tmp = "Cc:";
		for(size_t i = 0; i < message.CCList().size()-1; i++){
			tmp += " " + message.CCList()[i] + ",";
		}
		tmp += " " + message.CCList()[ message.CCList().size() - 1] + "\r\n";
		SendLines.push_back( tmp );
	}

	SendLines.push_back( "\r\n" ); // Empty line to divide headers from body
	
	SendLines.push_back( "--" + boundary + "\r\n" );
	SendLines.push_back( "Content-Type: text/plain; charset=utf-8\r\n");
	SendLines.push_back( "\r\n" ); // Empty line to divide headers from body

	vector<twine> bodyLines = message.Body().split("\n");
	for(size_t i = 0; i < bodyLines.size(); i++){
		SendLines.push_back( bodyLines[i] + "\r\n" );
	}

	vector< EMailAttachment >& attachments = message.AttachmentList();
	for(size_t i = 0; i < attachments.size(); i++){
		SendLines.push_back( "\r\n--" + boundary + "\r\n" );
		tmp.format( "Content-Type: %s; charset=utf-8\r\n", attachments[i].mimeType() );
		SendLines.push_back( tmp );
		tmp.format( "Content-Disposition: attachment; filename=\"%s\"\r\n", attachments[i].fileName());
		SendLines.push_back( tmp );
		SendLines.push_back( "Content-Transfer-Encoding: base64\r\n");
		SendLines.push_back( "\r\n" ); // Empty line to divide headers from body

		// base64 encode the attachment
		attachments[i].data->encode64();

		twine b64; b64.set( attachments[i].data->data(), attachments[i].data->size() );
		vector<twine> attachLines = b64.split("\n");
		for(size_t j = 0; j < attachLines.size(); j++){
			SendLines.push_back( attachLines[j] + "\r\n" );
		}
		SendLines.push_back( "\r\n" ); // Terminate the data
	}

	SendLines.push_back( "\r\n" ); // Empty line to signal end of section
	SendLines.push_back( "--" + boundary + "--\r\n" ); // Last boundary has -- before and after boundary text

	/*
	for(size_t i = 0; i < SendLines.size(); i++){
		printf("%s", SendLines[i]() );
	}
	*/

}
