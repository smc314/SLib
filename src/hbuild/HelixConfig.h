/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixConfig_H
#define HelixConfig_H

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
class DLLEXPORT HelixConfig
{
	public:
		static HelixConfig& getInstance();

		/// Standard Destructor
		virtual ~HelixConfig();

		/// Returns a flag indicating whether we are using a separate core folder to support us
		bool UseCore();

		/// Returns the core folder, if any, that we should use
		twine CoreFolder();

		/// Returns a list of our Qx Applications
		vector<twine> QxApps();

		/// Returns a list of our Logic folders
		vector<twine> Logics();

		/// Returns a list of dependencies for a given logic folder
		vector<twine> LogicDepends( const twine& logic );

		/// Returns a list of 
		// Returns the list of Install Nodes
		vector<xmlNodePtr> Installs();

	private:
		/// Standard Constructor
		HelixConfig();

		/// Standard Copy Constructor
		HelixConfig(const HelixConfig& c) = delete;

		/// Standard Assignment Operator
		HelixConfig& operator=(const HelixConfig& c) = delete;

		// Our hbuild cache file
		sptr<xmlDoc, xmlFreeDoc> m_config;

};

}} // End Namespace stack

#endif // HelixConfig_H defined