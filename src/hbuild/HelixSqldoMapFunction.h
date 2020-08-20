/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldoMapFunction_H
#define HelixSqldoMapFunction_H

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
class DLLEXPORT HelixSqldoMapFunction
{
	public:
		HelixSqldoMapFunction();
		HelixSqldoMapFunction( xmlNodePtr elem );
		twine GenCPPHeader(const twine& className);
		twine GenCPPBody(const twine& className, map<twine, HelixSqldoParam>& allParams);

		twine name;
		vector< HelixSqldoParam > fields;
};

}} // End Namespace stack

#endif // HelixSqldoMapFunction_H defined
