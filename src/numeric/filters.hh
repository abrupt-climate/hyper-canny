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
namespace filter
{
    template <typename Input>
    typename array_traits<Input>::copy_type gradient(Input const &input, unsigned axis)
    {
        using value_type = typename array_traits<Input>::value_type;
        NdArray<value_type, 1> kernel({3}, {0.5, 0.0, -0.5});
        return convolve_1d(input, kernel, axis);
    }

    template <typename real_t>
    NdArray<real_t, 1> gaussian_kernel(unsigned n, float sigma)
    {
        NdArray<real_t, 1> kernel({2*n + 1});
        // real_t norm = 1. / (sqrt(2 * M_PI) * sigma);

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

    template <typename Input, typename Kernel>
    typename array_traits<Input>::copy_type sobel(
            Input const &input, unsigned axis,
            Kernel const &smooth_kernel,
            Kernel const &gradient_kernel)
    {
        constexpr unsigned D = array_traits<Input>::dimension;
        using output_type = typename array_traits<Input>::copy_type;

        output_type output(input.shape()), buffer(input.shape());

        // NdArray<real_t, 1> smooth_kernel({3}, {0.25, 0.50, 0.25});
        // NdArray<real_t, 1> gradient_kernel({3}, {0.5, 0.0, -0.5});

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

    template <typename Input>
    NdArray<typename array_traits<Input>::value_type, array_traits<Input>::dimension+1>
    smooth_sobel(Input const &input, unsigned n, double sigma)
    {
        constexpr unsigned D = array_traits<Input>::dimension;
        using real_t = typename array_traits<Input>::value_type;
        using output_type = NdArray<real_t, D+1>;

        auto G = gaussian_kernel<real_t>(n, sigma);
        auto smooth_kernel = convolve_padding_zero(
                NdArray<real_t, 1>({3}, {0.25, 0.50, 0.25}), G);
        auto gradient_kernel = convolve_padding_zero(
                NdArray<real_t, 1>({3}, {0.5, 0.0, -0.5}), G);

        output_type output(extend_one(input.shape(), D+1));
        for (unsigned k = 0; k < D; ++k)
        {
            auto outs = output.sel(0, k);
            outs = sobel(input, k, smooth_kernel, gradient_kernel);
        }

        auto vec_view = output.template view_reduced_to<D+1>();
        #pragma omp parallel
        {
            #pragma omp for nowait
            for (size_t i = 0; i < vec_view.size(); ++i)
            {
                auto &x = vec_view[i];

                real_t l2 = 0.0;
                for (unsigned k = 0; k < D; ++k)
                    l2 += x[k]*x[k];

                if (l2 == 0.0)
                {
                    x[D] = std::numeric_limits<real_t>::infinity();
                    continue;
                }

                x[D] = 1. / sqrt(l2);
                for (unsigned k = 0; k < D; ++k)
                    x[k] *= x[D];
            }
        }
        return output;
    }
    /*! \brief Sobel edge filter
     *
     *  Filters the input array with the Sobel kernel for each orthogonal
     *  direction, storing the resulting vector field in a new `NdArray`.
     *  This new array has a rank of one higher that the input array.
     *  It contains the resulting vector field in homogeneous coordinates,
     *  where the directional components have been normalised.
     */
    template <typename Input>
    NdArray<typename array_traits<Input>::value_type, array_traits<Input>::dimension+1>
    sobel(Input const &input)
    {
        constexpr unsigned D = array_traits<Input>::dimension;
        using real_t = typename array_traits<Input>::value_type;
        using output_type = NdArray<real_t, D+1>;

        output_type output(extend_one(input.shape(), D+1));
        for (unsigned k = 0; k < D; ++k)
        {
            auto outs = output.sel(0, k);
            outs = sobel(input, k);
        }

        auto vec_view = output.template view_reduced_to<D+1>();
        std::for_each(vec_view.begin(), vec_view.end(),
            [] (auto &x)
        {
            real_t l2 = 0.0;
            for (unsigned k = 0; k < D; ++k)
                l2 += x[k]*x[k];

            if (l2 == 0.0)
            {
                x[D] = std::numeric_limits<real_t>::infinity();
                return;
            }

            x[D] = 1. / sqrt(l2);
            for (unsigned k = 0; k < D; ++k)
                x[k] *= x[D];
        });

        return output;
    }


    template <typename Input>
    NdArray<bool, array_traits<Input>::dimension - 1>
    edge_thinning(Input const &input)
    {
        using real_t = typename array_traits<Input>::value_type;
        constexpr unsigned D = array_traits<Input>::dimension - 1;

        auto vec_view = input.template const_view_reduced_to<D + 1>();
        shape_t<D> window_shape;
        window_shape.fill(3);

        auto output_shape = reduce_one(input.shape(), 0);
        NdArray<bool, D> output(output_shape);

        auto outbit = output.begin();
        // #pragma omp parallel
        {
            // #pragma omp for nowait
            // for (size_t i = 0; i < vec_view.size(); ++i)
            for (auto i = vec_view.begin(); i != vec_view.end(); ++i)
            {
                // shape_t<D> index = vec_view.slice().index(i);
                shape_t<D> index = i.index();
                stride_t<D> window_offset = index - window_shape / 2;
                NdArray<real_t, D> window(window_shape);
                window = input.sel(0, D).const_periodic_view(window_offset, window_shape);

                shape_t<D> wia, wib;
                for (unsigned k = 0; k < D; ++k)
                {
                    unsigned d = round((*i)[k]);
                    wia[k] = 1 - d;
                    wib[k] = 1 + d;
                }

                real_t value = (*i)[D];
                real_t a = window[wia];
                real_t b = window[wib];

                // #pragma omp critical
                {
                if (not std::isfinite(value))
                    *outbit = false;
                else
                    *outbit = (value <= a) && (value <= b);
                    // output[i] = (value <= a) && (value <= b);
                }
                //*outbit = ((*i)[D] <= window[wia] && (*i)[D] <= window[wib]);
                ++outbit;
            }
        }

        return output;
    }

    template <typename Location, typename Predicate, typename Action, typename GetNeighbours>
    void floodfill(Predicate predicate, Action action, GetNeighbours get_neighbours, Location const &start)
    {
        std::queue<Location> queue;
        queue.push(start);

        while (not queue.empty())
        {
            Location const &current = queue.front();

            action(current);

            for (Location const &neighbour : get_neighbours(current))
                if (predicate(neighbour))
                    queue.push(neighbour);

            queue.pop();
        }
    }

    template <typename Input, typename Mask>
    NdArray<bool, array_traits<Input>::dimension - 1>
    double_threshold(Input const &input, Mask const &mask, double lower, double upper)
    {
        constexpr unsigned D = array_traits<Input>::dimension - 1;

        if (reduce_one(input.shape(), 0) != mask.shape())
            throw Exception("Shapes of input and mask do not match.");

        Slice<D> slice(mask.shape());
        auto value = input.template const_view_reduced_to<D + 1>();
        NdArray<bool, D> output(mask.shape());
        NdArray<bool, D> done(mask.shape());
        Grid<D> grid(mask.shape());
        std::fill(output.begin(), output.end(), false);
        std::fill(done.begin(), done.end(), false);

        shape_t<D> window_shape;
        window_shape.fill(3);

        auto predicate = [&] (size_t i)
        {
            if (done[i]) return false;

            done[i] = true;
            return mask[i] && !output[i] && (value[i][D] <= upper);
        };

        auto action = [&] (size_t i)
        {
            output[i] = true;
        };

        auto get_neighbours = [&] (size_t i)
        {
            shape_t<D> index = slice.index(i);
            stride_t<D> offset;
            for (unsigned k = 0; k < D; ++k)
                offset[k] = (signed long)index[k] - 1;

            return grid.const_periodic_view(offset, window_shape);
        };

        for (size_t i = 0; i < grid.size(); ++i)
        {
            if (!mask[i] || done[i] || (value[i][D] > lower))
                continue;
            floodfill(predicate, action, get_neighbours, i);
        }

        return output;
    }
}}}
