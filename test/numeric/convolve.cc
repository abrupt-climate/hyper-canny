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
