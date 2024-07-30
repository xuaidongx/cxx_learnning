#include <type_traits>

template<std::size_t N>
constexpr std::size_t Fibonacci()
{
    if constexpr (N == 0)
        return 0;
    else if constexpr (N == 1)
        return 1;
    else if constexpr (N == 2)
        return 1;
    else if constexpr (N == 3)
        return 2;
    else
        return Fibonacci<N-1>() + Fibonacci<N-2>();
}

//Implementing the Fibonacci sequence at compile time without using templates and function recursion
constexpr std::size_t Fibonacci_non_template_and_function_recursion(std::size_t N)
{
    if (N == 0)
        return 0;
    else if (N == 1)
        return 1;

    auto a{0}, b{1};
    auto result{0};
    for(auto index{2}; index <= N ; index++)
    {
        result = a + b;
        a = b;
        b = result;
    }
    return result;
}
//constexpr std::size_t value{Fibonacci_non_template_and_function_recursion(3)};

template<typename container>
concept must_be_have_push_back = requires(container c, typename container::value_type v)
{
    {c.push_back(v)};
};

template<typename container, typename... Types>
requires must_be_have_push_back<container>
void push_back_values(container& c, Types... args)
{
    (c.push_back(args),...);
}
