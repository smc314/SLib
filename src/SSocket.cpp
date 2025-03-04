
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
#include <string.h>

#ifndef _WIN32
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif // ! _WIN32

#ifdef AIX
#include <strings.h> // For bzero
#endif

#include "AnException.h"
#include "SSocket.h"
#include "Log.h"
using namespace SLib;

#include <errno.h>

static int ctx_usage = 0;
static SSL_CTX *ctx_common;
static char cert_file_name[256];
static int cert_file_init = 0;


#ifdef _WIN32
static int is_win32_socket_initialized = 0;

void init_winsocks();
void fini_winsocks();

void init_winsocks()
{
	WORD wVerReq;
	WSADATA wsaData;
	int err;

	wVerReq = MAKEWORD(2,0);
	err = WSAStartup(wVerReq, &wsaData);
	if(err != 0){
		printf("No usable WinSock DLL found\n");
		return;
	}

	if(LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 0)
	{
		WSACleanup();
		printf("No usable WinSock DLL found\n");
		return;
	}
}

void fini_winsocks()
{
	WSACleanup();
}
	
#endif // _WIN32


/* *********************************************************************** */
/* OpenSSL needs some functions to handle mutex locking and thread ID's.   */
/* *********************************************************************** */
void thread_setup(void);
void thread_cleanup(void);

#ifdef _WIN32

static HANDLE *lock_cs;
void win32_locking_callback(int mode, int type, const char *file, int line);

void thread_setup(void)
{
	int i;
	TRACE(FL, "Enter Win32::thread_setup");

	lock_cs=(HANDLE *)malloc(CRYPTO_num_locks() * sizeof(HANDLE));
	if(lock_cs == NULL){
		throw AnException(0, FL, "Error allocating lock_cs");
	}
	for (i=0; i<CRYPTO_num_locks(); i++) {
		lock_cs[i]=CreateMutex(NULL,FALSE,NULL);
	}

	CRYPTO_set_locking_callback( win32_locking_callback);

	TRACE(FL, "Exit  Win32::thread_setup");

	/* id callback defined already on windows. */
}

void thread_cleanup(void)
{
	int i;
	TRACE(FL, "Enter Win32::thread_cleanup");

	CRYPTO_set_locking_callback(NULL);
	for (i=0; i<CRYPTO_num_locks(); i++){
		CloseHandle(lock_cs[i]);
	}
	free(lock_cs);
	TRACE(FL, "Exit  Win32::thread_cleanup");
}

void win32_locking_callback(int mode, int type, const char *file, int line)
{
	if (mode & CRYPTO_LOCK) {
		WaitForSingleObject(lock_cs[type],INFINITE);
	} else {
		ReleaseMutex(lock_cs[type]);
	}
}

#else

static pthread_mutex_t *lock_cs;
static long *lock_count;
void pthreads_locking_callback(int mode, int type, const char *file, int line);

void thread_setup(void)
{
	int i;

	lock_cs=(pthread_mutex_t *)malloc(
		CRYPTO_num_locks() * sizeof(pthread_mutex_t));
	lock_count=(long *)malloc(CRYPTO_num_locks() * sizeof(long));

	for (i=0; i<CRYPTO_num_locks(); i++) {
		lock_count[i]=0;
		pthread_mutex_init(&(lock_cs[i]),NULL);
	}

	CRYPTO_set_id_callback((unsigned long (*)())pthread_self);
	//CRYPTO_set_locking_callback((void (*)())pthreads_locking_callback);
	CRYPTO_set_locking_callback(pthreads_locking_callback);
}

void thread_cleanup(void)
{
	int i;

	CRYPTO_set_locking_callback(NULL);
	for (i=0; i<CRYPTO_num_locks(); i++) {
		pthread_mutex_destroy(&(lock_cs[i]));
	}
	free(lock_cs);
	free(lock_count);
}

void pthreads_locking_callback(int mode, int type, const char *file, int line)
{
	if (mode & CRYPTO_LOCK) {
		pthread_mutex_lock(&(lock_cs[type]));
		lock_count[type]++;
	} else {
		pthread_mutex_unlock(&(lock_cs[type]));
	}
}

#endif // _WIN32


	


void SSocket::SetCertFilename(const char *filename)
{
	if(strlen(filename) > 255){
		throw AnException(0, FL, "Certificate filename longer than 255 chars.");
	}
	memset(cert_file_name, 0, 256);
	strcpy(cert_file_name, filename);
	cert_file_init = 1;
}


SSocket::SSocket()
{
	TRACE(FL, "Enter SSocket::SSocket()");

	/* ***************************************************** */
	/* This version of the constructor will only be called   */
	/* by another socket class.  It will handle populating   */
	/* our data for us.                                      */
	/* ***************************************************** */
	SocketType = SERVER_SOCK;

	// This is common to both client and server:
	InitOurSSL();


	TRACE(FL, "Exit SSocket::SSocket()");
}

SSocket::SSocket(int port)
{
	int err;

	TRACE(FL, "Enter SSocket::SSocket(int port)");

#ifdef _WIN32
	if(is_win32_socket_initialized == 0){
		init_winsocks();
		is_win32_socket_initialized = 1;
	}
#endif

	/* ********************************************** */
	/* First, initialize our SSL structures:          */
	/* ********************************************** */

	SocketType = SERVER_SOCK;

	// This is common to both client and server:
	InitOurSSL();

	// For the server we need to make sure our certs are in place for
	// the first socket created.  All the rest will use the same stuff.
	if (ctx_usage == 1) {
		if (SSL_CTX_use_certificate_file(ctx_common,
		                                 cert_file_name, 
						 SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(Log::FileHandle());
			throw AnException(0, FL,
			                  "Error finding the server certificate: %s",
			                  cert_file_name);
		}

		if (SSL_CTX_use_PrivateKey_file(ctx_common, 
						cert_file_name,
		                                SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(Log::FileHandle());
			throw AnException(0, FL,
			                  "Error finding the server private key: %s",
			                  cert_file_name);
		}

		if (!SSL_CTX_check_private_key(ctx_common)) {
			throw AnException(0, FL,
			                  "Private key does not match the "
			                  "certificate public key.  Cert file = (%s) "
			                  "Key file = (%s).",
			                  cert_file_name, cert_file_name);
		}
	}

	/* ********************************************** */
	/* Then do the regular initialization.            */
	/* ********************************************** */

	the_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (the_socket < 0) {
		throw AnException(0, FL, "Server Socket Creation Failed!");
	}

	SOCKADDR_IN the_addr;

#ifdef _WIN32
	the_addr.sin_family = AF_INET;
	the_addr.sin_port = htons(port);
	the_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(the_socket, (const struct sockaddr *)&the_addr,
		sizeof(SOCKADDR)) == SOCKET_ERROR )
	{
		err = errno;
#else

	the_addr = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));

	the_addr->sin_family = AF_INET;
	the_addr->sin_port = htons(port);
	the_addr->sin_addr.s_addr = INADDR_ANY;

	if (::bind(the_socket, (const struct sockaddr *)the_addr,
	         sizeof(sockaddr)) < 0) 
	{
		err = errno;
		free(the_addr);
#endif // _WIN32
		FreeSock();

		switch (err)
		{
			case EBADF:
				throw AnException(EBADF, FL,
				                  "Error binding to socket on port: %d Error = EBADF", port);
				break;
#ifndef _WIN32
			case ENOTSOCK:
				throw AnException(ENOTSOCK, FL,
				                  "Error binding to socket on port: %d Error = ENOTSOCK", port);
				break;
			case EADDRNOTAVAIL:
				throw AnException(EADDRNOTAVAIL, FL,
				                  "Error binding to socket on port: %d Error = EADDRNOTAVAIL", port);
				break;
			case EADDRINUSE:
				throw AnException(EADDRINUSE, FL,
				                  "Error binding to socket on port: %d Error = EADDRINUSE", port);
				break;
#endif
			case EINVAL:
				throw AnException(EINVAL, FL,
				                  "Error binding to socket on port: %d Error = EINVAL", port);
				break;
			case EACCES:
				throw AnException(EACCES, FL,
				                  "Error binding to socket on port: %d Error = EACCES", port);
				break;
			case EFAULT:
				throw AnException(EFAULT, FL,
				                  "Error binding to socket on port: %d Error = EFAULT", port);
				break;
			case ENODEV:
				throw AnException(ENODEV, FL,
				                  "Error binding to socket on port: %d Error = ENODEV", port);
				break;
			default:
				throw AnException(0, FL,
				                  "Error binding to socket on port: %d Error Is not known: %d", port, err);
		}
	}

#ifndef _WIN32
	free(the_addr);
#endif
	TRACE(FL, "Exit SSocket::SSocket(int port)");
}

SSocket::SSocket(int port, char *localipaddr)
{
	int err;

	TRACE(FL, "Enter SSocket::SSocket(int port)");

#ifdef _WIN32
	if(is_win32_socket_initialized == 0){
		init_winsocks();
		is_win32_socket_initialized = 1;
	}
#endif

	/* ********************************************** */
	/* First, initialize our SSL structures:          */
	/* ********************************************** */

	SocketType = SERVER_SOCK;

	// This is common to both client and server:
	InitOurSSL();

	// For the server we need to make sure our certs are in place for
	// the first socket created.  All the rest will use the same stuff.
	if (ctx_usage == 1) {
		if (SSL_CTX_use_certificate_file(ctx_common,
		                                 cert_file_name, 
						 SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(Log::FileHandle());
			throw AnException(0, FL,
			                  "Error finding the server certificate: %s",
			                  cert_file_name);
		}

		if (SSL_CTX_use_PrivateKey_file(ctx_common, 
						cert_file_name,
		                                SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(Log::FileHandle());
			throw AnException(0, FL,
			                  "Error finding the server private key: %s",
			                  cert_file_name);
		}

		if (!SSL_CTX_check_private_key(ctx_common)) {
			throw AnException(0, FL,
			                  "Private key does not match the "
			                  "certificate public key.  Cert file = (%s) "
			                  "Key file = (%s).",
			                  cert_file_name, cert_file_name);
		}
	}

	/* ********************************************** */
	/* Then do the regular initialization.            */
	/* ********************************************** */

	the_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (the_socket < 0) {
		throw AnException(0, FL, "Server Socket Creation Failed!");
	}

	SOCKADDR_IN the_addr;

	/* ********************************************** */
	/* Need this to convert from dotted notation to   */
	/* network byte order							  */
	/* ********************************************** */
	unsigned long proper_localip_inaddr=INADDR_NONE;


#ifdef _WIN32
	the_addr.sin_family = AF_INET;
	the_addr.sin_port = htons(port);
//	the_addr.sin_addr.s_addr = INADDR_ANY;
	/* ********************************************** */
	/* If a spcecific local IP address is provided,   */
	/* convert from dotted notation to network byte	  */
	/* order, then use as IN_ADDR for the bind.		  */
	/* ********************************************** */
	if(localipaddr!=NULL)
	{
		proper_localip_inaddr=inet_addr(localipaddr);
		if(proper_localip_inaddr==INADDR_NONE)
		{
			FreeSock();
			throw AnException(INADDR_NONE, FL, "Server inet_addr Failed!  Parameter localipaddr = %s",localipaddr);
		}
		the_addr.sin_addr.s_addr=proper_localip_inaddr;

	}
	else the_addr.sin_addr.s_addr = INADDR_ANY;


	if (bind(the_socket, (const struct sockaddr *)&the_addr,
		sizeof(SOCKADDR)) == SOCKET_ERROR )
	{
		err = errno;
#else

	the_addr = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));

	the_addr->sin_family = AF_INET;
	the_addr->sin_port = htons(port);
//	the_addr->sin_addr.s_addr = INADDR_ANY;
	/* ********************************************** */
	/* If a spcecific local IP address is provided,   */
	/* convert from dotted notation to network byte	  */
	/* order, then use as IN_ADDR for the bind.		  */
	/* ********************************************** */
	if(localipaddr!=NULL)
	{
		proper_localip_inaddr=inet_addr(localipaddr);
		if(proper_localip_inaddr==INADDR_NONE)
		{
			FreeSock();
			free(the_addr);
			throw AnException(INADDR_NONE, FL, "Server inet_addr Failed!  Parameter localipaddr = %s",localipaddr);
		}
		the_addr->sin_addr.s_addr=proper_localip_inaddr;

	}
	else the_addr->sin_addr.s_addr = INADDR_ANY;


	if (::bind(the_socket, (const struct sockaddr *)the_addr,
	         sizeof(sockaddr)) < 0) 
	{
		err = errno;
		free(the_addr);
#endif // _WIN32
		FreeSock();

		switch (err)
		{
			case EBADF:
				throw AnException(EBADF, FL,
				                  "Error binding to socket on port: %d Error = EBADF", port);
				break;
#ifndef _WIN32
			case ENOTSOCK:
				throw AnException(ENOTSOCK, FL,
				                  "Error binding to socket on port: %d Error = ENOTSOCK", port);
				break;
			case EADDRNOTAVAIL:
				throw AnException(EADDRNOTAVAIL, FL,
				                  "Error binding to socket on port: %d Error = EADDRNOTAVAIL", port);
				break;
			case EADDRINUSE:
				throw AnException(EADDRINUSE, FL,
				                  "Error binding to socket on port: %d Error = EADDRINUSE", port);
				break;
#endif
			case EINVAL:
				throw AnException(EINVAL, FL,
				                  "Error binding to socket on port: %d Error = EINVAL", port);
				break;
			case EACCES:
				throw AnException(EACCES, FL,
				                  "Error binding to socket on port: %d Error = EACCES", port);
				break;
			case EFAULT:
				throw AnException(EFAULT, FL,
				                  "Error binding to socket on port: %d Error = EFAULT", port);
				break;
			case ENODEV:
				throw AnException(ENODEV, FL,
				                  "Error binding to socket on port: %d Error = ENODEV", port);
				break;
			default:
				throw AnException(0, FL,
				                  "Error binding to socket on port: %d Error Is not known: %d", port, err);
		}
	}

#ifndef _WIN32
	free(the_addr);
#endif
	TRACE(FL, "Exit SSocket::SSocket(int port)");
}

SSocket::SSocket(char *machine, int port)
{
	X509* the_cert;
	int err;
	char *str;
	struct hostent * that_host;

	TRACE(FL, "Enter SSocket::SSocket(char *machine, int port)");
#ifdef _WIN32
	if(is_win32_socket_initialized == 0){
		init_winsocks();
		is_win32_socket_initialized = 1;
	}
#endif

	SocketType = CLIENT_SOCK;

	/* ********************************************** */
	/* First, initialize our SSL structures:          */
	/* ********************************************** */

	InitOurSSL();

	/* ********************************************** */
	/* Then do the regular initialization.            */
	/* ********************************************** */


	the_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (the_socket < 0 ) {
		throw AnException(0, FL, "Client Socket Creation failed!");
	}

	/* ********************************************** */
	/* Get the local name and address of this machine */
	/* ********************************************** */
	that_host = gethostbyname(machine);
	if (that_host == NULL) {
		FreeSock();
		throw AnException(0, FL,
		                  "Error getting host information for %s", 
				  machine);
	}

	SOCKADDR_IN the_addr;

	/* ********************************************** */
	/* Now open the connection to that machine.       */
	/* ********************************************** */
#ifdef _WIN32
	the_addr.sin_family = AF_INET;
	the_addr.sin_port = htons(port);
	the_addr.sin_addr = *((IN_ADDR *)that_host->h_addr);
	if(connect(the_socket, (struct sockaddr *)&the_addr,
		sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
#else
	the_addr = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));

	the_addr->sin_family = AF_INET;
	the_addr->sin_port = htons(port);
	the_addr->sin_addr = *((in_addr *)that_host->h_addr);

	if (connect(the_socket, (struct sockaddr *)the_addr,
	            sizeof(sockaddr)) < 0 ) {
		free(the_addr);
#endif
		FreeSock();
		throw AnException(0, FL,
		                  "Error connecting to %s on port %d", 
		                  machine, port);
	}
#ifndef _WIN32
	free(the_addr);
#endif


	/* ********************************************** */
	/* Finally, handle the client SSL initialization: */
	/* ********************************************** */

	m_ssl = SSL_new(ctx_common);

	if (m_ssl == NULL){
		FreeSock();
		throw AnException(0, FL, "SSL structure is null after init.");
	}

	SSL_set_fd(m_ssl, the_socket);
	err = SSL_connect(m_ssl);
	if (err < 0) {
		ERR_print_errors_fp(Log::FileHandle());
		FreeSock();
		throw AnException(0, FL, "Error createing new SSL socket "
		                  "connection to machine (%s) on port (%d).  "
		                  "See above SSL error dump.", machine, port);
	}

	INFO(FL, "SSL connection using %s", SSL_get_cipher (m_ssl));

	// Get client's certificate (note: beware of dynamic allocation) - opt

#if OPENSSL_VERSION_NUMBER >= 0x3000000L
	the_cert = SSL_get1_peer_certificate (m_ssl);
#else
	the_cert = SSL_get_peer_certificate (m_ssl);
#endif
	if (the_cert != NULL) {
		str = X509_NAME_oneline ( X509_get_subject_name(the_cert),
		                          0, 0);
		if (str != NULL) {
			INFO(FL, "Server Cert: subject: %s", str);
			free (str);
		}
		str = X509_NAME_oneline (X509_get_issuer_name(the_cert),
		                         0, 0);
		if (str != NULL) {
			INFO(FL, "Server Cert: issuer: %s", str);
			free (str);
		}

		// We could do all sorts of certificate verification
		// stuff here before deallocating the certificate.

		X509_free(the_cert);
	} else {
		INFO(FL, "Server does not have certificate.");
		ERR_print_errors_fp(Log::FileHandle());
		FreeSock();
		throw AnException(0, FL,
		                  "Trying to contact server without a valid "
		                  "certificate. Machine = (%s) Port = (%d)",
		                  machine, port);
	}


	TRACE(FL, "Exit SSocket::SSocket(char *machine, int port)");
}

SSocket::SSocket(char *machine, int port, char *localipaddr)
{
	X509* the_cert;
	int err;
	char *str;
	struct hostent * that_host;

	TRACE(FL, "Enter SSocket::SSocket(char *machine, int port)");
#ifdef _WIN32
	if(is_win32_socket_initialized == 0){
		init_winsocks();
		is_win32_socket_initialized = 1;
	}
#endif

	SocketType = CLIENT_SOCK;

	/* ********************************************** */
	/* First, initialize our SSL structures:          */
	/* ********************************************** */

	InitOurSSL();

	/* ********************************************** */
	/* Then do the regular initialization.            */
	/* ********************************************** */


	the_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (the_socket < 0 ) {
		throw AnException(0, FL, "Client Socket Creation failed!");
	}

	SOCKADDR_IN this_addr;

	
	/* ********************************************** */
	/* Need this to convert from dotted notation to   */
	/* network byte order							  */
	/* ********************************************** */
	unsigned long proper_localip_inaddr=INADDR_NONE;
	

	
#ifdef _WIN32
	this_addr.sin_family = AF_INET;
	this_addr.sin_port = htons(0);
	
	/* ********************************************** */
	/* If a spcecific local IP address is provided,   */
	/* convert from dotted notation to network byte	  */
	/* order, then use as IN_ADDR for the bind.		  */
	/* ********************************************** */
	if(localipaddr!=NULL)
	{
		proper_localip_inaddr=inet_addr(localipaddr);
		if(proper_localip_inaddr==INADDR_NONE)
		{
			FreeSock();
			throw AnException(INADDR_NONE, FL, "Client inet_addr Failed!  Parameter localipaddr = %s",localipaddr);
		}
		this_addr.sin_addr.s_addr=proper_localip_inaddr;

	}
	else this_addr.sin_addr.s_addr = INADDR_ANY;



	if (bind(the_socket, (const struct sockaddr *)&this_addr,
	         sizeof(SOCKADDR)) == SOCKET_ERROR ) 
	{
		err = errno;

#else
	this_addr = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));
	this_addr->sin_family = AF_INET;
	this_addr->sin_port = htons(0);
	/* ********************************************** */
	/* If a spcecific local IP address is provided,   */
	/* convert from dotted notation to network byte	  */
	/* order, then use as IN_ADDR for the bind.		  */
	/* ********************************************** */
	if(localipaddr!=NULL)
	{
		proper_localip_inaddr=inet_addr(localipaddr);
		if(proper_localip_inaddr==INADDR_NONE)
		{
			FreeSock();
			free(this_addr);
			throw AnException(INADDR_NONE, FL, "Client inet_addr Failed!  Parameter localipaddr = %s",localipaddr);
		}
		this_addr->sin_addr.s_addr=proper_localip_inaddr;

	}
	else this_addr->sin_addr.s_addr = INADDR_ANY;


	if (::bind(the_socket, (const struct sockaddr *)this_addr,
	         sizeof(sockaddr)) < 0) 
	{
		err = errno;
		free(this_addr);
#endif // _WIN32
		FreeSock();

		switch (err)
		{
			case EBADF:
				throw AnException(EBADF, FL,
				                  "Error binding to socket on port: %d Error = EBADF", port);
				break;
#ifndef _WIN32
			case ENOTSOCK:
				throw AnException(ENOTSOCK, FL,
				                  "Error binding to socket on port: %d Error = ENOTSOCK", port);
				break;
			case EADDRNOTAVAIL:
				throw AnException(EADDRNOTAVAIL, FL,
				                  "Error binding to socket on port: %d Error = EADDRNOTAVAIL", port);
				break;
			case EADDRINUSE:
				throw AnException(EADDRINUSE, FL,
				                  "Error binding to socket on port: %d Error = EADDRINUSE", port);
				break;
#endif
			case EINVAL:
				throw AnException(EINVAL, FL,
				                  "Error binding to socket on port: %d Error = EINVAL", port);
				break;
			case EACCES:
				throw AnException(EACCES, FL,
				                  "Error binding to socket on port: %d Error = EACCES", port);
				break;
			case EFAULT:
				throw AnException(EFAULT, FL,
				                  "Error binding to socket on port: %d Error = EFAULT", port);
				break;
			case ENODEV:
				throw AnException(ENODEV, FL,
				                  "Error binding to socket on port: %d Error = ENODEV", port);
				break;
			default:
				throw AnException(0, FL,
				                  "Error binding to socket on port: %d Error Is not known: %d", port, err);
		}
	}

	/* ********************************************** */
	/* Get the local name and address of this machine */
	/* ********************************************** */
	that_host = gethostbyname(machine);
	if (that_host == NULL) {
		FreeSock();
		throw AnException(0, FL,
		                  "Error getting host information for %s", 
				  machine);
	}

	SOCKADDR_IN the_addr;

	/* ********************************************** */
	/* Now open the connection to that machine.       */
	/* ********************************************** */
#ifdef _WIN32
	the_addr.sin_family = AF_INET;
	the_addr.sin_port = htons(port);
	the_addr.sin_addr = *((IN_ADDR *)that_host->h_addr);
	if(connect(the_socket, (struct sockaddr *)&the_addr,
		sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
#else
	the_addr = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));

	the_addr->sin_family = AF_INET;
	the_addr->sin_port = htons(port);
	the_addr->sin_addr = *((in_addr *)that_host->h_addr);

	if (connect(the_socket, (struct sockaddr *)the_addr,
	            sizeof(sockaddr)) < 0 ) {
		free(the_addr);
#endif
		FreeSock();
		throw AnException(0, FL,
		                  "Error connecting to %s on port %d", 
		                  machine, port);
	}
#ifndef _WIN32
	free(the_addr);
#endif


	/* ********************************************** */
	/* Finally, handle the client SSL initialization: */
	/* ********************************************** */

	m_ssl = SSL_new(ctx_common);

	if (m_ssl == NULL){
		FreeSock();
		throw AnException(0, FL, "SSL structure is null after init.");
	}

	SSL_set_fd(m_ssl, the_socket);
	err = SSL_connect(m_ssl);
	if (err < 0) {
		ERR_print_errors_fp(Log::FileHandle());
		FreeSock();
		throw AnException(0, FL, "Error createing new SSL socket "
		                  "connection to machine (%s) on port (%d).  "
		                  "See above SSL error dump.", machine, port);
	}

	INFO(FL, "SSL connection using %s", SSL_get_cipher (m_ssl));

	// Get client's certificate (note: beware of dynamic allocation) - opt

	the_cert = SSL_get_peer_certificate (m_ssl);
	if (the_cert != NULL) {
		str = X509_NAME_oneline ( X509_get_subject_name(the_cert),
		                          0, 0);
		if (str != NULL) {
			INFO(FL, "Server Cert: subject: %s", str);
			free (str);
		}
		str = X509_NAME_oneline (X509_get_issuer_name(the_cert),
		                         0, 0);
		if (str != NULL) {
			INFO(FL, "Server Cert: issuer: %s", str);
			free (str);
		}

		// We could do all sorts of certificate verification
		// stuff here before deallocating the certificate.

		X509_free(the_cert);
	} else {
		INFO(FL, "Server does not have certificate.");
		ERR_print_errors_fp(Log::FileHandle());
		FreeSock();
		throw AnException(0, FL,
		                  "Trying to contact server without a valid "
		                  "certificate. Machine = (%s) Port = (%d)",
		                  machine, port);
	}


	TRACE(FL, "Exit SSocket::SSocket(char *machine, int port)");
}

SSocket::~SSocket()
{
	TRACE(FL, "Enter SSocket::~SSocket()");

	DEBUG(FL, "Freeing the SSL stuff.");
	FreeOurSSL();

	FreeSock();

	TRACE(FL, "Exit SSocket::~SSocket()");
}

void SSocket::FreeSock(void)
{
	DEBUG(FL, "Closing the socket fd.");
#ifdef _WIN32
	closesocket(the_socket);
#else
	close(the_socket);
#endif
}

int SSocket::GetRawData(char *buffer, int max)
{
	int cnt;

	TRACE(FL, "Enter SSocket::GetRawData(char *buffer, int max)");

	cnt = SSL_read (m_ssl, buffer, max);
	if (cnt < 0) {
		ERR_print_errors_fp(Log::FileHandle());
		throw AnException(0, FL, "Error receiving data on the socket.  See above SSL error dump.");
	}

	TRACE(FL, "Exit SSocket::GetRawData(char *buffer, int max)");
	return cnt;
}

int SSocket::TimedGetRawData(char *buffer, int max, int mills)
{
	// first check for data:
	if (IsDataThere(mills)) {
		return GetRawData(buffer, max);
	} else {
		throw ReadTimeout(FL);
	}
}

int SSocket::SendData(twine* buffer)
{
	return SendData((char *)buffer->c_str(), buffer->size());
}

int SSocket::SendData(twine& buffer)
{
	return SendData((char *)buffer.c_str(), buffer.size());
}

int SSocket::SendData(char *buffer, int size)
{
	int cnt, err;

	TRACE(FL, "Enter SSocket::SendData(char *buffer, int size)");
	TRACE(FL, "Size is: %d", size);
	
	cnt = 0;
	do {
		err = SSL_write(m_ssl, buffer + cnt, size - cnt);
		if (err < 0) {
			ERR_print_errors_fp(Log::FileHandle());
			throw AnException(0, FL,
			                  "Error sending data on the socket.  See above SSL error dump.");
		}
		cnt += err;
	} while (cnt < size);

	TRACE(FL, "Exit SSocket::SendData(char *buffer, int size)");
	return size;
}

GSocket *SSocket::Listen(void)
{
	int err;
	char *str;
	SOCKET new_sock;
	SOCKADDR_IN new_addr;
	dptr<SSocket> tmp; // don't leak memory
	// On a SUN system, this has to be int:
	//      int addr_size;
	// On a Linux system, this has to be socklen_t
#ifdef _WIN32
	//int addr_size;
#else
	socklen_t addr_size;
#endif

	X509* the_cert;

	TRACE(FL, "Enter SSocket::Listen()");

	if (SocketType != SERVER_SOCK) {
		throw AnException(0, FL, "Listen: Not a server socket");
	}

	DEBUG(FL, "Socket calling listen on (%d)", the_socket);
	err = listen(the_socket, 5);
	if (err < 0) {
		throw AnException(0, FL, "Error listening on socket");
	}

	DEBUG(FL, "Socket calling accept on socket\n");

#ifdef _WIN32
	new_sock = accept(the_socket,
	                  (struct sockaddr *) & new_addr, NULL);
#else
	addr_size = sizeof(new_addr);
	new_sock = accept(the_socket,
	                  (struct sockaddr *) & new_addr, &addr_size);
#endif

	if (new_sock < 0) {
		throw AnException(0, FL,
		                  "Error during accept of connection in Listen");
	}

	DEBUG(FL, "Socket instantiating new Socket\n");

	tmp = new SSocket();

	tmp->the_socket = new_sock;
	tmp->SocketType = SERVER_SOCK;

	/* ******************************************************** */
	/* Then do the SSL specific stuff.                          */
	/* ******************************************************** */

	tmp->m_ssl = SSL_new(ctx_common);
	if (tmp->m_ssl == NULL)
		throw AnException(0, FL, "SSL structure is null after init.");

	DEBUG(FL, "Setting up the m_ssl with fd: (%d)", tmp->the_socket);
	SSL_set_fd(tmp->m_ssl, tmp->the_socket);
	err = SSL_accept(tmp->m_ssl);
	if (err < 0) {
		ERR_print_errors_fp(Log::FileHandle());
		delete tmp;
		throw AnException(0, FL, "Error accepting new socket connection.  See above SSL error dump.");
	}

	INFO(FL, "SSL connection using %s", SSL_get_cipher (tmp->m_ssl));

	// Get client's certificate (note: beware of dynamic allocation) - opt

	the_cert = SSL_get_peer_certificate (tmp->m_ssl);
	if (the_cert != NULL) {
		str = X509_NAME_oneline ( X509_get_subject_name(the_cert),
		                          0, 0);
		if (str != NULL) {
			INFO(FL, "Client Cert: subject: %s", str);
			free (str);
		}
		str = X509_NAME_oneline (X509_get_issuer_name(the_cert),
		                         0, 0);
		if (str != NULL) {
			INFO(FL, "Client Cert: issuer: %s", str);
			free (str);
		}

		// We could do all sorts of certificate verification
		// stuff here before deallocating the certificate.

		X509_free(the_cert);
	} else {
		INFO(FL, "Client does not have certificate.");
	}

	TRACE(FL, "Exit SSocket::Listen()");
	return tmp.release();
}

GSocket *SSocket::Listen(int timeout)
{
	int err;
	char *str;
	SOCKET new_sock;
	SOCKADDR_IN new_addr;
	dptr<SSocket> tmp; // don't leak memory
	// On a SUN system, this has to be int:
	//      int addr_size;
	// On a Linux system, this has to be socklen_t
#ifdef _WIN32
	//int addr_size;
#else
	socklen_t addr_size;
#endif

	X509* the_cert;

	TRACE(FL, "Enter SSocket::Listen()");

	if (SocketType != SERVER_SOCK) {
		throw AnException(0, FL, "Listen: Not a server socket");
	}

	DEBUG(FL, "Socket calling listen on (%d)", the_socket);
	err = listen(the_socket, 5);
	if (err < 0) {
		throw AnException(0, FL, "Error listening on socket");
	}


	if( IsDataThere( timeout ) <= 0 ){
		// No inbound sockets in the given timeout.
		return NULL;
	}

	DEBUG(FL, "Socket calling accept on socket\n");

#ifdef _WIN32
	new_sock = accept(the_socket,
	                  (struct sockaddr *) & new_addr, NULL);
#else
	addr_size = sizeof(new_addr);
	new_sock = accept(the_socket,
	                  (struct sockaddr *) & new_addr, &addr_size);
#endif

	if (new_sock < 0) {
		throw AnException(0, FL,
		                  "Error during accept of connection in Listen");
	}

	DEBUG(FL, "Socket instantiating new Socket\n");

	tmp = new SSocket();

	tmp->the_socket = new_sock;
	tmp->SocketType = SERVER_SOCK;

	/* ******************************************************** */
	/* Then do the SSL specific stuff.                          */
	/* ******************************************************** */

	tmp->m_ssl = SSL_new(ctx_common);
	if (tmp->m_ssl == NULL)
		throw AnException(0, FL, "SSL structure is null after init.");

	DEBUG(FL, "Setting up the m_ssl with fd: (%d)", tmp->the_socket);
	SSL_set_fd(tmp->m_ssl, tmp->the_socket);
	err = SSL_accept(tmp->m_ssl);
	if (err < 0) {
		ERR_print_errors_fp(Log::FileHandle());
		delete tmp;
		throw AnException(0, FL, "Error accepting new socket connection.  See above SSL error dump.");
	}

	INFO(FL, "SSL connection using %s", SSL_get_cipher (tmp->m_ssl));

	// Get client's certificate (note: beware of dynamic allocation) - opt

	the_cert = SSL_get_peer_certificate (tmp->m_ssl);
	if (the_cert != NULL) {
		str = X509_NAME_oneline ( X509_get_subject_name(the_cert),
		                          0, 0);
		if (str != NULL) {
			INFO(FL, "Client Cert: subject: %s", str);
			free (str);
		}
		str = X509_NAME_oneline (X509_get_issuer_name(the_cert),
		                         0, 0);
		if (str != NULL) {
			INFO(FL, "Client Cert: issuer: %s", str);
			free (str);
		}

		// We could do all sorts of certificate verification
		// stuff here before deallocating the certificate.

		X509_free(the_cert);
	} else {
		INFO(FL, "Client does not have certificate.");
	}

	TRACE(FL, "Exit SSocket::Listen()");
	return tmp.release();
}

void SSocket::InitOurSSL(void)
{
	TRACE(FL, "Enter SSocket::InitOurSSL(void)");

	m_meth = NULL;
	m_ssl = NULL;


	if (ctx_usage == 0) {
		/* ***************************************************** */
		/* Certain stuff needs to be globally shared by all ssl  */
		/* sockets.  Init that stuff here.                       */
		/* ***************************************************** */

		thread_setup();

		SSL_load_error_strings();
		SSLeay_add_ssl_algorithms();

		// setup a default certificate filename.
		if(cert_file_init == 0){
			memset(cert_file_name, 0, 256);
			sprintf(cert_file_name, "./foo-cert.pem");
		}

		if (SocketType == CLIENT_SOCK) {
			m_meth = (SSL_METHOD*)SSLv23_client_method();
		} else {
			m_meth = (SSL_METHOD*)SSLv23_server_method();
		}
		if (m_meth == NULL) {
			ERR_print_errors_fp(Log::FileHandle());
			throw AnException(0, FL,
			                  "SSL Method is null in initialization.  See errors above.");
		}
		ctx_common = SSL_CTX_new (m_meth);
		if (!ctx_common) {
			ERR_print_errors_fp(Log::FileHandle());
			throw AnException(0, FL,
			                  "Error initializing the SSL ctx. "
			                  "See above SSL error dump. ");
		}
	}

	ctx_usage ++;

	TRACE(FL, "Exit SSocket::InitOurSSL(void)");
}

void SSocket::FreeOurSSL(void)
{
	TRACE(FL, "Enter SSocket::FreeOurSSL(void)");

	if (m_ssl != NULL) {
		DEBUG(FL, "Doing the SSL_shutdown.");
		SSL_shutdown(m_ssl);
		DEBUG(FL, "Doing the SSL_free.");
		SSL_free(m_ssl);
	}
	ctx_usage --;
	if (ctx_usage == 0) {
		DEBUG(FL, "Doing the SSL_CTX_free.");
		SSL_CTX_free(ctx_common);
		thread_cleanup();
	}

	TRACE(FL, "Exit SSocket::FreeOurSSL(void)");
}

int SSocket::IsDataThere(void)
{
	/* **************************************** */
	/* Wait for a very short amount of time .   */
	/* **************************************** */
	return IsDataThere(1);
}

int SSocket::IsDataThere(int mills)
{
	fd_set rfds;
	struct timeval tv;
	size_t n, ret;

	try {
		FD_ZERO(&rfds);
		FD_SET(the_socket, &rfds);
		n = the_socket + 1;

		tv.tv_sec = mills / 1000;            // 1000 mills = 1 Second
		tv.tv_usec = (mills % 1000) * 1000;  // 1000 usec = 1 mills

		ret = select(n, &rfds, NULL, NULL, &tv);
	} catch (...) {
		printf("Caught exception trying to check data on socket\n");
		throw;
		return -1;
	}
	return ret;
}

void SSocket::SetKeepalive( bool tf )
{
	int optkeepalive = tf ? 1 : 0;
	int rc = setsockopt( the_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&optkeepalive, sizeof(optkeepalive) );
	if(rc != 0){
#ifdef _WIN32
		int err = WSAGetLastError();
#else
		int err = errno;
#endif
		throw AnException(0, FL, "Error setting SO_KEEPALIVE on socket: %d", err);
	}
}

void SSocket::SetNoDelay( bool tf )
{
	int optnodelay = tf ? 1 : 0;
	int rc = setsockopt( the_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optnodelay, sizeof(optnodelay) );
	if(rc != 0){
#ifdef _WIN32
		int err = WSAGetLastError();
#else
		int err = errno;
#endif
		throw AnException(0, FL, "Error setting TCP_NODELAY on socket: %d", err);
	}
}

unsigned short SSocket::GetLocalPort(void)
{
	SOCKADDR_IN localad;
	int addrlen, rc, err;
	unsigned short localPort;

	addrlen = sizeof(sockaddr);

#ifdef _WIN32
	localad.sin_family = AF_INET;
	rc = getsockname( the_socket, (struct sockaddr*)&localad, &addrlen );
#else
	localad = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));
	rc = getsockname( the_socket, (struct sockaddr*)localad, (socklen_t*)&addrlen );
#endif
	if(rc != 0){
#ifdef _WIN32
		err = WSAGetLastError();
		throw AnException(0, FL, "Error getting local port with getsockname socket: %d", err);
#else
		err = errno;
		free( localad ); // free this before throwing the exception
		throw AnException(0, FL, "Error getting local port with getsockname socket: %d", err);
#endif
	}
		
#ifdef _WIN32
	localPort = ntohs( localad.sin_port );
#else
	localPort = ntohs( localad->sin_port );
	free( localad );
#endif

	return localPort;
}

int SSocket::GetDataToRead( int* bytesAvailable )
{
	int rc;
#ifdef _WIN32
	u_long tmp;
	rc = ioctlsocket( the_socket, FIONREAD, &tmp );
	*bytesAvailable = (int)tmp;
#else
	rc = ioctl( the_socket, FIONREAD, bytesAvailable );
#endif

	return rc;
}
