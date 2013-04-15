#ifndef GSOCKET_H
#define GSOCKET_H
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
#include <windows.h>
//#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif


#include "twine.h"
#include "AnException.h"

/**
  * @memo define used to identify a server style socket.
  */
#define SERVER_SOCK 0

/**
  * @memo define used to identify a client style socket.
  */
#define CLIENT_SOCK 1

#ifndef _WIN32

/**
  * @memo This define is usefull for translations between os's.
  */
#define SOCKET int

/**
  * @memo This define is also usefull for translations between os's.
  */
#define SOCKADDR_IN struct sockaddr_in *

#endif

/**
  * The timeout exception is thrown by timed read calls.
  */
class ReadTimeout : public AnException
{
	public:
		ReadTimeout( const char *file, int line) :
		AnException(0, file, line, "Socket Read Timed Out.") {}

		virtual ~ReadTimeout() {}
};

namespace SLib {

/**
  * @memo The Generic Socket class.
  * @doc  This class is our pure virtual definition of the socket interface.
  *       This will provide all of our application and anyone who uses
  *       our foundation code with the definition of how to interact with 
  *       our socket functionality.
  * @author Steven M. Cherry
  * @version $Revision: 1.2 $
  */
class DLLEXPORT GSocket
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
		GSocket(int port) {}

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
		GSocket(char *machine, int port) {}

		/**
		  * @memo This is the destructor for the socket class.
		  * @doc  This will handle shutting down the connection if
		  *       it still exists, and freeing any system resources
		  *       associated with this socket.
		  */
		virtual ~GSocket() {}

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
		virtual int GetRawData(char *buffer, int max) = 0;

		/**
		  * @memo Receives up to *max* bytes from socket.
		  * @doc  This data will receive up to max bytes from the
		  *       socket and store them in buffer.  All data is
		  *       treated as binary and handled as such.  This will
		  *       throw a ReadTimeout exception if no data appears 
		  *       within the given timeout interval.
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
		virtual int TimedGetRawData(char *buffer, int max, int mills) = 0;

		/**
		  * @memo Checks to see if there is data available to be
		  *       read on the socket.
		  * @doc  This is a quick and non-blocking way to check to
		  *       see if there is any data available to be read on
		  *       the socket.
		  * @return The return value is 1 (Data is present) or 0 
		  *         (no data present).
		  */
		virtual int IsDataThere(void) = 0;

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
		virtual int IsDataThere(int mills) = 0;

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
		virtual int SendData(char *buffer, int size) = 0;
		/// Same as above
		virtual int SendData(twine* buffer) = 0;
		/// Same as above
		virtual int SendData(twine& buffer) = 0;

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
		virtual GSocket *Listen(void) = 0;

		/** Use this version of Listen when you want to check for a period of time
		  * for an incomming socket connection.  If the new socket connection is not
		  * forthcomming within the given timeout, this will return NULL.  The timeout
		  * is measured in milliseconds.
		  */
		virtual GSocket *Listen(int timeout) = 0;

	protected:

		/**
		  * @memo Internal method of generating a new Socket:
		  * @doc  This version of the constructor is used only 
		  *       internally to create a new Socket.  It may not be
		  *       called by the outside world.
		  */
		GSocket() {}

}
;

} // End namespace.

#endif // GSOCKET_H defined
