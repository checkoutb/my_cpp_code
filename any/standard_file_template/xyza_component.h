// xyza_component.h    -*-C++-*-

#ifndef INCLUDED_XYZA_COMPONENT
#define INCLUDED_XYZA_COMPONENT

// @PURPOSE: xxxx(one line)
//
// @CLASS:
//   xyza::Class1;
//   xyza::Class2;
//
// @DESCRIPTION: xxxxxx
//
/// Usage                            // 提供使用样例
/// xxx
//

#include <xyza_conponent1.h>         // 同一包  // xyz是包组，xyza是包
#include <xyza_conponent2.h>                   // 按包组编译: xyz.o
#include <xyza_conponent3.h>         // 总使用<>

#include <xyzb_conponent1.h>         // 同一包组，不同包
#include <xyzb_conponent2.h>
#include <xyzc_conponent1.h>
#include <xyzq_conponent1.h>

#include <abcq_conponent1.h>         // 不同包组
#include <abcq_conponent2.h>

#include <iosfwd>                    // 标准库

namespace MyLongCompanyName {        // 企业级命名空间

namespace abcz { class ClassA; }          // 其他包(没有include)中定义的类型的局部声明
namespace qrsy { class ClassB; }
namespace qrsy { class ClassA; }

namespace xyza {                     // 包级命名空间

class Class1;                        // 当前包中定义的类型的局部声明
class Class2;

class Class1 {

public:
    Class1();
    int func1(qrsy::ClassA *obj);    // 非内联，定义在 .cpp 中
    int func2(qrsy::ClassB *obj);    // 内联函数的声明，定义在本文件的稍后位置，定义时inline
};

bool operator==(const Class1& lhs, const Class1& rhs);   // 这里只是声明

std::ostream& operator<<(std::ostream& stream, const Class1& obj);

void swap(Class1& a, Class1& b);


class Class2 {
    // ...
};
 

// =============================
//  INLINE FUNCTION DEFINITIONS
// =============================

inline int Class1::func2(qrsy::ClassB *obj) {
    // ...
}

// ... 其他的内联函数的定义， 不包括 自由函数的定义

} // close package namespace xyza

inline bool xyza::operator==(const Class1& lhs, const Class1& rhs) {   // 自由函数的定义
    // ...
}

inline void xyza::swap(Class1 &a, Class1 &b) {
    // ...
}

 

namespace abcmf {

template<>
struct IsBig<xyza::Class1> : std::true_type {};     // 其他包中模板的特化

}
 
} // close enterprise namespace MyLongCompanyName


#endif // INCLUDED_XYZA_COMPONENT

