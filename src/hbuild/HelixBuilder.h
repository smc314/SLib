/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixBuilder_H
#define HelixBuilder_H

#include <vector>
#include <memory>
#include <map>

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

namespace Helix {
namespace Build {

/** This class represents our helix file system.
  */
class DLLEXPORT HelixBuilder
{
	public:
		/// Standard Constructor
		HelixBuilder();

		/// Standard Copy Constructor
		HelixBuilder(const HelixBuilder& c) = delete;

		/// Standard Assignment Operator
		HelixBuilder& operator=(const HelixBuilder& c) = delete;

		/// Standard Destructor
		virtual ~HelixBuilder();

		/// Use this to Build a certain path in the filesystem
		void Build( const twine& folderPath );

		/// Use this to Clean a certain path in the filesystem
		void Clean( const twine& folderPath );

		/// Use this to Build our C# folder
		void BuildCS();

		/// Use this to Clean our C# folder
		void CleanCS();

		/// Use this to Generate the data objects from sql.xml files if necessary
		void GenerateSqldo(bool forceRegen = false);
		void UpdateHelixPdfGenProj( vector<HelixFSFile>& allSqldo );
		void GenerateJSApi();

		/// Use this to Clean out the data objects from sql.xml files
		void CleanSqldo();

		void CleanAllRuntime( const twine& physicalPath );

	private:

};

}} // End Namespace stack

#endif // HelixBuilder_H defined
