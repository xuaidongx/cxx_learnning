#include <any>
#include <iostream>
#include <tuple>
#include <string>
#include <regex>
#include <charconv>

//-name1=value1 -name2=value2 ...
decltype(auto) parseArgument(int argc, char** argv)
{
    using vec_element = std::tuple<std::string, std::variant<int, bool, std::string>>;
    auto argv2tuple = [](const char* c_arg) -> std::optional<vec_element>
    {
        static auto isdigit = [](std::string_view sv) -> std::optional<int>
        {
            if (!sv.empty() && std::all_of(std::begin(sv), std::end(sv), ::isdigit))
            {
                int value{0};
                auto [ptr, errc] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
                return errc == std::errc{} ? value : std::optional<int>{};
            }
            return std::nullopt;
        };

        static auto isBool = [](std::string_view sv) -> std::optional<bool>
        {
            std::string result(sv.size(), '\0');
            std::transform(sv.cbegin(), sv.cend(), result.begin(), [](const auto& value){return std::tolower(value);});
            if (result == "true")
                return true;
            else if (result == "false")
                return false;
            else
                return std::nullopt;
        };

        static std::regex pattern{R"-(-(.*)=(.*))-"};
        std::cmatch results;
        if (std::regex_search(c_arg, results, pattern))
        {
            auto value = results[2].str();
            std::variant<int, bool, std::string> variant;
            if (isdigit(value))
                variant = *isdigit(value);
            else if (isBool(value))
                variant = *isBool(value);
            else 
                variant = value;
            return std::make_tuple(results[1].str(), variant);
        }
        return std::nullopt;
    };

    std::vector<vec_element> result;
   
    if (argc > 1)
    {
        for (auto index{1}; index < argc; ++index)
        {
            auto value = argv2tuple(argv[index]);
            if (value)
                result.emplace_back(*value);
        }
    }
    return result;
}

enum class errorcode : int64_t
{
    error_param,
    error_url
};

// return diff type value
std::variant<std::vector<int>, errorcode> getData(bool fail)
{
    if (fail)
        return errorcode::error_param;
    else
        return std::vector<int>{1, 2, 3, 4};
}

template <typename... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

int main(int argc, char **argv)
{
    // default initialization(type has to a default ctor)
    std::variant<int, float> intFloat;
    std::cout << intFloat.index() << ", value " << std::get<int>(intFloat) << std::endl;
    std::cout << std::holds_alternative<int>(intFloat) << std::endl;

    // first type has to a default ctor and ambiguity resoved by in_place
    std::variant<std::monostate, int, float> monoIntFloat{std::in_place_index<2>, 10.5};

    // assigment
    std::variant<int, std::string> intString;
    intString = "xuaidong";
    std::cout << intString.index() << ", value " << std::get<1>(intString) << std::endl;
    intString.emplace<0>(200);
    std::cout << intString.index() << ", value " << std::get<0>(intString) << std::endl;

    // return nullptr or pointer to the value
    if (auto v = std::get_if<int>(&intString))
    {
        *v += 200;
        std::cout << std::get<int>(intString) << std::endl;
    }

    std::variant<int, float, std::string> intFloatString{"Hello"};

    try
    {
        std::string s = std::get<std::string>(intFloatString);
        std::cout << s << std::endl;

        std::get<int>(intFloatString);
    }
    catch (std::bad_variant_access &error)
    {
        std::cout << error.what() << std::endl;
    }

    if (!intFloatString.valueless_by_exception())
        std::visit([](const auto &v)
                   { std::cout << v << std::endl; }, intFloatString);

    auto result = getData(false);
    std::visit(overload{[](const std::vector<int> &vecs)
                        {
                            std::copy(std::begin(vecs), std::end(vecs), std::ostream_iterator<int>{std::cout, R"(,)"});
                            std::cout<<std::endl;
                        },
                        [](errorcode &ec)
                        {
                            std::cout << static_cast<int64_t>(ec) << std::endl;
                        }},result);
    

    auto key_value = parseArgument(argc, argv);

    std::for_each(std::begin(key_value), std::end(key_value), [](const auto& element)
    {
        auto [name , value] = element;

        std::visit(overload{[&](int v){std::cout<<"name: "<<name<<" int value:"<< v<<std::endl;},
                            [&](bool v){std::cout<<"name: "<<name<<" bool value:"<< v<<std::endl;},
                            [&](std::string v){std::cout<<"name: "<<name<<" string value:"<< v<<std::endl;}}, value);

    });

    return 0;
}