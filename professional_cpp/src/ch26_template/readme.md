


# 类偏特化

```C++
export template <size_t W, size_t H>
class Grid<const char*, W, H>        // Grid<xxx>
{
    // ...
};

template <size_t W, size_t H>
const std::optional<std::string>&
Grid<const char*, W, H>::at(size_t x, size_t y) const
{
    // ...
}

Grid<int, 2, 2> myint;      // ok
Grid<const char*, 2, 2> mystr;  // ok
Grid<2, 2> myerr;       // !!!error!!!
```


```C++
export template <typename T>
class Grid<T*>
{
};

Grid<int*> mp;      // T is int, T* is int*

```



# 通过重载模拟函数部分特化

```C++
template <typename T>
size_t find(T* value, T* const* arr, size_t size)
{

}
```




# 折叠表达式

|名称|表达式|含义|
|--|--|--|
|一元右折叠|(pack OP ...)|pack0 OP (pack1 OP (... OP (packn-1 OP packn)))|
|一元左折叠|(... OP pack)|((pack0 OP pack1) OP ...) OP packn|
|2元右折叠|(pack OP ... OP INIT)|pack0 OP (... OP (packn-1 OP (packn OP INIT)))|
|2元左折叠|(INIT OP ... OP pack)|(((INIT OP pack0) OP pack1) OP ...) OP pack0|


```C++

template<typename... Tn>
void processValues(const Tn&... args)
{
    (handleValues(args), ...);       // 非递归，不需要 基本情形 (无参的 processValues)
}

template<typename... Values>
void printValues(const Values&... args)
{
    ((cout<<args<<endl), ...);
}


```


# 打印tuple

`if constexpr`      // 减少 基本情况 就是 <0>
`tuple_size<tuple<int, int, string>>::value`       // 有这东西，直接for一下，懒得用模板

```C++
template <typename TupleType, int n = tuple_size<TupleType>::value>
void tuplePrint(const TupleType& t)
{
    if constexpr (n > 1) {
        tuplePrint<TupleType, n - 1>(t);
    }
    cout<<get<n-1>(t)<<endl;
}
```

`std::integer_sequence` @ `<utility>`
编译时整数序列


```C++
template <typename Tuple, size_t... idxs>
void tuplePrintHelper(const Tuple& t, index_sequence<idxs...>)
{
    ((cout<<get<idxs>(t)<<endl), ...);
}
template <typename... Args>
void tuplePrint(const tuple<Args...>& t)
{
    tuplePrintHelper(t, index_sequence_for<Args...>());
}
```



# trait


// conjunction_v    // and
// disjunction_v    // or
// negation_v       // !














