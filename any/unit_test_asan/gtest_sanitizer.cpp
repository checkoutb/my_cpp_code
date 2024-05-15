

// https://google.github.io/googletest/primer.html
// now use test suite, not test case
// A test suite contains one or many tests.
// 这文章对于代码感觉没用
// When using a fixture, use TEST_F() instead of TEST() as it allows you to access objects and subroutines in the test fixture:


// https://zhuanlan.zhihu.com/p/661950698
// 代码来自上面



#include <gtest/gtest.h>
#include <vector>
#include <iostream>



//////////////////////////

int add(int a, int b)
{
    return a + b;
}
TEST(TEST_ADD, UNSIGNED_INT_VALUE)
{
    int result = add(100, 200);
    EXPECT_EQ(result, 300);
    result = add(200, 300);
    EXPECT_NE(result, 400);
}

//////////////////////////

// TEST_F 复用测试案例 (每次都是重新初始化)

class VectorTest : public testing::Test
{
protected:
    void SetUp() override {
        std::cout<<"init vi\n";     // 有2个TEST_F，打印了2次
        vi = new std::vector<int>(5, 0);       // 每个 TEST_F 以这个值为准
    }
    void TearDown() override {
        delete vi;
    }
    std::vector<int>* vi;
};
TEST_F(VectorTest, VI_APPEND_TEST)
{
    vi->push_back(123);
    EXPECT_EQ(vi->size(), 6);
    EXPECT_EQ(vi->operator[](0), 0);
    EXPECT_EQ(vi->at(5), 123);
}
TEST_F(VectorTest, VI_POP_TEST)
{
    vi->pop_back();
    EXPECT_EQ(vi->size(), 4);       // 4, 使用的是 相同的 vi， 而不是 在上一个的基础上 继续操作。
                            // 确实，在上一个的基础上 继续操作的话，为什么不写到 同一个 TEST 中呢。。
    EXPECT_EQ(vi->at(2), 0);
}

//////////////////////////

// TEST_P 复用逻辑，多种输入 测试 同一段代码。

namespace {
    namespace MyTest{
        struct TestData {
            int a;
            int b;
            int result;
            char type;
        };

        class TestDataTest : public ::testing::TestWithParam<TestData> {
        protected:
            void checkData() {
                int a = GetParam().a;
                int b = GetParam().b;
                int result = GetParam().result;
                switch (GetParam().type) {
                    case '+':
                        EXPECT_EQ(a + b, result);
                        break;
                    case '-':
                        EXPECT_EQ(a - b, result);
                        break;
                    case '*':
                        EXPECT_EQ(a * b, result);
                        break;
                    default:
                        break;
                }
            }
        };
        TEST_P(TestDataTest, Test)
        {
            checkData();
        }
        INSTANTIATE_TEST_SUITE_P(TestMyClassParams, TestDataTest, ::testing::Values(
            TestData{1,2,3,'+'},
            TestData{4,2,2,'-'},
            TestData{3,5,15,'*'}
        ));
    }
}



//////////////////////////





int main(int argc, char **argv) {

    int arr[] = {3};
    // std::cout<<arr[5]<<std::endl;

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// int main()
// {
//     testing::InitGoogleTest();
//     return RUN_ALL_TESTS();
// }

/*

mkdir build
cd build/
cmake ..
make

会生成很多文件，估计只有 lib/ 是有用的。
`ar vt libgtest.a`
gtest-all.cc.o      。。。


`g++ gtest.cpp  -l gtest -L /googletest-1.14.0/build/lib -I /googletest-1.14.0/googletest/include/`
指定 lib 位置， 头文件 位置


```text
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from TEST_ADD
[ RUN      ] TEST_ADD.UNSIGNED_INT_VALUE
[       OK ] TEST_ADD.UNSIGNED_INT_VALUE (0 ms)
[----------] 1 test from TEST_ADD (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```




    ASSERT_*断言：当检查点失败时，退出当前函数；
    EXPECT_*断言：当检查点失败时，继续往下执行。






*/

///////////////////////

/*

Valgrind 应该更好，因为 Valgrind 是一个工具，不需要编译进来。 使用 Valgrind 运行 a.out ， 但是速度更慢。






sanitizer

`sudo dnf install libasan`

安装:
 libasan          x86_64          13.2.1-7.fc39          updates          473 k

事务概要
================================================================================
安装  1 软件包

总下载：473 k
安装大小：1.4 M



`g++ gtest.cpp  -l gtest -L /googletest-1.14.0/build/lib -I /googletest-1.14.0/googletest/include/ -fsanitize=address -g`

```text
$ ./a.out
=================================================================
==30143==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7fe548d01944 at pc 0x000000409ff3 bp 0x7ffcb4368ca0 sp 0x7ffcb4368c98
READ of size 4 at 0x7fe548d01944 thread T0
    #0 0x409ff2 in main /my_cpp_code/any/unit_test/gtest.cpp:123          // <<<<< 行号，需要 -g
    #1 0x7fe54ac46149 in __libc_start_call_main (/lib64/libc.so.6+0x28149) (BuildId: 7dd93cb9991a89f0ec53d9443a0b78ad952269bc)
    #2 0x7fe54ac4620a in __libc_start_main_impl (/lib64/libc.so.6+0x2820a) (BuildId: 7dd93cb9991a89f0ec53d9443a0b78ad952269bc)
    #3 0x406fb4 in _start (/my_cpp_code/any/unit_test/a.out+0x406fb4) (BuildId: 6aa99bbec2eec062e49852db009762944567993f)

Address 0x7fe548d01944 is located in stack of thread T0 at offset 68 in frame
    #0 0x409eff in main /my_cpp_code/any/unit_test/gtest.cpp:120

  This frame has 2 object(s):
    [48, 52) 'arr' (line 122)
    [64, 68) 'argc' (line 120) <== Memory access at offset 68 overflows this variable
```





*/


