module;
#include <stdexcept>            // 无法放到 cc 文件中， gcm.cache 中没有这个，说明 老式的头文件 必须 在 文件中， 不能从 module 中继承。  。 安装到 gcm.cache 中应该就可以了。 就像cstddef 那样。   就是 import的 可以移到 cc 文件。  #include 的不行。
// #include <format>
#include <utility>

module spreadsheet;

// export import spreadsheet;

// import <cstddef>;        // cc中 #include 了，这里可以不需要了。  加上也不会报错。

// #include <cstddef>
// #include <stdexcept>
// #include <format>
// import <format>;

import <iostream>;


using namespace std;

Spreadsheet::Spreadsheet(std::size_t width, size_t height) : m_id {++ms_counter}, m_width { width }, m_height {height}
{
    cout<<"constructor..."<<endl;
    m_cells = new SpreadsheetCell*[m_width];
    for (size_t i {0}; i < m_width; ++i)
    {
        m_cells[i] = new SpreadsheetCell[m_height];
    }
}

// int Spreadsheet::qwe()
// {
//     return 5567;
// }

Spreadsheet::~Spreadsheet()
{
    cout<<"destructor..."<<endl;
    for (size_t i {0}; i < m_width; ++i)
        delete[] m_cells[i];
    delete[] m_cells;
    m_cells = nullptr;
}

// 拷贝构造函数， 使用 委托构造函数
Spreadsheet::Spreadsheet(const Spreadsheet& src) : Spreadsheet { src.m_width, src.m_height }
{
    cout<<"copy constructor..."<<endl;
    for (size_t i {0}; i < m_width; ++i)
    {
        for (size_t j {0}; j < m_height; ++j)
        {
            m_cells[i][j] = src.m_cells[i][j];
        }
    }
}

// 拷贝赋值， 被赋值， 通常使用 swap，保证 原子性。
Spreadsheet& Spreadsheet::operator=(const Spreadsheet& rhs)
{
    cout<<"copy =..."<<endl;
    Spreadsheet tmp {rhs};
    swap(tmp);
    return *this;
}

void Spreadsheet::swap(Spreadsheet& other) noexcept
{
    std::swap(m_width, other.m_width);      // std::swap 的代码： T temp{std::move(a)}; a = std::move(b); b = std::move(temp);          //使用 move 来 高效 swap
    std::swap(m_height, other.m_height);
    std::swap(m_cells, other.m_cells);
}


void swap(Spreadsheet& first, Spreadsheet& second) noexcept
{
    std::cout<<"swap(a, b)"<<std::endl;
    first.swap(second);
}

void Spreadsheet::cleanup() noexcept
{
    for (size_t i {0}; i < m_width; ++i)
    {
        delete[] m_cells[i];
    }
    delete[] m_cells;
    m_cells = nullptr;
    m_width = m_height = 0;
}

void Spreadsheet::moveFrom(Spreadsheet& src) noexcept
{
    // m_width = src.m_width;
    // m_height = src.m_height;
    // m_cells = src.m_cells;       // 使用 exchange 替换。
    // src.m_width = 0;
    // src.m_height = 0;
    // src.m_cells = nullptr;

    m_width = std::exchange(src.m_width, 0);
    m_height = exchange(src.m_height, 0);
    m_cells = exchange(src.m_cells, nullptr);           // 基本类型 使用 赋值。
    //m_name = std::move(src.m_move);               // >>>>  对象使用 move  <<<<

}

Spreadsheet::Spreadsheet(Spreadsheet&& src) noexcept
{
    std::cout<<"move constructor"<<std::endl;
    // moveFrom(src);           // ok

    // 使用 swap， 更推荐，因为 增加属性的话，只需要修改 swap 中就可以了。 不需要 修改 cleanup, moveFrom。
    // 可以删除 cleanup, moveFrom 方法的定义 声明 了。

    // swap(*this, src);        // 书上是这个， 编译失败了。
                        // 即使将 swap(a,b) 的声明提前到 Spreadsheet 的声明前，还是不行。
                // 把 单参数的 swap 删除后， 这里可以使用 swap(a,b) 了。。。 不知道 为什么 swap(*this, src) 会决议出 单参数的。。
    // swap(src, *this);        // 也不行的。
                // 单参数，那就只可能 是 看到 this 了？ 但也不对。 形参不匹配，怎么可能决议 使用 单参数的。
    // swap(src, src);      // ...不行，  一直都是  备选需要 1 实参，但提供了 2 个 。。
                            // g，将就用下，不要那么讲究。
    this->swap(src);

}
Spreadsheet& Spreadsheet::operator=(Spreadsheet&& rhs) noexcept
{
    std::cout<<"move ="<<std::endl;

    // if (this == &rhs)        // ok
    //     return *this;
    // cleanup();
    // moveFrom(rhs);

    // swap(*this, rhs);
    this->swap(rhs);

    return *this;
}

void Spreadsheet::verify(size_t x, size_t y) const {
    // std::cout<<"1aqwe2"<<std::endl;
    // std::cout<<string("bad y.") + to_string(y);      // 会导致 段错误（核心已转储）， 之前还可以啊。
    // // std::cout<<to_string(y);
    // std::cout<<string("<");
    // std::cout<<to_string(m_height);
    // std::cout<<string("?\n\n\n");
    // std::cout<<string("bad y.") + to_string(y) + string("<") + to_string(m_height) + string("?");
    // std::cout<<"22233 "<<x<<", "<<y<<", "<<m_width<<", "<<m_height<<std::endl;

    if (x >= m_width)
        throw out_of_range {
            // std::format("x. {} < {}?", x, m_width)
            // "x." + x + "<" + m_width + "?"
            "bad x"
            // string("bad x.") + to_string(x) + string("<") + to_string(m_width) + string("?")
        };
    if (y >= m_height)
        throw out_of_range {
            // std::format("y. {} < {}?", y, m_height)
            // to_string("y." + y + "<" + m_height + "?")
            // string("bad y.") + to_string(y) + string("<") + to_string(m_height) + string("?")
            "bad y"
        };
}

void Spreadsheet::setCellAt(size_t x, size_t y, const SpreadsheetCell& cell)
{
    // if (!inRange(x, m_width)) {
    //     throw out_of_range { format("{} must be less then {}.", x, m_width) };
    // }
    // if (!inRange(y, m_height)) {
    //     throw out_of_range { format("{} must < {}.", y, m_height) };
    // }
    verify(x, y);
    // std::cout<<"333"<<std::endl;
    m_cells[x][y] = cell;
}

SpreadsheetCell& Spreadsheet::getCellAt(size_t x, size_t y)
{
    // if (!inRange(x, m_width) || !inRange(y, m_height)) {
    //     throw out_of_range { format("{} < {}? {} < {}?", x, m_width, y, m_height) };
    // }

    // verify(x, y);
    // return m_cells[x][y];

    return const_cast<SpreadsheetCell&>(as_const(*this).getCellAt(x, y));       // <<<<<< as_const 在 <utility>

}

const SpreadsheetCell& Spreadsheet::getCellAt(size_t x, size_t y) const
{
    verify(x, y);
    return m_cells[x][y];
}

// size_t Spreadsheet::ms_counter;         // 必须有。 初始化为0。
size_t Spreadsheet::ms_counter {123555};    // 没有的话 链接失败。 书上也说了， 为 ms_counter 分配空间 并初始化。
                                // 所以 类中的 定义 只是定义， 没有 分配空间。

// static
// size_t Spreadsheet::ms_counter2 { 1244443 };


Spreadsheet::Cell::Cell(double init) {

}

Spreadsheet::Cell2::Cell2(double init) {

}
