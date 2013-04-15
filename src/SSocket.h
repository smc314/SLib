#ifndef SSOCKET_H
#define SSOCKET_H
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


/* OpenSSL stuff */
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/des.h>

#include "GSocket.h"

namespace SLib
{

/**
  * @memo The Secure Socket class.
  * @doc  This class addes an SSL protocol on top of our normal Socket
  *       class.  This will give a secure, encrypted channel for all of
  *       our communications.  The SSL protocol implementation is provided
  *       by the OpenSSL software package.
  * @author Steven M. Cherry
  * @version $Revision: 1.3 $
  */
class DLLEXPORT SSocket : public GSocket
{

	public:

		/**
		  * @memo Starts a server socket on the local machine
		  * @doc  This version of the constructor will set up a
		  *       server socket that will listen on the current
		  *       machine for incoming connections.
		  * @param port This is the port to listen on for incomming
		  *             connections.
		  */
		SSocket(int port);

		/**
		  * @memo Starts a server socket on the local machine
		  * @doc  This version of the constructor will set up a
		  *       server socket that will listen on the current
		  *       machine for incoming connections.
		  * @param port This is the port to listen on for incomming
		  *             connections.
		  * @param localipaddr Optional null terminated string that
		  *					   specifies the specific local IP address
		  *					   to use for binding this socket.  Use NULL
		  *					   to specify using INADDR_ANY.  INADDR_ANY
		  *					   is the default.  To specify a specific
		  *					   local ip address, pass the Null-terminated 
		  *					   character string representing a number 
		  *					   expressed in the Internet standard ".'' 
		  *
		  */
		SSocket(int port, char *localipaddr);

		/**
		  * @memo Starts a client socket that connects to the 
		  *       designated addr.
		  * @doc  This version of the constructor will set up a
		  *       client socket that will connect to a particular
		  *       machine on the port indicated.
		  * @param machine This is the name of the machine to connect
		  *                to.
		  * @param port This is the port on the target machine to
		  *             use for the connection.
		  */
		SSocket(char *machine, int port);

		/**
		  * @memo Starts a client socket that connects to the 
		  *       designated addr.
		  * @doc  This version of the constructor will set up a
		  *       client socket that will connect to a particular
		  *       machine on the port indicated.
		  * @param machine This is the name of the machine to connect
		  *                to.
		  * @param port This is the port on the target machine to
		  *             use for the connection.
		  * @param localipaddr Optional null terminated string that
		  *					   specifies the specific local IP address
		  *					   to use for binding this socket.  Use NULL
		  *					   to specify using INADDR_ANY.  INADDR_ANY
		  *					   is the default.  To specify a specific
		  *					   local ip address, pass the Null-terminated 
		  *					   character string representing a number 
		  *					   expressed in the Internet standard ".'' 
		  *
		  */
		SSocket(char *machine, int port, char *localipaddr);

		/**
		  * @memo This is the destructor for the socket class.
		  * @doc  This will handle shutting down the connection if
		  *       it still exists, and freeing any system resources
		  *       associated with this socket.
		  */
		virtual ~SSocket();

		/**
		  * @memo Receives up to *max* bytes from socket.
		  * @doc  This data will receive up to max bytes from the
		  *       socket and store them in buffer.  All data is
		  *       treated as binary and handled as such.
		  * @param buffer This is where the data will be stored. It
		  *               is assumed to be at least of size max.
		  * @param max This is the maximum amount of data that will
		  *            be stored in the buffer.  It could be less,
		  *            but it will not exceed max.
		  * @return The return value is the actual number of bytes
		  *         placed in buffer.
		  */
		virtual int GetRawData(char *buffer, int max);

		/**
		  * @memo Receives up to *max* bytes from socket.
		  * @doc  This data will receive up to max bytes from the
		  *       socket and store them in buffer.  All data is
		  *       treated as binary and handled as such.  This will
		  *       throw an exception if no data appears within the
		  *       given timeout interval.
		  * @param buffer This is where the data will be stored. It
		  *               is assumed to be at least of size max.
		  * @param max This is the maximum amount of data that will
		  *            be stored in the buffer.  It could be less,
		  *            but it will not exceed max.
		  * @param mills This is the amount of time to wait
		  *              for data to show up.  1000 mills = 1 second.
		  * @return The return value is the actual number of bytes
		  *         placed in buffer.
		  */
		virtual int TimedGetRawData(char *buffer, int max, int mills);

		/**
		  * @memo Checks to see if there is data available to be
		  *       read on the socket.
		  * @doc  This is a quick and non-blocking way to check to
		  *       see if there is any data available to be read on
		  *       the socket.
		  * @return The return value is 1 (Data is present) or 0 
		  *         (no data present).
		  */
		virtual int IsDataThere(void);

		/**
		  * @memo Checks to see if there is data available to be
		  *       read on the socket using a configurable wait time.
		  * @doc  Checks to see if there is data available to be
		  *       read on the socket using a configurable wait time.
		  *       This works the same as the IsDataThere with no
		  *       parms, only it allows you to indicate how long
		  *       to wait to see if data shows up.
		  * @param mills This is the amount of time to wait
		  *              for data to show up.  1000 mills = 1 second.
		  * @return The return value is 1 (Data is present) or 0 
		  *         (no data present).
		  */
		virtual int IsDataThere(int mills);

		/**
		  * @memo Sends *size* bytes to socket
		  * @doc  This function allows you to send data on the socket.
		  *       It will read size number of bytes from buffer and
		  *       send them out on the socket.  This might result
		  *       in multiple actual socket send calls, but all of the
		  *       calls will happen within this one function.
		  * @param buffer This is where the data will be read from. It
		  *               is assumed to be at least size bytes long.
		  * @param size This is the amount of data to be read from 
		  *             buffer and written to the socket.
		  * @return The return value is the actual number of bytes
		  *         sent on the socket.  This should always equal
		  *         the input size.
		  */
		virtual int SendData(char *buffer, int size);
		virtual int SendData(twine* buffer);
		virtual int SendData(twine& buffer);

		/**
		  * @memo Server method only - returns new data socket
		  * @doc  This method is only allowed to be called on server
		  *       sockets.  It implements the socket listen and 
		  *       accept methods and returns the new socket class to
		  *       be used to transmit the data back and forth.  This
		  *       instance may then be used to Listen() again for
		  *       more inbound requests.
		  * @return The return value is a new Socket instance ready
		  *         to be used to send and receive data.
		  */
		virtual GSocket *Listen(void);

		/** Use this version of Listen when you want to check for a period of time
		  * for an incomming socket connection.  If the new socket connection is not
		  * forthcomming within the given timeout, this will return NULL.  The timeout
		  * is measured in milliseconds.
		  */
		virtual GSocket *Listen(int timeout);

		/**
		  * @memo This method allows you to set the name of the
		  *       certificate file used.  This should only be done
		  *       once, at startup.
		  * @doc  This method allows you to set the name of the
		  *       certificate file used.  This should only be done
		  *       once, at startup.
		  */
		static void SetCertFilename(const char *filename);
		static void SetCertFilename(twine* filename) {
			SetCertFilename(filename->c_str());
		}
		static void SetCertFilename(twine& filename) {
			SetCertFilename(filename.c_str());
		}


	protected:

		/**
		  * Common method that allows us to free the socket
		  * from a single point.
		  */
		void FreeSock(void);

		/**
		  * @memo Internal method of generating a new Socket:
		  * @doc  This version of the constructor is used only 
		  *       internally to create a new Socket.  It may not be
		  *       called by the outside world.
		  */
		SSocket();

		/**
		  * @memo This is where we handle the SSL structure
		  *       initialization stuff.
		  */
		virtual void InitOurSSL(void);

		/**
		  * @memo This is where we free up all of our SSL structures.
		  */
		virtual void FreeOurSSL(void);

		/**
		  * @memo This is our socket connection.
		  */
		SOCKET the_socket;

		/**
		  * @memo This tells us whether we are a server or a client
		  *       socket.
		  */
		int SocketType;                    // 0 = Server 1 = Client;

		/**
		  * @memo The SSL structure.
		  */
		SSL* m_ssl;

		/**
		  * @memo The method definition for our SSL connection.
		  */
		SSL_METHOD *m_meth;


}
;

} // End namespace.

#endif // SSOCKET_H defined
