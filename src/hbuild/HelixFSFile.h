/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixFSFile_H
#define HelixFSFile_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;


namespace Helix {
namespace Build {

// Forward declare the class so that we can do the alias.
class DLLEXPORT HelixFSFile_Bare;
using HelixFSFile = std::shared_ptr<HelixFSFile_Bare>;

/** This class represents our helix file system.
  */
class DLLEXPORT HelixFSFile_Bare
{
	public:
		/// Constructor requires a folder name and a file name
		HelixFSFile_Bare(const twine& folderName, const twine& fileName);

		/// Standard Copy Constructor
		HelixFSFile_Bare(const HelixFSFile& c) = delete;

		/// Standard Assignment Operator
		HelixFSFile_Bare& operator=(const HelixFSFile_Bare& c) = delete;

		/// Standard Move Constructor
		HelixFSFile_Bare(const HelixFSFile_Bare&& c) = delete;

		/// Standard Move Assignment Operator
		HelixFSFile_Bare& operator=(const HelixFSFile_Bare&& c) = delete;

		/// Standard Destructor
		virtual ~HelixFSFile_Bare();

		void Reload();

		const twine& FileName() const;
		const twine& FolderName() const;
		twine LastFolderName();
		const twine& PhysicalFileName() const;
		const twine& PhysicalDotOh() const;

		xmlDocPtr Xml();
		const vector<twine>& Lines();
		const vector<HelixFSFile>& Dependencies();
		HelixFSFile FindDep(const twine& dependentFile );
		bool NeedsRebuild();
		void AddChildDeps( HelixFSFile dep );
		bool AddUniqueDep( HelixFSFile dep );

		twine DataObjectName( );
		const twine& DotOh() const;
		vector<pair<twine, twine>>& Apis();

		// Returns whether this file is newer than another file
		bool IsNewerThan( HelixFSFile other );
		bool IsNewerThan( const twine& otherFilePath );
		static bool IsNewerThan( const twine& thisFilePath, const twine& otherFilePath );

		// Returns the size of the file
		long FileSize();

		// Returns the last modification time of the file
		Date LastModified();

	private:
		void LoadDependenciesExplicitly();
		void LoadDependenciesFromCache();
		void SaveExplicitDependenciesToCache();

		twine m_folder;
		twine m_file;
		mutable twine m_physical_file;
		mutable twine m_physical_dotoh;
		mutable twine m_dotoh;
		vector<twine> m_lines;
		vector<HelixFSFile> m_deps;
		bool m_deps_loaded = false;
		sptr<xmlDoc, xmlFreeDoc> m_xml_doc;
		vector<pair<twine, twine>> m_api_list;

		Date m_file_last_modified;
		long m_file_size;

};

}} // End Namespace stack

#endif // HelixFSFile_H defined
