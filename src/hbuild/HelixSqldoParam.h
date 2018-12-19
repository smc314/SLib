/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldoParam_H
#define HelixSqldoParam_H

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
class DLLEXPORT HelixSqldoParam
{
	public:
		HelixSqldoParam();
		HelixSqldoParam( xmlNodePtr elem );

		bool MatchesType( const HelixSqldoParam& p );
		twine CPPType() const;
		twine CPPParm() const;
		twine CPPInit() const;
		twine CPPXmlGet() const;
		twine CPPXmlSet() const;
		twine CPPJsonGet() const;
		twine CPPJsonSet() const;
		twine CPPReadDB(int pos) const;
		twine CPPReplaceInput() const;
		twine CPPReplaceInputObj() const;

		twine CSType() const;
		twine CSParm() const;
		twine CSInit() const;
		twine CSXmlGet() const;
		twine CSXmlSet() const;
		twine CSReadDB(int pos) const;
		twine CSReplaceInput() const;
		twine CSReplaceInputObj() const;

		twine JSPropDef(bool first) const;
		twine JSXmlGet(const twine& app) const;
		twine JSXmlSet(const twine& app) const;
		twine JSClone(const twine& app) const;

		twine name;
		twine type;
		bool isInput;
		bool desc;
		bool usecdata;
};

}} // End Namespace stack

#endif // HelixSqldoParam_H defined
