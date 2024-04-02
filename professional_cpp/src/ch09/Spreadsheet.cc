module;
#include <cstddef>      // both or none
// #include <stdexcept>

export module spreadsheet;
export import spreadsheet_cell;

// #include <cstddef>       // error
// import <cstddef>;        // ok, just one line

using namespace std;


export class Spreadsheet;       // 必须有 export，不然 main.cpp 中 找不到 Spreadsheet
export void swap(Spreadsheet& first, Spreadsheet& second) noexcept;

// export           // 上面export class 后，这里可以注释掉。 注不注释，移动构造/赋值 中 都看不到 swap(a,b)
class Spreadsheet
{
public:
    // int asdd = 1211;
    // int asd() {
    //     return 123;
    // }
    // int qqq = 12555;
    Spreadsheet(size_t width, size_t height);
    // Spreadsheet(int width, int height);
    ~Spreadsheet();
    Spreadsheet(const Spreadsheet& src);
    Spreadsheet& operator=(const Spreadsheet& rhs);
    void swap(Spreadsheet& other) noexcept;

    Spreadsheet(Spreadsheet&& src) noexcept;
    Spreadsheet& operator=(Spreadsheet&& rhs) noexcept;

    // int qwe();

    void setCellAt(size_t x, size_t y, const SpreadsheetCell& cell);
    SpreadsheetCell& getCellAt(size_t x, size_t y);

    const SpreadsheetCell& getCellAt(size_t x, size_t y) const;

    // void verify(size_t x, size_t y) const;


    // public是为了方便测试。
    // static数据成员， const 数据成员， 引用数据成员，const引用数据成员
    static size_t ms_counter;

    // 按照书上的说法，C++17后，可以将 static 数据成员 声明为 inline， 这样就不必 在文件中 为它分配空间。   kdevelop也提示了 static inline 是 c++17 的
    // 但不行，单纯的 static 是可以的。 (就像上面的)
    // + inline 后 ( 加不加 { 33 } 都一样 )， 编译器会说 重定义 (因为cpp中没有删除 分配空间的语句)
    // cpp中删除 语句后， 编译报错， undefined reference. (加不加 {33} 都一样报错)
    // static inline size_t ms_counter2 {33};

    const size_t m_id { 223344 };        // 构造器中使用 ms_counter

    static const size_t MaxHeight{100};
    static const size_t MaxWidth{100};

    // SpreadsheetApp& app;         // 没试，应该可以。 这个需要在 构造器的 初始化列表中 进行初始化。 外部需要传进来这个东西。
    // const SpreadsheetApp& app;


    // 嵌套类。 可以访问外围类的 private。  外围类只能访问 内部类的 public
    class Cell
    {
    public:
        Cell() = default;
        Cell(double init);
    };

    class Cell2;    // 前置声明

    // 类内枚举
    enum class Color { Red = 1, Green, Blue, Yellow };
    Color m_color { Color::Green };

private:
    void cleanup() noexcept;
    void moveFrom(Spreadsheet& src) noexcept;
    // bool inRange(size_t v, size_t upper) const;
    void verify(size_t x, size_t y) const;
    size_t m_width {0};
    size_t m_height {0};
    SpreadsheetCell** m_cells {nullptr};

};

class Spreadsheet::Cell2
{
public:
    Cell2() = default;
    Cell2(double init);
};




