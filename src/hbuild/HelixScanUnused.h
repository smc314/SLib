/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixScanUnused_H
#define HelixScanUnused_H

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
class DLLEXPORT HelixScanUnused
{
	public:
		/// Constructor requires a folder and a file
		HelixScanUnused();

		/// Standard Copy Constructor
		HelixScanUnused(const HelixScanUnused& c);

		/// Standard Assignment Operator
		HelixScanUnused& operator=(const HelixScanUnused& c);

		/// Standard Move Constructor
		HelixScanUnused(const HelixScanUnused&& c);

		/// Standard Move Assignment Operator
		HelixScanUnused& operator=(const HelixScanUnused&& c);

		/// Standard Destructor
		virtual ~HelixScanUnused();

		/// Triggers the generation of all artifacts produced from our input file
		void Generate(const twine& logic, const twine& forDO, bool onlyShowUnused);


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
		twine m_targetLogic;
		twine m_targetDO;
		int m_foundMethodUse;
};

}} // End Namespace stack

#endif // HelixScanUnused_H defined
