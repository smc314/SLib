/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixGenTask_H
#define HelixGenTask_H

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
class DLLEXPORT HelixGenTask
{
	public:
		/// Constructor requires a folder and a file
		HelixGenTask(HelixFSFolder* folder, HelixFSFile* file);

		/// Standard Copy Constructor
		HelixGenTask(const HelixGenTask& c);

		/// Standard Assignment Operator
		HelixGenTask& operator=(const HelixGenTask& c);

		/// Standard Move Constructor
		HelixGenTask(const HelixGenTask&& c);

		/// Standard Move Assignment Operator
		HelixGenTask& operator=(const HelixGenTask&& c);

		/// Standard Destructor
		virtual ~HelixGenTask();

		/// Triggers the generation of all artifacts produced from our input file
		void Generate();

		HelixFSFile* File();
		HelixFSFolder* Folder();

	protected:

	private:
		
		HelixFSFolder* m_folder;
		HelixFSFile* m_file;
		HelixSqldo m_sqldo;

};

}} // End Namespace stack

#endif // HelixGenTask_H defined
