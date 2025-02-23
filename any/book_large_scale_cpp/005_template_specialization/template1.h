
#include <iostream>
#include <typeinfo>

template <typename T> int compare(const T& a, const T& b) {
  std::cout<<"template  "<<typeid(a).name()<<std::endl;
  if (a > b) {
    return 1;
  } else if (a == b){
    return 0;
  } else {
    return -1;
  }
}

template <> int compare<char>(const char &a, const char &b);

int compare(const int &a, const int &b);


// 特化 和 普通函数一样，不能定义2次，所以要么 inline，要么 声明和定义分离
template <>
inline
int compare<double>(const double &a, const double &b) {
  std::cout<<"specialization\n";
  return a - b;
}
