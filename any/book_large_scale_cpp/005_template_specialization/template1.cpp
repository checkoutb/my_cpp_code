
#include "template1.h"
#include <iostream>


template <> int compare<char>(const char &a, const char &b) {
  std::cout<<"template specialization\n";
  return a - b;
}

int compare(const int &a, const int &b) {
  std::cout<<"normal fun\n";
  return a - b;
}




