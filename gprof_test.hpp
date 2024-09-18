#ifndef __GPROF_TEST_HPP__
#define __GPROF_TEST_HPP__

#include <vector>
#include <string_view>
#include <fstream>

namespace gprof_test
{

    class NameDb
    {
    public:
        NameDb(std::string_view filepath)
        {
            std::ifstream in{filepath, std::ios::binary | std::ios::in};
            if (!in.is_open())
                throw std::invalid_argument{"unable to open file"};

            std::string name;
            while (in >> name)
            {
                if (nameExsits(name))
                    increamentNameCount(name);
                else
                    addNewName(name);
            }
        }

    private:
        std::vector<std::pair<std::string, int>> m_names;

        bool nameExsits(std::string_view sv) const
        {
            for (auto &name : m_names)
            {
                auto &[nm, count] = name;
                if (nm == sv)
                    return true;
            }
            return false;
        }

        void increamentNameCount(std::string_view sv)
        {
            for (auto &name : m_names)
            {
                auto &[nm, count] = name;
                if (nm == sv)
                    count++;
            }
        }

        void addNewName(std::string_view sv)
        {
            m_names.push_back(std::make_pair(sv.data(), 1));
        }
    };
}

#endif //__GPROF_TEST_HPP__