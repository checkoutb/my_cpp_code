
#include <iostream>
#include <string>
#include <iomanip>
#include <locale>



// https://zhuanlan.zhihu.com/p/375929434
// 但是，我本地试了，有问题。 tmp != "，" 永远成立的， 加不加u8 都一样。所以导致不会分词。
// 而且，是不是 ， 。 不是 2个字节的？ 这中间输出有问题啊。
// 确实，"你好，中文。" 的size 是18， 估计， 。 是3字节的。 但是中间错了，后续的应该也全错了。
// 是的 "你好，中文" size 是15。 。。 写mine的时候，才注意到，都是3个字节。 3*6=18。 和 utf-8 对应不起来。。能对应了。
// 不过看了这段代码，结合 utf-8的格式，自己可以分字的。
int main_bad() {
    std::string tmp;
    std::string str;
    // std::cin>>str;
    getline(std::cin, str);

    std::string word;
    std::cout<<str.size()<<std::endl;
    for (int i = 0; i < str.size(); i += 2) {
        tmp += str[i];
        tmp += str[i + 1];

        // std::cout<<" -- "<<tmp<<std::endl;
        if (tmp != u8"，" && tmp != u8"。") {
            word += tmp;
        } else {
            std::cout<<" ! "<<i<<" : "<<word<<std::endl;
            word = "";
        }
        tmp = "";
        std::cout<<" ? "<<i<<" : "<<word<<std::endl<<std::endl;
    }
    std::cout<<word<<std::endl;

    return 0;
}


//////////////////////
// mine

// ok
/*

你好，中文asd！@#。啊。       /// 这个是输入， ！是中文的
32
你好, 6, 2
中文asd！@#, 14, 8
啊, 3, 1

*/

// 2个地方
// 1. 记得：java是可以处理中文的，它是 utf-16。所以想成一个中文2字节的。
// 2. zhihu的代码，就是 2个字节的。 所以 配上第一点， 就把 中文当作2字节了，所以出现3字节，就很奇怪。
// 而且 zhihu上的代码说，宽度。。无稽之谈。。
// 不过他的代码也给了例子，说明他确实运行起来了，这就不知道为什么了， 估计 字符编码不同。
// 看截图，用的是 win + codeblock， 不知道具体编译器。


int len2(char ch) {
    // std::cout<<"~ : "<<int(ch)<<std::endl; // ffffffe4, 只有e4是有效的，前面的f 是 char转 int后增加的。
           // e4 就是 1110 0100
    // 一  4e00
    // 中 是 4e2d
    // 那 这里的 e4 代表什么？
    // 中： 3个字节， e4,b8,ad    1110 0100， 1011 1000， 1010 1101 去掉前缀后，变成
                          // (0100)  (1110) (00  10) (1101)   ok 没有问题了。
    // 一： 3个字节， e4,b8,80  。 确实是这个，我看到有个举例就是 一， 就是 e4 b8 80 但是怎么和 U+4E00 对应？
                 //  1110 0100 1011 1000 1000 0000
              // (0100) (11 10) (00 00) (0000)   好像确实是 4e00
    // 上面的逻辑是 utf-8 获得 unicode， 对本次没有意义。  本次只是 识别 字
    // std::cout<<std::hex<<int(ch)<<std::endl;
    if ((ch & 0b11110000) == 0b11110000) {
        return 4;
    }
    if ((ch & 0b11100000) == 0b11100000) {
        return 3;
    }
    if ((ch & 0b11000000) == 0b11000000) {
        return 2;
    }
    if ((ch & 0b10000000) == 0b10000000) {
        return -1; // 不是一个字的开始。
    }
    return 1;
}

int main_ok() {
    std::string tmp;
    std::string str;

    std::cin>>str;
    std::string word;

    std::cout<<str.size()<<std::endl;
    for (int i = 0; i < str.size(); ++i) {
        // std::cout<<std::hex<<short(str[i])<<std::endl;
    }
    int cnt = 0;
    for (int i = 0; i < str.size();) {
        char ch = str[i];
        int len = len2(ch);
        tmp = "";
        // std::cout<<"! : "<<len<<std::endl;
        while (len-- > 0) {
            tmp += str[i];
            i++;
        }
        // std::cout<<"? : "<<tmp<<std::endl;
        if (tmp == u8"，" || tmp == "。") {
            // std::cout<<" . split "<<std::endl;
            std::cout<<word<<", "<<word.size()<<", "<<cnt<<std::endl;
            word = "";
            cnt = 0;
        } else {
            word += tmp;
            ++cnt;
        }
    }

    return 0;
}



// 不行，中文乱码。
// ok了。  setlocale 的问题，之前是 "chs"，输出乱码， 现在可以了。
int main() {

    std::setlocale(LC_ALL, "zh_CN.utf8");

    std::wstring wstr;
    std::wcin>>wstr;

    std::wcout<<wstr.c_str()<<", "<<wstr.size()<<std::endl;

    std::wstring wstr2 = L"中文，你好。asdf";
    std::wcout<<wstr2<<std::endl; // 输出 ??,???asdf

    std::wcout<<"\n======\n"<<std::endl;
    
    wchar_t warr[] = L"中文，你好。asdqqq";
    std::wcout<<warr<<std::endl;
    
    return 0;
}


// 还是不行。 输出，中文是乱码。
// 可以了。
// 这里是因为 之前先 cout输出 size，然后 wcout， 看起来，wcout 和 cout是冲突的，不能在一起出现。
int main_oookk() {
    std::setlocale(LC_ALL, "zh_CN.utf8");
    std::wcout.imbue(std::locale("zh_CN.utf8"));
    
    std::wstring ws = L"你好，asdfqwe";

    std::wcout<<ws[1]<<std::endl;
    //std::cout<<ws.size()<<std::endl; // 2
    std::wcout<<ws.size()<<std::endl;
    std::wcout<<ws.c_str()<<std::endl; // 直接ws 也一样 都是输出 `}
    std::wcout<<ws[0]<<std::endl;

    return 0;
}



