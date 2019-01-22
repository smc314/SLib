/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixExtractStrings_H
#define HelixExtractStrings_H

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
class DLLEXPORT HelixExtractStrings
{
	public:
		/// Constructor requires a folder and a file
		HelixExtractStrings();

		/// Standard Copy Constructor
		HelixExtractStrings(const HelixExtractStrings& c);

		/// Standard Assignment Operator
		HelixExtractStrings& operator=(const HelixExtractStrings& c);

		/// Standard Move Constructor
		HelixExtractStrings(const HelixExtractStrings&& c);

		/// Standard Move Assignment Operator
		HelixExtractStrings& operator=(const HelixExtractStrings&& c);

		/// Standard Destructor
		virtual ~HelixExtractStrings();

		/// Triggers the generation of all artifacts produced from our input file
		void Generate();


	protected:

		void FindAllLayoutFiles( );
		void ProcessFile( const twine& fileName );
		void FindAllCPPFiles();
		void ProcessCPPFile ( HelixFSFile file );
		void FindCPPStaticLabel( const twine& line );
		void FindCPPStaticLabelInKey( const twine& line, const twine& key );
		void HandleLayoutNode( xmlNodePtr node );
		void LoopChildNodes( xmlNodePtr node );
		void OutputStaticLabel( const twine& staticLabel );
		void UpdateStaticLabelFile();

	private:

		twine m_qdRoot;
		twine m_currentPackage;
		twine m_currentPOFile;
		suvector<twine> m_staticLabels;
};

}} // End Namespace stack

#endif // HelixExtractStrings_H defined
