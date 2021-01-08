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
		HelixLinkTask(HelixFSFolder* folder);

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

		HelixFSFolder* Folder();

		bool RequiresLink();
		twine LinkTarget();
		twine LinkLibs1( const twine& tpl );
		twine LinkLibs2( const twine& tpl );
		twine LinkLibs3( const twine& tpl );
		twine LinkLibs4( const twine& tpl );
		twine LinkLibs5( );
		twine Link( const twine& bin, const twine& outLib);
		twine LinkMain( const twine& bin, const twine& outLib);
		twine ObjList( const twine& folder );
		twine BinLib( const twine& bin, const twine& libName);
		twine AddApache( const twine& tpl );

		static twine ObjExt();
		static twine LibExt();
		static twine DLLExt();
		static twine BinExt();

	private:

		twine FixPhysical( const twine& path );

		HelixFSFolder* m_folder;

};

}} // End Namespace stack

#endif // HelixLinkTask_H defined
