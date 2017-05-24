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

/*! \file base/generator.hh
 *  \brief Generator class
 */

#include "exceptions.hh"
#include "iterating.hh"
#include <functional>
#include <iterator>

namespace HyperCanny
{
    class GeneratorExit {};

    template <typename T>
    class Generator: public std::iterator<std::forward_iterator_tag, T>
    {
        std::function<T ()> step;
        bool is_end;
        T buffer;

        public:
            template <typename Fn>
            Generator(Fn f)
                : step(f)
                , is_end(false)
            {
                ++(*this);
            }

            Generator()
                : is_end(true)
            {}

            Generator &operator++()
            {
                try {
                    buffer = step();
                }
                catch (GeneratorExit const &e) {
                    is_end = true;
                }
                return *this;
            }

            T const &operator*() const
            {
                if (not is_end)
                    return buffer;
                else
                    throw Exception("At end of generator.");
            }

            bool operator!=(Generator const &other) const
            {
                return other.is_end != is_end;
            }
    };

    template <typename Fn>
    auto make_generator(Fn f) -> Span<Generator<decltype(f())>>
    {
        using T = decltype(f());
        return Span<Generator<T>>(Generator<T>(f), Generator<T>());
    }
}
