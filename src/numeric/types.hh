#pragma once

#include <array>
#include <cstddef>
#include <iostream>

namespace HyperCanny { namespace numeric
{
    using std::size_t;

    template <unsigned D>
    using shape_t = std::array<uint32_t, D>;

    template <unsigned D>
    using stride_t = std::array<int32_t, D>;

    template <typename T>
    inline std::ostream &operator<<(std::ostream &out, std::array<T, 2> const &d)
    {
        return out << d[0] << "x" << d[1];
    }
}}

