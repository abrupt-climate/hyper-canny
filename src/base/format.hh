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
#include <string>
#include <iostream>
#include <sstream>

/*! \file base/format.hh
 *  \brief Defines the `format` function, which creates a string from its
 *  arguments by passing them through a `std::ostringstream`.
 */

namespace HyperCanny
{
    inline void format_to(std::ostream &out) {}

    template <typename First, typename ...Rest>
    void format_to(std::ostream &out, First a, Rest &&...rest)
    {
        out << a;
        format_to(out, std::forward<Rest>(rest)...);
    }

    template <typename ...Args>
    std::string format(Args &&...args)
    {
        std::ostringstream ss;
        format_to(ss, std::forward<Args>(args)...);
        return ss.str();
    }
}
