

operator*
operator->
operator.*     不允许重载。 operator. 也不允许
operator->*


# 转换运算符
为 SpreadsheetCell 编写一个 可以转换为 double 的 转换运算符
定义： `operator double() const;`
```C++
SpreadsheetCell::operator double() const {
    return getValue();
}
```
现在可以
```C++
SpreadsheetCell cell {1.23};
double d1 {cell};       // <<<<<<
```

---

可以使用auto让编译器自动推导
SpreadsheetCell 转 double 可以写成：
`operator auto() const {return getValue();}`

auto返回类型推导的方法必须对类的用户可见，所以这里 方法的实现 直接放在 类的定义中

auto会移除 const 和 引用。如果返回了 T的引用，需要手动加
`operator const auto&() const { ... }`


---

double d2 { cell + 3.3 };   会失败。
因为 编译器不知道
通过 operator double() 将 cell 转为 double，然后执行 double 加法，
还是 通过 double 构造器 将3.3 转为 SpreadsheetCell， 然后+

C++11之前，是把 构造器 标记为 explicit。
C++11后， 可以把 double类型转换运算符 标记未 explicit
`explicit operator double() const;`

---
转bool

指针可以 if(ptr != nullptr)  if(ptr)  if(!ptr)

转换运算符常用的指针类型为 void*
`operator void*() const { return m_ptr; }`

另一种是 重载 operator bool(), 而不是 operator void*()
`operator bool() const { return m_ptr != nullptr; }`
这种不能 if (p != nullptr) , 可以 if (p != NULL)



# 重载 new， delete

SpreadsheetCell* cell {new SpreadsheetCell()};
先用 operator new 申请空间，然后 使用 构造函数 初始化

delete cell;
先调用cell的析构器，然后 operator delete 释放内存。


## placement new
在已经分配的内存上 调用构造器。
用于 内存池。


# 字面量


