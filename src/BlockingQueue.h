#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <windows.h>

// std::queue
#include <queue>

template <typename Data>
class BlockingQueue
{
	private:
		std::queue<Data> the_queue;
		//mutable boost::mutex the_mutex;
		//boost::condition_variable the_condition_variable;

		mutable CRITICAL_SECTION CritSection;
		mutable CONDITION_VARIABLE ConditionVar;

	public:
		BlockingQueue()
		{
			InitializeCriticalSection( &CritSection );
			InitializeConditionVariable( &ConditionVar );
		}

		virtual ~BlockingQueue()
		{
			DeleteCriticalSection( &CritSection );
		}

		void push(Data const& data)
		{
			//boost::mutex::scoped_lock lock(the_mutex);
			EnterCriticalSection( &CritSection );
			the_queue.push(data);
			//lock.unlock();
			LeaveCriticalSection( &CritSection );
			//the_condition_variable.notify_all();
			WakeConditionVariable( &ConditionVar );
		}

		bool empty() const
		{
			bool ret;
			EnterCriticalSection( &CritSection );
			ret = the_queue.empty();
			LeaveCriticalSection( &CritSection );
			return ret;	

			//boost::mutex::scoped_lock lock(the_mutex);
			//return the_queue.empty();
		}

		Data& pop(void)
		{
			//boost::mutex::scoped_lock lock(the_mutex);
			EnterCriticalSection( &CritSection );
			while(the_queue.empty()) {
				//the_condition_variable.wait(lock);
				SleepConditionVariableCS( &ConditionVar, &CritSection, INFINITE );
			}

			Data& ret = the_queue.front();
			the_queue.pop();

			LeaveCriticalSection( &CritSection );
			return ret;
		}

		int size() const
		{
			//boost::mutex::scoped_lock lock(the_mutex);
			int ret;
			EnterCriticalSection( &CritSection );
			ret = (int)the_queue.size();
			LeaveCriticalSection( &CritSection );
			return ret;
		}

};

#endif // BLOCKINGQUEUE_H Defined
