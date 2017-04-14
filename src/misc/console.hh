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

#include "fancy_string.hh"

namespace Misc
{
    namespace console
    {
        using fancy::ptr;
        using fancy::Style;
        using colour::Colour;

        /* Console style generators */
        inline ptr fg(unsigned r, unsigned g, unsigned b) {
            return std::make_unique<Style>(format("\033[38;2;", r, ";", g, ";", b, "m"));
        }

        inline ptr fg(Colour const &c) {
            return fg(c.r, c.g, c.b);
        }

        inline ptr bg(unsigned r, unsigned g, unsigned b) {
            return std::make_unique<Style>(format("\033[48;2;", r, ";", g, ";", b, "m"));
        }

        inline ptr bg(Colour const &c) {
            return bg(c.r, c.g, c.b);
        }

        inline ptr reset() {
            return std::make_unique<Style>("\033[m");
        }
    }
}
