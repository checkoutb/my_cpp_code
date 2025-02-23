
#include "cpp_concurrency_code.h"
#include <exception>
#include <stack>
#include <queue>

struct empty_stack : std::exception
{
	const char* what() const throw();
};

template<typename T>
class ts_stack
{
private:
	std::stack<T> stk;
	mutable std::mutex mtx;

public:
	ts_stack() {}
	ts_stack(const ts_stack& other)
	{
		std::lock_guard<std::mutex> lock(other.mtx);
		stk = other.stk;
	}

	ts_stack& operator=(const ts_stack&) = delete;

	void push(T val)
	{
		std::lock_guard<std::mutex> lock(mtx);
		stk.push(std::move(val));
	}

	std::shared_ptr<T> pop()
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (stk.empty())
			throw empty_stack();

		std::shared_ptr<T> const res(std::make_shared<T>(std::move(stk.top())));
		stk.pop();
		return res;
	}

	void pop(T& val)
	{
		std::lock_guard<std::mutex> lk(mtx);
		if (stk.empty())
			throw empty_stack();

		val = std::move(stk.top());
		stk.pop();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mtx);
		return stk.empty();
	}

};



template<typename T>
class ts_queue
{
private:
	mutable std::mutex mtx;
	std::queue<T> que;
	std::condition_variable cv;

public:
	ts_queue() {}

	void push(T val)
	{
		std::lock_guard<std::mutex> lk(mtx);
		que.push(std::move(val));
		cv.notify_one();
	}

	void wait_and_pop(T& val)
	{
		std::unique_lock<std::mutex> lk(mtx);
		cv.wait(lk, [this]
		//() 
			{ return !que.empty(); });
		val = std::move(que.front());
		que.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mtx);
		cv.wait(lk, [this] { return !que.empty(); });
		std::shared_ptr<T> res(std::make_shared<T>(std::move(que.front())));
		que.pop();
		return res;
	}

	bool try_pop(T& val)
	{
		std::lock_guard<std::mutex> lk(mtx);
		if (que.empty())
			return false;
		val = std::move(que.front());
		que.pop();
		return true;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk(mtx);
		if (que.empty())
			return std::shared_ptr<T>();

		std::shared_ptr<T> res(std::make_shared<T>(std::move(que.front())));
		que.pop();
		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mtx);
		return que.empty();
	}
};













int main()
{




	std::this_thread::sleep_for(std::chrono::seconds(1));
	return 0;
}
