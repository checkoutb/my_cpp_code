
module spreadsheet_cell;

// import <string>;

// import <charconv>;       // 放到 cc 文件中了。可以的。
// #include <charconv>

using namespace std;

void SpreadsheetCell::setValue(double value)
{
    m_value = value;
}

// inline 放 定义上
inline double SpreadsheetCell::getValue() const
{
    m_value = m_value;      // m_value 声明为 mutable 后，就可以在 const 方法中进行 赋值(修改)
    return this->m_value;
}

void SpreadsheetCell::setString(string_view value)
{
    this->m_value = stringToDouble(value);
}

std::string SpreadsheetCell::getString() const
{
    return doubleToString(m_value);
}

string SpreadsheetCell::doubleToString(double value)
// const
{
    return to_string(value);
}

// static       // 写了 编译失败
double SpreadsheetCell::stringToDouble(string_view value)
// const        // static 不能有 CV限定符
{
    double number {0};
    // 之前是 from_char， 错误：‘from_char’不是‘std’的成员 。。 以为是 module 的问题。
    std::from_chars(value.data(), value.data() + value.size(), number);
    // number = 123.123;
    return number;
}

// SpreadsheetCell SpreadsheetCell::operator +(const SpreadsheetCell& cell) const
// {
//     return SpreadsheetCell { getValue() + cell.getValue() };
// }

SpreadsheetCell::SpreadsheetCell(string_view sv)
{
    setString(sv);
}

SpreadsheetCell::SpreadsheetCell(double d)
{
    setValue(d);
}

SpreadsheetCell::SpreadsheetCell()
{

}


SpreadsheetCell operator+(const SpreadsheetCell& lhs, const SpreadsheetCell& rhs)
{
    return SpreadsheetCell {lhs.getValue() + rhs.getValue()};
}
