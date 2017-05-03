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

#include <cmath>
#include <queue>

namespace HyperCanny {
namespace numeric {
/*! \brief Filters
 */
namespace filter
{
    /*! \brief Gradient operator
     *
     * Filters input with a \f$[0.5, 0.0, -0.5]\f$ kernel on the given axis.
     */
    template <typename Input>
    typename array_traits<Input>::copy_type gradient(Input const &input, unsigned axis)
    {
        using value_type = typename array_traits<Input>::value_type;
        NdArray<value_type, 1> kernel({3}, {0.5, 0.0, -0.5});
        return convolve_1d(input, kernel, axis);
    }


    /*! \brief Gaussian smoothing kernel
     *
     * Generates a normalised Gaussian kernel.
     *   \param n The length of the output will be \f$2n + 1\f$.
     *   \param sigma The standard deviation of the kernel, measured in pixels.
     *   \return 1d array of length \f$2n + 1\f$.
     */
    template <typename real_t>
    NdArray<real_t, 1> gaussian_kernel(unsigned n, float sigma)
    {
        NdArray<real_t, 1> kernel({2*n + 1});

        for (unsigned i = 0; i < n; ++i)
        {
            real_t x = n - i;
            real_t y = exp(- x*x / (2*sigma*sigma));
            kernel[i] = y;
            kernel[2*n - i] = y;
        }
        kernel[n] = 1.0;
        kernel /= kernel.sum();

        return kernel;
    }

    /*! \brief Gaussian smoothing
     *
     * Filters the input with a Gaussian kernel.
     */
    template <typename Input>
    typename array_traits<Input>::copy_type gaussian(
            Input const &input, unsigned n, float sigma)
    {
        constexpr unsigned D = array_traits<Input>::dimension;
        using real_t = typename array_traits<Input>::value_type;
        using output_type = typename array_traits<Input>::copy_type;

        auto kernel = gaussian_kernel<real_t>(n, sigma);
        output_type output(input.shape()), buffer(input.shape());

        convolve_1d(input, kernel, output, 0);
        for (unsigned axis = 1; axis < D; ++axis)
        {
            std::swap(output, buffer);
            convolve_1d(buffer, kernel, output, axis);
        }

        return output;
    }

    /*! \brief Sobel operator
     *
     *  Applies Sobel operator on one axis by filtering with a
     *  smoothing kernel in all other axes, and a gradient kernel
     *  on the given axis.
     *
     *  \param input Input array.
     *  \param axis Direction in which to take the gradient
     *  \param smooth_kernel Custom smoothing kernel
     *  \param gradient_kernel Custom gradient kernel
     *
     *  The last two arguments can be omitted. They will default to
     *  \f$[1/4, 1/2, 1/4]\f$ for smoothing and \f$[1/2, 0, -1/2]\f$
     *  for taking the gradient.
     */
    template <typename Input, typename Kernel>
    typename array_traits<Input>::copy_type sobel(
            Input const &input, unsigned axis,
            Kernel const &smooth_kernel,
            Kernel const &gradient_kernel)
    {
        constexpr unsigned D = array_traits<Input>::dimension;
        using output_type = typename array_traits<Input>::copy_type;

        output_type output(input.shape()), buffer(input.shape());

        convolve_1d(input, smooth_kernel, buffer, (axis + 1) % D);
        for (unsigned k = 2; k < D; ++k)
        {
            std::swap(output, buffer);
            convolve_1d(output, smooth_kernel, buffer, (axis + k) % D);
        }
        convolve_1d(buffer, gradient_kernel, output, axis);

        return output;
    }

    template <typename Input>
    typename array_traits<Input>::copy_type sobel(
            Input const &input, unsigned axis)
    {
        using real_t = typename array_traits<Input>::value_type;

        NdArray<real_t, 1> smooth_kernel({3}, {0.25, 0.50, 0.25});
        NdArray<real_t, 1> gradient_kernel({3}, {0.5, 0.0, -0.5});
        return sobel(input, axis, smooth_kernel, gradient_kernel);
    }
}}}
