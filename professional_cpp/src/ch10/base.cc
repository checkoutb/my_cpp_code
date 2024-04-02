
export module base;

import <iostream>;

export namespace base
{
    class Base
    {
    public:
        // void someMethod() {
        //     std::cout<<"base.someMethod\n";
        // }
        virtual void someMethod();
        void noVgo() {
            std::cout<<"base.noVgo\n";
        }
        void noVgo(int i) {
            std::cout<<"base.noVgo.int\n";
        }
        virtual void multiBase();
    protected:
        int m_protectedInt { 0 };
    private:
        int m_privateInt { 0 };
    };

    class Base2
    {
    public:
        // void someMethod() {
        //     std::cout<<"base.someMethod\n";
        // }
        virtual void someMethod();
        void noVgo() {
            std::cout<<"base2.noVgo\n";
        }
        void noVgo(int i) {
            std::cout<<"base2.noVgo.int\n";
        }
        virtual void multiBase();
    protected:
        int m_protectedInt { 0 };
    private:
        int m_privateInt { 0 };
    };
};
