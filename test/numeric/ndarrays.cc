#include "base.hh"
#include "numeric/ndarray.hh"

#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

using namespace HyperCanny;

TEST (NdArray, Selecting)
{
    using numeric::NdArray;

    NdArray<int,2> a1({2, 2});
    std::iota(a1.begin(), a1.end(), 1);

    NdArray<int,1> b1({2}, {1, 2}),
                   b2({2}, {3, 4}),
                   b3({2}, {1, 3}),
                   b4({2}, {2, 4});

    ASSERT_EQ(a1.sel<1>(0), b1);
    ASSERT_EQ(a1.sel<1>(1), b2);
    ASSERT_EQ(a1.sel<0>(0), b3);
    ASSERT_EQ(a1.sel<0>(1), b4);
}

TEST (NdArray, SubSlicing)
{
    using numeric::NdArray;

    NdArray<int,2> a1({4, 4});
    std::iota(a1.begin(), a1.end(), 1);

    NdArray<int, 2> b1({2, 2}, {6, 7, 10, 11}),
                    b2({1, 4}, {6, 7, 10, 11}),
                    b3({2, 2}, {1, 3, 13, 15}),
                    b4({2, 2}, {4, 3, 8, 7});

    ASSERT_EQ(a1.sub<0>(1, 3).sub<1>(1, 3), b1);
    ASSERT_NE(a1.sub<0>(1, 3).sub<1>(1, 3), b2);
    ASSERT_EQ(a1.sub<0>(0, 4, 2).sub<1>(0, 4, 3), b3);
    ASSERT_EQ(a1.sub<0>(2, 4).sub<1>(0, 2).reverse<0>(), b4);
}

TEST (NdArray, Assignment)
{
    using numeric::NdArray;

    NdArray<int,2> a1({4, 4}, 0);
    a1.sub<0>(0, 4, 2).sub<1>(0, 4, 2) = 1;

    NdArray<int,2> b1({4, 4}, {
            1, 0, 1, 0, 0, 0, 0, 0,
            1, 0, 1, 0, 0, 0, 0, 0});
    ASSERT_EQ(a1, b1);
}

