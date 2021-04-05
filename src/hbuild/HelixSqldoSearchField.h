/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldoSearchField_H
#define HelixSqldoSearchField_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

namespace Helix {
namespace Build {

/** This class represents a sqldo input/output parameter
  */
class DLLEXPORT HelixSqldoSearchField
{
	public:
		HelixSqldoSearchField();
		HelixSqldoSearchField( xmlNodePtr elem );

		twine NormalizeName() const;

		twine name;
		twine sql;
		twine type;
		twine alias1;
		twine alias2;
		twine alias3;
		twine alias4;
		twine alias5;
};

}} // End Namespace stack

#endif // HelixSqldoSearchField_H defined
