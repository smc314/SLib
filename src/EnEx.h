#ifndef ENEX_H
#define ENEX_H

#ifdef _WIN32
#	ifndef DLLEXPORT
#		define DLLEXPORT __declspec(dllexport)
#       endif
#else
#	define DLLEXPORT 
#endif

/* C Standard Headers */
#include <stdint.h>

/* C++ Standard Headers */
#include <vector>
#include <string>
#include <map>
using namespace std;

/* SLib headers */
#include "Lock.h"
#include "twine.h"
#include "xmlinc.h"
namespace SLib {

class DLLEXPORT EnExProfile {
	public:

		EnExProfile(const char* methodName);

		virtual ~EnExProfile();

		void Reset(void);
		unsigned long Hits(void);
		void Hits(unsigned long);
		void HitsInc(void);

		bool StopProfile(void);
		void StopProfile(bool tf);

		double AvgTime(void);
		double MinTime(void);
		double MaxTime(void);
		double TotalTime(void);

		void RecordEntryExit(uint64_t entry, uint64_t exit);

		/** Add information from another profile object into ours.  This is primarily used
		  * when we are held in the global profile list and the thread-based profiles are
		  * adding their information into us.
		  */
		void Add( const EnExProfile& eep);

	private:
		const char* m_methodName;
		uint64_t m_hits;
		uint64_t m_totalTime;
		uint64_t m_minTime;
		uint64_t m_maxTime;
		bool m_stopProfile;
};


class DLLEXPORT EnterExit {
	public:
		/** Constructor requires a name of the current method.
		 */
		EnterExit(const char* methodName);

		/** Constructor requires a name of the current method and an indicator as to
		 * whether we should add our stats to the global cache at exit.
		 */
		EnterExit(const char* methodName, bool saveToGlobal);

		/** Constructor requires a name of the current method.
		 */
		EnterExit(const char* file, int line, const char* methodName);

		/** Constructor requires a name of the current method and an indicator as to
		 * whether we should add our stats to the global cache at exit.
		 */
		EnterExit(const char* file, int line, const char* methodName, bool saveToGlobal);

		/** Destructor handles logging the exit.
		 */
		virtual ~EnterExit();

		/** This will print out  the hit counters for the current thread 
		  */
		static void PrintHitMap(void);

		/** This will print out  the hit counters for all threads based on what has been
		    stored in the global hit counter aggregate area.
		  */
		static void PrintGlobalHitMap(void);

		/** This will print out  the hit counters for all threads based on what has been
		    stored in the global hit counter aggregate area.
		  */
		static void PrintGlobalHitMap(twine& output);

		/** This will save our current hit map data as a series of HitMap nodes added as
		 * children under the given node.
		 */
		static void RecordGlobalHitMap(xmlNodePtr node);

		/** This will print our stack trace to standard output
		  */
		static void PrintStackTrace(void);

		/** This will log our stack trace to the log file using the given log channel.
		  */
		static void PrintStackTrace(int channel);
			
		/** This will capture our stack trace to a twine and return it.
		  */
		static twine GetStackTrace(void);

		/** This handles saving our thread-local hit counter information into the
		    global structure.  This will take more time because we have to lock the
		    global structure mutex, but if called only seldom on major functions, this
		    will be effective at keeping the global aggregate numbers accurate and up
		    to date.
		*/
		void SaveToGlobal(void);

	private:

		/** Inner version of the constructor.
		 */
		void Init(void);

		/** Looks up our thread-specific hit counter from the global list.
		 */
		static map<const char*, EnExProfile*>* FindOurHitCounter(void);

		/** Looks up our thread-specific stack trace from the global list.
		 */
		static vector<const char*>* FindOurStackTrace(void);

		const char* m_file;
		int m_line;
		const char* m_methodName;
		uint64_t m_methodEntryStamp;
		uint64_t m_methodExitStamp;
		bool m_saveToGlobal;
		EnExProfile* m_methodProfile;
		map<const char*, EnExProfile*>* m_hitCounter;
		vector<const char*>* m_stackTrace;
};

/** This is a mirror of the EnterExit class, but it does nothing.  We use a define to swap between these
  * two classes so that we can identify the amount of overhead that profiling adds to standard execution
  * time.
  */
class EnterExitLight {
	public:
		EnterExitLight(const char* methodName) {}
		EnterExitLight(const char* methodName, bool saveToGlobal) {}
		EnterExitLight(const char* file, int line, const char* methodName) {}
		EnterExitLight(const char* file, int line, const char* methodName, bool saveToGlobal){}


		virtual ~EnterExitLight() {}

		static void PrintHitMap(void) {printf("Compiled with light version.  Doing no profiling.\n");}
		static void PrintGlobalHitMap(void) {printf("Compiled with light version.  Doing no profiling.\n");}
		static void PrintGlobalHitMap(twine& output){};
		static void RecordGlobalHitMap(xmlNodePtr node) {}
		static void PrintStackTrace(void){}
		static void PrintStackTrace(int channel){}
		static twine GetStackTrace(void) {return twine("");}
		void SaveToGlobal(void) {}

};

#define EnEx	EnterExit
//#define EnEx	EnterExitLight

} // End SLib namespace

#endif // ENEX_H Defined
