/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixFS_H
#define HelixFS_H

#include <vector>
#include <memory>
#include <map>

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

#include "HelixFSFolder.h"
#include "HelixFSFile.h"

namespace Helix {
namespace Build {

/** This class represents our helix file system.
  */
class DLLEXPORT HelixFS
{
	public:
		static HelixFS& getInstance();

		/// Standard Destructor
		virtual ~HelixFS();

		/// Use this to load all of the files and folders in our file system up into memory
		void Load();

		// Finds the first match of the given file name recursively in our folder and all children
		HelixFSFile FindFile( const twine& fileName );

		// Finds all files that end with the given file type
		vector<HelixFSFile> FindFilesByType( const twine& fileType );

		// Finds a top level folder by name
		HelixFSFolder FindFolder( const twine& folderName );

		// Finds a folder by its full path
		HelixFSFolder FindPath( const twine& folderPath );

		// Converts forward to backslash if necessary based on current platform
		static twine FixPhysical( const twine& path );

		// Returns our cache 
		xmlDocPtr GetCache();

		// Saves our current cache to the disk
		void SaveCache();

	private:
		/// Standard Constructor
		HelixFS();

		/// Standard Copy Constructor
		HelixFS(const HelixFS& c) = delete;

		/// Standard Assignment Operator
		HelixFS& operator=(const HelixFS& c) = delete;

		// Our list of top level folders
		std::vector<HelixFSFolder> m_folders;

		// Our hbuild cache file
		sptr<xmlDoc, xmlFreeDoc> m_hbuild_cache;

};

}} // End Namespace stack

#endif // HelixFS_H defined
