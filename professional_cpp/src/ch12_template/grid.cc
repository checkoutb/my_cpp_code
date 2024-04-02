// module;
// #include <vector>

export module grid;

import <optional>;
import <vector>;
import <cstddef>;   // size_t
import <stdexcept>;     // out_of_range
import <format>;
import <utility>;   // std::as_const
import <iostream>;

export template<typename T>
class Grid2
{
public:
    explicit Grid2() {
        vi.resize(5);
    }
// private:
    std::vector<std::vector<std::optional<T>>> vi;
};

export template<typename T, size_t W = 10, size_t H = 10>
class Grid3
{
public:
    explicit Grid3() {
        vi.resize(5);
    }

    template<typename E, size_t W2, size_t H2>
    Grid3(const Grid3<E, W2, H2>& src);

private:
    std::vector<std::vector<std::optional<T>>> vi;
};

template<typename T, size_t W, size_t H>
template<typename E, size_t W2, size_t H2>
Grid<T, W, H>::Grid(const Grid<E, W2, H2>& src)
{
    // for for if < && < m=src.at(i,j) else m.reset()
}


// export template<>        // 告知编译器，这个类是 Grid<T> 的 const char* 特化. 直接写 class Grid 是不行的，因为已有一个 Grid 的模板类了。
// class Grid<const char*>
// {
//      // ...
// }

// 方法定义时 不需要 template<>
// Grid<const char*>::Grid(size_t w, size_t h) : m_w{w}, m_h{h} {}

export template<typename T>
class GameBoard : public Grid2<T>       // Grid 在下面，需要前向声明才能用。
{

};



////////////////

using MyInt = GameBoard<int>;       // 模板别名。。。 myvvi, myvi      typedef 可以做到

export template<typename T, typename E>
class MyTemplate {};

template<typename T>
using MyA = MyTemplate<T, int>;     // typedef 无法做到

//////////////////

template<typename T1, typename T2>
auto add(const T1& t1, const T2& t2) { return t1 + t2; }        // c++14 可以写 auto

template<typename T1, typename T2>
decltype(auto) add(const T1& t1, const T2& t2) { return t1 + t2; }        // decltype 不会丢掉 const 和 引用

// template<typename T1, typename T2>
// decltype(t1 + t2) add(const T1& t1, const T2& t2) { return t1 + t2; }    // c++14之前，没有auto。 不过这种是错误的，因为 decltype(t1 + t2) 的时候 t1,t2 还没有出现

template<typename T1, typename T2>
auto add(const T1& t1, const T2& t2) -> decltype(t1+t2)     // c++11 使用 返回类型后置。
{
    return t1 + t2;
}


//////////////

decltype(auto) add(const auto& t1, const auto& t2) { return t1 + t2; }  // c++20 简化函数模板
// 参数列表中的 auto 会成为 不同的 模板类型参数。

/////////////

template<typename T>
constexpr T pi { T{3.141592653123123123 }};     // c++14, 变量模板

float pif {pi<float>};
auto pild {pi<long double>};

// c++20, <numbers>， 定义了 常用的数学常量， std::numbers::pi





export template<typename T>
class Grid
{
public:
    explicit Grid(size_t width = DefaultWidth, size_t height = DefaultHeight);
    virtual ~Grid() = default;

    Grid(Grid&& src) = default;
    Grid& operator=(Grid&& rhs) = default;      // 可以 Grid<T>。 编译器对于 类中 Grid 会解释为 Grid<T>， 类外 必须 <T>

    std::optional<T>& at(size_t x, size_t y);
    const std::optional<T>& at(size_t x, size_t y) const;

    size_t getHeight() const { return m_height; }
    size_t getWidth() const { return m_width; }

    static const size_t DefaultWidth { 10 };
    static const size_t DefaultHeight { 10 };


    template<typename E>
    Grid(const Grid<E>& src);


private:
    void verifyCoordinate(size_t x, size_t y) const;

    std::vector<std::vector<std::optional<T>>> m_cells;
    // std::vector<int> m_cells;

    size_t m_width { 0 }, m_height { 0 };
};

template<typename T>        // 必须分开，不能 template<typename T, typename E>
template<typename E>
Grid<T>::Grid(const Grid<E>& src) : Grid {src.getWidth(), src.getHeight()}
{
    // for { for { m_cells[i][j] = src.at(i,j) }}
}

template<typename T>
Grid<T>::Grid(size_t w, size_t h) : m_width {w}, m_height {h}
{
    // std::cout<<m_width<<", "<<m_height<<", "<<m_cells.size()<<"\n";
    m_cells.resize(m_width);
    for (auto& column : m_cells)
        column.resize(m_height);
    std::cout<<m_cells.size()<<" -- \n";
}

template<typename T>
void Grid<T>::verifyCoordinate(size_t x, size_t y) const
{
    if (x >= m_width)
        throw std::out_of_range {
            // std::format("{} must < {}", x, m_width)
            // std::format("asd")
            "x > m_w"
        };
    if (y >= m_height)
        throw std::out_of_range { "y < m_height" };
}

template<typename T>
const std::optional<T>& Grid<T>::at(size_t x, size_t y) const
{
    verifyCoordinate(x, y);
    return m_cells[x][y];
}

template<typename T>
std::optional<T>& Grid<T>::at(size_t x, size_t y)
{
    return const_cast<std::optional<T>&>(std::as_const(*this).at(x, y));
}


// int main2()
// {
//     std::vector<int> vi;
//     vi.resize(10);
//     std::cout<<vi.size()<<std::endl;
//
//     Grid2<int> g2;
//     std::cout<<g2.vi.size()<<std::endl;
//
//     Grid<int> g3;
//     return 0;
// }
