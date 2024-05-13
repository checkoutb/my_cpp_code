
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <numeric>
#include <functional>
#include <iterator> // ostream_iterator
#include <string>


// cppcon 2018, 《105 stl algorithm in less than an hour》三



// ================
// forward declare
std::vector<int> generate_random_vi(int sz = 10);
void showVectorInt(std::vector<int>& vi);



// =================


// https://en.cppreference.com/w/cpp/algorithm/transform_reduce   example







void heap_max_heap()
{
    // make_heap, push_heap, pop_heap
    // priq slower than this ?

    std::vector<int> vi = generate_random_vi();
    showVectorInt(vi);

    std::ranges::make_heap(vi);
    // showVectorInt(vi);
    std::make_heap(begin(vi), end(vi));
    showVectorInt(vi);

    vi.push_back(123);
    std::ranges::push_heap(vi);
    showVectorInt(vi);

    std::pop_heap(begin(vi), end(vi));
    showVectorInt(vi);
    vi.pop_back();

    // std::pop_heap(begin(vi), end(vi));
    std::ranges::pop_heap(vi);
    showVectorInt(vi);
    vi.pop_back();

}


void sort()
{
    std::vector<int> vi = generate_random_vi();
    showVectorInt(vi);

    // 这里的把 ranges:: 去掉就可以了，不需要改 参数。

    std::ranges::nth_element(begin(vi), begin(vi) + 2, end(vi));  // st, md, en       // no ranges
    showVectorInt(vi);

    std::ranges::partial_sort(begin(vi), begin(vi) + 5, end(vi));
    showVectorInt(vi);

    std::vector<int> v2 = generate_random_vi();
    std::ranges::make_heap(v2);
    showVectorInt(v2);
    // std::ranges::sort_heap(begin(v2), end(v2));      // ok
    std::ranges::sort_heap(v2);
    showVectorInt(v2);

    std::ranges::sort(vi);
    showVectorInt(vi);

    vi.insert(end(vi), begin(v2), end(v2));
    showVectorInt(vi);

    // std::ranges::inplace_merge(begin(vi), begin(vi) + 10, end(vi));      // ok
    std::ranges::inplace_merge(vi, begin(vi) + 10);
    showVectorInt(vi);
}


void partition()
{
    std::vector<int> vi = generate_random_vi();
    showVectorInt(vi);
    std::ranges::partition(vi, [](int& a){
        return a % 2 == 0;      // true is prefix, false is suffix
    });
    showVectorInt(vi);

    auto p = std::ranges::partition_point(vi, [](int& a){return a % 2 == 0;});
    std::cout<<"partition_point: "<<std::ranges::distance(begin(vi), p)<<std::endl;

}


void permutation()
{
    std::vector<int> vi = generate_random_vi();
    std::ranges::sort(vi);
    showVectorInt(vi);

                    // get first 2 elements, put these 2 elements to tail
    std::ranges::rotate(vi, std::begin(vi) + 2);
    showVectorInt(vi);

    std::mt19937 mt(std::random_device{}());
    std::ranges::shuffle(vi, mt);
    showVectorInt(vi);

    std::ranges::sort(vi);
    std::ranges::prev_permutation(vi);      // become max
    showVectorInt(vi);
    std::ranges::next_permutation(vi);
    showVectorInt(vi);
    std::ranges::prev_permutation(vi);
    showVectorInt(vi);

    std::ranges::reverse(vi);
    showVectorInt(vi);

}


void secret_runes()
{
    // stable_*
    // stable_sort, stable_partition


    // is_*
    // is_sorted, is_partitioned, is_heap


    // is_*_until
    // is_sorted_until, is_partitioned_until, is_heap_until
}



void query()
{
    // std::vector<int> vi = generate_random_vi(100);
    // std::cout<<std::ranges::count(vi);

    std::vector<bool> vb (10, false);
    vb[1] = vb[2] = vb[5] = true;
    std::cout<<std::ranges::count(vb, false)<<std::endl;

    std::vector<int> vi = generate_random_vi();
    // std::cout<<std::ranges::accumulate(vi, 0);       // no ranges.. <numeric> no ranges
    std::cout<<std::accumulate(begin(vi), end(vi), 0)<<std::endl;

    std::vector<int> v2{vi};
    std::vector<int> v3{vi};
    std::vector<int> v4{vi};
    showVectorInt(vi);
    std::partial_sum(begin(vi), end(vi), begin(vi));       // no ranges // 第三个参数 可以等于 第一个参数
    showVectorInt(vi);

    std::inclusive_scan(begin(v2), end(v2), begin(v2)); // 和partial_sum 一样，但可以并发
    showVectorInt(v2);

    std::exclusive_scan(begin(v3), end(v3), begin(v3), 0);  // 和 partial_sum 一样，但是 不包含当前元素，且需要 指定 第一个元素的值。
    showVectorInt(v3);

    std::transform_exclusive_scan(begin(v4), end(v4), std::ostream_iterator<int>(std::cout, " "), 0, std::plus<int>{}, [](int x) { return x * 10;});
    std::cout<<std::endl;
    std::transform_inclusive_scan(begin(v4), end(v4), std::ostream_iterator<int>(std::cout, " "), std::plus<int>{}, [](int x) { return x * 10;});
    std::cout<<std::endl;


    v2 = generate_random_vi();
    v3 = generate_random_vi();
    int a = std::inner_product(begin(v2), end(v2), begin(v3), 0,
                                [](int a, int b){ return a - b; },
                                [](int a, int b){ return a * b; });     // for { init = op1(init, op2(v2, v3)) }
                                                        // default op1 is +, op2 is *
    showVectorInt(v2);
    showVectorInt(v3);
    std::cout<<a<<std::endl;

    std::adjacent_difference(begin(v2), end(v2), begin(v3));        // v3[0] == v2[0] - 0,  v3[1] = v2[1] - v2[0]
    showVectorInt(v2);
    showVectorInt(v3);

    // 随机取 N 个元素
    // cppreference
    // std::sample(in.begin(), in.end(), std::back_inserter(out), 4,
    //             std::mt19937 {std::random_device{}()});




    // https://en.cppreference.com/w/cpp/algorithm/ranges/all_any_none_of
    // 范围为空，返回值是写死的。
    // T
    std::cout<<std::ranges::all_of(std::vector<int>(), [](int a){
        return true;
    });
    std::cout<<std::endl;

    // F
    std::cout<<std::ranges::any_of(std::vector<int>(), [](int a){
        return true;
    })<<std::endl;

    // T
    std::cout<<std::ranges::none_of(std::vector<int>(), [](int a){
        return true;
    })<<std::endl;


    std::vector<int> v5 {v4};
    std::cout<<std::ranges::equal(v4, v5)<<std::endl;

    v5.push_back(5);
    std::cout<<std::ranges::lexicographical_compare(v4, v5)<<std::endl;

    v5[5] = -1;
    // std::ranges::in_in_result<int, int> iir = std::ranges::mismatch(v4, v5);
    auto iir = std::ranges::mismatch(v4, v5);
    std::cout<<(*(iir.in1))<<", "<<(*(iir.in2))<<", "<<std::distance(begin(v4), iir.in1)<<std::endl;


    // https://en.cppreference.com/w/cpp/types/type_info
    std::cout<<typeid(iir).name()<<std::endl;
    std::cout<<typeid(v5).name()<<std::endl;
    std::cout<<typeid(v5[5]).name()<<std::endl;
    std::cout<<typeid(&v5).name()<<std::endl;

    // ???   https://en.cppreference.com/w/cpp/algorithm/ranges/mismatch
    // const auto end = std::ranges::mismatch(in, in | std::views::reverse).in1;



    // search a value
    // from not sort
    showVectorInt(v5);
    // find_if, find_if_not, find_last, find_last_if, find_last_if_not
    // find_end, find_first_of
    std::vector<int>::iterator it = std::ranges::find(v5, -1);
    std::cout<<std::distance(begin(v5), it)<<std::endl;

    // 从begin开始的最长非降序列的 最后一个元素的 iter， (即 长度 - 1)
    it = std::ranges::adjacent_find(v5, std::greater<>());
    std::cout<<std::distance(begin(v5), it)<<std::endl;

    v5[6] = -1;
    it = std::ranges::adjacent_find(v5);    // 连续出现
    std::cout<<std::distance(begin(v5), it)<<std::endl; // 5
    // distance 的 第二个元素是 开区间

    // from sort
    v5[0] = -123;
    std::ranges::sort(v5);


    // https://en.cppreference.com/w/cpp/algorithm/ranges/equal_range
    // for (std::cout << rem; auto const& e : v)
    // 类型挺奇怪的，如果是 iter 表示的 范围，返回 std::ranges::subrange,
    //             如果是 R&& 表示的范围，返回 std::ranges::borrowed_subrange_t
    // https://en.cppreference.com/w/cpp/ranges/subrange        // 很详细，看起来容易使用
    // https://en.cppreference.com/w/cpp/ranges/borrowed_iterator_t     // 什么都没有，是一个别名， std::conditional_t 的东西。太麻烦了。

    // std::ranges::borrowed_subrange_t<std::vector<int>>   // 不行
    auto bst = std::ranges::equal_range(v5, -1);
    std::cout<<typeid(bst).name()<<std::endl;
    for (auto const & a : bst)
    {
        std::cout<<a<<',';
    }
    std::cout<<std::endl;
    std::cout<<std::distance(begin(v5), begin(bst))<<std::endl;

    std::cout<<std::ranges::binary_search(v5, v5[5])<<std::endl;
    std::cout<<std::ranges::binary_search(v5, -15)<<std::endl;

    std::vector<int> v6(begin(v5) + 4, begin(v5) + 7);
    bst = std::ranges::search(v5, v6);
    std::cout<<std::distance(begin(v5), begin(bst))<<std::endl;

    // ranges::borrowed_iterator_t<R1>
    auto bst2 = std::ranges::find_first_of(v5, v6);
    std::cout<<std::distance(begin(v5), bst2)<<std::endl;


    // 非ranges 的 minmax_element 是一个 pair
    std::cout<<*std::ranges::max_element(v5)<<", "<<*std::ranges::min_element(v5)<<", "<<std::endl;
    std::ranges::min_max_result mmr = std::ranges::minmax_element(v5);
    std::cout<<*mmr.min<<", "<<*mmr.max<<std::endl;

    std::ranges::min_max_result<int> mmr2 = std::ranges::minmax(v5);
    std::cout<<mmr2.min<<", "<<mmr2.max<<std::endl;


}


// Set operations (on sorted ranges)
void sort_set_algo()
{
    std::vector<int> vi = generate_random_vi();
    std::vector<int> v2 = generate_random_vi();
    for (int i = 0; i < 4; ++i)
        v2[i] = vi[i];
    std::ranges::sort(vi);
    std::ranges::sort(v2);

    showVectorInt(vi);
    showVectorInt(v2);

    // A - B
    std::vector<int> diff{};
    std::ranges::set_difference(vi, v2, std::back_inserter(diff));
    showVectorInt(diff);

    // A & B
    diff.clear();
    std::ranges::set_intersection(vi, v2, std::back_inserter(diff));
    showVectorInt(diff);

    // (A - B) + B
    // 如果元素 同时出现在 A 和B， 那么只取一个， 如果 2个元素在 A中重复，那么2个都会取
    diff.clear();
    std::ranges::set_union(vi, v2, std::back_inserter(diff));
    showVectorInt(diff);

    // (A - A&B) + (B - A&B)
    // (A - B) + (B - A)
    diff.clear();
    std::ranges::set_symmetric_difference(vi, v2, std::back_inserter(diff));
    showVectorInt(diff);

    std::vector<int> v3(begin(v2) + 2, begin(v2) + 5);
    std::cout<<std::ranges::includes(v2, v3)<<std::endl;

    // A + B    // 全部元素
    diff.clear();
    std::ranges::merge(vi, v2, std::back_inserter(diff));
    showVectorInt(diff);

}


void move_copy()
{
    std::vector<int> vi = generate_random_vi();
    std::vector<int> ot;

    // return : copy_result<ranges::borrowed_iterator_t<R>, O>      // ...
    std::ranges::copy(vi, std::back_inserter(ot));
    showVectorInt(ot);

    ot.clear();
    std::ranges::copy_if(vi, std::back_inserter(ot), [](int i){
        return i % 2 == 0;
    });
    showVectorInt(ot);

    // 6
    std::ranges::copy_if(vi, std::ostream_iterator<int>(std::cout, ", "), [](int i){ return i % 2 == 1; });
    std::cout<<std::endl;

    ot.clear();
    std::ranges::move(vi, std::back_inserter(ot));
    showVectorInt(vi);      // 基本类型没有move，直接copy 的代价很低，并且也没有内部的资源，所以

    std::vector<std::string> vs {"asd","bbb","qqq"};
    std::ranges::move(vs, std::ostream_iterator<std::string>(std::cout, ", "));
    std::cout<<"\n\n---\n\n";
    for (const std::string& s : vs)
        std::cout<<s<<", ";     // still exist ...
    std::cout<<"\n\n======\n\n";

    std::string ss = "asdasd";
    std::string tt {std::move(ss)};
    std::cout<<ss<<", "<<tt<<std::endl;     // ss is empty

    std::vector<std::string> vt;
    std::ranges::move(vs, std::back_inserter(vt));
    for (const std::string& s : vs)     // empty, has been moved.
    {
        std::cout<<s<<", ";
    }
    std::cout<<"\n ....\n";
    for (const std::string& s : vt)
        std::cout<<s<<", ";
    std::cout<<"\n\n-=-=--=\n\n";




    std::vector<int> v2 = generate_random_vi();
    showVectorInt(vi);
    showVectorInt(v2);
    std::cout<<&vi<<" - "<<&v2<<", "<<&v2[0]<<",,,"<<&vi[0]<<std::endl;
    std::ranges::swap_ranges(vi, v2);
    showVectorInt(vi);
    showVectorInt(v2);
    std::cout<<&vi<<" - "<<&v2<<", "<<&v2[0]<<"..."<<&vi[0]<<std::endl;
    // 各个元素发生了交换，所以 栈地址，堆地址没有变。
    // 如果是 swap 的话， 栈地址不变， 堆地址交换。


    // 注意自我copy，元素会被覆盖，需要从后往前。
//     copy_backward, move_backward


}


void value_modifier()
{
    std::vector<int> vi = generate_random_vi();
    std::ranges::fill(vi, -3);
    std::cout<<std::ranges::all_of(vi, [](int a){return a==-3;})<<std::endl;
    std::cout<<(std::ranges::count(vi, -3) == vi.size())<<std::endl;

    int cnt = 3;
    std::ranges::generate(vi, [&cnt](){cnt += 2; return cnt;});
    showVectorInt(vi);

    std::ranges::iota(vi, 555);     // c++23 的。
    showVectorInt(vi);

    std::ranges::replace(vi, 557, -1111111);
    showVectorInt(vi);

}


void struct_change()
{
    std::vector<int> vi = generate_random_vi();
    for (int i = 4; i < 8; ++i)
        vi[i] = -333;

    auto bst = std::ranges::remove(vi, -333);   // 只是移动，不是erase
    showVectorInt(vi);  // 将非 -333 的值前移， 执行后，非-333后面的值是不可控的。 (就是 双指针，非-333就移动到前面)
    // bst 是需要丢弃的 range
    std::cout<<std::distance(begin(vi), begin(bst))<<", "<<std::distance(begin(vi), end(bst))<<std::endl;

    vi.erase(std::ranges::remove(vi, -333).begin(), end(vi));   // 上面已经remove一次了，再次remove，所以2次 showVectorInt 不同。  2次 remove 不同的结果。(根据代码 是正常的)。
    showVectorInt(vi);

    // remove_if, remove_copy_if


    for (int i = 1; i < 4; ++i)
        vi[i] = -11;
    vi.push_back(-11);
    showVectorInt(vi);
    std::ranges::unique(vi);
    showVectorInt(vi);

    std::ranges::sort(vi);
    vi.erase(std::ranges::unique(vi).begin(), end(vi));     // 2次 unique 也不同的。 因为 unique 是 连续的才会移除， 所以必须先 sort，然后 unique 才有意义。  O(n)
    showVectorInt(vi);


    // *_copy
    // remove / unique / reverse / rotate / replace / partition / partition_sort   _copy
    // 将输出 输出到 output iterator 上

    // *_if
    // find / count / remove / remove_copy / replace / replace_copy / copy    _if

    // find_if_not


}



void for_each_transform()
{
    std::vector<int> vi = generate_random_vi();
    std::vector<int> ot;
    std::cout<<ot.size()<<", "<<ot.capacity()<<std::endl;   // 0, 0
    // ot.resize(vi.size());    // size，不是容量。 默认0值。 无法调整容量。
    std::ranges::transform(vi, std::back_inserter(ot), [](int a){
        return a * 100;
    });
    showVectorInt(ot);
    std::cout<<ot.size()<<", "<<ot.capacity()<<std::endl;   // 10, 16

    std::vector<int> v2 = generate_random_vi();
    ot.clear();
    std::ranges::transform(vi, v2, std::back_inserter(ot), [](int a, int b){
        return a * 1000 + b;    // a is vi,  b is v2
    });
    showVectorInt(vi);
    showVectorInt(v2);
    showVectorInt(ot);


    int sum2 = 0;
    std::ranges::for_each(vi, [&sum2](int a) -> void { sum2 += a; });
    std::cout<<sum2<<" =?= "<<std::accumulate(begin(vi), end(vi), 0)<<std::endl;

    // for_each 可以做到 其他的 stl算法， 但是 优先选择其他stl算法，因为更加精确。


}


void raw_memory()
{
    // fill, copy, move , 对象必须已经构建成功

    // Operations on uninitialized memory
    // Defined in header <memory>

    // *_n

    // ranges::uninitialized_copy
    // ranges::uninitialized_fill
    // ranges::uninitialized_move_n
    // ranges::uninitialized_default_construct
    // ranges::uninitialized_value_construct_n
    // ranges::destroy
    // ranges::construct_at

    // 这些估计不可能用到。。似乎只能用于 池化。 平时没人用的啊。 而且池化的话，直接 malloc 不好吗。。不，malloc 是获得 未初始化的 内存，
    // 这里 是 初始化。
    // 但一般用 placement new 吧
    //
    // 复制估计是 字节复制，所以可以避免 operator= 的 移动赋值，拷贝赋值 之类的。  但是 破坏了 在 对象上的 一些 约束啊。


    // https://en.cppreference.com/w/cpp/memory/uninitialized_fill
    const std::size_t sz = 4;
    std::allocator<std::string> alloc;
    std::string* p = alloc.allocate(sz);
    std::uninitialized_fill(p, p + sz, "Example");
    // std::for_each(p, p + sz, [](void* s){std::cout<<s<<std::endl;});
    std::for_each_n(p, sz, [](const std::string& s) { std::cout<<typeid(s).name()<<": "<<s<<std::endl; });
    std::string s = "asd";
    std::cout<<typeid(s).name()<<std::endl;

    https://en.cppreference.com/w/cpp/memory/construct_at



    std::vector<int> ot;
    std::fill_n(std::back_inserter(ot), 5, -123);
    showVectorInt(ot);


}









std::vector<int> generate_random_vi(int sz)     // default value must in declare, not define
{
    std::mt19937 mt(std::random_device{}());
    std::uniform_int_distribution<int> dis(0, 100);

    std::vector<int> vi(sz);
    for (int i = 0; i < sz; ++i)
        vi[i] = dis(mt);

    return vi;
}

void showVectorInt(std::vector<int>& vi)
{
    std::ranges::for_each(vi, [](int i){
        std::cout<<i<<", ";
    });
    std::cout<<std::endl;
}

int main()
{
    // heap_max_heap();

    // sort();

    // partition();

    // permutation();

    // secret_runes();

    // query();

    // sort_set_algo();

    // move_copy();

    // value_modifier();

    // struct_change();

    // for_each_transform();

    raw_memory();

}
