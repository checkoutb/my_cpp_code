
export module variadic_mix;

import <iostream>;

export class Mixin1
{
public:
    Mixin1(int i) : m_value {i} {}
    virtual void mx1fun() { std::cout<<"mixin1: "<<m_value<<"\n"; }
private:
    int m_value;
};

export class Mixin2
{
public:
    Mixin2(int i) : m_value {i} {}
    virtual void mx2fun() { std::cout<<"mx222: "<<m_value<<'\n'; }
private:
    int m_value;
};

export template <typename... Mixin>
class MyClass : public Mixin...
{
public:
    MyClass(const Mixin&... mxs) : Mixin { mxs }... {}
    virtual ~MyClass() = default;
};

