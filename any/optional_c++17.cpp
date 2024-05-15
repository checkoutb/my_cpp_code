
#include <optional>
#include <iostream>
#include <string>
#include <concepts>

template<typename T>
std::optional<T> getValue(T a)
{
    // if (typeid(T) == typeid(int))        // 应该没有办法， 只能特化
    //     return a % 2 == 0 ? std::optional<T>{a} : std::nullopt;      // std::nullopt
    // else
        return std::optional<T>{a};
}


// template<>
// std::optional<int> getValue(int a)           // 和 下面的 重命名了 错误：symbol `_Z8getValueIiESt8optionalIT_ES1_' is already defined
// {
//     return a % 2 == 0 ? std::optional<int>{a} : std::nullopt;
// }


template<typename T>
// std::optional<T> getValue(T a) requires std::integral<T>        // -std=c++20    // ok
std::optional<T> getValue(T a) requires requires{ a % 2; }      // ok
{
    return a % 2 == 0 ? std::optional<int>{a} : std::nullopt;
}

int main()
{
    std::optional<int> op1 = getValue(4);

    if (op1)
        std::cout<<op1.value()<<std::endl;      //

    op1 = getValue(3);
    // if (!op1)
        std::cout<<"no value, "<<op1.value_or(-11223344)<<std::endl;        //

    std::optional<std::string> op2 = getValue<std::string>("5");
    std::cout<<op2.value_or("asd")<<std::endl;

}
