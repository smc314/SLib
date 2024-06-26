#ifndef XMLINC_H
#define XMLINC_H
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

/* ************************************************************* */
/* This file is used by everyone to get the right xml includes   */
/* ************************************************************* */


/* ************************************************************* */
/* The libxml2 include files all assume a ms compiler and set of */
/* defines.  In order to get it to do what we need, we have to   */
/* set up those defines ourselves on a win32 platform.           */
/* ************************************************************* */

#ifdef _WIN32
#	ifndef WIN32
#		define WIN32
#	endif
#	ifndef _MSC_VER
#		define _MSC_VER
#		define UNDO_MSCVER
#	endif
#endif

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

// For XSLT
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

// For XPath
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

/* ************************************************************* */
/* Be sure and undo the msc_ver variable, because it will cause  */
/* trouble elsewhere with standard include files that pay it     */
/* attention for what it /really/ is supposed to be used for ;-) */
/* ************************************************************* */

#ifdef UNDO_MSCVER
#undef _MSC_VER
#endif

// Convenience literal syntax so xmlChar* string literals can be written as "tag"_x rather
// than needing (const xmlChar*)"tag" casts all over the place
inline const xmlChar* operator "" _x( const char * literal, std::size_t ) { return (const xmlChar*)literal; }

#endif // XMLINC_H Defined
