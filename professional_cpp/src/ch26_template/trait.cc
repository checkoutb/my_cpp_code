module;
#include <type_traits>

export module mytrait;

import <iostream>;

// template <typename T>
// constepxr conditional<is_nothrow_move_assignable_v<T>, T&&, const T&>::type move_assign_if_noexcept(T& t) noexcept
// {
//     return std::move(t);
// }


//////////////////////
// enable_if  SFINAE

export template <typename T1, typename T2>
std::enable_if_t<std::is_same_v<T1, T2>, bool> checkType(const T1& t1, const T2& t2)
{
    std::cout<<t1<<" =type= "<<t2<<'\n';
    return true;
}

export template <typename T1, typename T2>
std::enable_if_t<!std::is_same_v<T1, T2>, bool> checkType(const T1& t1, const T2& t2)
{
    std::cout<<t1<<" !type= "<<t2<<'\n';
    return false;
}


//////////////////
// 使用 if constepxr 代替 enable_if

export class Clz {
public:
    void ado() const { std::cout<<"Clz::do ing...\n"; }
};

export template <typename T>
std::enable_if_t<std::is_base_of_v<Clz, T>, void> call(const T& t)
{
    t.ado();
}

export template <typename T>
std::enable_if_t<!std::is_base_of_v<Clz, T>, void> call(const T& )
{
    std::cout<<"cannot call Clz::do\n";
}

//---------- if constexpr
export template <typename T>
void call2(const T& [[maybe_unused]] t)
{
    // if constexpr (std::is_base_of_v<Clz, T>) {                  // ok
    if constexpr (std::is_invocable_v<decltype(&Clz::ado), T>) {
        t.ado();
    } else {
        std::cout<<"cannot call ddooo;\n";
    }
}

// conjunction_v    // and
// disjunction_v    // or
// negation_v       // !




///////////////////////////

static_assert(sizeof(void*) == 8, "require 32bit os.");     // ==4, 错误：static assertion failed: require 32bit os.

template <typename T>
void foo(const T& t)
{
    static_assert(std::is_integral_v<T>, "T should be integral type.");
}
// --------- c++20
template <std::integral T>
void foo(const T& t) {}

void foo(const std::integral auto& t) {}


/////////////////////












