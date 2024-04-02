module;
#include <cstddef>


export module mygrid_v1;

import <iostream>;
import <vector>;
import <stdexcept>;
import <utility>;
import <optional>;


// 大部分 e <
template <typename Container>
concept ContainerType = requires(Container c)
{
    c.resize(1);
    typename Container::value_type;
};

// export template <typename T, ContainerType Container>
export template <typename T, ContainerType Container = std::vector<std::optional<T>>>
class Grid
{
public:
    explicit Grid(size_t w = DefaultW, size_t h = DefaultH);
    virtual ~Grid() = default;

    Grid(const Grid& src) = default;
    Grid& operator=(const Grid& rhs) = default;

    Grid(Grid&& src) = default;
    Grid& operator=(Grid&& rhs) = default;

    typename Container::value_type& at(size_t x, size_t y);
    const typename Container::value_type& at(size_t x, size_t y) const;

    size_t getHeight() const {return m_height;}
    size_t getWidth() const {return m_width;}

    static const size_t DefaultW {10};
    static const size_t DefaultH {10};

private:
    void verify(size_t x, size_t y) const;

    std::vector<Container> m_cells;
    size_t m_width {0}, m_height {0};
};

template <typename T, ContainerType Container>
Grid<T, Container>::Grid(size_t w, size_t h) : m_width{w}, m_height{h}
{
    this->m_cells.resize(m_width);
    for (auto& column : m_cells)
    {
        column.resize(m_height);
    }
}

template <typename T, ContainerType Container>
void Grid<T, Container>::verify(size_t x, size_t y) const
{
    if (x >= m_width)
    {
        throw std::out_of_range {"xxxxxx"};
    }
    if (y >= m_height)
    {
        throw std::out_of_range {"yyyyyy"};
    }
}

template <typename T, ContainerType Container>
const typename Container::value_type& Grid<T, Container>::at(size_t x, size_t y) const
{
    verify(x, y);
    return this->m_cells[x][y];
}

template <typename T, ContainerType Container>
typename Container::value_type& Grid<T, Container>::at(size_t x, size_t y)
{
    return const_cast<typename Container::value_type&>(std::as_const(*this).at(x,y));
}


