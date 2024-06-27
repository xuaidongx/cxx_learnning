#include <random>
#include <time.h>
#include <iostream>

int main(int argc, char **argv)
{
    std::random_device rd;
    const auto seed = rd.entropy() ? rd() : time(nullptr);
    std::mt19937 mt{static_cast<std::mt19937::result_type>(seed)};
    std::uniform_int_distribution distribution{1, 6};
    auto generator = [&]
    { return distribution(mt); };

    std::string input;
    while(true)
    {
        std::cout<<"do you need to roll the dice ? yes or other"<<std::endl;
        std::cin >> input;
        for(auto& c : input)
            c = std::tolower(c);

        if (input == "yes")
            std::cout<<"roll number: "<<generator() <<" "<< generator() <<std::endl;
        else
        {
            std::cout<<"see you"<<std::endl;
            break;
        }
        
        input.clear();
    }

    return 0;
}