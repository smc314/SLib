/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixInstallTask_H
#define HelixInstallTask_H

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
class DLLEXPORT HelixInstallTask
{
	public:
		/// Constructor requires a folder and a file
		HelixInstallTask(const twine& sourceFolder, const twine& filePattern, const twine& targetFolder);

		/// Standard Copy Constructor
		HelixInstallTask(const HelixInstallTask& c);

		/// Standard Assignment Operator
		HelixInstallTask& operator=(const HelixInstallTask& c);

		/// Standard Move Constructor
		HelixInstallTask(const HelixInstallTask&& c);

		/// Standard Move Assignment Operator
		HelixInstallTask& operator=(const HelixInstallTask&& c);

		/// Standard Destructor
		virtual ~HelixInstallTask();

		/// Performs the installation based on our parameters
		void Execute();

	private:
		
		twine m_source_folder;
		twine m_file_pattern;
		twine m_target_folder;

};

}} // End Namespace stack

#endif // HelixInstallTask_H defined