

// ok
// #include <boost/lambda/lambda.hpp>
// #include <iostream>
// #include <iterator>
// #include <algorithm>
//
// int main()
// {
//     using namespace boost::lambda;
//     typedef std::istream_iterator<int> in;
//
//     std::for_each(in(std::cin), in(), std::cout<<(_1 * 3)<<" ");
// }
//
//
// // g++ -I /usr/local/boost_1_84_0 main.cpp
// // echo 1 2 3 | ./a.out


// build后，就不需要 -I 了， 我看 kdevelop 能找到 boost的库了。


#include <boost/regex.hpp>
#include <iostream>
#include <string>

int main()
{
    std::string line;
    boost::regex pat ("^Subject: (Re: |Aw: )*(.*)");
    while (std::cin)
    {
        std::getline(std::cin, line);
        boost::smatch matches;
        if (boost::regex_match(line, matches, pat))
            std::cout<<matches[2]<<std::endl;
        else
            std::cout<<"###"<<std::endl;
    }
}

// g++ main.cpp
// 可以，不过我不知道怎么输入 才能让它走第一个分支。。所以一直是 ###。。。
// kdevelop识别到了库，在 /usr/local/include/boost/ 中。 这个 include 只有 boost 一个文件夹。
// C++20的 regex 我都没有看。

