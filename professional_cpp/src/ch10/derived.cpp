
module derived;

//virtual  // error: virtual用在类声明以外
void derived::Derived::someMethod() // override // virt-specifiers in ‘someMethod’ not allowed outside a class definition

{
    std::cout<<"derived.someMethod\n";
}
