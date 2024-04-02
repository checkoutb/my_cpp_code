
#include <concepts>

/*
概念定义
template <parameter-list>
concept concept-name = constraints-expression;

概念使用
concept-name<argument-list>

require表达式
requires (parameter-list) { requirements; }

复合requirement
{expression} noexcept -> type-constraint;       // noexcept, type-constraint 都是可选的

*/

// 约束
template<typename T>
concept C = sizeof(T) == 4;

// 简单require
template<typename T>
concept Incrementable = requires(T x) { ++x; x++; };

// 类型require， 特定类型是否有效
template<typename T>
concept CC = requires { typename T::value_type; };
// 类型require 验证模板是否可以实例化
template<typename T>
concept CCC = requires {typename A_template<T>;};

// 复合require，验证 某些东西 不会抛出异常， 某个方法 是否 返回某个类型
template<typename T>
concept CCCC = requires (T x, T y) {
    {x.swap(y)} noexcept;
};
// 复合require 类型约束
template<typename T>
concept CCCCC = requires (const T x) {
    { x.size() } -> convertible_to<size_t>;
};
template<typename T>
concept CCCCCC = requires(cosnt T a, const T b) {
    { a == b } -> convertible_to<bool>;
    { a < b } -> convertible_to<bool>;
};

// 嵌套require
template<typename T>
concept C2 = requires (T t) {
    requires sizeof(t) == 4;
    ++t; --t; t++; t--;
};

// 组合概念表达式
template<typename T>
concept CCC_C2 = CCC<T> && C2<T>;


//////////////////

template<typename T>
concept IsFromFoo = std::derived_from<T, Foo>;


Incrementable auto v1 { 1 };    // ok, auto推导为int，符合 Incrementable
Incrementable auto v2 {"a"s};   // 失败，string不符合Incrementable

///////////
template<convertible_to<bool> T>
void handle(const T& t);

template<Incrementable T>
void process(const T& t);

/////////////
template<typename T> requires Incrementable<T>  // requires + 产生bool的常量表达式
void process(const T& t);

template<typename T> requires convertible_to<T, bool>   // requires + 预定义的标准概念
void handle(const T& t);

template<typename T> requires requires(T x) {x++;++x;}  // requires + requires子句 (2个requires)
void process(const T& t);

template<typename T> requires (sizeif(T) == 4)      // 常量表达式
void process(const T& t);

template<typename T> requires Incrementable<T> && CC<T>     // conjunction, disjunction
void p(T t);

template<typename T> requires is_arithmetic_v<T>    // 类型萃取
void p(T t);

template<typename T>
void p(T t) requires Incrementable<T>;      // 后置 requires 子句

void p(const Incrementable auto& t);
///////////

// 约束包含。 总是使用最具体的
template<typename T> requires integral<T>
void p(T t) { cout<<"1"; }
template<typename T> requires (integral<T> && sizeof(T)==4)
void p(T t) { cout<<"2"; }

// 调用
p(int{1});      // 2
p(short(1));    // 1

//////////////

// 类型约束与类模板

template<std::derived_from<GamePiece> T>
class GameBoard : public Grid<T>
{
    // move
};
template<std::derived_from<GamePiece> T>
void GameBoard<T>::move() {}

// or
template<typename T> requires std::derived_from(T, GamePiece)
class GameBoard : public Grid<T>
{

};

// 类方法

template<std::derived_from<GamePiece> T>
class GameBoard : public Grid<T>
{
public:
    void move() requires std::movable<T>;       // <<<<  定义时也需要 requires
};


// 类型约束 可以 模板特化
template<typename T>
size_t find()
{
    // if (arr[i] == value) { return i; }
}

template<std::floating_point T>
size_t find()
{
    // if (AreEqual(arr[i], valuie)) {return i;}
}


//////////////





int main()
{
    return 0;
}
