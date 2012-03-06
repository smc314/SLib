#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <queue>
#include <boost/thread/condition_variable.hpp>

template <typename Data>
class BlockingQueue
{
	private:
		std::queue<Data> the_queue;
		mutable boost::mutex the_mutex;
		boost::condition_variable the_condition_variable;

	public:
		void push(Data const& data)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			the_queue.push(data);
			lock.unlock();
			the_condition_variable.notify_all();
		}

		bool empty() const
		{
			boost::mutex::scoped_lock lock(the_mutex);
			return the_queue.empty();
		}

		Data& pop(void)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			while(the_queue.empty()) {
				the_condition_variable.wait(lock);
			}

			Data& ret = the_queue.front();
			the_queue.pop();
			return ret;
		}

		int size() const
		{
			boost::mutex::scoped_lock lock(the_mutex);
			return (int)the_queue.size();
		}

};

#endif // BLOCKINGQUEUE_H Defined
