
#include "cpp_concurrency_code.h"



volatile int num = 0;
std::mutex mtx;

std::unique_lock<std::mutex> get_lock();

void add1()
{
	int t3 = num + 1;

	//int t2 = rand() % 5;
	//// ...不随机啊。每次都是 同一个随机数列。。
	////std::cout << "                will sleep " << t2 << std::endl;
	//std::this_thread::sleep_for(std::chrono::milliseconds(t2));

	std::this_thread::yield();		// 这里是上面的不行，上面的，不加互斥，是100， 试了几次都是100。。
				// 加了 volatile，  sleep 还是100.

	num = t3;
}

void increase(bool useMutex)
{
	if (useMutex)
	{
					// get_lock 中 lk 是 auto 变量， 不需要显式 std::move()
				// get_lock 类似网关，所有的访问都要经过 网关。 通过网关 来 控制 并发。 互斥的 管理中心。
		std::unique_lock<std::mutex> lk(get_lock());			// ok

		//std::lock_guard<std::mutex> g(mtx);		// ok


		//std::cout << "got :  " << std::this_thread::get_id() << std::endl;
		//++num;
		add1();
		//cout << "release : " << std::this_thread::get_id() << endl;
	}
	else
	{
		//++num;
		add1();
	}
}

void repeat(int cnt, bool mtx)
{
	for (int i = 0; i < cnt; ++i)
	{
		increase(mtx);
		
		//std::this_thread::yield();		// 效果没有

		//int t2 = rand() % 5;
		//// ...不随机啊。每次都是 同一个随机数列。。
		////std::cout << "                will sleep " << t2 << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(t2));
	}
}

class X
{
private:
	int val;
public:
	std::mutex m;

	X(int const& v) : val(v) { }
	friend void swap(X& lhs, X& rhs)
	{
		if (&lhs == &rhs)
			return;
		
		//std::lock(lhs.m, rhs.m);
		//std::lock_guard<std::mutex> lka(lhs.m, std::adopt_lock);
		//std::lock_guard<std::mutex> lkb(rhs.m, std::adopt_lock);
		



		// >= C++17
		//std::scoped_lock g(lhs.m, rhs.m);		// 等价于 std::scoped_lock<std::mutex,std::mutex> guard(lhs.m,rhs.m);



		//除非你想将 std::unique_lock 的所有权进行转让，或是
		//	对其做一些其他的事情外，你最好使用C++17中提供的 std::scoped_lock
		


		std::unique_lock<std::mutex> lka(lhs.m, std::defer_lock);
		std::unique_lock<std::mutex> lkb(rhs.m, std::defer_lock);		// 并未上锁
		cout << "begin to lock" << endl;
		std::lock(lka, lkb);		// 上锁

		std::swap(lhs.val, rhs.val);
	}
	int getVal() { return val; }
	//std::mutex getM() { return m; }  // mutex 没有复制构造函数。 不过这里应该是 移动构造。它也没有移动构造。
};








// move lock/mutex
std::unique_lock<std::mutex> get_lock()
{
	// 编译失败。找不到这个 外部符号。。
	//extern std::mutex amtx;				// 但是没有定义啊。不知道没有定义的情况下，是不是就是 static 单例
	
	//cout << "... " << &mtx << endl;			// 这个和 下面的 cout ，都是 同一个地址。

	extern std::mutex mtx;		// 这个可以。 但是估计没什么用， 因为 不写这个，也是能直接访问的。这个只是声明。 
								// 不，有用的， 之前 编译错误，找不到外部符号。。这里不会报错，说明 找到了一个。

	//cout << "amtx -> " << &mtx << endl;

	std::unique_lock<std::mutex> lk(mtx);
	return lk;
}







std::once_flag onceFlg;
void beCalledOnce()
{
	cout << "      beeen calllled " << endl;
}
void callOnce()
{
	std::call_once(onceFlg, beCalledOnce);
}





// shared_lock
//std::shared_lock<std::shared_mutex> get_shared_lock()
//{
//	static std::shared_mutex smtx;
//
//	return std::shared_lock<std::shared_mutex>(smtx);
//}
std::shared_mutex shmtx;
int a = 0;
void read_shared_lock()
{
	std::shared_lock<std::shared_mutex> slk(shmtx);
	cout << " shared_read " << a << " -- " << std::this_thread::get_id() << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 30));
	cout << "    end    of   shared read " << std::this_thread::get_id() << endl;
}
void write_shared_lock()
{
	std::lock_guard<std::shared_mutex> lk(shmtx);
	cout << " start to write shared_mtx-----------------------\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	cout << "     end of write shared_mtx\n";
}
void shared_lock()
{
	std::thread t2([]() { for (int i = 0; i < 10; ++i) read_shared_lock(); });
	t2.detach();
	
	std::thread t3([]() { for (int i = 0; i < 5; ++i) write_shared_lock(); });
	t3.detach();

	std::thread([]() {for (int i = 0; i < 10; ++i) read_shared_lock(); }).detach();
	
	// 要让 read write 交替，好像不太行。 估计是 偏向的， 读先获得锁，那么 后续，只要读请求不断，就一直是读。。
	// 也可能是 for 的次数太少。  毕竟上面会 饥饿。 应该会考虑到的。
	
	// 并发读，看到了。

}






int main()
{
	std::cout << std::time(NULL) << std::endl;
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	bool useMtx = true;

	cout << useMtx << endl;

	int cnt = 100;
	std::thread t1(repeat, cnt, useMtx);
	std::thread t2(repeat, cnt, useMtx);

	//t1.join();		// 线性执行，没有并发。
	//t2.join();		// 应该是 交替执行，但是是在 main线程上 交替，所以 main线程 就是锁。
					// bu， 看文档，就是 新建thread， 只不过 main 线程 会等待 这2个线程 结束
			// 但是 试下来 真的是 在main上运行啊。

	t1.detach();
	t2.detach();








	X a(1);
	X b(2);

	swap(a, b);

	cout << a.getVal() << " -- " << b.getVal() << endl;

	swap(a, a);				// swap(a, a)， 即 lock(a.m, a.m) 还是会死锁的。 需要 判断是否相等。
	cout << a.getVal() << endl;

	//std::lock_guard<std::mutex> g(b.getM());
	try 
	{
		std::lock_guard<std::mutex> g(b.m);
		swap(a, b);		// exception
	}
	catch (std::string* s)
	{
		cout << "exception.1 " << s << endl;
	}
	catch (...)
	{
		cout << "exception.2 " << endl;				// 走的这个。
	}








	for (int i = 0; i < 3; ++i)
	{
		cout << " start to call callOnce" << endl;
		callOnce();
	}






	shared_lock();







	std::this_thread::sleep_for(std::chrono::seconds(3));

	std::cout << num << std::endl;
	return 0;
}





