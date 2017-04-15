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
#include "base/string_utils.hh"

namespace Misc
{
    namespace fancy
    {
        struct LongString
        {
            std::vector<std::string> words;
            unsigned width;
            std::function<ptr ()> left;

            LongString(std::string const &s, unsigned width_,
                std::function<ptr ()> const &left_):
                width(width_), left(left_)
            {
                string_split(s, ' ', std::back_inserter(words));
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
