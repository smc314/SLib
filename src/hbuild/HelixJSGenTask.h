/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixJSGenTask_H
#define HelixJSGenTask_H

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
class DLLEXPORT HelixJSGenTask
{
	public:
		/// Constructor requires a folder and a file
		HelixJSGenTask();

		/// Standard Copy Constructor
		HelixJSGenTask(const HelixJSGenTask& c);

		/// Standard Assignment Operator
		HelixJSGenTask& operator=(const HelixJSGenTask& c);

		/// Standard Move Constructor
		HelixJSGenTask(const HelixJSGenTask&& c);

		/// Standard Move Assignment Operator
		HelixJSGenTask& operator=(const HelixJSGenTask&& c);

		/// Standard Destructor
		virtual ~HelixJSGenTask();

		/// Triggers the generation of all artifacts produced from our input file
		void Generate(const twine& app);

	protected:

	private:
		

};

}} // End Namespace stack

#endif // HelixJSGenTask_H defined
