
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

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>

int main(void)
{
	unsigned char response[4096];
	int len, rlen, res, count, i;
	unsigned char *ptr1, *ptr2;
	HEADER *head;
	char domain_name[128];
	char target_name[128];
	int type, pref, size;

	int max_pref = 0;
	char best_target[128];

	int ret = res_init();	
	rlen = res_search("redhat.com", C_IN, T_MX, response, 4095);

	if(rlen < 0){
		printf("Error on lookup: (%d)\n", rlen);	
		exit(1);
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
			printf("Error expanding name.\n");
			exit(1);
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
			printf("Error2 expanding name.\n");
			exit(1);
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
			printf("Error decoding target name.\n");
			exit(1);
		}

		printf("Domain (%s) Mail host (%s) Preference (%d)\n",
			domain_name, target_name, pref);

		if(pref > max_pref){
			max_pref = pref;
			memset(best_target, 0, 128);
			strcpy(best_target, target_name);
		}

		// point to the next record and keep going
		ptr1 = ptr2;

	}

	// we don't care about the other records in the answer.

	printf("For domain (%s) the best mail host is (%s)\n",
		domain_name, best_target);
}
