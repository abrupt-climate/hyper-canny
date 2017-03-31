#pragma once
#include "ndarray.hh"
#include <vector>

namespace HyperCanny { namespace numeric
{
    extern NdArray<size_t, 2> hilbert_array(unsigned n);
    extern std::vector<size_t> hilbert_walk(unsigned n);
}}

