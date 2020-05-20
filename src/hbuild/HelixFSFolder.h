/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixFSFolder_H
#define HelixFSFolder_H

#include <vector>
#include <memory>
#include <map>

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

#include "HelixFSFile.h"

namespace Helix {
namespace Build {

// Forward declar the class so that we can do the using.
class DLLEXPORT HelixFSFolder;
using HelixFSFolder_svect = std::unique_ptr< std::vector<HelixFSFolder*>, VectorDelete<HelixFSFolder> >;

/** This class represents our helix file system.
  */
class DLLEXPORT HelixFSFolder
{
	public:
		/// Constructor requires a folder name
		HelixFSFolder(const twine& folderName);

		/// Standard Copy Constructor
		HelixFSFolder(const HelixFSFolder& c) = delete;

		/// Standard Assignment Operator
		HelixFSFolder& operator=(const HelixFSFolder& c) = delete;

		/// Standard Move Constructor
		HelixFSFolder(const HelixFSFolder&& c) = delete;

		/// Standard Move Assignment Operator
		HelixFSFolder& operator=(const HelixFSFolder&& c) = delete;

		/// Standard Destructor
		virtual ~HelixFSFolder();

		void Load();

		const twine& FolderName() const;
		const twine& LastName() const;
		twine PhysicalFolderName() const;
		bool FromCore() const;

		vector<HelixFSFolder*>& SubFolders();
		vector<HelixFSFile*>& Files();

		// Finds the first match of the given file name recursively in our folder and all children
		HelixFSFile* FindFile( const twine& fileName );

		// Finds all files that end with the given file type
		void FindFilesByType( const twine& fileType, vector<HelixFSFile*>& results);

		// Finds one of our folders by name - does not recurse
		HelixFSFolder* FindFolder( const twine& folderName );

		// Removes a file in our folder by name - if it exists
		void DeleteFile( const twine& fileName );


	private:

		// Our list of files - we own the files, and we'll make sure they are deleted
		HelixFSFile_svect m_files;

		// Our list of sub-folders - we own the folders, and we'll make sure they are deleted
		HelixFSFolder_svect m_folders;
	
		// Our folder name
		twine m_name;

		// Our last name
		twine m_last_name;

};

}} // End Namespace stack

#endif // HelixFSFolder_H defined
