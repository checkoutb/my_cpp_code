
import <iostream>;

using namespace std;

// cmake 不行， 需要cmake 3.28, gcc14

// 命令行：
    // install module (to this project)
    // g++ -std=c++20 -fmodules-ts -xc++-system-header iostream

    // compile
    // g++ -fmodules-ts -std=c++20 amain.cpp

int main()
{
    std::cout<<"hi"<<endl;
    return 0;
}

// holy

// at first, you must install used module (in this file, it is iostream).
//      after install, you will find the folder: gcm.cache
// then compile this cpp file

// install module (to this project)
// g++ -std=c++20 -fmodules-ts -xc++-system-header iostream

// compile
// g++ -fmodules-ts -std=c++20 amain.cpp

// tour_of_cpp 的 ch16 也是这样的， 忘记了。。


// ------------- cmake ---------------
// 1. 修改 professional_cpp 的 CMakeLists.txt  。。用 src 下的应该也可以。 但是 执行的目录就要变
// 2. cd进入 professional_cpp/build.
// 3. 执行 `cmake ../../professional_cpp/`   。。cmake 应该是自动在这个文件夹下 寻找 cmake的配置文件。
//      配置文件中的 cmake min version 是 3.0， 所以有个警告。 看打印也是成功的，但是我怎么不能运行。。
//      所以我改了下 cmake min version 为 3.20, 没有警告了，就3行：
/*

-- Configuring done (0.0s)
-- Generating done (0.0s)
-- Build files have been written to: /_______/my_cpp_code/professional_cpp/build

*/
// 忘记了，还有一步。。。
// 4. `cmake --build .`
//         完蛋。 不是 module 的形式的。 所以还得找一个 module 的 cmake 步骤。

// g. 需要cmake 3.28, gcc14




