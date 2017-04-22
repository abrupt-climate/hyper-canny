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

/*! \file numeric/types.hh
 *  \brief Some basic typedefs
 */

#include <array>
#include <cstddef>
#include <iostream>

namespace HyperCanny { namespace numeric
{
    /*!
     * \addtogroup NdArrays
     * @{
     */
    using std::size_t;

    template <unsigned D>
    using shape_t = std::array<size_t, D>;

    template <unsigned D>
    using stride_t = std::array<ptrdiff_t, D>;

    template <unsigned long D>
    stride_t<D> operator-(shape_t<D> const &a, shape_t<D> const &b)
    {
        stride_t<D> x;
        for (unsigned i = 0; i < D; ++i)
            x[i] = static_cast<ptrdiff_t>(a[i]) - b[i];
        return x;
    }

    template <typename T, unsigned long D, typename T2>
    std::array<T, D> operator/(std::array<T, D> const &a, T2 denom)
    {
        std::array<T, D> x;
        for (unsigned i = 0; i < D; ++i)
            x[i] = a[i] / denom;
        return x;
    }

    template <typename T, unsigned long D, typename T2>
    std::array<T, D> operator-(std::array<T, D> const &a, T2 y)
    {
        std::array<T, D> x;
        for (unsigned i = 0; i < D; ++i)
            x[i] = a[i] - y;
        return x;
    }

    template <typename T, unsigned long D, typename T2>
    std::array<T, D> operator*(std::array<T, D> const &a, T2 factor)
    {
        std::array<T, D> x;
        for (unsigned i = 0; i < D; ++i)
            x[i] = a[i] * factor;
        return x;
    }
    /*! @} */
}}
