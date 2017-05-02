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
#include "counter.hh"

#include <numeric>
#include <algorithm>

namespace HyperCanny {
namespace numeric {
    /*! \brief N-dimensional convolution
     *
     * Computes the convolution by the definition
     *
     * \f[(f * g)[n]\ =\ \sum f[m]\, g[n - m]\f]
     *
     * taking the input array to be cyclic.
     *
     * \param data Input array.
     * \param kernel Kernel array.
     * \return Convolution of input array with kernel.
     */
    template <typename C1, typename C2, typename Output>
    Output &convolve(C1 const &data, C2 const &kernel, Output &output)
    {
        using real_t = typename array_traits<C1>::value_type;
        constexpr unsigned D = array_traits<C1>::dimension;

        if (output.shape() != data.shape())
            throw Exception("Shapes do not match.");

        for (auto i = output.begin(); i != output.end(); ++i)
        {
            shape_t<D> index = i.index();
            stride_t<D> window_offset = index - kernel.shape() / 2;
            auto const window = data.periodic_view(window_offset, kernel.shape());
            *i = std::inner_product(
                window.begin(), window.end(),
                kernel.reverse_all().begin(), (real_t)0.0);
        }

        return output;
    }

    template <typename C1, typename C2>
    typename array_traits<C1>::copy_type convolve(C1 const &data, C2 const &kernel)
    {
        using real_t = typename array_traits<C1>::value_type;
        constexpr unsigned D = array_traits<C1>::dimension;

        NdArray<real_t,D> result(data.shape());
        return convolve(data, kernel, result);
    }

    template <typename Input, typename Kernel>
    typename array_traits<Input>::copy_type
    convolve_padding_zero(
            Input const &input,
            Kernel const &kernel)
    {
        using real_t = typename array_traits<Input>::value_type;
        constexpr unsigned D = array_traits<Input>::dimension;

        shape_t<D> padding = kernel.shape() / 2;
        shape_t<D> padded_shape = input.shape() + kernel.shape();

        NdArray<real_t,D> padded_input(padded_shape);
        std::fill(padded_input.container().begin(), padded_input.container().end(), (real_t)0.0);
        padded_input.sub(padding, input.shape()) = input;

        return convolve(padded_input, kernel);
    }

    /*! \brief Convolve input in one direction with a one dimensional
     *  kernel.
     *
     *  \param input Input array.
     *  \param kernel Kernel array.
     *  \param output Output array, should have same shape as input.
     *  \param axis Axis over which to convolve.
     */
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
        Slice<D-1> flat(orthogonal_slice.shape);

        #pragma omp parallel
        {
        #pragma omp for nowait
        for (size_t i = 0; i < orthogonal_slice.size; ++i)
        //for (NdCounter<D-1> i(orthogonal_slice.offset, orthogonal_slice);
        //     i != NdCounter<D-1>(); ++i)
        {
            size_t j = orthogonal_slice.flat_index(flat.index(i));
            Slice<1> line(j, {length}, {stride});
            output.view(line) = convolve(input.view(line), kernel);
        }
        }

        return output;
    }

    template <typename Input, typename Kernel>
    typename array_traits<Input>::copy_type convolve_1d(
            Input const &input,
            Kernel const &kernel,
            unsigned axis)
    {
        using output_type = typename array_traits<Input>::copy_type;
        output_type output(input.shape());
        convolve_1d(input, kernel, output, axis);
        return output;
    }
}} // HyperCanny::numeric
