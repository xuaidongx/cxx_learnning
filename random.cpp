#include <random>
#include <iostream>
#include <string>

template<typename container>
void fill_random2container(container& cnt, int min, int max)
{
    std::random_device rnd;
    const auto seed = rnd.entropy() ? rnd() : time(nullptr);
    std::mt19937 mt{static_cast<std::mt19937::result_type>(seed)};
    std::uniform_int_distribution<int> distribution{min, max};

    auto generator = [&](){return distribution(mt);};
    std::generate(std::begin(cnt), std::end(cnt), generator);
}

int main()
{
    std::vector<int> values(100);
    fill_random2container(values, 0, 99);
    std::copy(std::begin(values), std::end(values), std::ostream_iterator<int>{std::cout, R"( )"});
    return 0;
}