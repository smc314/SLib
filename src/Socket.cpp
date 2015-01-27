
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

#ifndef _WIN32

// Stuff necessary on unix
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#endif  // ! _WIN32

#ifdef AIX
#include <strings.h> // For bzero
#endif

#include <errno.h>

#include "AnException.h"
#include "sptr.h"
#include "Lock.h"
#include "Socket.h"
using namespace SLib;

#ifdef _WIN32
static int is_win32_socket_initialized = 0;

void init_winsock();
void fini_winsock();

void init_winsock()
{
	WSADATA wsaData;
	int err;

	err = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(err != 0){
		printf("No usable WinSock DLL found\n");
		return;
	}
}

void fini_winsock()
{
	WSACleanup();
}
	
#endif // _WIN32
	
#ifndef BOOL
#define BOOL bool
#endif




Socket::Socket()
{
	/* ***************************************************** */
	/* This version of the constructor will only be called   */
	/* by another socket class.  It will handle populating   */
	/* our data for us.                                      */
	/* ***************************************************** */
}

/* ********************************************************* */
/* This constructor will generate a server style socket      */
/* on the local machine that will bind itself to the given   */
/* port and return.  After instantiating one of these, call  */
/* the Listen method to wait for a connection.               */
/* ********************************************************* */
Socket::Socket(int port)
{
	int err;


#ifdef _WIN32
	if(is_win32_socket_initialized == 0){
		init_winsock();
		is_win32_socket_initialized = 1;
	}
#endif

	the_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (the_socket < 0) {
		throw AnException(0, FL, "Server Socket Creation Failed!");
	}

#ifdef _WIN32
	BOOL reuseaddr = true;
	if( 0 != setsockopt( the_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseaddr, sizeof(reuseaddr) ) ){
		throw AnException(0, FL, "Error setting SO_REUSEADDR on socket.");
	}
#endif

	SetNoDelay( true );

	SOCKADDR_IN this_addr;

#ifdef _WIN32
	memset(&this_addr, 0, sizeof(SOCKADDR) );
	this_addr.sin_family = AF_INET;
	this_addr.sin_port = htons(port);
	this_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(the_socket, (const struct sockaddr *)&this_addr, sizeof(SOCKADDR)) == SOCKET_ERROR ) 
	{
		err = WSAGetLastError();

#else
	this_addr = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));
	this_addr->sin_family = AF_INET;
	this_addr->sin_port = htons(port);
	this_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(the_socket, (const struct sockaddr *)this_addr, sizeof(sockaddr)) < 0) 
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
#ifdef _WIN32
			case WSANOTINITIALISED:
				throw AnException(WSANOTINITIALISED, FL,
				                  "Error binding to socket on port: %d Error = WSANOTINITIALISED", port);
				break;
			case WSAENETDOWN:
				throw AnException(WSAENETDOWN, FL,
				                  "Error binding to socket on port: %d Error = WSAENETDOWN", port);
				break;
			case WSAEACCES:
				throw AnException(WSAEACCES, FL,
				                  "Error binding to socket on port: %d Error = WSAEACCES", port);
				break;
			case WSAEADDRINUSE:
				throw AnException(WSAEADDRINUSE, FL,
				                  "Error binding to socket on port: %d Error = WSAEADDRINUSE", port);
				break;
			case WSAEADDRNOTAVAIL:
				throw AnException(WSAEADDRNOTAVAIL, FL,
				                  "Error binding to socket on port: %d Error = WSAEADDRNOTAVAIL", port);
				break;
			case WSAEFAULT:
				throw AnException(WSAEFAULT, FL,
				                  "Error binding to socket on port: %d Error = WSAEFAULT", port);
				break;
#else
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
	free(this_addr);
#endif

	SocketType = SERVER_SOCK;

}

/* ********************************************************* */
/* This constructor will generate a server style socket      */
/* on the local machine that will bind itself to the given   */
/* port and return, with the option to specify using UDP or	 */
/* TCP and a local IP address to bind to on multi-homed		 */
/* systems.  After instantiating one of these, call			 */
/* the Listen method to wait for a connection.			     */
/* ********************************************************* */
Socket::Socket(int port, bool useUDP, char *localipaddr)
{
	int err;


#ifdef _WIN32
	if(is_win32_socket_initialized == 0){
		init_winsock();
		is_win32_socket_initialized = 1;
	}
#endif


	/* ********************************************** */
	/* Use UDP of useUDP paramerter is TRUE, else     */
	/* use TCP.		     							  */
	/* ********************************************** */
	if(!useUDP) the_socket = socket(AF_INET, SOCK_STREAM, 6);
	else the_socket = socket(AF_INET, SOCK_DGRAM, 17);
	if (the_socket < 0) {
		throw AnException(0, FL, "Server Socket Creation Failed!");
	}

	SOCKADDR_IN this_addr;

	
	/* ********************************************** */
	/* Need this to convert from dotted notation to   */
	/* network byte order							  */
	/* ********************************************** */
	unsigned long proper_localip_inaddr=INADDR_NONE;
	

	
#ifdef _WIN32
	this_addr.sin_family = AF_INET;
	this_addr.sin_port = htons(port);
	
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
	this_addr->sin_port = htons(port);
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
			throw AnException(INADDR_NONE, FL, "Server inet_addr Failed!  Parameter localipaddr = %s",localipaddr);
		}
		this_addr->sin_addr.s_addr=proper_localip_inaddr;

	}
	else this_addr->sin_addr.s_addr = INADDR_ANY;


	if (bind(the_socket, (const struct sockaddr *)this_addr,
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

#ifndef _WIN32
	free(this_addr);
#endif

	SocketType = SERVER_SOCK;

}

/* ********************************************************* */
/* This constructor will generate a client style socket      */
/* originating on the machine passed as a parameter on the   */
/* port passed as a parameter.                               */
/* ********************************************************* */
Socket::Socket(char *machine, int port)
{
	struct hostent * that_host;


#ifdef _WIN32
	if(is_win32_socket_initialized == 0){
		init_winsock();
		is_win32_socket_initialized = 1;
	}
#endif

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
		                  "Error getting host information for %s", machine);
	}

	/* ********************************************** */
	/* Now open the connection to that machine.       */
	/* ********************************************** */
	SOCKADDR_IN that_addr;

#ifdef _WIN32
	that_addr.sin_family = AF_INET;
	that_addr.sin_port = htons(port);
	that_addr.sin_addr = *((IN_ADDR *)that_host->h_addr);

	if (connect(the_socket, (struct sockaddr *)&that_addr,
	            sizeof(SOCKADDR)) == SOCKET_ERROR ) 
	{
#else
	that_addr = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));

	that_addr->sin_family = AF_INET;
	that_addr->sin_port = htons(port);
	that_addr->sin_addr = *((in_addr *)that_host->h_addr);

	if (connect(the_socket, (struct sockaddr *)that_addr,
	            sizeof(sockaddr)) < 0 ) 
	{
		free(that_addr);
#endif
		FreeSock();
		throw AnException(0, FL,
		                  "Error connecting to %s on port %d", machine, port);
	}

#ifndef _WIN32
	free(that_addr);
#endif

	SocketType = CLIENT_SOCK;
}

/* ********************************************************* */
/* This constructor will generate a client style socket      */
/* originating on the machine passed as a parameter on the   */
/* port passed as a parameter, with the option to specify 	 */
/* using UDP or TCP and a local IP address to bind to 		 */
/* on multi-homed systems.									 */
/* ********************************************************* */
Socket::Socket(char *machine, int port, bool useUDP, char *localipaddr)
{
	struct hostent * that_host;
	int err;

	DEBUG(FL,"Client socket on machine %s, port %d, UDP %d, local ip %s",
		machine, port, useUDP, localipaddr);

#ifdef _WIN32
	if(is_win32_socket_initialized == 0){
		init_winsock();
		is_win32_socket_initialized = 1;
	}
#endif

	/* ********************************************** */
	/* Use UDP of useUDP paramerter is TRUE, else     */
	/* use TCP.		     							  */
	/* ********************************************** */
	if(!useUDP) the_socket = socket(AF_INET, SOCK_STREAM, 0);
	else the_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (the_socket < 0) {
		throw AnException(0, FL, "Client Socket Creation Failed!");
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


	if (bind(the_socket, (const struct sockaddr *)this_addr,
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
		                  "Error getting host information for %s", machine);
	}

	/* ********************************************** */
	/* Now open the connection to that machine.       */
	/* ********************************************** */
	SOCKADDR_IN that_addr;

#ifdef _WIN32
	that_addr.sin_family = AF_INET;
	that_addr.sin_port = htons(port);
	that_addr.sin_addr = *((IN_ADDR *)that_host->h_addr);

	if (connect(the_socket, (struct sockaddr *)&that_addr,
	            sizeof(SOCKADDR)) == SOCKET_ERROR ) 
	{
#else
	that_addr = (SOCKADDR_IN)malloc(sizeof(struct sockaddr_in));

	that_addr->sin_family = AF_INET;
	that_addr->sin_port = htons(port);
	that_addr->sin_addr = *((in_addr *)that_host->h_addr);

	if (connect(the_socket, (struct sockaddr *)that_addr,
	            sizeof(sockaddr)) < 0 ) 
	{
		free(that_addr);
		free(this_addr);
#endif
		FreeSock();
		throw AnException(0, FL,
		                  "Error connecting to %s on port %d", machine, port);
	}

#ifndef _WIN32
	free(that_addr);
	free(this_addr);
#endif

	SocketType = CLIENT_SOCK;
}

Socket::~Socket()
{
	TRACE(FL, "Enter Socket::~Socket()");
	FreeSock();
	TRACE(FL, "Exit  Socket::~Socket()");
}

void Socket::FreeSock(void)
{
#ifdef _WIN32
	DEBUG(FL, "Calling closesocket()");
	closesocket(the_socket);
#else
	DEBUG(FL, "Calling close()");
	close(the_socket);
#endif
}

/* ******************************************************* */
/* This function assumes that we have already connected to */
/* or have accepted a connection from someone and are ready*/
/* to use the recv function on the socket.                 */
/* ******************************************************* */
int Socket::GetRawData(char *buffer, int max)
{
#ifdef _WIN32
	int cnt;
#else
	ssize_t cnt;
#endif

#ifdef _WIN32
	cnt = recv(the_socket, buffer, max, 0);
#else
	cnt = read(the_socket, buffer, max);
#endif
	if (cnt < 0 ) {
		throw AnException(0, FL, "Error receiving data on the socket");
	}

	return cnt;
}

int Socket::TimedGetRawData(char *buffer, int max, int mills)
{
	// first check for data:
	if (IsDataThere(mills)) {
		return GetRawData(buffer, max);
	} else {
		throw ReadTimeout(FL);
	}
}

int Socket::SendData(twine* buffer)
{
	return SendData((char *)buffer->c_str(), buffer->size());
}

int Socket::SendData(twine& buffer)
{
	return SendData((char *)buffer.c_str(), buffer.size());
}

/* *************************************************************** */
/* Using the appropriate socket for client or server, send bytes   */
/* through until all have been sent.  Normally, send() will return */
/* *size*, exiting the loop after one pass.  Otherwise, *cnt* is   */
/* added to the buffer pointer so that the first bytes are not re- */
/* sent.                                                           */
/* *************************************************************** */
int Socket::SendData(char *buffer, int size)
{
	int cnt;
#ifdef _WIN32
	int err;
#else
	ssize_t err;
#endif

	cnt = 0;
	do {
#ifdef _WIN32
		err = send(the_socket, buffer + cnt, size - cnt, 0);
#else
		err = write(the_socket, buffer + cnt, size - cnt);
#endif
		if (err < 0 ) {
			throw AnException(0, FL,
					  "Error sending data on the socket.");
		}
		cnt += err;
	} while (cnt < size);

	return size;
}


/* ********************************************************* */
/* This method is only valid for Server sockets.  It will    */
/* sit and listen on the socket for a connection.  Once a    */
/* Connection is detected, it will use the accept function   */
/* to establish a new socket, fill out a new Server socket   */
/* class with the new socket information and return the new  */
/* socket class.  The new class may then be used to send and */
/* receive data.                                             */
/* ********************************************************* */

GSocket *Socket::Listen(void)
{
	int err;
	SOCKET new_sock;
	SOCKADDR_IN new_addr;
	Socket *tmp;
	// On a SUN system, this has to be int:
	//	int addr_size;
	// On a Linux system, this has to be socklen_t
#ifdef _WIN32
	//int addr_size;
#else
	socklen_t addr_size;
#endif

	if (SocketType != SERVER_SOCK) {
		throw AnException(0, FL, "Listen: Not a server socket");
	}

	err = listen(the_socket, 5);
	if (err < 0) {
		throw AnException(0, FL, "Error listening on socket");
	}


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

	tmp = new Socket();
	tmp->the_socket = new_sock;
	tmp->SocketType = SERVER_SOCK;
	tmp->SetNoDelay( true );

	return tmp;
}

GSocket *Socket::Listen(int timeout)
{
	int err;
	SOCKET new_sock;
	SOCKADDR_IN new_addr;
	Socket *tmp;
	// On a SUN system, this has to be int:
	//	int addr_size;
	// On a Linux system, this has to be socklen_t
#ifdef _WIN32
	//int addr_size;
#else
	socklen_t addr_size;
#endif

	if (SocketType != SERVER_SOCK) {
		throw AnException(0, FL, "Listen: Not a server socket");
	}

	err = listen(the_socket, 5);
	if (err < 0) {
		throw AnException(0, FL, "Error listening on socket");
	}

	// Wait to verify that there is a new socket to be accepted:
	if( IsDataThere( timeout ) <= 0 ){
		// No incomming sockets to be accepted.
		return NULL;
	}

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

	tmp = new Socket();
	tmp->the_socket = new_sock;
	tmp->SocketType = SERVER_SOCK;
	tmp->SetNoDelay( true );

	return tmp;
}

int Socket::IsDataThere(void)
{
	/* **************************************** */
	/* Wait for a very short amount of time .   */
	/* **************************************** */
	return IsDataThere(1);
}

int Socket::IsDataThere(int mills)
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

void Socket::SetKeepalive( bool tf )
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

void Socket::SetNoDelay( bool tf )
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

unsigned short Socket::GetLocalPort(void)
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

int Socket::GetDataToRead( int* bytesAvailable )
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
