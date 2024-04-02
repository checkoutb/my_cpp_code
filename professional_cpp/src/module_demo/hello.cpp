
module;
#include <iostream>
#include <string_view>

export module hello;

export void print(const std::string_view& name)
{
    std::cout<<"hi, "<<name<<std::endl;
}


