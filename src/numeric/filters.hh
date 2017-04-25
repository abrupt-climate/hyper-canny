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

/*! \file numeric/filters.hh
 *  \brief Filters
 */

#include "ndarray.hh"
#include "counter.hh"
#include "convolution.hh"

namespace HyperCanny {
namespace numeric {
namespace filter
{
    template <typename Input, typename Kernel>
    typename array_traits<Input>::copy_type convolve_1d(
            Input const &input,
            Kernel const &kernel,
            unsigned axis)
    {
        constexpr unsigned D = array_traits<Input>::dimension;
        using output_type = typename array_traits<Input>::copy_type;

        size_t length = input.shape()[axis];
        int stride = input.stride()[axis];
        auto orthogonal_slice = input.slice().sel(axis, 0);

        output_type output(input.shape());

        for (NdCounter<D-1> i(orthogonal_slice.offset, orthogonal_slice);
             i != NdCounter<D-1>(); ++i)
        {
            Slice<1> line(*i, {length}, {stride});
            output.view(line) = convolve(input.view(line), kernel);
        }

        return output;
    }

    template <typename Input, typename Kernel, typename Output>
    Output &convolve_1d(
            Input const &input,
            Kernel const &kernel,
            Output &output,
            unsigned axis)
    {
        constexpr unsigned D = array_traits<Input>::dimension;

        size_t length = input.shape()[axis];
        int stride = input.stride()[axis];
        auto orthogonal_slice = input.slice().sel(axis, 0);

        for (NdCounter<D-1> i(orthogonal_slice.offset, orthogonal_slice);
             i != NdCounter<D-1>(); ++i)
        {
            Slice<1> line(*i, {length}, {stride});
            output.view(line) = convolve(input.view(line), kernel);
        }

        return output;
    }

    template <typename Input>
    typename array_traits<Input>::copy_type gradient(Input const &input, unsigned axis)
    {
        using value_type = typename array_traits<Input>::value_type;
        NdArray<value_type, 1> kernel({3}, {0.5, 0.0, -0.5});
        return convolve_1d(input, kernel, axis);
    }

    template <typename Input>
    typename array_traits<Input>::copy_type sobel(
            Input const &input, unsigned axis)
    {
        constexpr unsigned D = array_traits<Input>::dimension;
        using value_type = typename array_traits<Input>::value_type;
        using output_type = typename array_traits<Input>::copy_type;

        output_type output(input.shape()), buffer(input.shape());

        NdArray<value_type, 1> smooth_kernel({3}, {0.25, 0.50, 0.25});
        NdArray<value_type, 1> gradient_kernel({3}, {0.5, 0.0, -0.5});

        convolve_1d(input, smooth_kernel, buffer, (axis + 1) % D);
        for (unsigned k = 2; k < D; ++k)
        {
            std::swap(output, buffer);
            convolve_1d(output, smooth_kernel, buffer, (axis + k) % D);
        }
        convolve_1d(buffer, gradient_kernel, output, axis);

        return output;
    }
}}}
