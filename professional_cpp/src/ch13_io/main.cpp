
#include <iostream>
// #include <cstdlib>
// #include <string>
#include <cstring>      // std::strlen ...  or c's <string.h>
#include <thread>
#include <chrono>

#include <ios>
#include <iomanip>      // 操作算子

#include <string>

#include <format>

#include <sstream>

#include <fstream>

#include <filesystem>

// 操作算子
void manipulator()
{
    // 还有输入算子： boolalpha(false->false,其他->true), noboolalpha(0->false,其他->true), dec,hex,oct, skipws,noskipws, ws(跳过流中当前位置的一串空白字符), get_money,get_time,quoted

    // 可以自定义算子，但很少需要。

    bool b {false};
    std::cout<<"default: "<<b
        <<"\n, it should be 'true' : "<<std::boolalpha<<b
        <<"\n it should be '1' : "<<std::noboolalpha<<b<<std::endl;

    int i {123};
    printf("ccc:%6d\n", i);
    std::cout<<"'   123':"<<std::setw(6)<<i<<std::endl;

    printf("ccc:%06d\n", i);
    std::cout<<"c++:"<<std::setfill('@')<<std::setw(6)<<i<<std::endl;
    std::cout<<std::setfill(' ');

    double d1 {1.452};
    double d2 {5};
    std::cout<<" 5.000:"<<std::showpoint<<d2<<std::endl; // 默认 noshowpoint
    std::cout<<"' 5':"<<std::setw(2)<<std::noshowpoint<<d2<<std::endl;
    std::cout<<"'':"<<std::setw(3)<<std::noshowpoint<<d1<<std::endl;

    std::cout.imbue(std::locale{""});

    std::cout<<std::put_money("1234123")<<std::endl;        // 12,341.23
    std::time_t tt {std::time(nullptr)};        // 这里 2个 std:: 可以不写的。
    std::cout<<std::put_time(std::localtime(&tt), "%c")<<std::endl;     // 2024年03月11日 星期一 15时50分27秒

    std::cout<<std::quoted("asd \"asd \".")<<std::endl;

    std::cout<<"1.2345 : "<<std::setprecision(5)<<123445.123123<<std::endl;     // 1.2345e+05

    int t2 = std::cout.precision(4);        // 返回5,设置为4， 用完后可以恢复为t2
    std::cout<<"zzz "<<t2<<std::endl;

}

void put_write()
{
    const char* s {"hi,cpp\n"};     // 看起来，遇到 \n 会进行flush()。 如果把\n删除，要等到flush的时候才显示
    std::cout.write("b", 1);            // c++的源码。不具备可读性。。
    std::cout.write(s, std::strlen(s));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout.put('a');
    std::cout.put('\n');        // will flush
    std::cout.put('a');
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout.flush();


    //////////////////////////////////////////

    if (std::cout.good())
    {
        std::cout<<"cout is good\n";
    }
    if (!std::cout.fail())          // 遇到文件结果标记， good 和 fail 都是 false。  good() == (!fail() && !eof())
        std::cout<<"cout is not fail\n";
    if (std::cout)
        std::cout<<"okkkkk\n";

    std::cerr<<"errrrrrr\n";        // 没有缓冲，  clog是带缓冲的cerr


    /////////////////////////////
    // expcetion

    try {
        std::cout.setstate(std::ios::failbit);
        std::cout.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);  // 书上，这行在try外，且没有上面的 setstate，有下面的 hi。 但是 我运行的时候没有异常
                // 现在加上 setstate 后有异常了。 不过应该是 exceptions 抛出的，不是 下面的 hi。
        // std::cout<<" hi "<<std::endl;
    } catch (const std::ios_base::failure& ex) {
        std::cerr<<"catch exception:"<<ex.what()<<", code="<<ex.code()<<std::endl;

        if (std::cout.good())
        {
            std::cerr<<"good\n";
        }
        else
        {
            std::cerr<<"bad\n";     //
        }

        std::cout.clear();  // 重置流的错误状态。 不然下面的 end... 不会显示。
    }

    std::cout<<"end...";
}


void input_pattern()
{
    std::cout<<"plz input, ctrl+d in linux, ctrl+z in windows, to finish\n";
    int sum = 0;
    if (!std::cin.good())
    {
        std::cerr<<"standard input is in bad state"<<std::endl;
        return;
    }
    while (!std::cin.bad())
    {
        int n;
        std::cin>>n;
        if (std::cin.good()) {
            sum += n;
        } else if (std::cin.eof()) {
            break;
        } else if (std::cin.fail()) {
            std::cin.clear();
            std::string badtoken;
            std::cin>>badtoken;
            std::cerr<<"WARNING. BAD INPUT: "<<badtoken<<std::endl;
        }
    }
    std::cout<<" sum is "<<sum<<std::endl;
}


std::string getName(std::istream& stream)
{
    std::string name;

    // while (stream) {
    //     int next = stream.get();
    //     if (!stream || next == std::char_traits<char>::eof())
    //         break;
    //     name += static_cast<char>(next);
    // }

    char next;      // simple
    while (stream.get(next)) {
        name += next;
    }

    return name;
}
void input_get_unget()
{
    // 2xia ctrl + d
    // std::cout<<"get name: "<<getName(std::cin)<<std::endl;

    // unget, 将读的字符放回去. 如果是流的起始位置，unget会失败
    std::string name;
    int partySize {0};
    char ch;
    std::cin>>std::noskipws;        // 输入算子。
    while (std::cin >> ch)
    {
        using namespace std;
        if (std::isdigit(ch)) {
            cin.unget();                      // ..
            if (cin.fail())
                cout<<"unget fail"<<endl;
            break;
        }
        name += ch;
    }
    if (std::cin)
        std::cin>>partySize;
    if (!std::cin) {
        std::cout<<"error get party size\n";
        return;
    }

    std::cout<<std::format("thank you {}, party of {}", name, partySize)<<std::endl;

    if (partySize > 10) {
        std::cout<<"dddddddd"<<std::endl;
    }

}

void cin_putback_peek()
{
    using namespace std;
    char ch;
    cin>>ch;
    cout<<"1 "<<ch<<endl;
    cin.putback('z');           // 修改
    char ch2 = cin.peek();      // peek
    cout<<"ch2 "<<ch2<<endl;
    cin>>ch;
    cout<<"2 "<<ch<<endl;
}

void input_getline()
{
    char buf[12] {0};
    std::cin.getline(buf, 4);       // 这个也可以 自定义结束符， 和 std::getline 一样
    std::cout<<buf<<std::endl;

    std::cout<<" --  "<<std::cin.good()<<std::endl;     // 多于3个，这里 0, 就是 not good..

    std::string s;      // 如果上面输入 多于3个的话，这里会是 回车。  下面的也是 回车。
    std::getline(std::cin, s);      // in <string>
    std::cout<<s<<std::endl;

    std::getline(std::cin, s, '@');
    std::cout<<s<<std::endl;
}

void string_stream()
{
    using namespace std;
    cout<<"enter tokens, ctrl+d, ctrl+z\n";
    std::ostringstream oss;
    while (cin) {
        string token;
        cout<<"next token:";
        cin>>token;             // 空格为分隔符，所以 输入空格会连续输出next token:。
        if (!cin || token == "done")
            break;
        oss<<token<<"\t";
    }
    cout<<"result is:"<<oss.str();
}

void file_stream()
{
    // ios_base::app/ate/binary/in/out/trunc
    // ios_base::app | ios_base::binary
    // ofstream 自动包含out，  ifstream 自动包含in , 它们的析构函数自动关闭底层文件

    {
        std::ofstream ofile {"test.ignore", std::ios_base::trunc};
        if (!ofile.good())
        {
            std::cerr<<"error\n";
            return;
        }
        ofile<<"sth to write";
        ofile<<"\n123\nzzz";
    }
    {
        std::ifstream ifile {"test.ignore"};
        while (ifile.good())
        {
            std::string s;
            ifile>>s;
            std::cout<<" -- "<<s;       //  -- sth -- to -- write -- 123 -- zzz
                            // 空格，回车 作为分隔符。
        }
    }
}

void file_stream_seek_tell()
{
    // seekg, seekp

    using namespace std;
    ofstream fout {"test.2.ignore"};
    if (!fout) {
        cerr<<"error open file for write\n";
        return;
    }
    fout<<"54321";

    std::streampos curPos {fout.tellp()};
    if (curPos == 5) {
        cout<<"now at 5\n";
    } else {
        cout<<"fail, not at 5\n";
    }

    fout.seekp(2, ios_base::beg);

    fout<<0;
    fout.close();

    ifstream fin {"test.2.ignore"};
    if (!fin) {
        cerr<<"error open file to read\n";
        return;
    }

    int val;
    fin>>val;
    if (!fin) {
        cerr<<"error read \n";
        return;
    }

    const int want {54021};
    if (val == want) {
        cout<<"ok \n";
    } else {
        cout<<"wrong\n";
    }
}

void link_stream()
{
    using namespace std;
    ifstream fin {"empty.ignore"};   // must exist
    ofstream fout {"output.ignore"};

    fin.tie(&fout);     // tie

    fout<<"hi aaaa";
    // fout.flush();
    string token;
    fin>>token;
    cout<<"got:"<<token;        // 和书上的不一样，我这里什么都没有， 加上 flush 也是什么都没有。
                    // 是的，本来就没有。
                    // 书上的意思是， fin读取的时候 会自动 flush fout。 但是我从 empty.ignore中读取，本来就没有东西。 我误解成 fin 从 fout 中读取了， 就是 token 是 hi aaaa  。 不可能的。
}

void file_lib()
{
    std::filesystem::path p1 { R"(test.ignore)" };

    std::cout<<p1.root_name()<<std::endl;
    std::cout<<p1<<std::endl;

    // std::cout<<

    // ...
}


// C: fprintf, fwrite, fputs; fscanf, fread, fgets

int main()
{

    // put_write();     // put, write, flush, clear, exception, fail, good

    // manipulator();      // 操作算子

    // input_pattern();            // !!!

    // input_get_unget();

    // cin_putback_peek();

    // input_getline();

    // string_stream();

    // file_stream();

    // file_stream_seek_tell();

    // link_stream();      // tie

    // 双向IO ： 同时执行 输入输出。 iostream, fstream,

    file_lib();


    return 0;
}
