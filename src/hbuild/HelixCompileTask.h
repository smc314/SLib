/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixCompileTask_H
#define HelixCompileTask_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

#include "HelixFSFolder.h"

namespace Helix {
namespace Build {

/** This class represents our helix file system.
  */
class DLLEXPORT HelixCompileTask
{
	public:
		/// Constructor requires a folder and a file
		HelixCompileTask(HelixFSFolder folder, HelixFSFile file);

		/// Standard Copy Constructor
		HelixCompileTask(const HelixCompileTask& c);

		/// Standard Assignment Operator
		HelixCompileTask& operator=(const HelixCompileTask& c);

		/// Standard Move Constructor
		HelixCompileTask(const HelixCompileTask&& c);

		/// Standard Move Assignment Operator
		HelixCompileTask& operator=(const HelixCompileTask&& c);

		/// Standard Destructor
		virtual ~HelixCompileTask();

		/// Retrieves the command line to accomplish this task
		twine GetCommandLine();

		HelixFSFile File();
		HelixFSFolder Folder();

	private:
		
		twine FixPhysical(const twine& path);
		twine CC(const twine& tpl);
		twine LogicIncludes(const twine& logicName, bool fromSqldo = false);
		twine DependentInclude(const twine& ourLogic, const twine& depLogic, bool fromSqldo = false);

		HelixFSFolder m_folder;
		HelixFSFile m_file;

};

}} // End Namespace stack

#endif // HelixCompileTask_H defined
