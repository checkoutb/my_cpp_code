
#include "cpp_concurrency_code.h"




std::mutex mtx;
std::condition_variable cv;

					// 这个 配合 [] ， 下面的报错 是 配合 [&flg] 的报错
bool flg = false;			// “flg”: 简单捕获必须是含自动存储持续时间(在 lambda 的访问范围中声明)的变量
void test1()
{
//	bool flg = false;		// ok .. 这个配合  [&flg] 的。		无法配合 []，[] 会报错： 函数 无法访问 flg 
	std::thread(
	//	[&flg]
		[]
		() { 
		cout << "1  start to wait cv ... \n";
		//std::lock_guard<std::mutex> lk(mtx);
		std::unique_lock<std::mutex> lk(mtx);						// cv 内部需要 加解锁， 所以 不能 lock_guard
		
		cv.wait(lk, 
			//[&flg]
			[]
			() { cout << "     i wake up !\n"; return flg; });

		cout << "5  -- cv is ok\n" << endl;

		lk.unlock();
	}).detach();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	cout << "2 after sleep 100 ms\n";
	
	cv.notify_one();
	cout << "3 --- after  fake   notify ---- 1\n";
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	{
		std::lock_guard<std::mutex> lk(mtx);
		flg = true;
		cout << "4 -- start to    really  notify \n";
		cv.notify_one();
	}


}




struct X
{
	int a;
	X(int _a) : a(_a) {}
	void print(int b)
	{
		cout << " this is x.print " << (a + b) << endl;
	}
};

void testFuture()
{
	std::future<int> ans = std::async([] {				// 或者 [] ()->int {} ..    []->int {} 是错误的。
		cout << "sleep ... \n";
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		cout << " wake up...\n";
		return 123666;
	});
	cout << " just do sth other ..\n";
	cout << " ans is : " << ans.get() << endl;			// 阻塞的是 ans.get， 所以 "ans is:" 在 sleep 之前打印。




	ans = std::async(std::launch::deferred | std::launch::async, [](int a) {
		return a + 100000;
		}, 
		666			// 传递参数， 作为 lambda 的 形参a。
	);
	cout << " 2 ans : " << ans.get() << endl;



	//ans = std::async(std::launch::deferred, &X::print, X(1), 2);			// ans is future<int>
	//cout << " 3 ans: " << ans.get() << endl;




	std::future<void> ans2 = std::async(std::launch::deferred, &X::print, X(1), 2);
	ans2.get();			// 不写这行， future不干活。
			// 也可以 ans2.wait()

}



// 在线程间 传递 packaged_task。
// std::deque<std::packaged_task<void()> > tasks;

void test_packaged_task()
{
	std::packaged_task<int(int)> tsk([](int a) -> int {
		cout << "3 this is packaged_task ... \n";
		return a + 10;
	});
	cout << "1 packaged task\n";
	std::future<int> ans = tsk.get_future();
	cout << "2 packaged task\n";
	tsk(5);

	cout << ans.get() << endl;
}



struct accu
{
	int operator()(int a, int b)
	{
		return a + b;
	}
};

void test_packaged_task_invoke_noname_fn()
{
	std::packaged_task<int(int, int)> tsk(std::move(accu()));			// 必须加 move。 但是函数返回值 不就当做 右值计算的吗。
	std::future<int> ans = tsk.get_future();
	tsk(5, 2);
	cout << ans.get() << endl;
}





void test_promise_future()
{
	//std::promise<int> prm([]() { cout << " this is promise \n"; return 654; });
	std::promise<int> prm;

	std::future<int> ans = prm.get_future();
	
	std::async(std::launch::async, [&prm]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
		cout << " after ..sleep 200ms \n";
		prm.set_value(23); 

		// if exception: prm.set_exception(std::current_exception());

	}).wait();					// . 这里是 async 返回了一个 future，调用 future.get 或 wait。 会阻塞在这里。 这里完成后，才继续。

	cout << " this is .." << endl;

	cout << " ----" << endl;
	cout<<ans.get();

}




double sqrt2(double x)
{
	if (x < 0)
		throw std::out_of_range(" x < 0 ; ");

	return x + 10;
}

void test_exception()
{
	//sqrt2(-1);

	std::future<double> f = std::async(sqrt2, -2);

	try 
	{
		double a = f.get();
		cout << a << endl;
	}
	catch (...)
	{
		cout << "errrrrrrr \n";
	}


}






void test_shared_future()
{
	std::promise<int> p;
	std::future<int> f(p.get_future());
	assert(f.valid());
	cout << f.valid() << endl;					// 1
	std::shared_future<int> sf(std::move(f));
	assert(!f.valid());
	assert(sf.valid());
	cout << f.valid() << " - " << sf.valid() << endl;		// 0 - 1
	std::shared_future<int> sf2(sf);
	cout << sf2.valid() << endl;			// 1

	cout << &sf << " - " << &sf2 << endl;		// 不同地址。

	//auto sf3 = f.share();		// exception. 应该是无效后，就不能 share() 了。

	p.set_value(23);

	cout << sf.get() << " - " << sf2.get() << endl;		// 23 - 23


	//cout << sf3.get() << endl;

}





std::ostream& operator<<(std::ostream& ostr, const std::list<int>& list)
{
	for (auto& i : list)
		ostr << ' ' << i;

	return ostr;
}

template<typename T>
std::list<T> test_quick_sort_sequence(std::list<T> input)
{
	if (input.empty())
		return input;

	std::list<T> result;

	// 把 input 的元素 插入到 result的begin 之前。 不不不，不是 复制-插入，  而是 移动。
	result.splice(result.begin(), input, input.begin());		// splice(): moves elements from another list
	// 。。。。。。。 上面这个 只移动 一个 元素， 就是 input.begin() 这个元素。。。。

	T const& pivot = *result.begin();

//	cout << "1  " << result << endl;
//	cout << "2  " << input << endl;

	auto divide = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot;  });

	std::list<T> lower_part;
	// 二分后的 前半段，
	lower_part.splice(lower_part.end(), input, input.begin(), divide);

//	cout << "3  " << lower_part << endl;

	auto new_lower(test_quick_sort_sequence(std::move(lower_part)));
	auto new_higher(test_quick_sort_sequence(std::move(input)));

	result.splice(result.end(), new_higher);
	result.splice(result.begin(), new_lower);

	return result;
}

template<typename T>
std::list<T> quick_sort_parallel(std::list<T> input)
{
	if (input.empty())
		return input;

	std::list<T> result;
	result.splice(result.begin(), input, input.begin());
	
	T const& pivot = *result.begin();

	auto divide = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });

	std::list<T> lowp;
	lowp.splice(lowp.end(), input, input.begin(), divide);


	
	std::future<std::list<T>> new_lower(std::async(&quick_sort_parallel<T>, std::move(lowp)));



	auto new_higher(quick_sort_parallel(std::move(input)));

	result.splice(result.end(), new_higher);
	result.splice(result.begin(), new_lower.get());

	return result;

}








int main()
{
	//test1();

	//testFuture();

	// test_packaged_task     这个有点少。写不出。
		// 将 函数 封装到 packaged_task 中，然后  () 调用函数， get_future 获得 future。
	// IOC 啊， 之前 要自己 调用函数， 现在 调用 packaged_task， 不，还是得调用。 算 是一种封装，统一。

	//test_packaged_task();

	//test_promise_future();

	//test_exception();

	//test_shared_future();

	//std::list<int> lst = { 7,6,5,4,8,9,1,2 };
	//std::list<int> lst2 = test_quick_sort_sequence(lst);
	//std::list<int> lst2 = quick_sort_parallel(lst);
	//cout << "after sort : " << lst2 << endl;


	test_packaged_task_invoke_noname_fn();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	return 0;
}
