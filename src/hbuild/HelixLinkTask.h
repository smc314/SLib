/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixLinkTask_H
#define HelixLinkTask_H

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
class DLLEXPORT HelixLinkTask
{
	public:
		/// Constructor requires a folder and a file
		HelixLinkTask(HelixFSFolder folder);

		/// Standard Copy Constructor
		HelixLinkTask(const HelixLinkTask& c);

		/// Standard Assignment Operator
		HelixLinkTask& operator=(const HelixLinkTask& c);

		/// Standard Move Constructor
		HelixLinkTask(const HelixLinkTask&& c);

		/// Standard Move Assignment Operator
		HelixLinkTask& operator=(const HelixLinkTask&& c);

		/// Standard Destructor
		virtual ~HelixLinkTask();

		/// Retrieves the command line to accomplish this task
		twine GetCommandLine();

		HelixFSFolder Folder();

		bool RequiresLink();
		twine LinkTarget();

	private:

		twine FixPhysical( const twine& path );

		HelixFSFolder m_folder;

};

}} // End Namespace stack

#endif // HelixLinkTask_H defined
