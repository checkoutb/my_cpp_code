

export module meta_program;

export template <unsigned char f>
class Factorial
{
public:
    static const unsigned long long value = (f * Factorial<f - 1>::value);
};

export template <>
class Factorial<0>
{
public:
    static const unsigned long long value { 1 };
};

// C++20 可以： consteval immediate
export consteval unsigned long long factorial(unsigned char f)
{
    if (f == 0)
        return 1;
    return f * factorial(f - 1);
}


/////////////////////

// 编译时 展开循环。 通常 编译器 足够智能，会自动展开 可以展开的 循环
export template <int i>
class Loop
{
public:
    template <typename FuncType>
    static inline void run(FuncType func) {
        Loop<i - 1>::run(func);
        func(i);
    }
};
export template <>
class Loop<0>
{
public:
    template <typename FuncType>
    static inline void run(FuncType /* func */) { }
};













