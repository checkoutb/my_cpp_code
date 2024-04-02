
module base;

import <iostream>;

void base::Base::someMethod()
{
    std::cout<<"base.someMethod\n";
}

void base::Base::multiBase()
{
    std::cout<<"base.multiBase\n";
}


// ------------
void base::Base2::someMethod()
{
    std::cout<<"base2.someMethod\n";
}

void base::Base2::multiBase()
{
    std::cout<<"base2.multiBase\n";
}

