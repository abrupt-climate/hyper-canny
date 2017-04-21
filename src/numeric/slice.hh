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

/*! \file numeric/slice.hh
 *  \brief The `Slice` class provides NdArrays with their basic shape and
 *  stride structure.
 */

#include "ndrange.hh"

namespace HyperCanny { namespace numeric
{
    /*!
     * \addtogroup NdArrays
     * @{
     */

    /*! \brief Contains all information on slicing a contiguous block of data.
     */
    template <unsigned D>
    class Slice
    {
        public:
            size_t offset;          /*!< Offset, should reference first element
                                         in the sliced array. */
            size_t size;            /*!< The number of elements in the sliced
                                         array. */
            shape_t<D> shape;       /*!< Size in each dimension. */
            stride_t<D> stride;     /*!< Memory step size in each dimension */

            Slice() {}

            /*! \brief Constructs a slice with a given shape, representing the
             *  complete contiguous array.
             */
            explicit Slice(shape_t<D> const &shape):
                offset(0), size(calc_size<D>(shape)),
                shape(shape),
                stride(calc_stride<D>(shape))
            {}

            /*! \brief Constructs a slice with all members given.
             */
            Slice(size_t offset, shape_t<D> const &shape, stride_t<D> const &stride):
                offset(offset), size(calc_size<D>(shape)),
                shape(shape),
                stride(stride)
            {}

            /*! \brief Return the flat vector index belonging to a given
             *  N-dimensional index.
             */
            size_t flat_index(shape_t<D> const &x) const
            {
                size_t j = offset;
                for (unsigned i = 0; i < D; ++i)
                    j += x[i] * stride[i];
                return j;
            }

            size_t flat_index(stride_t<D> const &x) const
            {
                return flat_index(modulo(x, shape));
            }

            /*! \brief Transpose the slice, by reversing the shape and stride.
             */
            Slice<D> transpose() const // reverse axes
            {
                stride_t<D> new_stride;
                shape_t<D> new_shape;
                for (unsigned i = 0; i < D; ++i)
                {
                    new_stride[D-i-1] = stride[i];
                    new_shape[D-i-1] = shape[i];
                }
                return Slice<D>(offset, new_shape, new_stride);
            }

            /*! \brief Take a sub-section of the slice in one axis.
             */
            template <unsigned axis>
            Slice<D> sub(size_t begin, size_t end, int step = 1)
            {
                size_t new_offset = offset + begin * stride[axis];
                stride_t<D> new_stride = stride;
                shape_t<D> new_shape = shape;
                new_stride[axis] *= step;
                new_shape[axis] = (int)(end - begin + step - 1) / abs(step);
                return Slice<D>(new_offset, new_shape, new_stride);
            }

            /*! \brief Select a row, reducing the dimensionality by one.
             */
            template <unsigned axis>
            Slice<D-1> sel(size_t idx)
            {
                size_t new_offset = offset + idx * stride[axis];
                stride_t<D-1> new_stride = reduce_one<D, axis>(stride);
                shape_t<D-1> new_shape = reduce_one<D, axis>(shape);
                return Slice<D-1>(new_offset, new_shape, new_stride);
            }

            /*! \brief Reverse one axis.
             *
             *  This negates the stride in the specified axis, but also the
             *  offset has to be recomputed as the starting point of iteration
             *  has changed.
             */
            template <unsigned axis>
            Slice<D> reverse()
            {
                size_t new_offset = offset + stride[axis] * (shape[axis] - 1);
                stride_t<D> new_stride = stride;
                new_stride[axis] = -stride[axis];
                return Slice<D>(new_offset, shape, new_stride);
            }

            NdRange<D> begin() const
            {
                return NdRange<D>(offset, shape, stride);
            }

            NdRange<D> end() const
            {
                return NdRange<D>();
            }
    };

    /*! \brief Stops recursive definition of `sel` method.
     */
    template <>
    class Slice<0>
    {
        // Slice(shape_t<0> const &) {}
        // Slice(size_t offset, shape_t<0> const &, stride_t<0> const &) {}
    };

    /*! @} */
}} // namespace numeric
