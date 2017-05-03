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

/*! \file numeric/canny.hh
 *  \brief Canny edge detection
 */

#include "filters.hh"

namespace HyperCanny {
namespace numeric {
namespace filter {
    /*! \brief Normalize homogeneous vectors
     *
     *  The output of the Sobel filter is stored in an array of
     *  homogeneous vectors. These vectors have one extra dimension to
     *  store an extra scaling factor. This function puts the
     *  homogeneous coordinates in a normalised form.
     */
    template <typename Input>
    void normalize_homogeneous_vectors(Input &input)
    {
        constexpr unsigned D = array_traits<Input>::dimension - 1;
        using real_t = typename array_traits<Input>::value_type;

        auto vec_view = input.template view_reduced_to<D + 1>();
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
    }

    /*! \brief Gaussian smoothing and sobel operator.
     *
     *  Calls the sobel() function with kernels that have been
     *  convolved with a specified Gaussian. This provides smoothing
     *  and Sobel filtering in one go. Since the resulting filter has
     *  a size of the sum of the specified Gaussian and sobel operators,
     *  this version may actually be slower than having a separate
     *  Gaussian filter step.
     */
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

        normalize_homogeneous_vectors(output);
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

        normalize_homogeneous_vectors(output);
        return output;
    }


    /*! \brief Edge thinning by non-maximum supression.
     *
     *  This assumes that the input consists of homogeneous n-vectors in
     *  normalised form. By rounding a unit-vector to 0/1 values the
     *  neighbouring pixels up and down the gradient direction are found. If
     *  the vector magnitude of the center pixel is larger than both selected
     *  neighbouring pixels, it is kept as an edge pixel.
     *
     *  \param input NdArray<real_t,D+1> with shape <D+1, n_1, ..., n_D>
     *  \return bit mask NdArray<bool,D> with shape <n_1, ..., n_D>
     *
     *  The loop in this function is not parallel, because writing to a boolean
     *  array is not safe, and overhead for an omp critical write is too much.
     */
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
        for (auto i = vec_view.begin(); i != vec_view.end(); ++i)
        {
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

            if (not std::isfinite(value))
                *outbit = false;
            else
                *outbit = (value <= a) && (value <= b);
            ++outbit;
        }

        return output;
    }

    /*! \brief Generic queue based floodfill algorithm
     *
     *  \param predicate Predicate neighbour locations need to pass before
     *  admission to the queue.
     *  \param action Function to run on current location.
     *  \param get_neighbours Function that finds neighbouring locations.
     *  \param start Starting location.
     */
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

    /*! \brief Apply double threshold criterium to found edges.
     *
     *  Since the inverse of the vector magnitudes are stored, the
     *  lower values mean stronger edges.
     *
     *  \param input Output of sobel() function.
     *  \param mask Output of edge_thinning() function.
     *  \param lower Lower bound of double threshold, everything below this
     *  value is definitely an edge.
     *  \param upper Upper bound of double threshold, everything above this
     *  value is definitely *not* an edge. Everything between `upper` and
     *  `lower` is only considered an edge if it is connected to an edge.
     */
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
