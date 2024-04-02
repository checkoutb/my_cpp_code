
export module derived;

import base;

import <iostream>;
// import <string>;

export namespace derived
{

    class Derived final : public base::Base, public base::Base2 //final
    {
    public:
        void someOtherMethod() {
            std::cout<<"derived.someOtherMethod\n";
        }
        using base::Base::noVgo;
        void someMethod() override;
        void noVgo() {  // not virtual
            std::cout<<"derived.noVgo\n";
        }

        using Base::multiBase;
        void multiBase() override {
            Base::multiBase();
        }
        std::string getName() {
            return "asd";
        }
    };
};

