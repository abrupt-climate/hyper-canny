#include <gtest/gtest.h>

#include "numeric/convolution.hh"

using namespace HyperCanny;

TEST (Algorithms, Convolution)
{
    using numeric::NdArray;
    using numeric::convolve;
    
    NdArray<float,2> a({4, 4}, {
         1.0, 0.0, 2.0, 0.0,
         0.0, 0.0, 0.0, 0.0,
         3.0, 0.0, 4.0, 0.0,
         0.0, 0.0, 0.0, 0.0
    });
    NdArray<float,2> k({3, 3}, {
         0.0, -0.1,  0.0,
        -0.2,  1.0, -0.4,
         0.0, -0.8,  0.0
    });
    NdArray<float,2> expected({4, 4}, {
         1.0, -0.8,  2.0, -1.0,
        -1.1,  0.0, -2.0,  0.0,
         3.0, -2.0,  4.0, -2.2,
        -2.5,  0.0, -3.4,  0.0
    });

    ASSERT_EQ(convolve(a, k), expected);
}
