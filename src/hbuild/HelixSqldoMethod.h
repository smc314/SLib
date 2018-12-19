/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldoMethod_H
#define HelixSqldoMethod_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

#include "HelixSqldoParam.h"

namespace Helix {
namespace Build {

/** This class represents a sqldo method
  */
class DLLEXPORT HelixSqldoMethod
{
	public:
		HelixSqldoMethod();
		HelixSqldoMethod( xmlNodePtr elem );

		twine GenCPPHeader(const twine& className);
		twine GenCPPBody(const twine& className);
		twine GenCSBody(const twine& className);

		map<twine, twine>& BuildStatementParms(const twine& className);
		twine FlattenSql();
		bool HasAutoGen();
		bool HasIntInput();
		bool HasFloatInput();

		twine name;
		twine type;
		twine target;
		twine comment;
		twine sql;
		vector< HelixSqldoParam > inputs;
		vector< HelixSqldoParam > outputs;

		map<twine, twine> m_parms;
};

}} // End Namespace stack

#endif // HelixSqldoMethod_H defined
