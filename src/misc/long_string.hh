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

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <functional>

#include "fancy_string.hh"

namespace Misc
{
    namespace fancy
    {
        template <typename I>
        void split(std::string const &s, char d, I inserter)
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

        struct LongString
        {
            std::vector<std::string> words;
            unsigned width;
            std::function<ptr ()> left;

            LongString(std::string const &s, unsigned width_,
                std::function<ptr ()> const &left_):
                width(width_), left(left_)
            {
                split(s, ' ', std::back_inserter(words));
            }

            friend std::ostream &operator<<(std::ostream &out, LongString const &ls)
            {
                auto i = ls.words.begin();
                while (i != ls.words.end())
                {
                    MultiString m;
                    m.push_back(ls.left());

                    while (i != ls.words.end())
                    {
                        if (m.length() + i->length() > ls.width)
                            break;

                        m << *i << " ";
                        ++i;
                    }

                    out << m.str() << std::endl;
                }

                return out;
            }
        };
    }
}
