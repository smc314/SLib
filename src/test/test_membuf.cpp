
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

/* ******************************************************* */
/* Simple program to test the functions in base64.cpp      */
/* ******************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/des.h>

#include "MemBuf.h"
#include "Tools.h"
#include "File.h"
#include "AnException.h"
using namespace SLib;

#include "catch.hpp"

static RSA* membuftest_keypair = NULL;

void MemBufTest_CreateKeys(void)
{
	membuftest_keypair = RSA_generate_key(2048, 17, NULL, NULL);
	if(membuftest_keypair == NULL){
		printf("Generating RSA key pair failed.\n");
		return;
	}

	BIO* fp = BIO_new_file("./SLib.rsa", "wb" );
	if(fp == NULL){
		printf("Saving RSA key pair failed.\n");
		RSA_free( membuftest_keypair );
		membuftest_keypair = NULL;
		return;
	}
	int ret = 0;
	ret = PEM_write_bio_RSAPrivateKey(fp, membuftest_keypair, NULL, NULL, 0, NULL, NULL);
	if(!ret){
		printf("Error writing RSA private key.\n");
		RSA_free( membuftest_keypair );
		membuftest_keypair = NULL;
		BIO_free( fp );
		return;
	}
	BIO_free(fp);

	fp = BIO_new_file("./SLib.rsa_pub", "wb");
	if(fp == NULL){
		printf("Saving RSA key pair failed.\n");
		RSA_free( membuftest_keypair );
		membuftest_keypair = NULL;
		return;
	}
	ret = PEM_write_bio_RSAPublicKey(fp, membuftest_keypair);
	if(!ret){
		printf("Error writing RSA public key.\n");
		RSA_free( membuftest_keypair );
		membuftest_keypair = NULL;
		BIO_free( fp );
		return;
	}
	BIO_free(fp);
}

void MemBufTest_ReadKeys(void)
{
	BIO* fp = BIO_new_file("./SLib.rsa", "rb" );
	if(fp == NULL){
		printf("Reading RSA private key failed - error opening file.\n");
		return;
	}
	
	membuftest_keypair = PEM_read_bio_RSAPrivateKey(fp, &membuftest_keypair, NULL, NULL);
	if(membuftest_keypair == NULL){
		printf("Error reading RSA private key.\n");
		BIO_free(fp);
		return;
	}
	BIO_free(fp);

	fp = BIO_new_file("./SLib.rsa_pub", "rb");
	if(fp == NULL){
		printf("Reading RSA public key failed - error opening file.\n");
		RSA_free( membuftest_keypair );
		membuftest_keypair = NULL;
		return;
	}

	membuftest_keypair = PEM_read_bio_RSAPublicKey(fp, &membuftest_keypair, NULL, NULL);
	if(membuftest_keypair == NULL){
		printf("Error reading RSA public key.\n");
		BIO_free(fp);
		return;
	}
	BIO_free(fp);
}

void MemBufTest_FindKeys(void)
{
	CRYPTO_malloc_init();
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	SSLeay_add_ssl_algorithms();

	if(File::Exists("./SLib.rsa_pub")){
		// Our keys exist.  Load them.
		MemBufTest_ReadKeys();
	} else {
		// Our Keys don't exist, generate them
		MemBufTest_CreateKeys();
	}
}

RSA* MemBufTest_GetKeypair(void)
{
	if(membuftest_keypair == NULL){
		MemBufTest_FindKeys();
	}
	return membuftest_keypair;
}


TEST_CASE( "MemBuf - Digest - Unsigned - Hello World", "[membuf]" )
{
	// echo "Hello World" | openssl sha256
	// d2a84f4b8b650937ec8f73cd8be2c74add5a911ba64df27458ed8229da804a26
	// Easier to read version:
	// d2 a8 4f 4b 8b 65 09 37 ec 8f 73 cd 8b e2 c7 4a dd 5a 91 1b a6 4d f2 74 58 ed 82 29 da 80 4a 26

	MemBuf m( "Hello World\n" );
	MemBuf digest = m.Digest();
	REQUIRE( digest.size() == 32 );

	twine digestHex( digest.hex() ); // hex version of the digest

	REQUIRE( digestHex == "d2a84f4b8b650937ec8f73cd8be2c74add5a911ba64df27458ed8229da804a26" );


}

TEST_CASE( "MemBuf - Digest - Signed - Hello World", "[membuf]" )
{
/*
echo "Hello World" | openssl dgst -sha256 -hex -sign SLib.rsa
61ca40cb35fb7ebb94873f6ecea33ab02ebf47f10375e1d9bc12623ae9af64a3c6657b46c2c9cdf0c1963999c79befc69ac463797ad19773a101d0f110304a2e70717289c5d5d4a084f8b409f3c40d9469c4b038c43bfaa3200b8b00a7b79839eb268a0257a14daccfddaef9269a3e5ce0299f75a60fabff476b7d2e2a44fbfd9445f318fa14ca16c165b9177eae132898eb61d5940220151634c6b5794b00cd940d51823f2f8b762dec7cd2d21a6a081737110d43e22fc1ba545e06954ab5594b061a69cd9d2d00abdfe89784586b2649fe093130b868b5f29e9217834b881d7e26c5dc110171bb81ae9f8e1e18a54c0bbe328a6da33734528aeeba9bfeb961
*/

	MemBuf m( "Hello World\n" );
	MemBuf digest = m.Digest( MemBufTest_GetKeypair() );

	REQUIRE( digest.size() == 256 );

	twine digestHex( digest.hex() ); // hex version of the signed digest

	REQUIRE( digestHex == 
		"61ca40cb35fb7ebb94873f6ecea33ab02ebf47f10375e1d9bc12623ae9af64a3c6657b46c2c9cdf0c1963999c79befc69ac463797ad19773a101d0f110304a2e70717289c5d5d4a084f8b409f3c40d9469c4b038c43bfaa3200b8b00a7b79839eb268a0257a14daccfddaef9269a3e5ce0299f75a60fabff476b7d2e2a44fbfd9445f318fa14ca16c165b9177eae132898eb61d5940220151634c6b5794b00cd940d51823f2f8b762dec7cd2d21a6a081737110d43e22fc1ba545e06954ab5594b061a69cd9d2d00abdfe89784586b2649fe093130b868b5f29e9217834b881d7e26c5dc110171bb81ae9f8e1e18a54c0bbe328a6da33734528aeeba9bfeb961"
	);

}
