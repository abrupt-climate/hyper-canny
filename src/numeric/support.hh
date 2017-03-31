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
}}

