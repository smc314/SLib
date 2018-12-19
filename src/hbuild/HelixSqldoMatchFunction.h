/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldoMatchFunction_H
#define HelixSqldoMatchFunction_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

#include "HelixSqldoParam.h"

namespace Helix {
namespace Build {

/** This class represents a sqldo match function
  */
class DLLEXPORT HelixSqldoMatchFunction
{
	public:
		HelixSqldoMatchFunction();
		HelixSqldoMatchFunction( xmlNodePtr elem );
		twine GenCPPHeader(const twine& className);
		twine GenCPPBody(const twine& className);

		twine name;
		vector< HelixSqldoParam > fields;
};

}} // End Namespace stack

#endif // HelixSqldoMatchFunction_H defined
