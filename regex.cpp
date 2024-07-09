#include <regex>
#include <filesystem>
#include <fstream>
#include <iostream>

//remember: lookahead(?!) lookahead(?=), back reference, \1 \2 \3 refernce capture 1 2 3 match result
// (?:) improve priority but not capture group


//regex_match must be completely match 
bool isMatchYearMonthDay(const std::string_view source)
{
    static std::regex pattern{R"delimiter((\d{4})/(0?[1-9]|1[0-2])/(?:0?[0-9]|1[0-9]|2[0-9]|3[0-1]))delimiter"};
    std::cmatch result;
    std::regex_match(source.data(), result, pattern);//match entire string, ex: adasd2024/02/12 is error match
    std::for_each(std::begin(result), std::end(result), [](const auto& value)
    {
        std::cout<<value<<std::endl;
    });

    return !result.empty();
}

//regex_search first match string
bool searchFirstCommentFromFile(const std::string_view file_path)
{
    if (!std::filesystem::exists(file_path))
        return false;

    auto file_size = std::filesystem::file_size(file_path);
    std::ifstream in{file_path.data(), std::ios::in | std::ios::binary};
    if (!in.is_open())
        return false;

    std::string content;
    content.resize(file_size);
    in.read(content.data(), file_size);

    static std::regex pattern{R"delimiter(//\s*(.+)\s*\n)delimiter"};
    std::smatch result;
    if (std::regex_search(content, result, pattern))
    {
        std::cout<<"match string:"<<result[0].str()<<std::endl;
        std::cout<<"match prefix:"<<result.prefix()<<std::endl;
        std::cout<<"match suffix:"<<result.suffix()<<std::endl;
        std::cout<<"capture string:"<<result[1].str()<<std::endl;
        return true;
    }

    return false;
}

//regex_iterator match all instance by regex
bool searchAllCommentFromFile(const std::string_view file_path)
{
    if (!std::filesystem::exists(file_path))
        return false;

    auto file_size = std::filesystem::file_size(file_path);
    std::ifstream in{file_path.data(), std::ios::in | std::ios::binary};
    if (!in.is_open())
        return false;

    std::string content;
    content.resize(file_size);
    in.read(content.data(), file_size);

    static std::regex pattern{R"delimiter(//\s*(.+)\s*?)delimiter"};
    std::smatch result;
    const std::sregex_iterator end;
    for(auto begin = std::sregex_iterator{std::cbegin(content), std::cend(content), pattern}; begin != end; ++begin)
    {
        std::cout<< "result size:" << (*begin).size() << " capture string [0]:"<< (*begin)[0].str()<<std::endl;
        std::cout<< "result size:" << (*begin).size() << " capture string [1]:"<< (*begin)[1].str()<<std::endl;
    }

    return true;
}

//regex_token_iterator like regex_iterator, token a few capture string, default 0, choose entire match string
bool testingRegexTokenIterator()
{
    static const std::string examples{R"(2013/22/33 2023/11/39 2024/01/22)"};
    static std::regex pattern{R"delimiter((\d{4})/(0?[1-9]|1[0-2])/(0?[1-9]|1[0-2]|3[0-1]))delimiter"};

    {
        const std::sregex_token_iterator end;
        for (std::sregex_token_iterator begin{std::cbegin(examples), std::cend(examples), pattern, {1, 2}}; begin != end; ++begin)
        {
            std::cout << *begin << std::endl;
        }
    }
    //tokenization
    std::cout<<"tokenization"<<std::endl;
    {
        const std::sregex_token_iterator end;
        for (std::sregex_token_iterator begin{std::cbegin(examples), std::cend(examples), pattern, -1}; begin != end; ++begin)
        {
            std::cout << *begin << std::endl;
        }
    }

    return true;
}

//regex_replace, format: $n = the n capture, $& = entire regex string, $’ = prefix string, $‘ = suffix string
void regexReplaceString()
{
    static const std::string example{R"deli(<body><h1>Header</h1><p>Some Text</p></body> XXXXX <body><h1>Header</h1><p>Some Text</p></body>)deli"};
    static std::regex pattern{R"delimiter(<h1>(.*?)</h1><p>(.*?)</p>)delimiter"};
    static std::string replace_format{R"delimiter(H1 = $1, P1 = $2)delimiter"};
    {
        auto result{std::regex_replace(example, pattern, replace_format, std::regex_constants::format_default)};
        std::cout <<"format_default:"<< result << std::endl;
    }
    {
        auto result{std::regex_replace(example, pattern, replace_format, std::regex_constants::format_no_copy)};
        std::cout <<"format_no_copy:"<< result << std::endl;
    }
    {
        auto result{std::regex_replace(example, pattern, replace_format, std::regex_constants::format_first_only)};
        std::cout <<"format_first_only:"<< result << std::endl;
    }
}