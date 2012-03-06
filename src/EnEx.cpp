/* C Standard Headers */
#include <stdlib.h>
#include <stdio.h>

/* C++ Standard Headers */
#include <vector>
#include <utility>
#include <map>
using namespace std;

/* SLib Headers */
#include "EnEx.h"
#include "Timer.h"
#include "Thread.h"
#include "Log.h"
#include "xmlinc.h"
using namespace SLib;

/** This is the global hit counter collection that will be updated by
    each thread periodically as they do their work.  It's only guaranteed
    to be written to when you use the EnEx constructor with the saveToGlobal
    flag turned on.
*/
map<const char*, EnExProfile*> global_hit_counter;
SLib::Mutex global_hit_counter_mutex;

/** This is the list of entry exit stats that we keep for every thread
    that runs in the system.  It's indexed by the thread id and maintained
    by each individual thread.
*/
vector< pair< 
	THREAD_ID_TYPE, 
	map<const char*, EnExProfile*>* 
> > hit_counter_list;

SLib::Mutex hit_counter_list_add_mutex;


EnterExit::EnterExit(const char* methodName) : 
	m_methodName(methodName), 
	m_saveToGlobal(false),
	m_file(""),
	m_line(0)
{
	Init();
}

EnterExit::EnterExit(const char* methodName, bool saveToGlobal) : 
	m_methodName(methodName), 
	m_saveToGlobal(saveToGlobal),
	m_file(""),
	m_line(0)
{
	Init();
}

EnterExit::EnterExit(const char* file, int line, const char* methodName) : 
	m_methodName(methodName), 
	m_saveToGlobal(false),
	m_file(file),
	m_line(line)
{
	Init();
}

EnterExit::EnterExit(const char* file, int line, const char* methodName, bool saveToGlobal) : 
	m_methodName(methodName), 
	m_saveToGlobal(saveToGlobal),
	m_file(file),
	m_line(line)
{
	Init();
}

void EnterExit::Init(void)
{
	m_hitCounter = FindOurHitCounter();

	map<const char*, EnExProfile*>::iterator it = m_hitCounter->find(m_methodName);
	if(it != m_hitCounter->end()){
		// keep track of this for exit timing.
		m_methodProfile = it->second; 
		m_methodProfile->HitsInc();
	} else {
		// keep track of this for exit timing.
		m_methodProfile = new EnExProfile(m_methodName);
		(*m_hitCounter)[ m_methodName ] = m_methodProfile;
	}

	if(m_line) TRACE(m_file, m_line, "%s: Entering Method", m_methodName);
	m_methodEntryStamp = Timer::GetCycleCount();
}

map<const char*, EnExProfile*>* EnterExit::FindOurHitCounter(void)
{
	THREAD_ID_TYPE tid = Thread::CurrentThreadId();
	for(int i = 0, l = hit_counter_list.size(); i < l; i++){
		if(hit_counter_list[i].first == tid){
			return hit_counter_list[i].second;
		}
	}
	// If we get here, then our tid was not in the list.  Add it in.
	pair<THREAD_ID_TYPE, map<const char*, EnExProfile*>* > the_pair;
	the_pair.first = tid;
	the_pair.second = new map<const char*, EnExProfile*>();
	{ // for scope
		SLib::Lock the_lock(&hit_counter_list_add_mutex);
		hit_counter_list.push_back(the_pair);
	} // mutex released here
	return the_pair.second;
}

EnterExit::~EnterExit()
{
	m_methodExitStamp = Timer::GetCycleCount();
	if(m_line) TRACE(m_file, m_line, "%s: Exiting Method", m_methodName);

	m_methodProfile->RecordEntryExit(m_methodEntryStamp, m_methodExitStamp);

	if(m_saveToGlobal){
		// Save our whole hit counter map to the global aggregate
		SaveToGlobal();
	}
}

void EnterExit::SaveToGlobal(void)
{
	SLib::Lock the_lock(&global_hit_counter_mutex);

	map<const char*, EnExProfile*>::iterator our_it;
	map<const char*, EnExProfile*>::iterator glob_it;

	// Walk all of our hit counters:
	for(our_it = m_hitCounter->begin(); our_it != m_hitCounter->end(); our_it++){

		// Now find this entry in the global hit counter map:

		glob_it = global_hit_counter.find(our_it->first);
		EnExProfile* glob_method_profile = 0;
		if(glob_it != global_hit_counter.end()){
			// We found the method in the global map
			glob_method_profile = glob_it->second;
		} else {
			// We didn't find the method in the global map.  We need to
			// add a new one.
			glob_method_profile = new EnExProfile(our_it->first);
			glob_method_profile->Hits(0); // Zero out the hit count.
			global_hit_counter[ our_it->first ] = glob_method_profile;
		}

		// Now update the global method profile with our information:
		glob_method_profile->Add( *(our_it->second) );

		// After adding it to the global profile, remember to zero out our values
		// so that we don't double things up in the global profile later.
		our_it->second->Reset();

	}

	// Exiting this method releases the lock on the global hit counter map.
}

void EnterExit::PrintHitMap(void)
{
	map<const char*, EnExProfile*>* hit_counters = FindOurHitCounter();
	map<const char*, EnExProfile*>::iterator it;
	printf("%40s\t%12s\t%16s\t%16s\t%16s\t%16s\n",
		"Method Name",
		"Total Hits",
		"Average Cycles",
		"Min Cycles",
		"Max Cycles",
		"Total Cycles");
	printf("%40s\t%12s\t%16s\t%16s\t%16s\t%16s\n",
		"===========",
		"==========",
		"==============",
		"==========",
		"==========",
		"============");
	for(it = hit_counters->begin(); it != hit_counters->end(); it++){
		printf("%40s\t%12ld\t%16.2f\t%16.2f\t%16.2f\t%16.2f\n",
			it->first, it->second->Hits(),
			it->second->AvgTime(),
			it->second->MinTime(),
			it->second->MaxTime(),
			it->second->TotalTime()
		);
		if(it->second->StopProfile()){
			printf("\tProfiling stopped after a thousand hits with an average less than 0.0001\n");
		}
	}
}

void EnterExit::PrintGlobalHitMap(void)
{
	map<const char*, EnExProfile*>::iterator it;
	printf("%40s\t%12s\t%16s\t%16s\t%16s\t%16s\n",
		"Method Name",
		"Total Hits",
		"Average Cycles",
		"Min Cycles",
		"Max Cycles",
		"Total Cycles");
	printf("%40s\t%12s\t%16s\t%16s\t%16s\t%16s\n",
		"===========",
		"==========",
		"==============",
		"==========",
		"==========",
		"============");
	for(it = global_hit_counter.begin(); it != global_hit_counter.end(); it++){
		printf("%40s\t%12ld\t%16.2f\t%16.2f\t%16.2f\t%16.2f\n",
			it->first, it->second->Hits(),
			it->second->AvgTime(),
			it->second->MinTime(),
			it->second->MaxTime(),
			it->second->TotalTime()
		);
		if(it->second->StopProfile()){
			printf("\tProfiling stopped after a thousand hits with an average less than 0.0001\n");
		}
	}
}

void EnterExit::PrintGlobalHitMap(twine& output)
{
	twine tmp;
	map<const char*, EnExProfile*>::iterator it;
	tmp.format("%40s\t%12s\t%16s\t%16s\t%16s\t%16s\n",
		"Method Name",
		"Total Hits",
		"Average Cycles",
		"Min Cycles",
		"Max Cycles",
		"Total Cycles");
	output += tmp;
	tmp.format("%40s\t%12s\t%16s\t%16s\t%16s\t%16s\n",
		"===========",
		"==========",
		"==============",
		"==========",
		"==========",
		"============");
	output += tmp;
	for(it = global_hit_counter.begin(); it != global_hit_counter.end(); it++){
		tmp.format("%40s\t%12ld\t%16.2f\t%16.2f\t%16.2f\t%16.2f\n",
			it->first, it->second->Hits(),
			it->second->AvgTime(),
			it->second->MinTime(),
			it->second->MaxTime(),
			it->second->TotalTime()
		);
		output += tmp;
		if(it->second->StopProfile()){
			tmp.format("\tProfiling stopped after a thousand hits with an average less than 0.0001\n");
			output += tmp;
		}
	}
}

void EnterExit::RecordGlobalHitMap(xmlNodePtr node)
{
	twine tmp;
	map<const char*, EnExProfile*>::iterator it;

	for(it = global_hit_counter.begin(); it != global_hit_counter.end(); it++){
		xmlNodePtr child = xmlNewChild(node, NULL, (const xmlChar*)"HitMap", NULL);
		xmlSetProp(child, (const xmlChar*)"MethodName", (const xmlChar*)it->first);
		tmp.format("%ld", it->second->Hits());
		xmlSetProp(child, (const xmlChar*)"TotalHits", tmp);
		tmp.format("%ld", (long)it->second->AvgTime());
		xmlSetProp(child, (const xmlChar*)"AverageCycles", tmp);
		tmp.format("%ld", (long)it->second->MinTime());
		xmlSetProp(child, (const xmlChar*)"MinCycles", tmp);
		tmp.format("%ld", (long)it->second->MaxTime());
		xmlSetProp(child, (const xmlChar*)"MaxCycles", tmp);
		tmp.format("%ld", (long)it->second->TotalTime());
		xmlSetProp(child, (const xmlChar*)"TotalCycles", tmp);
	}
}

EnExProfile::EnExProfile(const char* methodName)
{
	m_methodName = methodName;
	m_hits = 1;	
	m_totalTime = 0;
	m_minTime = 0;
	m_maxTime = 0;
	m_stopProfile = false;
}

EnExProfile::~EnExProfile()
{

}

void EnExProfile::Reset(void)
{
	m_hits = 0;
	m_totalTime = 0;
	m_minTime = 0;
	m_maxTime = 0;
}

void EnExProfile::Add( const EnExProfile& eep)
{
	m_hits += eep.m_hits;
	m_totalTime += eep.m_totalTime;
	if(eep.m_minTime < m_minTime){
		m_minTime = eep.m_minTime;
	}
	if(eep.m_maxTime > m_maxTime){
		m_maxTime = eep.m_maxTime;
	}
}

unsigned long EnExProfile::Hits(void)
{
	return m_hits;
}

void EnExProfile::Hits(unsigned long h)
{
	m_hits = h;
}

void EnExProfile::HitsInc(void)
{
	m_hits ++;
}

bool EnExProfile::StopProfile(void)
{
	return m_stopProfile;
}

void EnExProfile::StopProfile(bool tf)
{
	m_stopProfile = tf;
}

double EnExProfile::AvgTime(void)
{
	return TotalTime() / m_hits * 1.0;
}

double EnExProfile::MinTime(void)
{
	return (double)m_minTime;
}

double EnExProfile::MaxTime(void)
{
	return (double)m_maxTime;
}

double EnExProfile::TotalTime(void)
{
	return (double)m_totalTime;
}

void EnExProfile::RecordEntryExit(unsigned long entry, unsigned long exit)
{
	//SLib::Lock the_lock(&m_mutex); // prevent simultaneous access to this method.

	unsigned long diff = exit - entry;
	if(diff < 0){
		diff = 0;
	}

	// Add To the total time:
	m_totalTime += diff;

	// Is it less than the min?
	if(m_minTime == 0 || diff < m_minTime){
		m_minTime = diff;
	}

	// Is it more than the max?
	if(m_maxTime == 0 || diff > m_maxTime){
		m_maxTime = diff;
	}

}
