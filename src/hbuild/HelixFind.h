/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixFind_H
#define HelixFind_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
#include <suvector.h>
using namespace SLib;

#include "HelixFSFolder.h"
#include "HelixSqldo.h"

namespace Helix {
namespace Build {

/** This class represents our helix file system.
  */
class DLLEXPORT HelixFind
{
	public:
		/// Constructor requires a folder and a file
		HelixFind();

		/// Standard Copy Constructor
		HelixFind(const HelixFind& c);

		/// Standard Assignment Operator
		HelixFind& operator=(const HelixFind& c);

		/// Standard Move Constructor
		HelixFind(const HelixFind&& c);

		/// Standard Move Assignment Operator
		HelixFind& operator=(const HelixFind&& c);

		/// Standard Destructor
		virtual ~HelixFind();

		/// Triggers the generation of all artifacts produced from our input file
		void Generate(const twine& searchString);


	protected:

		void FindInCPPAndH();
		void FindInSqlDo();
		void FindInJS();
		void FindInLayout();

		void FindAllLayoutFiles( );
		void FindAllJSFiles( const twine& start );
		void ProcessJSFile( const twine& fileName );

		void FindInLines( const twine& fileName, const vector<twine>& lines );

	private:

		twine m_qdRoot;
		twine m_currentPackage;
		twine m_searchString;
};

}} // End Namespace stack

#endif // HelixFind_H defined
