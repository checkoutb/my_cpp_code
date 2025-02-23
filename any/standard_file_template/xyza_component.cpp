// xyza_component    -*-C++-*-

// xxxx                        // 对实现的叙述
// xx
// xxxx

#include <xyza_component.h>    // 总使用<>

#include <xyza_component2.h>   // 顺序也是：同包，同包组，不同包组，标准库
#include <xyzq_component1.h>

#include <qrsg_component.h>

#include <iostream>

namespace MyLongCompanyName {
namespace xyza {

namespace {

} // close unnamed namespace
    

int Class1::func1(qrsy::ClassA *obj) {
    return -1;
}
} // close package namespace xyza


std::ostream& xyza::operator<<(std::ostream& stream, const Class1& obj) {

}

} // close enterprise namespace MyLongcompanyname


