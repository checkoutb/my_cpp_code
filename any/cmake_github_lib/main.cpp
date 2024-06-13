#include <compare>
#include <ios>
#include <iostream>
#include <set>
#include <fmt/format.h>

struct Point {
    int x;
    int y;
    auto operator<=>(const Point&) const = default;
};

int main() {

    Point pt1{1,1}, pt2{1,2};
    std::set<Point> s;
    s.insert(pt1);

    std::cout<<std::boolalpha
             << (pt1 == pt2) << ' '// F
             << (pt1 != pt2) << ' '// T
             << (pt1 < pt2) << ' '// T
             << (pt1 <= pt2) << ' '// T
             << (pt1 > pt2) << ' '// F
             << (pt1 >= pt2) << ' '; // F
    fmt::print("\nhi...\n");
}




