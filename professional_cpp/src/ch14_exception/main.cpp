
#include <stdexcept>
#include <iostream>
#include <exception>    // throw_with_nested, nested_exception, rethrow_if_nested, current_exception, exception_ptr
#include <cstdlib>

#include <source_location>

#include <format>

#include <cstddef>
#include <numeric>
#include <new>


int fun1(int a, int b) {
    if (b == 0)
        throw std::invalid_argument {"divide by 0"};
    return a / b;
}

int throwint() {
    throw 5;
    // return 1;
}

int throwconstchar() {
    throw "errrrrrrr";
}

int throwIntButNoExcept() noexcept {
    std::cout<<"will except in noexcept\n";
    int a = 5;
    if (a == 5)
        throw 5;        // warning: throw’ will always call ‘terminate’
                // 在noexcept 的方法中的 任意throw 都会触发 terminate。
    return 123;
}

constexpr bool useHardMode = false;

int throwIntExceptExpr() noexcept(useHardMode) {        // if useHardMode is true, warning: throw will call terminate
    throw 123;
}

int throwIntExceptWithUp() noexcept(noexcept(throwIntExceptExpr())) {
    throw 123;      // if useHardMode == true, warning
}

// throw list, 指定可抛出的异常， C++11 起不再推荐，C++17不再支持(除了 throw() (。。估计是真的 () 里面没有东西的)，throw() 和 noexcept 等效 )，C++20完全不支持throw()

// 417的异常类的继承体系
// 顶层的 exception 类有 virtual 的 what 方法
// 第二层是 logic_error, runtime_error, bad_xxx   bad_xxx的直接继承 exception的。 没有一个 bad_xxx的总类
// logic_error下面：domain_error,length_error,invalid_argument,out_of_range,future_error
// runtime_error下面：range_error,overflow_error,underflow_error,regex_error,format_error,system_error.
// bad_xxx 有：bad_typeid, bad_cast,bad_variant_access,bad_optional_access,bad_weak_ptr,bad_function_call,bad_alloc


// 直接构造器里 log 下 source_location， 不知道好不好。 C++没有java那种 异常堆栈信息啊。
//              要是没捕获到，不知道哪里抛出的。
//          捕获到了，也不会知道，毕竟一大堆代码。一大堆可能抛出这个异常的地方。
//      然后 用 值捕获， 让它构造下，构造的时候 有log。
//  不知道&的话，移动赋值会调用吗？应该会吧。毕竟扔出去了。
// 。。。不是构造器里。 而是 你在写 throw 的时候，throw前，log下。。。
// 422有，直接把 source_location 保存在异常中
class MyException : public std::exception
{
public:
    // MyException(std::string info) : m_info(std::move(info)) {}
    MyException(std::string info, std::source_location sl = std::source_location::current()) : m_info { std::move(info) } , m_sl { std::move(sl) } {}
    const char * what() const noexcept override { return m_info.c_str(); }
    virtual const std::string& getInfo() const noexcept { return m_info; };
    virtual const std::source_location& where() const noexcept { return m_sl; }
protected:
    virtual void setInfo(std::string info) { m_info = std::move(info); }
private:
    std::string m_info;
    std::source_location m_sl;
};

void throwMyException() {
    std::string str = std::format("just a test");
    throw MyException {str};            // m_sl is this line
}
void use_cpp20_source_location(const char* info,
                               std::source_location sl = std::source_location::current())
{
    std::cout<<info<<"\n"<<sl.function_name()<<" "<<sl.file_name()<<", "<<sl.line()<<", "<<sl.column()<<std::endl;
}
void code_line_file()
{
    // now i am at main.cpp, 81
    std::cout<<"there is an error, will throw MyException. now i am at "<< __FILE__ <<", "<<__LINE__<<"\n";

    // void code_line_file() main.cpp, 83, 30
    use_cpp20_source_location("wo zenme zhidao a");     // this line's localtion.

    try {
        throwMyException();
    } catch (const MyException& me) {
        use_cpp20_source_location(me.what(), me.where());
    }
}


void throwWithNested() {
    try {
        fun1(5, 0);
    } catch (const std::exception& e) {
        // 生成一个新的类型，多重继承，继承自 MyException 和 nested_exception
        std::throw_with_nested(MyException{std::format("there is a nested exception in me")});
    }
}

void rethrow() {
    try {
        fun1(5, 0);
    } catch (...) {
        std::cerr<<"catch and rethrow\n";
        throw;                  // <<<<<<<<<<<<<<<<<<<<<<<  throw e; 会截断对象。 千万不能用。
    }
}


void memory_allocate_fail()
{
    // 无法分配内存时，new， new[] 默认会抛出 bad_alloc

    // new 大段内存时，最好 try catch 下。

    // 内存分配失败时，可以调用自己的方法，
    // set_new_handler();       返回之前的方法。   默认情况下时 nullptr
    // new_handler 不抛异常 返回后，会再次尝试分配内存， 如果失败，再调用 new handler， 所以很容易就 死循环。
        // new_handler 抛出的异常 必须是 std::bad_alloc 或其子类。
    // new_handler 是一个 void xx() {} 的方法， 没有return，没有形参。


    int* ptr {nullptr};
    // size_t mxCnt { std::numeric_limits<size_t>::max() };
    size_t mxCnt {(std::numeric_limits<long long>::max()) / 100};


// try to allocate 18446744073709551615 int.
// failed to allocation memeory.std::bad_array_new_length       // 是因为 长度超出了最大长度，还没有开始 申请空间。

    // failed to allocation memeory.std::bad_alloc      // 这个才是 申请空间失败。
    std::cout<<std::format("try to allocate {} int.\n", mxCnt);
    try {
        // ptr = new int[mxCnt];
        ptr = new int[mxCnt];
    } catch (const std::bad_alloc& e) {
        std::cerr<<"failed to allocation memeory."<<e.what()<<std::endl;
        // return;
    }
    // process with ptr;

    try {
        std::cout<<"gogogog\n";
        // int* ptr2 = new(std::nothrow) int[123123123123123123];      // ok  result is nulllllll
        // int* ptr2 = new(std::nothrow) int[mxCnt];       // std::bad_array_new_length

        // const size_t mxCnt2 = std::numeric_limits<size_t>::max();
        // int* ptr2 = new(std::nothrow) int[mxCnt2];      // size ‘18446744073709551615’ of array exceeds maximum object size ‘9223372036854775807’    <<<<<<<<<<<<<<<

        size_t mxCnt3 = (std::numeric_limits<long long>::max()) / 100;      // const 也一样。
        int* ptr2 = new(std::nothrow) int[mxCnt3];              // result is nullll

        if (ptr2 == nullptr) {
            std::cerr<<"result is nulllllll.\n";
        }
    } catch (const std::exception& e) {
        //  ??? std::bad_array_new_length
        std::cerr<<" ??? "<<e.what()<<std::endl;
    }

}


void exception_in_constructor()
{
    // 在类中，构造器构造的时候，也要 添加 try catch，不然的话，可能 new 到一半， 就异常了， 之前 申请的资源 不会自动释放

    // 有个 function-try-blocks ， 但很少有人用。  可以用在 普通方法 和 构造器上， 但是 普通方法 可以转为内部的 try-catch
    // 很容易出错，不推荐，
    // 更合适的是 RAII

}


[[noreturn]] void myTerminate()
{
    std::cout<<"uncaught exception\n";
    _Exit(1);       // cstdlib
}


// 异常会一直自动抛出 直到 被catch， 会释放 中间的 堆栈帧， 如果中间的方法调用 中有 局部变量 会被析构， 但是指针不会被释放。 所以 会内存泄漏。 所以要使用 RAII。

int main()
{
        // 会返回 旧的 terminate_handler
    std::set_terminate(myTerminate);        // <exception>
        // 未捕获异常会导致 main 调用 terminate()， 这个函数 调用 cstdlib 的 abort 终止程序。

    try {
        std::cout<<fun1(5,2)<<std::endl;
        // std::cout<<fun1(5,0)<<std::endl;
        rethrow();
    } catch (const std::invalid_argument& e) {
        std::cerr<<e.what()<<std::endl;
    }

    try {
        throwint();
    } catch (int e) {
        std::cerr<<e<<std::endl;
    }

    try {
        throwconstchar();
    } catch (int e) {
        std::cerr<<e<<std::endl;
    } catch (const char* e) {
        std::cerr<<e<<std::endl;
    } catch (...) {
        std::cerr<<"catch all exception\n";
    }

    // throwint();

    try {
        // throwIntButNoExcept();       // will always terminate
        throwIntExceptExpr();
    } catch (...) {
        std::cerr<<"not show\n";
    }


    code_line_file();

    try {
        throwWithNested();
    } catch (const MyException& me) {
        use_cpp20_source_location(me.what(), me.where());
        const auto* nested { dynamic_cast<const std::nested_exception*>(&me) };             // <<<<<<<<
        if (nested) {
            try {
                nested->rethrow_nested();                   // <<<<<<<<<<<<
            } catch (const std::exception& e) {
                std::cout<<e.what()<<"     ---\n";
            }
        }

        // <<<<<<<<<<<<< 简化版
        try {
            std::rethrow_if_nested(me);         // <<<<<<<
        } catch (const std::exception& e) {
            std::cout<<e.what()<<"     --2222-\n";
        }

    }




    memory_allocate_fail();


    exception_in_constructor();


}

