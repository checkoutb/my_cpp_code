module;
#include <cstddef>


export module mygrid_v2;

import <iostream>;
import <vector>;
import <stdexcept>;
import <utility>;
import <optional>;

export namespace mygrid_v2_ref
{

// 大部分 e <
template <typename Container>
concept ContainerType = requires(Container c)
{
    c.resize(1);
    typename Container::value_type;
};



// template <typename T, const T& DEFAULT>
// class Grid
// {
// public:
// 	explicit Grid(size_t width = DefaultWidth, size_t height = DefaultHeight);
// 	virtual ~Grid() = default;
//
// 	// Explicitly default a copy constructor and copy assignment operator.
// 	Grid(const Grid& src) = default;
// 	Grid& operator=(const Grid& rhs) = default;
//
// 	// Explicitly default a move constructor and move assignment operator.
// 	Grid(Grid&& src) = default;
// 	Grid& operator=(Grid&& rhs) = default;
//
// 	std::optional<T>& at(size_t x, size_t y);
// 	const std::optional<T>& at(size_t x, size_t y) const;
//
// 	size_t getHeight() const { return m_height; }
// 	size_t getWidth() const { return m_width; }
//
// 	static const size_t DefaultWidth{ 10 };
// 	static const size_t DefaultHeight{ 10 };
//
// private:
// 	void verifyCoordinate(size_t x, size_t y) const;
//
// 	std::vector<std::vector<std::optional<T>>> m_cells;
// 	size_t m_width { 0 }, m_height { 0 };
// };
//
// template <typename T, const T& DEFAULT>
// Grid<T, DEFAULT>::Grid(size_t width, size_t height)
// 	: m_width{ width }
// 	, m_height{ height }
// {
// 	m_cells.resize(m_width);
// 	for (auto& column : m_cells) {
// 		column.resize(m_height);
// 		for (auto& element : column) {
// 			element = DEFAULT;
// 		}
// 	}
// }
//
// template <typename T, const T& DEFAULT>
// void Grid<T, DEFAULT>::verifyCoordinate(size_t x, size_t y) const
// {
// 	if (x >= m_width) {
// 		throw std::out_of_range{ std::format("{} must be less than {}.", x, m_width) };
// 	}
// 	if (y >= m_height) {
// 		throw std::out_of_range{ std::format("{} must be less than {}.", y, m_height) };
// 	}
// }
//
// template <typename T, const T& DEFAULT>
// const std::optional<T>& Grid<T, DEFAULT>::at(size_t x, size_t y) const
// {
// 	verifyCoordinate(x, y);
// 	return m_cells[x][y];
// }
//
// template <typename T, const T& DEFAULT>
// std::optional<T>& Grid<T, DEFAULT>::at(size_t x, size_t y)
// {
// 	return const_cast<std::optional<T>&>(std::as_const(*this).at(x, y));
// }


// template template 语法
// template <..., template <TemplateTypeParams> class ParamterName, ...>
// C++17开始，也可以用 typename 代替 class

// export template <typename T, ContainerType Container>
// export template <typename T, ContainerType Container = std::vector<std::optional<T>>>
// export       // 只能一个 export， export namespace 后，这里就不能 export了
// template <typename T, template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector, const T DEFAULT = T()>        // DEFAULT 非类型参数






// 使用 ref 的时候，不能有 Container，即使 方法内部没有用到 Container。
// 会报错： ‘DEFAULT’没有默认参数


template <typename T,
// template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector,
const T& DEFAULT>
class Grid
{
public:
    explicit Grid(size_t w = DefaultW, size_t h = DefaultH);
    virtual ~Grid() = default;

    Grid(const Grid& src) = default;
    Grid& operator=(const Grid& rhs) = default;

    Grid(Grid&& src) = default;
    Grid& operator=(Grid&& rhs) = default;

    // typename Container::value_type& at(size_t x, size_t y);
    // const typename Container::value_type& at(size_t x, size_t y) const;

    // std::optional<T>& at(size_t x, size_t y);
    const std::optional<T>& at(size_t x, size_t y) const;

    size_t getHeight() const {return m_height;}
    size_t getWidth() const {return m_width;}

    static const size_t DefaultW {10};
    static const size_t DefaultH {10};

private:
    void verify(size_t x, size_t y) const;

    // std::vector<Container> m_cells;
    // std::vector<Container<std::optional<T>>> m_cells;
    std::vector<std::vector<std::optional<T>>> m_cells;
    size_t m_width {0}, m_height {0};
};





///////////////////////////////////

template <typename T,
// template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector,
const T& DEFAULT>
Grid<T,
// Container,
DEFAULT>::Grid(size_t width, size_t height)
	: m_width{ width }
	, m_height{ height }
{
	m_cells.resize(m_width);
	for (auto& column : m_cells) {
		column.resize(m_height);
		for (auto& element : column) {
			element = DEFAULT;
		}
	}
}

template <typename T,
// template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector,,
const T& DEFAULT>
const std::optional<T>& Grid<T,
// Container,
DEFAULT>::at(size_t x, size_t y) const
{
	// verifyCoordinate(x, y);
	return m_cells[x][y];
}


// // template <typename T, ContainerType Container>
// // template <typename T, template <typename E, typename Allocator = std::allocator<E>> typename Container, const T DEFAULT>
// template <typename T, template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector, const T& DEFAULT>
// Grid<T, Container, DEFAULT>::Grid(size_t w, size_t h) : m_width{w}, m_height{h}
// {
//     this->m_cells.resize(m_width);
//     for (auto& column : m_cells)
//     {
//         column.resize(m_height);
//
//         ///////////////////////
//         for (auto& e : column) {
//             e = DEFAULT;
//         }
//     }
// }
//
// // template <typename T, ContainerType Container>
// // template <typename T, template <typename E, typename Allocator = std::allocator<E>> typename Container, const T DEFAULT>
// template <typename T, template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector, const T& DEFAULT>
// void Grid<T, Container, DEFAULT>::verify(size_t x, size_t y) const
// {
//     if (x >= m_width)
//     {
//         throw std::out_of_range {"xxxxxx"};
//     }
//     if (y >= m_height)
//     {
//         throw std::out_of_range {"yyyyyy"};
//     }
// }
//
// // template <typename T, ContainerType Container>
// // template <typename T, template <typename E, typename Allocator = std::allocator<E>> typename Container, const T DEFAULT>
// template <typename T, template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector, const T& DEFAULT>
// // const typename Container::value_type&
// const std::optional<T>&
// Grid<T, Container, DEFAULT>::at(size_t x, size_t y) const
// {
//     verify(x, y);
//     return this->m_cells[x][y];
// }
//
// // template <typename T, ContainerType Container>
// // template <typename T, template <typename E, typename Allocator = std::allocator<E>> typename Container, const T DEFAULT>
// template <typename T, template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector, const T& DEFAULT>
// // typename Container::value_type&
// std::optional<T>&
// Grid<T, Container, DEFAULT>::at(size_t x, size_t y)
// {
//     // return const_cast<typename Container::value_type&>(std::as_const(*this).at(x,y));
//     return const_cast<std::optional<T>&>(std::as_const(*this).at(x,y));
// }

}


export namespace mygrid_v2
{

// 大部分 e <
template <typename Container>
concept ContainerType = requires(Container c)
{
    c.resize(1);
    typename Container::value_type;
};

// template template 语法
// template <..., template <TemplateTypeParams> class ParamterName, ...>
// C++17开始，也可以用 typename 代替 class

// export template <typename T, ContainerType Container>
// export template <typename T, ContainerType Container = std::vector<std::optional<T>>>
// export       // 只能一个 export， export namespace 后，这里就不能 export了
template <typename T, template <typename E, typename Allocatot = std::allocator<E>> typename Container = std::vector, const T DEFAULT = T()>        // DEFAULT 非类型参数
class Grid
{
public:
    explicit Grid(size_t w = DefaultW, size_t h = DefaultH);
    virtual ~Grid() = default;

    Grid(const Grid& src) = default;
    Grid& operator=(const Grid& rhs) = default;

    Grid(Grid&& src) = default;
    Grid& operator=(Grid&& rhs) = default;

    // typename Container::value_type& at(size_t x, size_t y);
    // const typename Container::value_type& at(size_t x, size_t y) const;

    std::optional<T>& at(size_t x, size_t y);
    const std::optional<T>& at(size_t x, size_t y) const;

    size_t getHeight() const {return m_height;}
    size_t getWidth() const {return m_width;}

    static const size_t DefaultW {10};
    static const size_t DefaultH {10};

private:
    void verify(size_t x, size_t y) const;

    // std::vector<Container> m_cells;
    std::vector<Container<std::optional<T>>> m_cells;
    size_t m_width {0}, m_height {0};
};





///////////////////////////////////






// template <typename T, ContainerType Container>
template <typename T, template <typename E, typename Allocator = std::allocator<E>> typename Container, const T DEFAULT>
Grid<T, Container, DEFAULT>::Grid(size_t w, size_t h) : m_width{w}, m_height{h}
{
    this->m_cells.resize(m_width);
    for (auto& column : m_cells)
    {
        column.resize(m_height);

        ///////////////////////
        for (auto& e : column) {
            e = DEFAULT;
        }
    }
}

// template <typename T, ContainerType Container>
template <typename T, template <typename E, typename Allocator = std::allocator<E>> typename Container, const T DEFAULT>
void Grid<T, Container, DEFAULT>::verify(size_t x, size_t y) const
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

// template <typename T, ContainerType Container>
template <typename T, template <typename E, typename Allocator = std::allocator<E>> typename Container, const T DEFAULT>
// const typename Container::value_type&
const std::optional<T>&
Grid<T, Container, DEFAULT>::at(size_t x, size_t y) const
{
    verify(x, y);
    return this->m_cells[x][y];
}

// template <typename T, ContainerType Container>
template <typename T, template <typename E, typename Allocator = std::allocator<E>> typename Container, const T DEFAULT>
// typename Container::value_type&
std::optional<T>&
Grid<T, Container, DEFAULT>::at(size_t x, size_t y)
{
    // return const_cast<typename Container::value_type&>(std::as_const(*this).at(x,y));
    return const_cast<std::optional<T>&>(std::as_const(*this).at(x,y));
}

}







