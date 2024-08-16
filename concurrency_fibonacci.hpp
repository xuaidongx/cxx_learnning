#include <iostream>
#include <vector>
#include <thread>
#include <barrier>

namespace concurrency_fibonacci
{
    std::size_t Fibonacci(std::size_t value)
    {
        if (value == 0)
            return 0;
        else if (value == 1)
            return 1;

        return Fibonacci(value - 1) + Fibonacci(value - 2);
    }

    void testing()
    {
        std::vector<std::size_t> values{4, 9, 14, 17};
        std::vector<std::thread> vecThreads;
        for(auto i{0}; i < values.size(); ++i)
            vecThreads.emplace_back([=, &values](){values[i] = Fibonacci(values[i]);});
        
        std::for_each(std::begin(vecThreads), std::end(vecThreads), [](auto& t){t.join();});
        std::copy(values.begin(), values.end(), std::ostream_iterator<std::size_t>(std::cout, " "));
        std::cout<<std::endl;
    }
}