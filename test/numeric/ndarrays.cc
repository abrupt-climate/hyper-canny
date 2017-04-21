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
    std::iota(a1.begin(), a1.end(), 42);

    NdArray<int,1> b1({2}, {42, 43}),
                   b2({2}, {44, 45}),
                   b3({2}, {42, 44}),
                   b4({2}, {43, 45});

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

    a1.sub<1>(0, 4, 3) += 2;
    NdArray<int,2> b2({4, 4}, {
            3, 2, 3, 2, 0, 0, 0, 0,
            1, 0, 1, 0, 2, 2, 2, 2});
    ASSERT_EQ(a1, b2);
}
