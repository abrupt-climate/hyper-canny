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

/*! \file numeric/convolution.hh
 *  \brief Generic N-dimensional convolution.
 */

#include "base.hh"
#include "ndarray.hh"

#include <numeric>
#include <algorithm>

namespace HyperCanny {
namespace numeric {
    template <typename C1, typename C2>
    NdArray<typename array_traits<C1>::value_type, array_traits<C1>::dimension> convolve(C1 &data, C2 &kernel)
    {
        using real_t = typename array_traits<C1>::value_type;
        constexpr unsigned D = array_traits<C1>::dimension;

        NdArray<real_t,D> result(data.shape());

        for (auto i = result.begin(); i != result.end(); ++i)
        {
            shape_t<D> index = i.index();
            stride_t<D> window_offset = index - kernel.shape() / 2;
            auto window = data.periodic_view(window_offset, kernel.shape());
            *i = std::inner_product(
                window.begin(), window.end(),
                kernel.reverse_all().begin(), (real_t)0.0);
        }
        return result;
    }
}} // HyperCanny::numeric
