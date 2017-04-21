#include "base.hh"
#include "numeric/ndarray.hh"

#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

using namespace HyperCanny;

TEST (NdArray, PeriodicRange)
{
    using numeric::NdArray;
    using numeric::PeriodicNdArrayView;

    NdArray<int,2> a1({2, 2}, {1, 2, 3, 4}),
                   b1({2, 2}, {4, 3, 2, 1});

    PeriodicNdArrayView<int,2> r1(a1, {1, 1}, {2, 2});
    ASSERT_EQ(r1, b1);
}

