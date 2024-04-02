
export module mytest;           // export m o d u l e

export import :define;
export import :impl;

// import <cstddef>;

// int main()
// {
//     size_t sz = 5;
//     MyGrid<int> mg(sz);
//
//     return 0;
// }


// g
// 分区的话，不能在 impl 的方法中 使用 形参。。 一旦使用：
//      template_impl.cpp:2956129:36: 编译器内部错误：在 tsubst_copy 中，于 cp/pt.cc:17288

// 把impl 的内容复制到 define，并且 template_main 中删除 impl 的导入。
//      是可以的，并且可以使用 形参。



// https://github.com/carlesmartin85/procpp5e/tree/main
// 参照这里的，还是不行。

