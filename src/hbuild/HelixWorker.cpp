/* **************************************************************************

   Copyright (c): 2008 - 2013 Hericus Software, LLC

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#include <cstdlib>
#include <thread>

#include <AnException.h>
#include <EnEx.h>
#include <Log.h>
#include <dptr.h>
#include <XmlHelpers.h>
#include <Timer.h>
#include <Tools.h>
using namespace SLib;

#include "HelixWorker.h"
using namespace Helix::Build;

// Our global instance of the worker
static HelixWorker* m_helix_worker = nullptr;

HelixWorker& HelixWorker::getInstance()
{
	EnEx ee(FL, "HelixWorker::getInstance()");

	if(m_helix_worker == nullptr){
		m_helix_worker = new HelixWorker();

		// Start up 10 Compile threads
		std::thread t0( HelixWorker::CompileThread ); t0.detach();
		std::thread t1( HelixWorker::CompileThread ); t1.detach();
		std::thread t2( HelixWorker::CompileThread ); t2.detach();
		std::thread t3( HelixWorker::CompileThread ); t3.detach();
		std::thread t4( HelixWorker::CompileThread ); t4.detach();
		std::thread t5( HelixWorker::CompileThread ); t5.detach();
		std::thread t6( HelixWorker::CompileThread ); t6.detach();
		std::thread t7( HelixWorker::CompileThread ); t7.detach();
		std::thread t8( HelixWorker::CompileThread ); t8.detach();
		std::thread t9( HelixWorker::CompileThread ); t9.detach();

		std::thread g0( HelixWorker::GenerateThread ); g0.detach();
		std::thread g1( HelixWorker::GenerateThread ); g1.detach();
		std::thread g2( HelixWorker::GenerateThread ); g2.detach();
		std::thread g3( HelixWorker::GenerateThread ); g3.detach();
		std::thread g4( HelixWorker::GenerateThread ); g4.detach();
	}

	return *m_helix_worker;
}

HelixWorker::HelixWorker()
{
	EnEx ee(FL, "HelixWorker::HelixWorker()");

	m_compilers_working = 0;
	m_compiler_errors = 0;
	m_generators_working = 0;
	m_generator_errors = 0;
	m_finished = 0;
	m_needs_cs_rebuild = false;

}

HelixWorker::~HelixWorker()
{
	EnEx ee(FL, "HelixWorker::~HelixWorker()");

}

void HelixWorker::Add(HelixCompileTask* task)
{
	EnEx ee(FL, "HelixWorker::Add(HelixCompileTask* task)");

	m_compile_queue.push( task );
}

void HelixWorker::Add(HelixLinkTask* task)
{
	EnEx ee(FL, "HelixWorker::Add(HelixLinkTask* task)");

	dptr<HelixLinkTask> t = task;
	twine cmd( task->GetCommandLine() );
	if(cmd.empty()){
		return; // bail out immediately - no reason to wait on this
	}

	// Sleep for a moment to allow other threads to start and work
	Tools::msleep( 100 );

	// Wait for all compiles to finish
	while( m_compilers_working > 0 ){
		Tools::msleep( 100 );
	}
	if(m_compiler_errors > 0){
		return; // Skip the link step
	}

	INFO(FL, "Linking Folder: %s", task->Folder()->FolderName()() );
	if(!cmd.empty()){
		int ret = std::system( cmd() );
		if(ret != 0){
			throw AnException(0, FL, "Link task failed!\nCommand is:\n%s", cmd() );
		}
	}
}

void HelixWorker::Add(HelixInstallTask* task)
{
	EnEx ee(FL, "HelixWorker::Add(HelixInstallTask* task)");

	dptr<HelixInstallTask> t = task;
	if(IsFinished() || HasError()){
		return; // Finished or hit an error while we were waiting
	}

	// Run the install task in-line
	t->Execute();
}

void HelixWorker::Add(HelixGenTask* task)
{
	EnEx ee(FL, "HelixWorker::Add(HelixGenTask* task)");

	m_gen_queue.push( task );
}

void HelixWorker::WaitForGenerators()
{
	EnEx ee(FL, "HelixWorker::WaitForGenerators()");

	while( m_generators_working > 0 ){
		Tools::msleep( 100 );
	}
}


BlockingQueue<HelixCompileTask*>& HelixWorker::CompileQueue()
{
	return m_compile_queue;
}

BlockingQueue<HelixGenTask*>& HelixWorker::GenQueue()
{
	return m_gen_queue;
}

void HelixWorker::CompileStart()
{
	m_compilers_working ++;
}

void HelixWorker::CompileFinish()
{
	m_compilers_working --;
}

void HelixWorker::CompileError()
{
	m_compilers_working --;
	m_compiler_errors ++;
}

void HelixWorker::GenerateStart()
{
	m_generators_working ++;
}

void HelixWorker::GenerateFinish()
{
	m_generators_working --;
}

void HelixWorker::GenerateError()
{
	m_generators_working --;
	m_generator_errors ++;
}

bool HelixWorker::HasError()
{
	if(m_compiler_errors > 0){
		return true;
	}
	if(m_generator_errors > 0){
		return true;
	}
	return false;
}

void HelixWorker::Finish()
{
	m_finished = 1;
}

bool HelixWorker::IsFinished()
{
	return (m_finished != 0);
}

void HelixWorker::NeedsCSRebuild(bool tf)
{
	m_needs_cs_rebuild = tf;
}

bool HelixWorker::NeedsCSRebuild()
{
	return m_needs_cs_rebuild;
}

void HelixWorker::CompileThread()
{
	EnEx ee(FL, "HelixWorker::CompileThread()");

	HelixWorker& hw = HelixWorker::getInstance();
	while(hw.IsFinished() == false && hw.HasError() == false){
		dptr<HelixCompileTask> task = hw.CompileQueue().pop();
		if(hw.IsFinished() || hw.HasError()){
			break; // Finished or hit an error while we were waiting
		}

		if(task == nullptr) continue; // Don't process empty tasks

		// Run our compile task
		hw.CompileStart();
		INFO(FL, "Building File: %s/%s", task->File()->FolderName()(), task->File()->FileName()() );
		twine cmd( task->GetCommandLine() );
		if(!cmd.empty()){
			int ret = std::system( cmd() );
			if(ret != 0){
				hw.CompileError();
				ERRORL(FL, "Compile task failed!\nCommand is:\n%s", cmd() );
				break;
			}
		}
		hw.CompileFinish();
	}
}

void HelixWorker::GenerateThread()
{
	EnEx ee(FL, "HelixWorker::GenerateThread()");

	HelixWorker& hw = HelixWorker::getInstance();
	while(hw.IsFinished() == false && hw.HasError() == false){
		dptr<HelixGenTask> task = hw.GenQueue().pop();
		if(hw.IsFinished() || hw.HasError()){
			break; // Finished or hit an error while we were waiting
		}

		if(task == nullptr) continue; // Don't process empty tasks

		// Run our compile task
		hw.GenerateStart();
		INFO(FL, "Generating File: %s/%s", task->File()->FolderName()(), task->File()->FileName()() );
		try {
			task->Generate();
			hw.GenerateFinish();
		} catch (AnException& e){
			hw.GenerateError();
			ERRORL(FL, "Generate task failed!\n%s", e.Msg() );
			break; // Don't continue this thread
		}
	}
}

