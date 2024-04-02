module;
// #include <typeinfo>
#include <iostream>

export module main;

import base;
import derived;

// import <iostream>;

int main()
{
    // g++ -std=c++20 -fmodules-ts base.cc base.cpp derived.cc derived.cpp test_string.cpp
    // string’不是‘std’的成员

    int a = 0;

    auto aa = typeid(a).name();         // must ‘#include <typeinfo>’ before using ‘typeid’

    std::string s = "asd";

    std::cout<<s<<", "<<typeid(aa).name()<<"\n";

    return 0;
}

