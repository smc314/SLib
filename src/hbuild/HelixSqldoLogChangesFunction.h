/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldoLogChangesFunction_H
#define HelixSqldoLogChangesFunction_H

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
class DLLEXPORT HelixSqldoLogChangesFunction
{
	public:
		HelixSqldoLogChangesFunction();
		HelixSqldoLogChangesFunction( xmlNodePtr elem );
		twine GenCPPHeader(const twine& className);
		twine GenCPPBody(const twine& className, map<twine, HelixSqldoParam>& allParams);

		twine name;
		twine tableName;
		twine guid;
		vector< HelixSqldoParam > keys;
		vector< HelixSqldoParam > fields;
};

}} // End Namespace stack

#endif // HelixSqldoLogChangesFunction_H defined
