module;
#include <cstddef>


export module mytest:define;

import <vector>;
import <optional>;
// import <cstddef>;
import <iostream>;

export template<typename T>
class MyGrid {
public:
    MyGrid(
        int sz
    );

    // virtual ~MyGrid() = default;

    std::optional<T> getVal(int idx);
    void setVal(int idx, T val);
private:
    std::vector<std::optional<T>> m_vt;
};


/*
template<typename T>
MyGrid<T>::MyGrid(
    int sz
) {
    int sz2 = 13;
    std::cout<<"1 mygrid.constructor\t"<<sz2<<"\n";
    m_vt.resize(sz2);

};

template<typename T>
std::optional<T> MyGrid<T>::getVal(int idx) {
    // or m_vt.at(idx), it will exception
    // return idx >= this->m_vt.size() ? std::optional<T>{} : this->m_vt[idx];
    std::cout<<m_vt.size()<<"\n"<<&idx;
    return m_vt[32];
    // return m_vt.at(32);
    // return std::make_optional(7654);
};

template<typename T>
void MyGrid<T>::setVal(int idx, T val) {
    // if (idx < m_vt.size())
        // m_vt[idx] = std::make_optional<T>(val);
    // *m_vt[idx] = val;
};*/
