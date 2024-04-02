
#include <string>
#include <iostream>

#include <vector>
#include <optional>

#include <memory>

void msg(std::string& a)
{
    std::cout<<"lvalue "<<a<<std::endl;
}
void msg(std::string&& a)
{
    std::cout<<"rvalue "<<a<<std::endl;
}

template<typename T>
class Taa
{
public:
    // using namespace std;        // not allowed in class
    std::vector<std::vector<std::optional<T>>> vv;
};

int main()
{
    using namespace std;

    string a = "aaa";
    string b = "bbb";
    string c = a + b;

    std::cout<<a<<" , "<<b<<" , "<<c<<std::endl;

    msg(c); // l value

    msg(a + b); // r value
    msg("asd");     // r
    msg("zcs" + a);     // r

    vector<vector<optional<int>>> vvpi;
    vvpi.resize(11);
    for(auto& v : vvpi)
    {
        v.resize(5);
    }
    std::cout<<vvpi.size()<<" - "<<vvpi[0].size()<<"\n";

    Taa<int> ta;
    size_t aa = 5;
    std::cout<<ta.vv.size()<<std::endl;
    ta.vv.resize(aa);
    for (auto& v : ta.vv)
        v.resize(3);
    std::cout<<ta.vv.size()<<" , "<<ta.vv[0].size()<<"\n";

}
