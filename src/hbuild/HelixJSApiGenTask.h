/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixJSApiGenTask_H
#define HelixJSApiGenTask_H

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
class DLLEXPORT HelixJSApiGenTask
{
	public:
		/// Constructor requires a folder and a file
		HelixJSApiGenTask();

		/// Standard Copy Constructor
		HelixJSApiGenTask(const HelixJSApiGenTask& c);

		/// Standard Assignment Operator
		HelixJSApiGenTask& operator=(const HelixJSApiGenTask& c);

		/// Standard Move Constructor
		HelixJSApiGenTask(const HelixJSApiGenTask&& c);

		/// Standard Move Assignment Operator
		HelixJSApiGenTask& operator=(const HelixJSApiGenTask&& c);

		/// Standard Destructor
		virtual ~HelixJSApiGenTask();

		/// Triggers the generation of all artifacts produced from our input file
		void Generate(const twine& app);

		void GenerateGlobal( const twine& app );
		void GenerateNamespaced( const twine& app );

	protected:

	private:
		

};

}} // End Namespace stack

#endif // HelixJSApiGenTask_H defined
