
#include "cpp_concurrency_code.h"




template<typename Iter, typename T>
struct accumulate_block
{
	void operator()(Iter fst, Iter lst, T& result)
	{
		result = std::accumulate(fst, lst, result);
	}
};

template<typename Iter, typename T>
T parallel_accumulate(Iter fst, Iter lst, T init)
{
	unsigned long const length = std::distance(fst, lst);
	if (!length)
		return init;

	// 每根线程最少处理25个元素
	unsigned long const min_per_thread = 25;
	// 每根线程处理25个元素的情况下，线程数量
	unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
	// 硬件支持的数量
	unsigned long const hardware_threads = std::thread::hardware_concurrency();
	// 上面2个线程数量的 min
	// 但是我，感觉硬件支持的数量需要 - 2， 毕竟OS，其他软件也需要线程啊。
	unsigned long const num_threads = std::min(hardware_threads == 0 ? 2 : hardware_threads, max_threads);
	// 每个线程的元素个数
	unsigned long const block_size = length / num_threads;

	std::vector<T> results(num_threads);
	std::vector<std::thread> threads(num_threads - 1);		// 一个线程是自己，所以不需要计入

	Iter st = fst;
	for (unsigned long i = 0ul; i < (num_threads - 1); ++i)
	{
		Iter en = st;
		std::advance(en, block_size);

		threads[i] = std::thread(accumulate_block<Iter, T>(), st, en, std::ref(results[i]));
		st = en;
	}

	// 不写 <Iter, T>， 无法推导
	accumulate_block<Iter, T>()(st, lst, results[num_threads - 1]);		// result 有 num_threads 个元素，
														// threads 只有 num_threads - 1 个元素。

	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	return std::accumulate(results.begin(), results.end(), init);
}


template<typename Iter, typename T>
struct accu_block
{
	T operator()(Iter fst, Iter lst)
	{
		return std::accumulate(fst, lst, T());
	}
};

template<typename Iter, typename T>
T parallel_accu(Iter fst, Iter lst, T init)
{
	size_t const length = std::distance(fst, lst);
	if (!length)
		return init;

	size_t const min_per_thread = 25;
	size_t const max_threads = (length + min_per_thread - 1) / min_per_thread;

	const size_t hd_threads = std::thread::hardware_concurrency();
	const size_t num_threads = std::min(hd_threads == 0 ? 2 : hd_threads, max_threads);

	const size_t block_size = length / num_threads;

	std::vector<std::future<T>> futures(num_threads - 1);
	std::vector<std::thread> threads(num_threads - 1);

	Iter st = fst;
	for (size_t i = 0; i < (num_threads - 1); ++i)
	{
		Iter en = st;
		std::advance(en, block_size);
		std::packaged_task<T(Iter, Iter)> tsk(std::move(accu_block<Iter, T>()));
		futures[i] = tsk.get_future();
		threads[i] = std::thread(std::move(tsk), st, en);
		st = en;
	}
	T lst_res = accu_block<Iter, T>()(st, lst);

	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	T result = init;
	for (size_t i = 0; i < (num_threads - 1); ++i)
	{
		result += futures[i].get();
	}
	result += lst_res;
	return result;
}


class join_threads
{
	std::vector<std::thread>& threads;
public:
	explicit join_threads(std::vector<std::thread>& threads_) :threads(threads_) {}

	~join_threads()
	{
		for (size_t i = 0; i < threads.size(); ++i)
		{
			if (threads[i].joinable())
				threads[i].join();
		}
	}
};



template<typename Iter, typename Func>
void parallel_for_each(Iter fst, Iter lst, Func f)
{
	const size_t sz1 = std::distance(fst, lst);
	if (!sz1)
		return;

	const size_t min_per_th = 25;
	const size_t mx_th = (sz1 + min_per_th - 1) / min_per_th;
	const size_t hd_th = std::thread::hardware_concurrency();
	const size_t num_th = std::min(hd_th == 0 ? 2 : hd_th, mx_th);
	const size_t block_size = sz1 / num_th;

	std::vector<std::future<void>> futures(num_th - 1);
	std::vector<std::thread> threads(num_th - 1);
	join_threads joiner(threads);

	Iter st = fst;
	for (size_t i = 0; i < (num_th - 1); ++i)
	{
		Iter en = st;
		std::advance(en, block_size);
		std::packaged_task<void(void)> tsk([=]() {std::for_each(st, en, f); });
		futures[i] = tsk.get_future();
		threads[i] = std::thread(std::move(tsk));
		st = en;
	}

	std::for_each(st, lst, f);
	for (size_t i = 0; i < (num_th - 1); ++i)
	{
		futures[i].get();
	}
}




template<typename Iter, typename Func>
void parallel_for_each_async(Iter fst, Iter lst, Func f)
{
	const size_t sz1 = std::distance(fst, lst);
	if (!sz1)
		return;

	const size_t min_per_th = 25;
	if (sz1 < (2 * min_per_th))
	{
		std::for_each(fst, lst, f);
	}
	else
	{
		const Iter md = fst + sz1 / 2;
		std::future<void> fst_half = std::async(&parallel_for_each_async<Iter, Func>, fst, md, f);
		parallel_for_each_async(md, lst, f);
		fst_half.get();
	}
}






// 这里的 atomic<bool> 是否有必要？ 感觉可以直接 bool的， 只有一次写入的机会。不会 冲突的。
// 会不会放在 cache 中，导致一直 false？

template<typename Iter, typename MatchT>
Iter parallel_find(Iter fst, Iter lst, MatchT match)
{
	struct find_ele
	{
		void operator()(Iter st, Iter en, MatchT match, std::promise<Iter>* result, std::atomic<bool>* done)
		{
			try
			{
				for (; (st != en) && !done->load(); ++st)
				{
					if (*st == match)
					{
						result->set_value(st);
						done->store(true);
						return;
					}
				}
			}
			catch (...)
			{
				try
				{
					result->set_exception(std::current_exception());
					done->store(true);
				}
				catch(...)
				{ }
			}
		}
	};

	const size_t sz1 = std::distance(fst, lst);
	if (!sz1)
		return lst;

	const size_t min_per_th = 25;
	const size_t mx_th = (sz1 + min_per_th - 1) / min_per_th;
	const size_t hw_th = std::thread::hardware_concurrency();
	const size_t num_th = std::min(hw_th ? hw_th : 2, mx_th);
	const size_t blk_sz = sz1 / num_th;

	std::promise<Iter> result;
	std::atomic<bool> done(false);
	std::vector<std::thread> threads(num_th - 1);

	{
		join_threads joiner(threads);
		Iter st = fst;
		for (size_t i = 0; i < (num_th - 1); ++i)
		{
			Iter en = st;
			std::advance(en, blk_sz);
			threads[i] = std::thread(find_ele(), st, en, match, &result, &done);
			st = en;
		}
		find_ele()(st, lst, match, &result, &done);
	}

	if (!done.load())		// 这个可能在 find 没有执行完 的时候执行吗？
							// 不会， 上面的 {} 这个scope中的 join_threads 确保了 线程执行完。
		return lst;

	return result.get_future().get();
}




template<typename Iter, typename Need>
Iter parallel_find_async_impl(Iter fst, Iter lst, Need need, std::atomic<bool>& done)
{
	try
	{
		const size_t length = std::distance(fst, lst);
		const size_t min_per_th = 25;
		if (length < (2 * min_per_th))
		{
			for (; (fst != lst) && !done.load(); ++fst)
			{
				if (*fst == need)
				{
					done = true;
					return fst;
				}
			}
			return lst;
		}
		else
		{
			const Iter md = fst + length / 2;
			std::future<Iter> res = std::async(&parallel_find_async_impl<Iter, Need>, md, lst, need, std::ref(done));
			const Iter res2 = parallel_find_async_impl(fst, md, need, done);

			return (res2 == md) ? res.get() : res2;
		}
	}
	catch (...)
	{
		done = true;
		throw;
	}
}

template<typename Iter, typename Need>
Iter parallel_find_async(Iter fst, Iter lst, Need need)
{
	std::atomic<bool> done(false);
	return parallel_find_async_impl(fst, lst, need, done);
}








int main()
{
	//cout << INT_MAX << " - " << SIZE_MAX << endl;		// 31bit,  64bit
	//cout << LONG_MAX << " __ " << LLONG_MAX << endl;		// 31bit, 63bit

	std::vector<int> vi(500);
	for (int i = 0; i < 500; ++i)
		vi[i] = i + 1;

	//cout << parallel_accumulate(vi.begin(), vi.end(), 0) << endl;
	//std::cout << parallel_accu(vi.begin(), vi.end(), 1) << endl;

			// for_each 不是 transform
	//parallel_for_each(vi.begin(), vi.end(), [](int a)->int { return a * 2; });
	//parallel_for_each(vi.begin(), vi.end(), [](int a) -> void { cout << a << " "; });

	//for (int i : vi)
	//	cout << i << ", ";
	//cout << endl;
	cout << "\n\n===================\n\n";
	//parallel_for_each_async(vi.begin(), vi.end(), [](int a)->void {cout << a << ","; });


	//cout << (parallel_find(vi.begin(), vi.end(), 234) != vi.end()) << endl;
	//cout << (parallel_find(vi.begin(), vi.end(), 1234) != vi.end()) << endl;

	cout << (vi.end() != parallel_find_async(vi.begin(), vi.end(), 234)) << endl;
	cout << (vi.end() != parallel_find_async(vi.begin(), vi.end(), 1111)) << endl;







	//cout << std::thread::hardware_concurrency() << endl;		// cpp_concurrency_code.cpp 中也有
	return 0;
}
