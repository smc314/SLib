/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixSqldo_H
#define HelixSqldo_H

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
using namespace SLib;

#include "HelixFSFolder.h"
#include "HelixSqldoParam.h"
#include "HelixSqldoMethod.h"
#include "HelixSqldoChildVector.h"
#include "HelixSqldoChildObject.h"
#include "HelixSqldoSortFunction.h"
#include "HelixSqldoXlsxFunction.h"
#include "HelixSqldoMatchFunction.h"
#include "HelixSqldoValidateFunction.h"
#include "HelixSqldoMapFunction.h"

namespace Helix {
namespace Build {

/** This class represents our helix file system.
  */
class DLLEXPORT HelixSqldo
{
	public:
		/// Default Constructor
		HelixSqldo();

		/// Constructor requires a folder and a file
		HelixSqldo(HelixFSFolder* folder, HelixFSFile* file);

		/// Standard Copy Constructor
		HelixSqldo(const HelixSqldo& c);

		/// Standard Assignment Operator
		HelixSqldo& operator=(const HelixSqldo& c);

		/// Standard Move Constructor
		HelixSqldo(const HelixSqldo&& c);

		/// Standard Move Assignment Operator
		HelixSqldo& operator=(const HelixSqldo&& c);

		/// Standard Destructor
		virtual ~HelixSqldo();

		twine FQName();
		twine LogicFolder();
		twine CPPHeaderFileName();
		twine CPPBodyFileName();
		twine CSBodyFileName();
		twine JSBodyFileName(const twine& app);
		twine CPPTestHeaderFileName();
		twine CPPTestBodyFileName();

		const HelixSqldoParam& Param(const twine& paramName);
		const map<twine, HelixSqldoParam>& AllParams();
		const vector<HelixSqldoMethod>& Methods();
		const vector<HelixSqldoChildVector>& ChildVectors();
		const vector<HelixSqldoChildObject>& ChildObjects();
		const vector<HelixSqldoSortFunction>& SortFunctions();
		const vector<HelixSqldoXlsxFunction>& XlsxFunctions();
		const vector<HelixSqldoMatchFunction>& MatchFunctions();
		const vector<HelixSqldoValidateFunction>& ValidateFunctions();
		const vector<HelixSqldoMapFunction>& MapFunctions();

		HelixFSFile* File();
		HelixFSFolder* Folder();

		void ReadSqldo();

		/// This will run all of our methods and make sure that input/output types are consistent
		bool SanityCheck();

		map< twine, twine >& BuildObjectParms();
		twine GenCPPHeader();
		twine GenCPPBody();
		twine GenCSBody();
		twine GenJSBody(const twine& app);
		twine GenCPPTestHeader(bool includeCrud, bool includePage);
		twine GenCPPTestBody(bool includeCrud, bool includePage);
		twine GenSqlXml(const twine& dbFile, const twine& tableName, const twine& logic, const twine& objName );
		twine GenCRUDDeleteHeader( const twine& logic, const twine& objName );
		twine GenCRUDDeleteBody( const twine& logic, const twine& objName );
		twine GenCRUDGetAllHeader( const twine& logic, const twine& objName );
		twine GenCRUDGetAllBody( const twine& logic, const twine& objName );
		twine GenCRUDGetOneHeader( const twine& logic, const twine& objName );
		twine GenCRUDGetOneBody( const twine& logic, const twine& objName );
		twine GenCRUDGetPagedHeader( const twine& logic, const twine& objName );
		twine GenCRUDGetPagedBody( const twine& logic, const twine& objName );
		twine GenCRUDUpdateHeader( const twine& logic, const twine& objName );
		twine GenCRUDUpdateBody( const twine& logic, const twine& objName );

		twine GenRepSendHeader( const twine& logic, const twine& objName );
		twine GenRepSendBody( const twine& logic, const twine& objName );
		twine GenRepRecvHeader( const twine& logic, const twine& objName );
		twine GenRepRecvBody( const twine& logic, const twine& objName );

		static twine loadTmpl( const twine& tmplName, map<twine, twine>& vars );
		static twine replaceVars( const twine& tmplName, size_t lineIdx, twine line, map<twine, twine>& vars );

	private:


		twine m_class_name;
		twine m_package_name;
		twine m_package_dot_name;

		map< twine, HelixSqldoParam > m_all_params;
		vector< HelixSqldoMethod > m_methods;
		vector< HelixSqldoChildVector > m_child_vectors;
		vector< HelixSqldoChildObject > m_child_objects;
		vector< HelixSqldoSortFunction > m_sorts;
		vector< HelixSqldoXlsxFunction > m_xlsxs;
		vector< HelixSqldoMatchFunction > m_matches;
		vector< HelixSqldoValidateFunction > m_validations;
		vector< HelixSqldoMapFunction > m_maps;
		map< twine, twine > m_parms;
		
		HelixFSFolder* m_folder;
		HelixFSFile* m_file;

};

}} // End Namespace stack

#endif // HelixSqldo_H defined
