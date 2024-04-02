/*
module grid;

// import <optional>;
// import <vector>;
// import <cstddef>;   // size_t
// import <stdexcept>;     // out_of_range
// import <format>;
// import <utility>;   // std::as_const

// 类模板中方法的定义 需要对所有 使用该模板的客户代码 可见，这对 放置 方法定义的位置有一些限制。
//     通常，它们和 类模板定义本身 放在相同的文件中。
// 。。再说了，先分开试下。 书上也说了，稍后会讨论 绕过限制的方法。

// 还真不行。undefined reference to `Grid@grid<int>::Grid(unsigned long, unsigned long)'
// 移到 定义文件试下

template<typename T>
Grid<T>::Grid(size_t w, size_t h) : m_width {w}, m_height {h}
{
    m_cells.resize(m_width);
    for (auto& column : m_cells)
        column.resize(m_height);
}

template<typename T>
void Grid<T>::verifyCoordinate(size_t x, size_t y) const
{
    if (x >= m_width)
        throw std::out_of_range { std::format("{} must < {}", x, m_width) };
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

*/
