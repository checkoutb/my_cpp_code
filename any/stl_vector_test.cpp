
#include <iostream>
#include <vector>

/*

init capacity: 0
change cap, now cap: 1, now size: 1
change cap, now cap: 2, now size: 2
change cap, now cap: 4, now size: 3
change cap, now cap: 8, now size: 5
change cap, now cap: 16, now size: 9
change cap, now cap: 32, now size: 17
change cap, now cap: 64, now size: 33
change cap, now cap: 128, now size: 65
change cap, now cap: 256, now size: 129
change cap, now cap: 512, now size: 257
change cap, now cap: 1024, now size: 513
change cap, now cap: 2048, now size: 1025
change cap, now cap: 4096, now size: 2049
...
change cap, now cap: 524288, now size: 262145
change cap, now cap: 1048576, now size: 524289
=================
@ last: 1048576, 1

不收缩， 1000万也不收缩

*/




/*
      /**
       *  @brief  Removes last element.
       *
       *  This is a typical stack operation. It shrinks the %vector by one.
       *
       *  Note that no data is returned, and if the last element's
       *  data is needed, it should be retrieved before pop_back() is
       *  called.

      _GLIBCXX20_CONSTEXPR
      void
      pop_back() _GLIBCXX_NOEXCEPT
      {
        __glibcxx_requires_nonempty();
        --this->_M_impl._M_finish;
        _Alloc_traits::destroy(this->_M_impl, this->_M_impl._M_finish);
        _GLIBCXX_ASAN_ANNOTATE_SHRINK(1);
      }
*/
// 。。。 最后那行 SHRINK(1) ，看 #define 的 是空的。不知道怎么找。


// 只能 shrink_to_fit 了
/*
change cap, now cap: 512, now size: 257
change cap, now cap: 1024, now size: 513
=================
@ last: 1024, 1
@@ 1, 1
*/


int main() {
    std::vector<int> vi;
    std::cout<<"init capacity: "<<vi.capacity()<<std::endl;
    int cap = vi.capacity();
    vi.push_back(1133);
    auto it = vi.begin();
    // vi.push_back(1111);  // 段错误，因为先获得it，然后push_back，导致 重新申请内存，导致 it 失效
    std::cout<<(*it)<<std::endl;
    for (int i = 0; i < 1000; ++i) {
        vi.push_back(i);
        if (cap != vi.capacity()) {
            cap = vi.capacity();
            std::cout<<"change cap, now cap: "<<cap<<", now size: "<<vi.size()<<std::endl;

            // 输出中： vi地址不变 vi[0]每次都变
            std::cout<<" -- "<<&vi<<", "<<&(vi[0])<<std::endl;
        }
    }

    std::cout<<"=================\n";

    while (!vi.empty()) {
        vi.pop_back();
        if (cap != vi.capacity()) {
            cap = vi.capacity();
            std::cout<<"cap: "<<cap<<", size: "<<vi.size()<<std::endl;
        }
    }
    vi.push_back(1);
    std::cout<<"@ last: "<<vi.capacity()<<", "<<vi.size()<<std::endl;

    vi.shrink_to_fit();
    std::cout<<"@@ "<<vi.capacity()<<", "<<vi.size()<<std::endl;
    
    return 0;
}

