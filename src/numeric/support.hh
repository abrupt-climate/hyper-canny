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

/*! \file numeric/support.hh
 *  \brief Support functions.
 */

#include <array>
#include <cstdint>
#include "types.hh"

namespace HyperCanny { namespace numeric
{
    template <unsigned D>
    inline stride_t<D> calc_stride(shape_t<D> const &shape)
    {
        stride_t<D> stride;

        stride[0] = 1;
        for (unsigned i = 1; i < shape.size(); ++i)
            stride[i] = stride[i-1] * shape[i-1];

        return stride;
    }

    template <unsigned D>
    inline uint32_t calc_size(shape_t<D> const &shape)
    {
        uint32_t size = 1;
        for (unsigned i = 0; i < shape.size(); ++i)
            size *= shape[i];
        return size;
    }

    template <unsigned D, unsigned I, typename T>
    inline std::array<T,D-1> reduce_one(std::array<T,D> const &x)
    {
        std::array<T,D-1> result;
        for (unsigned j = 0, k = 0; j < D; ++j)
        {
            if (j != I)
            {
                result[k] = x[j];
                ++k;
            }
        }
        return result;
    }

	/*!
	 * calculates a mod b, by the mathematical definition:
	 * the C operator % does not give the right answer if a < 0
	 */
	inline int modulo(int a, int b)
	{
		if (a < 0) return b + (a % b);
		else return (a % b);
	}

    template <typename Arg1T, typename Arg2T, unsigned D>
    inline std::array<Arg2T, D> modulo(std::array<Arg1T, D> const &a, std::array<Arg2T, D> const &b)
    {
        std::array<Arg2T, D> result;
        for (unsigned i = 0; i < D; ++i)
        {
            result[i] = modulo(a[i], b[i]);
        }
        return result;
    }

    template <unsigned D>
    inline size_t affine(size_t offset, stride_t<D> const &stride, shape_t<D> const &index)
    {
        for (unsigned i = 0; i < D; ++i)
        {
            offset += stride[i] * index[i];
        }
        return offset;
    }

    template <unsigned D>
    stride_t<D> calc_semi_stride(shape_t<D> const &shape, stride_t<D> const &stride)
    {
        stride_t<D> semi_stride;
        semi_stride[0] = stride[0];

        for (unsigned i = 1; i < D; ++i)
        {
            semi_stride[i] = stride[i] - shape[i-1] * stride[i-1];
        }
    }
}}
