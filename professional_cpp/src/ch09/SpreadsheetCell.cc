
export module spreadsheet_cell;

import <string>;
import <string_view>;
import <charconv>;
import <iostream>;

// export struct SpreadsheetCell   // also ok
export class SpreadsheetCell
{
public:
    void setValue(double value);        // in C++20, function define can be with declare.
    [[nodiscard]] double getValue() const;

    void setValue(int) = delete;

    // c++20 模块中 导出的类， 即使 在 类中进行 定义，也不会 默认 inline，需要手动增加。
    // c++20 之前，或 c++20非模块 的类， 如果方法在类中定义， 会默认 inline。
    // inline 只是 提示/建议， 是否真的内联，要看 编译器的决定。
    inline const double& getV2() const & {     // 只能被 非临时对象 调用 。 被左值调用
        std::cout<<"not tmp\n";
        return m_value;
    }
    double&& getV2() && {           // 只能被 右值 调用
        // 临时对象，可以直接 move，节约资源。
        std::cout<<"tmp\n";
        return std::move(m_value);
    }

    void setString(std::string_view value);

    // CV限定符， 声明，定义 都必须， ，因为它 是 函数签名的 一部分。
    std::string getString() const;

    SpreadsheetCell(double initValue);
    SpreadsheetCell(std::string_view initValue);
    SpreadsheetCell();

    // SpreadsheetCell operator +(const SpreadsheetCell& cell) const;

private:
            // 只需要声明时 static， 定义不能写，写了会 编译失败
            // static 方法 不能有 CV 限定符。
    static std::string doubleToString(double value)
    // const
    ;
    static double stringToDouble(std::string_view value)
    // const
    ;
    mutable double m_value {0};
};

export struct Point
{
    double x;
    double y;
};


export SpreadsheetCell operator+(const SpreadsheetCell& lhs, const SpreadsheetCell& rhs);




