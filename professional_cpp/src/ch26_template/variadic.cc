

export module variadic;

import <iostream>;
import <string>;

export template <typename... Types>        // 可以0个， 要防止0个： <typename T1, typename... Types>
class MyVariadicTemplate { };

//////////////////////

void handleValue(int v) { std::cout<<"int: "<<v<<'\n'; }
void handleValue(double v) { std::cout<<"double: "<<v<<'\n'; }
void handleValue(std::string v) { std::cout<<"string: "<<v<<'\n'; }

export void processValues() {}

// // 类型安全的，会根据实际类型 自动调用正确的 handleValue 版本。         // 和下面的 && 版本 冲突,歧义。
// export template<typename T1, typename... Tn>
// void processValues(T1 a, Tn... b) {
//     std::cout<<(sizeof...(b))<<"    left value ...\n";
//     handleValue(a);
//     processValues(b...);
// }

// 引用的话，无法使用字面量。
export template <typename T1, typename... Tn>
void processValues(T1&& a, Tn&&... n)
{
    std::cout<<(sizeof...(n))<<"   right value \n";
    handleValue(std::forward<T1>(a));
    processValues(std::forward<Tn>(n)...);       //  ... 解开参数包，在每个参数上使用 forward，用逗号隔离。   比如： processValues(std::forward<A>(a), std::forward<B>(b), std::forward<C>(c));

}


/////////////////////////


export template<typename... Tn>
void processValues2(const Tn&... args)
{
    std::cout<<"      processValues2: \n";
    (handleValue(args), ...);       // 非递归，不需要 基本情形 (无参的 processValues)
}

export template<typename... Values>
void printValues2(const Values&... args)
{
    std::cout<<"   ----- printValues2: \n";
    ((std::cout<<args<<std::endl), ...);
}



export template <typename T, typename... Values>
double sumValues(const T& init, const Values&... values)
{
    return (init + ... + values);
}
