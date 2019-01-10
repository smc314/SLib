/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixAutoAsset_H
#define HelixAutoAsset_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

#include "HelixFSFolder.h"
#include "HelixSqldo.h"

namespace Helix {
namespace Build {

/** This class represents our helix file system.
  */
class DLLEXPORT HelixAutoAsset
{
	public:
		/// Constructor requires a folder and a file
		HelixAutoAsset();

		/// Standard Copy Constructor
		HelixAutoAsset(const HelixAutoAsset& c);

		/// Standard Assignment Operator
		HelixAutoAsset& operator=(const HelixAutoAsset& c);

		/// Standard Move Constructor
		HelixAutoAsset(const HelixAutoAsset&& c);

		/// Standard Move Assignment Operator
		HelixAutoAsset& operator=(const HelixAutoAsset&& c);

		/// Standard Destructor
		virtual ~HelixAutoAsset();

		/// Triggers the generation of all artifacts produced from our input file
		void Generate();


	protected:

		void FindAllJSFiles( const twine& start );
		void ProcessFile( const twine& fileName );
		vector<twine> FindIcons( vector<twine>& lines );
		void UpdateAssetTags( vector<twine>& lines, vector<twine>& icons );

	private:

		twine m_currentPackage;
		twine m_currentFile;
		

};

}} // End Namespace stack

#endif // HelixAutoAsset_H defined
