/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixCountLines_H
#define HelixCountLines_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
#include <suvector.h>
using namespace SLib;

#include "HelixFSFolder.h"
#include "HelixSqldo.h"

namespace Helix {
namespace Build {

/** This class represents our helix file system.
  */
class DLLEXPORT HelixCountLines
{
	public:
		/// Constructor requires a folder and a file
		HelixCountLines();

		/// Standard Copy Constructor
		HelixCountLines(const HelixCountLines& c);

		/// Standard Assignment Operator
		HelixCountLines& operator=(const HelixCountLines& c);

		/// Standard Move Constructor
		HelixCountLines(const HelixCountLines&& c);

		/// Standard Move Assignment Operator
		HelixCountLines& operator=(const HelixCountLines&& c);

		/// Standard Destructor
		virtual ~HelixCountLines();

		/// Triggers the generation of all artifacts produced from our input file
		void Generate();


	protected:

		void CountCPPAndH();
		void CountSqlDo();
		void CountJS();
		void CountLayout();

		void FindAllLayoutFiles( );
		void FindAllJSFiles( const twine& start );
		void ProcessJSFile( const twine& fileName );

	private:

		twine m_qdRoot;
		twine m_currentPackage;

		size_t m_cpp_line_count = 0;
		size_t m_cpp_file_count = 0;
		size_t m_c_line_count = 0;
		size_t m_c_file_count = 0;
		size_t m_h_line_count = 0;
		size_t m_h_file_count = 0;
		size_t m_sqldo_line_count = 0;
		size_t m_sqldo_file_count = 0;
		size_t m_layout_line_count = 0;
		size_t m_layout_file_count = 0;
		size_t m_js_line_count = 0;
		size_t m_js_file_count = 0;
		size_t m_overall_line_count = 0;
		size_t m_overall_file_count = 0;
};

}} // End Namespace stack

#endif // HelixCountLines_H defined
