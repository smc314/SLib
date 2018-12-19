/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldoChildVector_H
#define HelixSqldoChildVector_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

namespace Helix {
namespace Build {

/** This class represents a sqldo child vector
  */
class DLLEXPORT HelixSqldoChildVector
{
	public:
		HelixSqldoChildVector();
		HelixSqldoChildVector( xmlNodePtr elem );
		twine SimpleType();
		twine SimplePackage();
		twine IncludeLine();

		twine CPPCopy();
		twine CPPInit();
		twine CPPXmlGet();
		twine CPPXmlSet();
		twine CPPJsonGet();
		twine CPPJsonSet();

		twine CSXmlGet();
		twine CSXmlSet();

		twine JSInit(const twine& app);
		twine JSDispose(const twine& app);
		twine JSPropDef(const twine& app);
		twine JSXmlGet(const twine& app);
		twine JSXmlSet(const twine& app);
		twine JSClone(const twine& app);

		twine name;
		twine type;
};

}} // End Namespace stack

#endif // HelixSqldoChildVector_H defined
