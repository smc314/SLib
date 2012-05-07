
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

#include <vector>
using namespace std;

#include "AnException.h"
#include "File.h"
#include "ZipFile.h"
using namespace SLib;

int main (void)
{
	try {
		printf("Creating slib_src.zip...\n");
		vector<twine> files = File::listFiles( "." );
		ZipFile zf( "slib_src.zip" );
		for(size_t i = 0; i < files.size(); i ++){
			if(files[i] != "slib_src.zip"){
				printf("\tAdding file %s\n", files[i]());
				zf.AddFile( files[i] );
			}
		}
		printf("Closing slib_src.zip\n");
		zf.Close();


		printf("Extracting slib_src.zip into ./test_zip_folder\n");
		ZipFile::Extract( "slib_src.zip", "./test_zip_folder" );

		printf("test_zip complete.\n");
	} catch (AnException& e){
		printf("test_zip failed with: %s\n", e.Msg() );
	}

}

