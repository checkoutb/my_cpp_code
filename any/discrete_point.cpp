
#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <algorithm>


// 不知道 邻域怎么找。
// 距离。 二分搜索 距离？
// 找到 max {点 和 其他点的 最小距离}

// https://blog.51cto.com/u_16213631/9462399
// 代码是根据这里的1.2 来的，描述如下
/*
1. 查找每个点的所有邻域点
        // 邻域点，是什么，怎么计算，我不知道，
        // 目前是 找到 max {点 和 其他点的 最小距离} ，只要距离小于 这个值，就当作 邻域点
        // 看下面的描述，感觉 是固定的 k 个点， 是 固定的 k个距离最近的点。  但是 无法确定。
2. 计算每个点到其邻居的距离 dij, i表示一共m个点，j表示一共k个邻居
3. 根据高斯分布 模型化距离参数，计算 所有点 与领域的 距离的均值，及距离的标准差

1. 为每个点，计算其与邻居的距离均值
2. 遍历所有点，如果其距离均值 大于高斯分布的 指定 置信度 则移除。
*/

// 主要是没有找到 算法。
// 我看到还有一种是 以每个点 自己为圆心，画圆， 通过调节半径， 来 找出 圆中 点最少的 的圆心， 剔除这些圆心。

// 还有 样本 是 单点，多点，直线，多条直线，曲线 ？

// 或者是 图像噪声？ 不一样，图像噪声，是对图片的信息的一些xx。   这里想的是 找到离散点。


// 这里可以调整的 是2个地方， 生成点时的 stddev， 判断是否离散时，diff的倍数。
//      还有 cnt_neighbour， 决定 用 最小距离 还是 固定个数的点

// 最大最小距离 和 固定个数， 感觉差不多。 至少 这个样本差不多。 。
//      固定个数的话，修改个数，也会导致 结果偏差的。

// 个数 可以根据  样本总数 来设置， 比如 样本总数/5  之类的
// 最小距离，也可以 通过 TP95 之类的， 而不是 取 最大的最小， 可以取 第三大的最小， 不行，这样的话， 更偏的点 没有 邻居了。




std::vector<std::pair<double, double>> generate_points()
{
    // normal distribution 正态分布
    std::random_device rd;
    std::mt19937 gen(rd());
    double m = 0;   // mean
    double s = 4.0; // stddev
    std::normal_distribution<double> distr(m, s);

    // 100 * 100 2D

    // (10, 10), (30,70), (70.50)
    std::vector<std::pair<double, double>> vpdd {{10.0, 10.0},{30.0, 70.0},{75.0,50.0}};
    std::vector<std::pair<double, double>> vp;
    for (int i = 0; i < vpdd.size(); ++i)
    {
        int cnt = (i + 3) * 2 + 5;  // 11, 13, 15
        while (cnt-- > 0)
        {
            double x = vpdd[i].first + distr(gen);
            double y = vpdd[i].second + distr(gen);
            std::cout<<x<<", "<<y<<std::endl;
            vp.emplace_back(std::make_pair(x, y));
        }
        std::cout<<"\n\n=======\n\n";
    }

    vp.push_back(std::make_pair(20,20));
    vp.push_back(std::make_pair(40,20));

    return vp;
}


double distance_pow2(double x1, double y1, double x2, double y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

int main()
{

    std::vector<std::pair<double, double>> vp = generate_points();

    double mxlen = 0.0;
    for (int i = 0; i < vp.size(); ++i)
    {
        double mnlen = 123456789;
        double x = vp[i].first;
        double y = vp[i].second;
        for (int j = 0; j < vp.size(); ++j)
        {
            if (i == j)
                continue;
            mnlen = std::min(mnlen, distance_pow2(x,y, vp[j].first, vp[j].second));
        }
        mxlen = std::max(mxlen, mnlen);
    }
    mxlen += 0.02;      // 固定长度 还是 固定个数(就是距离最近的x个点)？ 好像是 固定个数。。。

    std::vector<double> vDis;
    std::vector<std::vector<double>> vvd(vp.size());

    int cnt_neighbour = 3;      // <=0 则 使用 mxlen ， 整数 就是 使用 最近的 cnt_neighbour 个邻居

    if (cnt_neighbour <= 0)
    {
        for (int i = 0; i < vp.size(); ++i)
        {
            double len = 0.0;
            double x = vp[i].first;
            double y = vp[i].second;
            for (int j = 0; j < vp.size(); ++j)
            {
                if (i == j)
                    continue;

                len = distance_pow2(x, y, vp[j].first, vp[j].second);
                if (len > mxlen)
                    continue;

                vDis.push_back(std::sqrt(len));
                vvd[i].push_back(vDis.back());
            }
        }
    }
    else
    {
        for (int i = 0; i < vp.size(); ++i)
        {
            double len = 0.0;
            double x = vp[i].first;
            double y = vp[i].second;

            for (int j = 0; j < vp.size(); ++j)
            {
                if (i == j)
                    continue;

                len = distance_pow2(x, y, vp[j].first, vp[j].second);
                vvd[i].push_back(len);
            }
            std::sort(std::begin(vvd[i]), std::end(vvd[i]));
            vvd[i].resize(cnt_neighbour);
            for (int j = 0; j < vvd[i].size(); ++j)
            {
                vvd[i][j] = std::sqrt(vvd[i][j]);
            }
            vDis.insert(std::begin(vDis), std::begin(vvd[i]), std::end(vvd[i]));
        }
    }

    double avg = std::accumulate(std::begin(vDis), std::end(vDis), 0.0) / vDis.size();
    double diff = 0.0;
    for (int i = 0; i < vDis.size(); ++i)
    {
        diff += (vDis[i] - avg) * (vDis[i] - avg);
    }
    diff /= vDis.size();    // 方差， 样本 与 样本avg 的 差的 平方 的 平均数
    diff = std::sqrt(diff); // 标准差， sqrt(方差)

    double tmn = (avg - diff * 1.5);
    double tmx = (avg + diff * 1.5);
    std::cout<<tmn<<" - "<<tmx<<std::endl;

    for (int i = 0; i < vvd.size(); ++i)
    {
        double t2 = std::accumulate(std::begin(vvd[i]), std::end(vvd[i]), 0.0) / vvd[i].size();
        if (t2 < tmn || t2 > tmx)
        {
            std::cout<<"       delete "<<vp[i].first<<", "<<vp[i].second<<". t2:"<<t2<<std::endl;
        }
        else
        {
            std::cout<<"chose "<<vp[i].first<<", "<<vp[i].second<<" "<<t2<<std::endl;
        }
    }

    return 0;
}


