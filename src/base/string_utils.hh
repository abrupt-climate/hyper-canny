/* Copyright 2017 Netherlands eScience Center
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
#pragma once

/*! \file base/string_utils.hh
 *  \brief Helper functions for string manipulation.
 */

#include "iterating.hh"
#include "generator.hh"
#include "optional.hh"
#include "format.hh"
#include <string>
#include <sstream>
#include <type_traits>
#include <limits>
#include <iomanip>
#include <memory>

namespace HyperCanny
{
    /*! \brief Extract a value from a string, by passing it through a
     *  `std::istringstream`.
     *  \param s Input string.
     *  \return Parsed value.
     */
    template <typename T>
    std::optional<T> from_string(std::string const &s)
    {
        T value;
        std::istringstream iss(s);

        iss >> value;
        if (iss.fail()) {
            return std::nullopt;
        }
        iss >> std::ws;
        if (not iss.eof()) {
            return std::nullopt;
        }

        return value;
    }

    /*! \brief Create a string from a value by passing it through a
     *  `std::ostringstream`.
     *  \param value Input value.
     *  \return Generated string.
     */
    template <typename T>
    std::string to_string(T const &value)
    {
        std::ostringstream out;

        if (std::is_floating_point<T>::value)
        {
            out << std::setprecision(std::numeric_limits<T>::digits10 + 1);
        }

        out << value;
        return out.str();
    }

    /*! \brief Intersperses a sequence of values with a string.
     *  \param r A range (having `begin` and `end` members).
     *  \param d Delimiting string.
     *  \return Generated string.
     *
     *  The input range may contain any value, as long as it can be streamed
     *  to a `std::ostream` object.
     */
    template <typename R>
    std::string string_join(R const &r, std::string const &d)
    {
        std::ostringstream out;

        out << head(r);
        for (auto b : tail(r))
            out << d << b;

        return out.str();
    }

    inline std::string string_pad(std::string const &s, int n, char d=' ')
    {
        std::string result(n, d);
        std::copy(s.begin(), s.end(), result.begin());
        return result;
    }

    /*! \brief Splits a string on a given delimiter.
     *  \param s Input string.
     *  \param d Delimiting character.
     *  \param inserter Output iterator for resulting strings.
     */
    /*{
        size_t q = 0;
        while (q < s.length())
        {
            size_t p = s.find_first_not_of(d, q);
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
    }*/

    inline Span<Generator<std::string>> string_split(std::string const &s, char d)
    {
        auto q = std::make_shared<size_t>(0);
        auto step_fn = [s, d, q] ()
        {
            if (*q >= s.length())
                throw GeneratorExit();

            size_t p = s.find_first_not_of(d, *q);
            if (p == std::string::npos)
                throw GeneratorExit();

            *q = s.find_first_of(d, p);
            if (*q == std::string::npos)
                return s.substr(p, s.length() - p);

            return s.substr(p, *q-p);
        };

        return make_generator(step_fn);
    }

    template <typename I>
    void string_split(std::string const &s, char d, I inserter)
    {
        auto g = string_split(s, d);
        std::copy(g.begin(), g.end(), inserter);
    }
}
