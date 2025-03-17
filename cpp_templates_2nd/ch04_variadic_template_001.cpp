

#include <iostream>
#include <type_traits>
#include <vector>
#include <array>
#include <tuple>



/*

变参模板
折叠
变参函数，变参表达式
变参索引
变参类模板

变参推导指引。 无
变参基类和using。 无


*/



namespace {

    // --------------------
    void myprint() {
    }
    template<typename T, typename... Types>
    void myprint(T firstArg, Types... args) {
        std::cout<<firstArg<<'\n';
        myprint(args...);
    }

    // ---------------------
    template<typename T>
    void print2(T arg) {
        std::cout<<arg<<'\n';
    }
    template<typename T, typename... Ts>
    void print2(T fa, Ts... args) {
        print2(fa);
        std::cout<<"sizeof...: "<<sizeof...(Ts)<<", "<<sizeof...(args)<<std::endl;
        print2(args...);
    }


    // --------------------
    template<typename T, typename... Ts>
    void print3(T fa, Ts... args) {
        std::cout<<fa<<std::endl;
        if constexpr(sizeof...(args) > 0) {
            print3(args...);
        }
    }


    // ===================
    template<typename... T>
    auto autoSum(T... args) {
        return (... + args);
    }
    
    // ------------------
    struct Node {
        int val;
        Node* left;
        Node* right;
        Node(int i = 0) : val(i), left(nullptr), right(nullptr) {
        }
    };

    auto lft = &Node::left;
    auto rht = &Node::right;
    
    template<typename T, typename... TP>
    Node* travel(T root, TP... path) {
        return (root ->* ... ->* path);
    }

    // -----------------
    template<typename... Ts>
    void print4(Ts... args) {
        (std::cout<< ... <<args)<<std::endl; // 中间没有间隔
    }

    // ----------------
    template<typename T>
    class AddSpace {
    private:
        T const& ref;
    public:
        AddSpace(T const& r) : ref(r) {
        }

        friend std::ostream& operator<<(std::ostream& os, AddSpace<T> as) {
            return os<<" --- "<<as.ref<<"\n";
        }
    };
    template<typename... Ts>
    void print5(Ts... args) {
        (std::cout<< ... <<AddSpace(args))<<std::endl;
    }


    // =============
    template<typename... Ts>
    void double_print(Ts const&... args) {
        print5(args + args...);  // const char* 没有实现 operator+
    }

    // --------------
    template<typename... Ts>
    void add_one(Ts const&... args) {
        // print5(args + 1 ...); // ok
                                 // 1... 不行，会当作 浮点数 1. 再加2个无法匹配的 .
        print5((args+1)...);
    }

    // -----------------
    template<typename T1, typename...TN>
    constexpr bool all_type_same(T1, TN...) { // 不需要形参名
        return (std::is_same<T1, TN>::value && ...); // is_same不需要()
    }

    // ==============
    template<typename C, typename... Idx>
    void printElements(C const& con, Idx... idx) {
        print5(con[idx]...);
        // print5(...[idx] con);  // Error，怎么写出 前缀折叠？
    }

    // -----------
    template<std::size_t... idx, typename C>
    void printEles(C const& con) {
        print5(con[idx]...);
    }


    // ==============
    template<std::size_t...>
    struct Idxs {
    };
    template<typename T, std::size_t... idx>
    void printByIdxs(T t, Idxs<idx...>) {
        print5(std::get<idx>(t)...);
    }

}


int main() {

    int a = 2;

    // 变参模板
    myprint(1,"asd",a);

    print2(123, "qwe", a);

    print3("this is 333", 123123, a, 3.2);

    // 折叠
    std::cout<<"\n\nsum: "<<autoSum(1,a,3,4,5)<<std::endl;

    // autoSum(); // 编译失败，fold无法展开
    std::cout<<" "<<autoSum(1)<<std::endl;

    Node* root = new Node(1);
    root->left = new Node(222);
    root->left->right = new Node(333);

    std::cout<<travel(root, lft, rht)->val<<" - "<<travel(root, lft)->val<<std::endl;

    print4(123, "this is 4444", a);

    print5(123, "this is 5555", a, 123.421231234);


    // 变参函数，变参表达式
    std::string str = "zxc";
    double_print(111, str, a, 123.123);  // const char[4] 没有实现 operator+

    add_one(123,a,"asd");   // asd -> sd ... pointer + 1...

    std::cout<<"all type same: "<<all_type_same(1234, a, 123)<<std::endl;
    std::cout<<"__: "<<all_type_same("asd", " ", "222")<<std::endl; // 值传递，所以都是 char const*


    // 变参索引
    int arr[123] = {1,2,3,4,5};
    printElements(arr, 2,3,4);

    printEles<0,1,2>(arr);


    // 变参类模板： std::variant, std::tuple
    std::vector<int> vi{1,2,3,4,5,6,7};
    //printByIdxs(vi, Idxs<1,2,3>()); // 报错：std::get<idx>(t) 需要一个 type，但是获得了 1.
                        // 确实不太对，因为 vector的长度在 编译期不可知，所以 编译器 不可能生成代码的。
    std::array<int, 5> arr2{1,2,3,4,5};
    printByIdxs(arr2, Idxs<1,2,3>());

    auto tup = std::make_tuple(123, "aaa", str, 2.3);
    printByIdxs(tup, Idxs<0,1,2,3>());


    // 变参推导指引

    // 变参基类 和using
    
    return 0;
}
