
#include "cpp_concurrency_code.h"






class spinlock_mtx
{
	std::atomic_flag flg = ATOMIC_FLAG_INIT;
public:
	//spinlock_mtx() : flg(ATOMIC_FLAG_INIT) {}				// 拷贝/move 构造器 = delete
	//spinlock_mtx()
	//{
	//	//this->flg = ATOMIC_FLAG_INIT;			// 拷贝/move 赋值 = delete
	//}

	void lock()
	{
		while (flg.test_and_set(std::memory_order_acquire));
	}
	void unlock()
	{
		flg.clear(std::memory_order_release);
	}
};

void test_atomic_flag()
{
	std::atomic_flag f = ATOMIC_FLAG_INIT;
	cout << f.test_and_set() << endl;			// 0
	cout << f.test_and_set() << endl;			// 1

	spinlock_mtx mtx;

	cout << " ... 2" << endl;

	//int cnt = 0;
	//std::thread([&]() {
	//	cout << " thread . 1 \n";
	//	std::lock_guard<spinlock_mtx>(mtx);					// 不写 <> ，编译：推不出模板， 写了<xxx>编译：没有合适的默认构造器。
	//													// 把构造器全删了，(用编译器生成的构造器)  还是说 没有合适的默认构造器。

	//	cout << " got lock . 1 \n";
	//	int t2 = cnt + 1;
	//	std::this_thread::sleep_for(std::chrono::milliseconds(7));
	//	cnt = t2;
	//});

	//std::thread([&]() {
	//	cout << " thread - 2 \n";
	//	std::lock_guard<spinlock_mtx>(mtx);
	//	cout << " got lock - 2\n";
	//	int t2 = cnt + 1;
	//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//	cnt = t2;
	//});


	//cout << "at last, " << cnt << endl;

}





std::atomic<bool> extb(false);

void test_atomic_bool()
{
	std::atomic<bool> b(false);
	b = true;

	bool x = b.load(std::memory_order_acquire);
	cout << " load .. " << x << endl;
	b.store(false);
	x = b.exchange(false, std::memory_order_acq_rel);
	cout << " exchange .. " << x << endl;

	std::thread([]() { 
		cout << " inner thread start and to sleep ..Zzz \n";
		std::this_thread::sleep_for(std::chrono::microseconds(5));
		cout << " inner thread wake up. \n";
		
		bool exp = false;
		extb.compare_exchange_weak(exp, true);	// change from false to true
					// 第一个参数 是 ref， 第二个是 value。 所以第一个必须有 变量。
		cout << " inner thread set ok \n" << endl;
	}).detach();

	bool exp = true;
	cout << " ??? " << extb.load() << endl;
	//while (extb.compare_exchange_weak(exp, false))
	//	;

	while (true)
	{
		// 这段代码对我产生了极大的冲击。
		// 观测到的： {false}.compare_exchange_weak(true, false)。 这个方法返回了 false，但是 extb 被改成了 false
		// 不 expb 本来就是 false。
		// 所以 这个方法 把 excepted 改成 false 了。。？？？？
		//
		// 确实：
		// https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange
		// if those are bitwise-equal, replaces the former with desired (performs read-modify-write operation). 
		// Otherwise, loads the actual value stored in *this into expected (performs load operation). 
		//
		// expected 	- 	reference to the value expected to be found in the atomic object. 
		//					Gets stored with the actual value of *this if the comparison fails. 
		// 
		// Demonstrates how compare_exchange_strong 
		// either changes the value of the atomic variable 
		// or the variable used for comparison.
		//
		// 那我要这个 CAS 干什么？ 难道我不会自己 load？

		// 这个就不是CAS， 这个有什么用？

		// 要完成CAS，只能 while (true) { exp = true, compare_exchange_() };
		// 但是 不能保证 所以地方都是这种写法啊。
		
		// 书上的 b.compare_exchange_() && !excepted 大有深意。
		// 语序没有问题， && 是 左到右， 所以 excepted 是 经过 compare_exchange 后的

		// while(true) + if 是不正确的， 因为 exp=false的时候 就直接 success 了。
		// 不，要改下位置。 exp 放在 CAS成功后。


//。。换一种想法，这个 compare_exchange_ 修改 expected 也有一点点的道理。
//。。：场景：这个值 增加 10， ：  compare_exchange_(original, original + 10)。
//一次不行，就2次。
//
//。。CAS 感觉 更多的是 想 作为 锁 / 乐观锁 。
		// 就是看 original 的值 是否重要。


		std::this_thread::yield;

		cout << " .. " << std::boolalpha << exp << " vs " << extb.load() << endl;
		if (
			
			//exp &&
			
			! extb.compare_exchange_weak(exp, false,
			std::memory_order_release,
			std::memory_order_relaxed)
			
			
			
			)
		{
			cout << " 0000 fail \n";
		}
		else
		{
			if (!exp)
				continue;

			cout << " 111 success \n";
			break;
		}
	}

	cout << " CAS successfully \n";
	cout << " ??? " << extb.load() << endl;



	cout << "\n\n =====   test strong =========== \n\n\n";
	extb.store(false);
	exp = true;
	extb.compare_exchange_strong(exp, true);

	cout << " CAS strong complete \n";
	cout << exp << " - " << extb.load() << endl;			// exp has been changed to false...

	cout << extb.is_lock_free() << endl;		// true
	
	//https://en.cppreference.com/w/cpp/atomic/atomic/is_lock_free
	// The C++ standard recommends(but does not require) that 
	// lock - free atomic operations are also address - free, 
	// that is, suitable for communication between processes using shared memory.
	//



}






class Foo {};
void test_atomic_pointer()
{
	Foo arr[5];

	std::atomic<Foo*> p(arr);				// 6

	Foo* x = p.fetch_add(2);		// 返回老值

	assert(x == arr);

	assert(p.load() == &arr[2]);

	x = (p -= 1);		// 返回新值

	assert(x == &arr[1]);
	assert(p.load() == &arr[1]);

	cout << "ok" << endl;

}





// 没有搞懂， 哪里将 cnt 设置为一个随机值的。。。
// -858993460表示一个没有初始化的值,即没有赋值的变量被输出了
// 应该是书上，代码后面的一段解释，但是太难看懂了
//thread 1 sleep -1
//thread 1 sleep -2
//thread 1 sleep -3
//after provider sleep
//? ? 9
//thread 2 sleep -858993460
//thread 1 sleep -858993461
//thread 2 sleep -858993462
//thread 1 sleep -858993463
//thread 2 sleep -858993464

std::vector<int> vi;
void test_publish_consume_queue()
{
	std::atomic<int> cnt = -1;

	vi.push_back(1);
	vi.clear();

	std::thread([&]() {
		while (true)
		{
			//cout << " thread 1\n";
			int idx = -123;
			if ((idx = cnt.fetch_sub(1
				//, std::memory_order_acquire
			)) <= 0)
			{
				cout << " thread 1 sleep " << idx << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(3));
			//cout << " thread 1 process " + vi[idx] + " - " + idx + " \n";
			cout << " thread 1 process " << " - " << idx << endl;
		}
	})
		//.join();			// 后续无法继续，死循环。
			.detach();

	std::thread([&]() {
		for (int i = 0; i < 10; ++i)
		{
			//cout << vi.size() << endl;
			//cout << i << endl;
			vi.push_back(i + 100);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
		cout << " after provider sleep \n";
		cnt.store(9
		//	, std::memory_order_release
		);
		cout << " ?? " << cnt.load() << endl;
	})
		//.detach();
		.join();				// 这个得join。 不然 push_back 到一半，vi销毁了。 consumer 中vi也可能被销毁。所以还是 static..不知道static行不行 ，感觉也不好，所以直接全局变量了

	std::thread([&]() {
		while (true)
		{
			//cout << " thread 2\n";
			int idx = -123;
			if ((idx = cnt.fetch_sub(1
				//, std::memory_order_acquire
			)) <= 0)
			{
				cout << " thread 2 sleep " << idx << endl;;
				std::this_thread::sleep_for(std::chrono::milliseconds(7));
				continue;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
			//cout << " thread 2 process " + vi[idx] + " - " + idx + "\n";
			cout << " thread 2 process " << idx << endl;
		}

	}).detach();
		//join();

}








//void write_x_then_y()
//{
//	x.store(true, std::memory_order_relaxed); // 1
//	std::atomic_thread_fence(std::memory_order_release); // 2
//	y.store(true, std::memory_order_relaxed); // 3
//}
// 内存屏障
























int main()
{

	//test_atomic_flag();

	//test_atomic_bool();

	//test_atomic_pointer();

	test_publish_consume_queue();





	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return 0;
}
