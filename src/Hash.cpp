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

/*
  *
  * @author Scott L. Reeder
  * @version $Revision: 1.5 $
  * @copyright 2002 Scott L. Reeder
  */

#include "Hash.h"


using namespace SLib;

Hash::Hash() :
	m_bReadyToHash (false),
		m_md (NULL),
		m_md_len (0)
{
	// Initialize the message digest
	memset(m_md_value,0,EVP_MAX_MD_SIZE);
	m_FinalHash.erase();
	OpenSSL_add_all_digests();
	m_md = EVP_get_digestbyname("SHA");

	if(m_md!=NULL) {
         EVP_DigestInit(&m_mdctx, m_md);
		 m_bReadyToHash=true;
	}
	else throw AnException(0, FL, "Unable to initialize %s digest","SHA");
}

Hash::Hash(twine& t):
	m_bReadyToHash (false),
		m_md (NULL),
		m_md_len (0)
{
	// Initialize the message digest
	memset(m_md_value,0,EVP_MAX_MD_SIZE);
	m_FinalHash.erase();
	OpenSSL_add_all_digests();
	m_md = EVP_get_digestbyname(t.c_str());

	if(m_md!=NULL) {
         EVP_DigestInit(&m_mdctx, m_md);
		 m_bReadyToHash=true;
	}
	else throw AnException(0, FL, "Unable to initialize %s digest",t.c_str());


}

Hash::Hash(twine* t):
	m_bReadyToHash (false),
		m_md (NULL),
		m_md_len (0)
{
	// Initialize the message digest
	memset(m_md_value,0,EVP_MAX_MD_SIZE);
	m_FinalHash.erase();
	OpenSSL_add_all_digests();
	m_md = EVP_get_digestbyname(t->c_str());

	if(m_md!=NULL) {
         EVP_DigestInit(&m_mdctx, m_md);
		 m_bReadyToHash=true;
	}
	else throw AnException(0, FL, "Unable to initialize %s digest",t->c_str());


}

Hash::~Hash()
{
	// not much to do.
	free((void *)m_md);
	m_md=NULL;
	m_bReadyToHash=false;
	m_FinalHash.erase();
	memset(m_md_value,0,EVP_MAX_MD_SIZE);
	
}

void Hash::operator+=(const char* c)
{
	if(m_bReadyToHash!=false)	{
		twine t(c);
		EVP_DigestUpdate(&m_mdctx, t.c_str(), t.length());
	}
	else throw AnException(0, FL, "Digest not initialized or reinitialized");

}

void Hash::operator+=(const twine& t)
{
	if(m_bReadyToHash!=false)	{
		EVP_DigestUpdate(&m_mdctx, t.c_str(), t.length());
	}
	else throw AnException(0, FL, "Digest not initialized or reinitialized");

}

void Hash::operator+=(const twine* t)
{
	if(m_bReadyToHash!=false)	{
		EVP_DigestUpdate(&m_mdctx, t->c_str(), t->length());
	}
	else throw AnException(0, FL, "Digest not initialized or reinitialized");

}

twine& Hash::GetFinalHash(void)
{
	
		
	if(m_bReadyToHash!=false)	{
		unsigned int i=0;
		char tmp[100]="\0";
		m_FinalHash.erase();
		EVP_DigestFinal(&m_mdctx, m_md_value, &m_md_len);
		for(i = 0; i < m_md_len; i++) {
			sprintf(tmp,"%02x\0", m_md_value[i]);
			m_FinalHash.append(tmp);
			memset(tmp,0,100);
		}
		//m_FinalHash = (const char *)m_md_value;
		free((void *)m_md);
		m_md=NULL;
		m_bReadyToHash=false;
		return m_FinalHash;

	}
	else throw AnException(0, FL, "Digest not initialized or reinitialized");


}

void Hash::ReInitialize(void)
{
	// Initialize the message digest
	free((void *)m_md);
	m_md=NULL;
	m_bReadyToHash=false;
	memset(m_md_value,0,EVP_MAX_MD_SIZE);
	m_md_len=0;
	m_FinalHash.erase();
	OpenSSL_add_all_digests();
	m_md = EVP_get_digestbyname("SHA");

	if(m_md!=NULL) {
         EVP_DigestInit(&m_mdctx, m_md);
		 m_bReadyToHash=true;
	}
	else throw AnException(0, FL, "Unable to reinitialize %s digest","SHA");

	

}

void Hash::ReInitialize(twine& t)
{
	// Initialize the message digest
	free((void *)m_md);
	m_md=NULL;
	m_bReadyToHash=false;
	memset(m_md_value,0,EVP_MAX_MD_SIZE);
	m_md_len=0;
	m_FinalHash.erase();
	OpenSSL_add_all_digests();
	m_md = EVP_get_digestbyname(t.c_str());

	if(m_md!=NULL) {
         EVP_DigestInit(&m_mdctx, m_md);
		 m_bReadyToHash=true;
	}
	else throw AnException(0, FL, "Unable to reinitialize %s digest",t.c_str());

}

void Hash::ReInitialize(twine* t)
{
	// Initialize the message digest
	free((void *)m_md);
	m_md=NULL;
	m_bReadyToHash=false;
	memset(m_md_value,0,EVP_MAX_MD_SIZE);
	m_md_len=0;
	m_FinalHash.erase();
	OpenSSL_add_all_digests();
	m_md = EVP_get_digestbyname(t->c_str());

	if(m_md!=NULL) {
         EVP_DigestInit(&m_mdctx, m_md);
		 m_bReadyToHash=true;
	}
	else throw AnException(0, FL, "Unable to reinitialize %s digest",t->c_str());

}