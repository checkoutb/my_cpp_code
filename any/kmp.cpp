
#include <iostream>
#include <string>
#include <vector>

using namespace std;




        // for (int j = 1; j < sz1; ++j)
        // {
        //     while (p[i] != p[j] && i > 0)
        //         i = nxt[i];
        //     if (p[i] == p[j])
        //         nxt[j] = ++i;
        // }




// prefix == suffix
// 感觉能理解，但是不牢固。
vector<int> checkPrefixSuffix(string& str)
{
    int sz1 = str.size();
    vector<int> nxt(sz1);
    int i = 0;          // 长度
    int j {0};
    for (j = 1; j < str.size(); ++j)        // j=0的话，i=j，前缀=后缀=整个str
    {
        while (str[i] != str[j] && i > 0)   // 不相等，就回退一位，i是长度，所以 i-1 就是 已经匹配的前缀的 最后一位的长度
            i = nxt[i - 1];                 // 长度 = 第一个未匹配的位置， 所以 i尝试从这里开始匹配。

        // cout<<"i, j: "<<i<<", "<<j<<endl;

        if (str[i] == str[j])       // dp， 相同则 长度+1,并且设置 nxt
            nxt[j] = ++i;
        else
            nxt[j] = 0;         // 实际上不需要，默认的就是0
    }
    return nxt;
}



class KMP
{
public:
    KMP(string& s2, string& p2): s{s2}, p{p2}, nxt {vector<int>(p.size())}
    {
        initNext();
    }

    void initNext()
    {
        int i {0}, j {0}, sz1 {static_cast<int>(p.size())};
        for (int j = 1; j < sz1; ++j)
        {
            while (p[i] != p[j] && i > 0)
                i = nxt[i];
            if (p[i] == p[j])
                nxt[j] = ++i;
            // else nxt[j] == 0 (default)
        }

        cout<<"next is: ";
        for (int i : nxt)
            cout<<i<<", ";
        cout<<endl;
    }

    int find(int ist = 0)
    {
        int j {0};
        for (int i = ist; i < s.size(); ++i)
        {
            if (s[i] == p[j])
            {
                ++j;
                if (j == p.size())
                    return i - p.size() + 1;
            }
            else
            {
                j = nxt[j];
            }
        }
        return -1;
    }


private:
    string& s;
    string& p;
    vector<int> nxt;
};


int main()
{
    string s = "abcdefqwerabcdezzabc";

    vector<int> vi = checkPrefixSuffix(s);

    for (int i : vi)
    {
        cout<<i<<", ";
    }
    cout<<endl;


    s = "asdfghjklzxghjkcvbnghjkm";
    string p = "ghjk";

    s = "asdasdfasdfghasdfghasdfgzxcvzxcvbnzxcvb";
    p = "sdasdf";


    KMP kmp(s, p);

    // cout<<kmp.find()<<endl;
    int idx = 0;
    do {
        idx = kmp.find(idx);
        if (idx != -1)
        {
            cout<<"find @ "<<idx<<", "<<s.substr(idx, p.size())<<endl;
            ++idx;
        }

    } while (idx != -1);

    return 0;
}



















