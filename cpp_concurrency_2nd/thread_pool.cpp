
#include "cpp_concurrency_code.h"



class thread_pool
{
	std::atomic_bool done;
	thread_safe_queue<std::function<void()>> work_queue;
	std::vector<std::thread> threads;
	join_threads joiner;

	void worker_thread()
	{
		while (!done)
		{
			std::function<void()> tsk;
			if (work_queue.try_pop(tsk))
			{
				tsk();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

public:
	thread_pool() : done(false), joiner(threads)
	{
		const size_t hw_th = std::thread::hardware_concurrency();

		try
		{
			for (size_t i = 0; i < hw_th; ++i)
			{
				threads.push_back(std::thread(&thread_pool::worker_thread, this));
			}
		}
		catch (...)
		{
			done = true;
			throw;
		}
	}

	~thread_pool()
	{
		done = true;
	}

	template<typename FuncType>
	void submit(FuncType f)
	{
		work_queue.push(std::function<void()>(f));
	}
};







class function_wrapper
{
	struct impl_base
	{
		virtual void call() = 0;
		virtual ~impl_base() {}
	};

	std::unique_ptr<impl_base> impl;

	template<typename F>
	struct impl_type : impl_base
	{
		F f;
		impl_type(F&& _f) : f(std::move(_f)) {}
		void call() { f(); }
	};

public:
	template<typename F>
	function_wrapper(F&& f) : impl(new impl_type<F>(std::move(f))) {}
	
	void operator()() { impl->call(); }
	
	function_wrapper() = default;

	function_wrapper(function_wrapper&& other) : impl(std::move(other.impl)) {}

	function_wrapper& operator=(function_wrapper&& other)
	{
		impl = std::move(other.impl);
		return *this;
	}

	function_wrapper(const function_wrapper&) = delete;
	function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator=(const function_wrapper&) = delete;
};

class thread_pool2
{
	thread_safe_queue<function_wrapper> work_queue;		// 不是 std::function

	void worker_thread()
	{
		while (!done)
		{
			function_wrapper tsk;
			if (work_queue.try_pop(tsk))
			{
				task();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

public:
	template<typename FuncType>
	std::future<typename std::result_of<FuncType()>::type> submit(FuncType f)
	{
		typedef typename std::result_of<FuncType()>::type result_type;

		std::packaged_task<result_type()> tsk(std::move(f));
		std::future<result_type> res(task.get_future());
		work_queue.push(std::move(tsk));
		return res;
	}
};






class thread_pool_3
{
	thread_safe_queue<function_wrapper> pool_work_queue;

	typedef std::queue<function_wrapper> local_queue_type;
	
	static thread_local std::unique_ptr<local_queue_type> local_work_queue;

	void worker_thread()
	{
		local_word_queue.reset(new local_queue_type);
		while (!done)
		{
			run_pending_task();
		}
	}

public:
	template<typename FuncType>
	std::future<typename std::result_of<FuncType()>::type> submit(FuncType f)
	{
		typedef typename std::result_of<FuncType()>::type result_type;

		std::packaged_task<result_type()> task(f);

		std::future<result_type> res(task.get_future());

		if (local_work_queue)
			local_work_queue->push(std::move(task));
		else
			pool_work_queue.push(std::move(task));

		return res;
	}

	void run_pending_task()
	{
		function_wrapper task;
		if (local_work_queue && !local_work_queue->empty())
		{
			task = std::move(local_work_queue->front());
			local_work_queue()->pop();
			task();
		}
		else if (pool_work_queue.try_pop(task))
			task();
		else
			std::this_thread::yield();
	}
};





class work_stealing_queue
{
private:
	typedef function_wrapper data_type;

	std::deque<data_type> the_queue;
	mutable std::mutex mtx;

public:
	work_stealing_queue() {}

	work_stealing_queue(const work_stealing_queue& other) = delete;

	void push(data_type data)
	{
		std::lock_guard<std::mutex> lock(mtx);
		the_queue.push_front(std::move(data));
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(mtx);
		return the_queue.empty();
	}

	bool try_pop(data_type& res)
	{
		std::lock_guard<std::mutex> lk(mtx);
		if (the_queue.empty())
			return false;

		res = std::move(the_queue.front());
		the_queue.pop_front();
		return true;
	}

	bool try_steal(data_type& res)
	{
		std::lock_guard<std::mutex> lk(mtx);
		if (the_queue.empty())
			return false;

		res = std::move(the_queue.back());
		the_queue.pop_back();
		return true;
	}

};

class thread_pool_6
{
	typedef function_wrapper task_type;

	std::atomic_bool done;
	thread_safe_queue<task_type> pool_work_queue;
	std::vector<std::unique_ptr<work_stealing_queue>> queues;
	std::vector<std::thread> threads;
	join_threads joiner;

	static thread_local work_stealing_queue* local_work_queue;
	static thread_local unsigned my_index;

	void worker_thread(unsigned my_index_)
	{
		my_index = my_index_;
		local_work_queue = queue[my_index].get();
		while (!done)
			run_pengding_task();
	}

	bool pop_task_from_local_queue(task_type& task)
	{
		return local_work_queue && local_work_queue->try_pop(task);
	}

	bool pop_task_from_pool_queue(task_type& task)
	{
		return pool_work_queue.try_pop(task);
	}

	bool pop_task_from_other_thread_queue(task_type& task)
	{
		for (size_t i = 0; i < queues.size(); ++i)
		{
			const unsigned idx = (my_index + i + 1) % queues.size();
			if (queueus[idx]->try_steal(task))
				return true;
		}
		return false;
	}

public:
	thread_pool() :done(false), joiner(threads)
	{
		size_t th_cnt = std::thread::hardware_concurrency();
		try
		{
			for (size_t i = 0; i < th_cnt; ++i)
			{
				queues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue));
				threads.push_back(std::thread(&thread_pool::worker_thread, this, i));
			}
		}
		catch (...)
		{
			done = true;
			throw;
		}
	}

	~thread_pool()
	{
		done = true;
	}

	template<typename FuncType>
	std::future<typename std::result_of<FuncType()>::type> submit(FuncType f)
	{
		typedef typename std::result_of<FuncType()>::type result_type;

		std::packaged_task<result_type()> task(f);
		std::future<result_type> res(task.get_future());
		if (local_work_queue)
			local_work_queue->push(std::move(task));
		else
			pool_work_queue.push(std::move(task));

		return res;
 	}

	void run_pending_task()
	{
		task_type task;

		if (pop_task_fron_local_queue(task) || pop_task_fron_pool_queue(task) || pop_task_from_other_thread_queue(task))
		{
			task();
		}
		else
		{
			std::this_thread::yield();
		}
	}
};







class interrupt_flag
{
public:
	void set();
	bool is_set() const;
};

thread_local interrupt_flag this_thread_interrupt_flag;

class interruptible_thread
{
	std::thread internal_thread;
	interrupt_flag* flag;
public:
	template<typename FuncType>
	interruptable_thread(FuncType f)
	{
		std::promise<interrupt_flag*> p;
		internal_thread = std::thread([f, &p] {
			p.set_value(&this_thread_interrupt_flag);
			f();
		});
		flag = p.get_future().get();
	}
	
	void interrupt()
	{
		if (flag)
			flag->set();
	}
};







int main()
{

	return 0;
}
