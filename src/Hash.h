#ifndef HASH_H
#define HASH_H
/*
 * Copyright (c) 2001,2002 Scott L. Reeder. All rights reserved.
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
#       ifndef DLLEXPORT
#               define DLLEXPORT __declspec(dllexport)
#       endif
#else
#       define DLLEXPORT
#endif

#include <openssl/evp.h>
#include "twine.h"
#include "AnException.h"


namespace SLib
{

/**
  * This object allows one to hash characters.
  *
  * @author Scott L. Reeder
  * @version $Revision: 1.4 $
  * @copyright 2002 Scott L. Reeder
  */
class DLLEXPORT Hash
{
	public:

		/** Standard constructor.
		*/
		Hash();

		/** Constructor with a twine that contains a message digest
		*   other than SHA.
		*   Use values like:
		*		md5, md4, md2, ripemd
		*/
		Hash(twine& t);
		Hash(twine* t);

		/** Standard Destructor.
		*/
		virtual ~Hash();
	
		/** Hash characters into the message digest.
		*/
		void operator+=(const char* c);

		/** Hash characters inside a twine into the message digest.
		*/
		void operator+=(const twine& t);
		void operator+=(const twine* t);
		
		/** Generate the final hash value
		*/
		twine& GetFinalHash(void);

		/** A way to re-initialize the message digest context.
		*   Use after calling GetFinalHash if you want to use the same Hash object instance
		*/
		void ReInitialize(void);

		/** A way to re-initialize the message digest context passing a 
		*	twine with a message digest other than SHA.
		*   Use after calling GetFinalHash if you want to use the same Hash object instance
		*/
		void ReInitialize(twine& t);
		void ReInitialize(twine* t);

	

	private:
		twine m_FinalHash;
		
		

	protected:

		EVP_MD_CTX m_mdctx;
		const EVP_MD *m_md;
		bool m_bReadyToHash;
		unsigned char m_md_value[EVP_MAX_MD_SIZE];
		unsigned int m_md_len;

};

} // End namespace

#endif // TIMER_H Defined
