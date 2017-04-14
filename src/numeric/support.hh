#pragma once
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
}}

