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
