// cpp_concurrency_code.cpp: 定义应用程序的入口点。
//

#include "cpp_concurrency_code.h"

using namespace std;


void hithread();	// ch1

class MyTask		// ch2
{
	void task1() const				// operator()() 是 const。。所以这里也是 const。
	{
		std::cout << "this is task11111" << endl;
	}
	void task2() const
	{
		std::cout << "this is task22222" << endl;
	}
public:
	void operator()() const
	{
		task1();
		cout << " ------- ----" << endl;
		task2();
	}
};
class MyTask;		// 这个前置声明无效，应该是由于 MyTask mt 生成时，必须要有定义。如果只是涉及指针， ref，那么是可以的。

class thread_guard
{
	std::thread& t;
public:
	explicit thread_guard(std::thread& t_) : t(t_) {}
	~thread_guard()
	{
		cout << "~~~~~~~ thread_guard\n";
		if (t.joinable())
		{
			cout << "join thread guard.\n";
			t.join();
		}
	}
	thread_guard(thread_guard const&) = delete;
	thread_guard& operator=(thread_guard const&) = delete;
};

void alongwork();
int caladd(int a, int b);

struct MyData
{
	int a;
	string s;

	MyData(int _a, string _s) : a(_a), s(_s) {}
//	string to_string(MyData& md) { return to_string(a) + " & " + s; }
};

string to_string(MyData& md) { return to_string(md.a) + " & " + md.s; }

void chgMyData(MyData& md);

void print2a(int a, string& name);

void print2a2();

void print2a3(int a, int name);

int main()
{
	std::thread t(hithread);
	t.join();		// 阻塞线程，  detach，线程分离，无法再交互，通常不用。
					// std::this_thread::sleep_for(std::chrono::milliseconds(1000));





	MyTask mt;
	std::thread t2(mt);		// std::thread my_thread(background_task()); 传递临时变量，被认为是 函数声明
						//std::thread my_thread((background_task())); // 1
						//std::thread my_thread{ background_task() }; // 2
						// 多组括号， 或 新的初始化语法。  或 传递命名变量
						// 使用lambda表达式也能避免这个问题

	t2.join();






	std::thread t3([] {					// ...这个lambda 没有 ()， 但是可以。
		cout << "thread lambda" << endl;
		});
	t3.join();






	cout << " === will test thread_guard. RAII " << endl;
	std::thread t4(alongwork);
	thread_guard tg(t4);
	// 由于多线程，所以这里的 cout 并不准。
	cout << " === end of thread_guard. RAII" << endl;

	// 换行，不能用 endl， 多线程的情况下， endl 和 前面的 string 并不是 原子的。。

	std::thread t5(caladd, 5, 2);		// 传参
	t5.join();







	MyData md(33333, "aaaaaa");
	cout << to_string(md) << endl;
	thread t6(chgMyData, std::ref(md));				// 这里强制 std::ref， 不然编译时 无法 实例化模板。
	t6.join();
	cout << to_string(md) << endl;





	thread t21(hithread);
	thread t22 = std::move(t21);
	t21 = std::thread(alongwork);
	std::thread t23;
	t23 = std::move(t22);
	//t21 = std::move(t23);				// runtime exception
	t23.join();
	t21.join();



	std::vector<std::thread> vs;
	for (int i = 0; i < 3; ++i)
	{
		//vs.push_back(std::thread(print2a, 5, "Thread_" + to_string(i)));
		//string s = "a";
		//std::thread tz(print2a, 5, s);			// 无法理解 为什么这个不行。 报错：模板只需要一个参数，但是提供了3 个。
		//vs.push_back(tz);
		//std::thread t5(caladd, 5, 2);

		//std::thread tz(print2a3, 5, 5);  // 这个可以， 看来是， 参数必须 同一类型？？？

		//vs.push_back(std::thread(caladd, 5, i));
		vs.push_back(std::thread(print2a3, 3, i));
	}
	std::for_each(std::begin(vs), end(vs), std::mem_fn(&std::thread::join));





	// 12 ?
	// 6核 12线程。 所以 不是 CPU核心数。  是CPU线程数。
	cout << " my  yingjian  cpu hexin shuliang : " + to_string(std::thread::hardware_concurrency()) << endl;
	// 无法获取信息时，返回0.









	cout << "Hello CMake." << endl;
	return 0;
}


//因为每个线程都必须具有一个初始函
//数(initial function)，新线程的执行从这里开始。对于应用程序来说，初始线程是main()，但是
//对于其他线程，可以在 std::thread 对象的构造函数中指定
void hithread()
{
	//for (int i = 0; i < 1000; ++i)
	//	;

	std::cout << "Hi concurrency\n";
}

void alongwork()
{
	cout << "start to long work\n";
	//for (int i = 1; i < 12345;)
	//	i = i + 1;
	std::cout << "end of long work\n";
}

int caladd(int a, int b)
{
	cout << "     got " + to_string(a) + ", " + to_string(b) + "\n";
	return a + b;
}

void chgMyData(MyData& md)
{
	md.a = 123123;
	md.s = "zzzzzz";
	cout << to_string(md) << endl;
}

void print2a(int a, string name)
{
	for (int i = 1; i < a; ++i)
		cout << "This is thread: " + name + ", value: " + to_string(i) + "\n";
}


void print2a3(int a, int name)
{
	//cout << std::thread::id << endl;
	cout << std::this_thread::get_id() << endl;
	for (int i = 1; i < a; ++i)
		cout << "This is thread: " + to_string(name) + ", value: " + to_string(i) + "\n";
}

void print2a2()
{
	for (int i = 1; i < 5; ++i)
		cout << "This is thread: no-name, value: " + to_string(i) + "\n";
}