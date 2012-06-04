#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#ifdef _WIN32
#include <windows.h>
#else
#include "Mutex.h"
#include "Lock.h"
#endif

// std::queue
#include <queue>

namespace SLib {

template <typename Data>
class BlockingQueue
{
	private:
		std::queue<Data> the_queue;

#ifdef _WIN32
		mutable CRITICAL_SECTION CritSection;
		mutable CONDITION_VARIABLE ConditionVar;
#else
		mutable Mutex the_mutex;
		pthread_cond_t the_condition_variable;
#endif

	public:
		BlockingQueue()
		{
#ifdef _WIN32
			InitializeCriticalSection( &CritSection );
			InitializeConditionVariable( &ConditionVar );
#else
			pthread_cond_init(&the_condition_variable, NULL);
#endif
		}

		virtual ~BlockingQueue()
		{
#ifdef _WIN32
			DeleteCriticalSection( &CritSection );
#else
			pthread_cond_destroy(&the_condition_variable);
#endif
		}

		void push(Data const& data)
		{
#ifdef _WIN32
			EnterCriticalSection( &CritSection );
#else
			Lock the_lock(&the_mutex);
#endif
			the_queue.push(data);

#ifdef _WIN32
			LeaveCriticalSection( &CritSection );
#else
			the_lock.UnLock();
#endif

#ifdef _WIN32
			WakeConditionVariable( &ConditionVar );
#else
			pthread_cond_signal( &the_condition_variable );
#endif
		}

		bool empty() const
		{
#ifdef _WIN32
			bool ret;
			EnterCriticalSection( &CritSection );
			ret = the_queue.empty();
			LeaveCriticalSection( &CritSection );
			return ret;	
#else
			Lock the_lock(&the_mutex);
			return the_queue.empty();
#endif
		}

		Data& pop(void)
		{
#ifdef _WIN32
			EnterCriticalSection( &CritSection );
#else
			Lock the_lock(&the_mutex);
#endif
			while(the_queue.empty()) {
#ifdef _WIN32
				SleepConditionVariableCS( &ConditionVar, &CritSection, INFINITE );
#else
				pthread_cond_wait(&the_condition_variable, the_mutex.internalMutex() );
#endif
			}

			Data& ret = the_queue.front();
			the_queue.pop();

#ifdef _WIN32
			LeaveCriticalSection( &CritSection );
#endif

			return ret;
		}

		int size() const
		{
#ifdef _WIN32
			int ret;
			EnterCriticalSection( &CritSection );
			ret = (int)the_queue.size();
			LeaveCriticalSection( &CritSection );
			return ret;
#else
			Lock the_lock(&the_mutex);
			return (int)the_queue.size();
#endif
		}

};

} // End Namespace

#endif // BLOCKINGQUEUE_H Defined
