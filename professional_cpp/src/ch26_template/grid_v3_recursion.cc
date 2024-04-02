module;
#include <cstddef>


export module mygrid_v3;

import <iostream>;
import <vector>;
import <stdexcept>;
import <utility>;
import <optional>;


export namespace mygridv3
{

    template <typename T, size_t N>
    class NGrid
    {
    public:
        explicit NGrid(size_t size = DefaultSize) { resize(size); }
        virtual ~NGrid() = default;

        NGrid<T, N-1>& operator[](size_t x) { return m_elements[x]; }
        const NGrid<T, N-1>& operator[](size_t x) const { return m_elements[x]; }
        void resize(size_t newsz)
        {
            m_elements.resize(newsz);
            for (auto& e : m_elements)
            {
                e.resize(newsz);
            }
        }
        size_t getSize() const { return m_elements.size(); }
        static const size_t DefaultSize { 10 };

    private:
        std::vector<NGrid<T, N-1>> m_elements;
    };


//////////////////////////////
    template <typename T>
    class NGrid<T, 1>
    {
    public:
        explicit NGrid(size_t size = DefaultSize) { resize(size); }
        virtual ~NGrid() = default;

        T& operator[](size_t x) { return m_elements[x]; }
        const T& operator[](size_t x) const { return m_elements[x]; }

        void resize(size_t newsz) { m_elements.resize(newsz); }
        size_t getSize() const { return m_elements.size(); }

        static const size_t DefaultSize { 10 };
    private:
        std::vector<T> m_elements;
    };


}

