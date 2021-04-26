/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldoSearchFunction_H
#define HelixSqldoSearchFunction_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

#include "HelixSqldoParam.h"
#include "HelixSqldoSearchField.h"

namespace Helix {
namespace Build {

/** This class represents a sqldo match function
  */
class DLLEXPORT HelixSqldoSearchFunction
{
	public:
		HelixSqldoSearchFunction();
		HelixSqldoSearchFunction( xmlNodePtr elem );
		twine GenCPPHeader(const twine& className);
		twine GenCPPBody(const twine& className, map<twine, HelixSqldoParam>& allParams);
		twine GenJSBody(const twine& className, const twine& app);

		twine name;
		vector< HelixSqldoSearchField > fields;
};

}} // End Namespace stack

#endif // HelixSqldoSearchFunction_H defined
