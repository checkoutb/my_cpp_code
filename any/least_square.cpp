

#include <vector>
#include <utility>
#include <iostream>
#include <random>
// #include <limits.h>  // LLONG_MAX
#include <limits>


// 当系数 大于 x 的值 时，系数占据了主导，导致出现误差(基本都是 产生 进位)
// 所以 generate_point 中 x 的范围 感觉需要是随即值的 3倍。
// 产生的点，应该是 <double, double>， 实际应该很少整数吧。

// 而且，我不知道，我这种 从最高位开始，低位当作0 的 逻辑是否正确。 所以出现了误差。 要修复的话， 应该是 看 后续 是否 产生了一个 较大的负数系数，如果有 负数系数，那么 高位 尝试 下降/上升 到0。

// x的幂次 越高， 越不容易出错， 因为 幂次越高，x 就占据了主导， 系数的影响就很小。 所以系数不会出现误差。
// 不是。 xxx 最高29 的时候 不会出现误差，  xxxx最高29的时候，必然进位
// 应该比较的是  1*x^4 和 29*x^3 的误差。


// x 可能在 指数上。  还有 log 之类的。





// https://blog.csdn.net/weixin_44613415/article/details/129603827
// https://zhuanlan.zhihu.com/p/390294059
// 偏导，矩阵。

// https://blog.csdn.net/m0_61209712/article/details/134373442
// 高斯牛顿方法和梯度下降方法
// 雅克比矩阵, 雅可比矩阵是一阶偏导数以一定方式排列成的矩阵
// 梯度
// 海森矩阵,是一个多元函数的二阶偏导数构成的方阵,       泰勒展开


// 二次函数
class quadratic_function
{
public:

    quadratic_function()
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dis(2,30);
        a = dis(mt);
        b = dis(mt);
        c = dis(mt);
        // x = 0;
    }

    // std::pair<int, int> generate_point()
    std::vector<std::pair<int, int>> generate_point()
    {
        std::vector<std::pair<int, int>> vp;
        int y = 0;
        for (int x = -40; x <= 50; ++x)
        {
            y = a * x * x + b * x + c;
            vp.emplace_back(std::make_pair(x, y));
        }
        return vp;
    }

    void print()
    {
        std::cout<<" real param : "<<a<<", "<<b<<", "<<c<<std::endl;
    }

private:
    int a, b, c;    // y = a*x^2 + b*x + c
    // int x;
};


class xxx_function
{
public:
    xxx_function()
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dis(2,30);
        a = dis(mt);
        b = dis(mt);
        c = dis(mt);
        d = dis(mt);
        // a = 29;
    }

    std::vector<std::pair<int, int>> generate_point()
    {
        std::vector<std::pair<int, int>> vp;
        int y = 0;
        for (int x = -40; x <= 50; ++x)
        {
            // y = a * x * x * x + b * x * x + c * x + d;
            // y = (x * x) * (a * x + b) + c * x + d;
            y = x * (x * (x * (a) + b) + c) + d;
            vp.emplace_back(std::make_pair(x, y));
        }
        return vp;
    }

    void print()
    {
        std::cout<<"real pamra: "<<a<<", "<<b<<", "<<c<<", "<<d<<std::endl;
    }
private:
    int a,b,c,d;    // a*x^3
};

class xxxx_function
{
public:

    xxxx_function()
    {
        std::mt19937 mt(std::random_device{}());
        std::uniform_int_distribution<int> dis(2, 30);
        a = dis(mt);
        b = dis(mt);
        c = dis(mt);
        d = dis(mt);
        e = dis(mt);
        // a = 29;
    }

    std::vector<std::pair<int, int>> generate_point()
    {
        std::vector<std::pair<int, int>> vp;
        int y = 0;
        for (int x = -40; x <= 50; ++x)
        {
            y = x * (x * (x * (x * (a) + b) + c) + d) + e;
            vp.push_back(std::make_pair(x, y));
        }
        return vp;
    }

    void print()
    {
        std::cout<<"xxxx: "<<a<<", "<<b<<", "<<c<<", "<<d<<", "<<e<<std::endl;
    }
private:
    int a,b,c,d,e;        // a*x^4
};


std::vector<std::pair<int, int>> generatePoint()
{
    // quadratic_function qf;
    // xxx_function qf;
    xxxx_function qf;

    qf.print();
    std::vector<std::pair<int, int>> vp = qf.generate_point();
    return vp;
}

// int cal(int a, int b, int c, int d, int e, int f, int x)
double cal(std::vector<int>& vi, int x)
{
    double t2 = 1.0;
    double ans = 0.0;
    for (int i = vi.size() - 1; i >= 0; --i)
    {
        ans += t2 * vi[i];
        t2 *= x;
    }
    return ans;
}

int main()
{
    // int a, b, c, d, e, f;   // y = a*x^5 + b*x^4 + c*x^3 + d*x^2 + e*x + f
    // a = b = c = d = e = f = 0;

    std::vector<int> vi(6, 0);  // vi[0] * x^5
    // std::vector<int> vi(3, 0);

    std::vector<std::pair<int, int>> vp = generatePoint();
    double llmax = static_cast<double>(std::numeric_limits<long long>::max());
    for (int a = 0; a < vi.size(); ++a)
    {
        double mn = std::numeric_limits<double>::max();
        double sum2 = 0.0;
        int mnv = -12345;
        for (int val = -10; val <= 150; ++val)
        {
            sum2 = 0LL;
            vi[a] = val;
            for (int i = 0; i < vp.size(); ++i)
            {
                double diff = cal(vi, vp[i].first);
                diff -= vp[i].second;
                if (diff > llmax)
                {
                    std::cout<<"error: "<<diff<<std::endl;
                }
                diff *= diff;
                sum2 += diff;
            }
            // std::cout<<sum2<<", "<<mn<<", "<<val<<std::endl;
            if (sum2 < mn)
            {
                mn = sum2;
                mnv = val;
            }
        }
        vi[a] = mnv;
    }


    for (int i = 0; i < vi.size(); ++i)
    {
        std::cout << vi[i] << " x^" << (vi.size() - i - 1) << std::endl;
    }


    // std::cout<<"below is difference result:\n";      // 没什么用，当系数不同的时候，必然(99.999%)全部都不一样的。
    // for (int i = 0; i < vp.size(); ++i)
    // {
    //     // std::cout<<"real: "<<vp[i].second<<", fitting: "<<cal(vi, vp[i].first)<<std::endl;
    //     int d = static_cast<int>(cal(vi, vp[i].first));
    //     if (d != vp[i].second)
    //     {
    //         std::cout<<"diff: real: "<<vp[i].second<<" fitting: "<<d<<std::endl;
    //     }
    // }

    return 0;
}











