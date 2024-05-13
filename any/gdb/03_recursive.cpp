
#include <iostream>

int factorial(const int &n)
{
    if (n)
    {
        return n * factorial(n - 1);
    } 
    else 
    {
        return 1;
    }
}

int main()
{
    int n;

    std::cout<<"please input a number: ";
    if (std::cin >> n && n >= 0)
    {
        std::cout<<n<<"! is "<<factorial(n)<<std::endl;
    }
    else
    {
        std::cout<<"input is wrong."<<std::endl;
    }
}


// -g 不会尾递归优化



