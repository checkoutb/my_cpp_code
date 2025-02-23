
#ifdef __cplusplus
extern "C" {
#endif

  void fun_cpp();
  
#ifdef __cplusplus
}
#endif

/*
.

`g++ -c be_invoked_cpp.cpp`
`gcc -c main.c`
`g++ main.o be_invoked_cpp.o -o a.out`  // link 必须用g++。
    // 用 gcc link，报错： undefined reference: `std::cout`
    
*/

int main() {

  fun_cpp();
  
  return 0;
}
