/* **************************************************************************

   Copyright (c): 2013 Hericus Software, Inc.

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#ifndef HelixWorker_H
#define HelixWorker_H

#include <vector>
#include <memory>
#include <map>
#include <atomic>

#include <twine.h>
#include <Date.h>
#include <xmlinc.h>
#include <sptr.h>
#include <File.h>
#include <BlockingQueue.h>
using namespace SLib;

#include "HelixCompileTask.h"
#include "HelixLinkTask.h"
#include "HelixInstallTask.h"
#include "HelixGenTask.h"

namespace Helix {
namespace Build {

/** This class represents our helix file system.
  */
class DLLEXPORT HelixWorker
{
	public:
		static HelixWorker& getInstance();

		/// Standard Destructor
		virtual ~HelixWorker();

		// Adds a compile task to the list of things we need to do - we take ownership and will handle deleting it
		void Add( HelixCompileTask* task );

		// Adds a link task to the list of things we need to do - we take ownership and will handle deleting it
		void Add( HelixLinkTask* task );

		// Adds an install task to the list of things we need to do - we take ownership and will handle deleting it
		void Add( HelixInstallTask* task );

		// Adds a Generate task to the list of things we need to do - we take ownership and will handle deleting it
		void Add( HelixGenTask* task );
		void WaitForGenerators();
		void WaitForCompilers();

		BlockingQueue<HelixCompileTask*>& CompileQueue();
		BlockingQueue<HelixGenTask*>& GenQueue();

		void CompileStart();
		void CompileFinish();
		void CompileError();

		void GenerateStart();
		void GenerateFinish();
		void GenerateError();

		bool HasError();
		void Finish();
		bool IsFinished();

		void NeedsCSRebuild(bool tf);
		bool NeedsCSRebuild();

		static void CompileThread();
		static void GenerateThread();

	private:
		/// Standard Constructor
		HelixWorker();

		/// Standard Copy Constructor
		HelixWorker(const HelixWorker& c) = delete;

		/// Standard Assignment Operator
		HelixWorker& operator=(const HelixWorker& c) = delete;

		// Our list of compile tasks
		BlockingQueue<HelixCompileTask*> m_compile_queue;

		// Our list of generate tasks
		BlockingQueue<HelixGenTask*> m_gen_queue;

		std::atomic<int> m_compilers_working;
		std::atomic<int> m_compiler_errors;
		std::atomic<int> m_generators_working;
		std::atomic<int> m_generator_errors;
		std::atomic<int> m_finished;

		std::atomic<bool> m_needs_cs_rebuild;
};

}} // End Namespace stack

#endif // HelixWorker_H defined
