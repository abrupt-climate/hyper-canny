#pragma once
#include "iterating.hh"
#include "optional.hh"
#include "format.hh"
#include <string>
#include <sstream>

namespace HyperCanny
{
    template <typename T>
    std::optional<T> from_string(std::string const &s)
    {
        T value;
        std::istringstream iss(s);

        try {
            iss >> value;
            if (iss.fail()) {
                return std::nullopt;
            }
        }
        catch (std::istream::failure const &e) {
            return std::nullopt;
        }

        return value;
    }

    template <typename T>
    std::string to_string(T const &value)
    {
        std::ostringstream out;
        out << value;
        return out.str();
    }

    template <typename R>
    std::string string_join(R const &r, std::string const &d)
    {
        std::ostringstream out;

        out << head(r);
        for (auto b : tail(r))
            out << d << b;

        return out.str();
    }

    template <typename I>
    void string_split(std::string const &s, char d, I inserter)
    {
        size_t p, q = 0;
        while (q < s.length())
        {
            p = s.find_first_not_of(d, q);
            if (p == std::string::npos) return;

            q = s.find_first_of(d, p);
            if (q == std::string::npos)
            {
                *inserter = s.substr(p, s.length() - p);
                return;
            }
            else
            {
                *inserter = s.substr(p, q-p);
                ++inserter;
            }
        }
    }
}
