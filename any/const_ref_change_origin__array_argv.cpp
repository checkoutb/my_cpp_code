
#include <iostream>
#include <vector>


int cnt = 1;

void change_origin()
{
    cnt = 123;
}

void const_ref(const int& cnt2)
{
    std::cout<<&cnt2<<std::endl;        // 0x40402c  same
    std::cout<<cnt2<<std::endl; // 1
    change_origin();
    std::cout<<cnt2<<std::endl; // 123
}

void restrict_(int * __restrict__ p)        // 必须 * 后， *前报错：不能应用于 int ， 现在是 应用在 指针p上， 而不是 指针p 指向的 int 上。
{

}

void restrict_2(std::vector<int> * __restrict__ vi) // 必须 指针。
{

}

void fun1(int a[])
{

}

void restrict_3(int a[5])       // 无法使用 __restrict__
{
    // std::cout<<"=================\n"<<sizeof(a)<<std::endl;     // pointer' size, compiler has warning
    for (int i = 0; i < 5; ++i)
        std::cout<<a[i]<<", ";
    std::cout<<std::endl;
}

void transfer2(int (&aa)[5])
{
    std::cout<<sizeof(aa)<<std::endl;   // 20   /4 = 5
    for (int i = 0; i < 5; ++i)
        std::cout<<aa[i]<<" ";
}

void transfer22(int (&aa)[5][5])
{
    std::cout<<sizeof(aa)<<std::endl;    // 100    /4 = 5*5
}

int main()
{
    int a = 2;
    std::cout<<&cnt<<", "<<&a<<std::endl;   // 0x40402c 低层堆， 0x7ffee1e044bc 高层栈
    const_ref(cnt);

    int arr[5] = {1,2,3,4,5};

    transfer2(arr);     // 必须5个元素

    restrict_3(arr);

    int arr2[5][5] = {{0}};
    transfer22(arr2);

    return 0;
}
