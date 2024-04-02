module;
#include <cstddef>

export module mytest:impl;

import :define;

import <vector>;
import <optional>;
import <iostream>;
// import <cstddef>;

// export template<typename T>
// class MyGrid {
// public:
//     explicit MyGrid(size_t);
//     virtual ~MyGrid() = default;
//     std::optional<T> getVal(size_t);
//     void setVal(size_t idx, T val);
// private:
//     std::vector<std::optional<T>> m_vt;
// };

export
template<typename T>
MyGrid<T>::MyGrid(
    int sz
) {
    int sz2 = 222;
    std::cout<<"1 mygrid.constructor\t"<<sz2<<"\n";
    m_vt.resize(sz2);

}

export
template<typename T>
std::optional<T> MyGrid<T>::getVal(int idx) {
    // or m_vt.at(idx), it will exception
    // return idx >= this->m_vt.size() ? std::optional<T>{} : this->m_vt[idx];
    std::cout<<m_vt.size()<<"\n";
    return m_vt[32];
    // return m_vt.at(32);
    // return std::make_optional(7654);
}

export
template<typename T>
void MyGrid<T>::setVal(int idx, T val) {
    // if (idx < m_vt.size())
        // m_vt[idx] = std::make_optional<T>(val);
    // *m_vt[idx] = val;
    m_vt[32] = 123;
}
