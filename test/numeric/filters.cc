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
#include <iostream>
#include <cmath>
#include "base.hh"
#include "numeric/filters.hh"

using namespace HyperCanny;

namespace HyperCanny {
namespace numeric
{
    template <typename T>
    std::ostream &operator<<(std::ostream &out, NdArrayImpl<T> const &v)
    {
        out << "  shape: { " << string_join(v.shape(), ", ") << "}\n";
        out << "  values: { " << head(v);
        for (auto const &x : tail(v))
            out << ", " << x;
        out << " }" << std::endl;
        return out;
    }
}}

template <typename T1, typename T2>
inline void assert_array_equal(T1 const &a, T2 const &b, double eps=1e-4)
{
    ASSERT_EQ(a.shape(), b.shape()) << "arrays should have same shape.";
    ASSERT_TRUE(std::equal(a.begin(), a.end(), b.begin(),
        [eps] (auto k, auto l)
    {
        return std::abs(k - l) < eps;
    })) << "arrays should be equal to an abs error of " << eps << "\n"
        << "first:\n" << a
        << "second:\n" << b;
}

TEST (Filters, Gradient)
{
    using numeric::NdArray;
    namespace filter = numeric::filter;

    NdArray<float, 2>
        a1({3, 3}, {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9 }),
        b1({3, 3}, {
            -0.5, 1.0, -0.5,
            -0.5, 1.0, -0.5,
            -0.5, 1.0, -0.5 }),
        b2({3, 3}, {
            -1.5, -1.5, -1.5,
             3.0,  3.0,  3.0,
            -1.5, -1.5, -1.5 });

    assert_array_equal(filter::gradient(a1, 0), b1);
    assert_array_equal(filter::gradient(a1, 1), b2);
}

TEST (Filters, Sobel)
{
    using numeric::NdArray;
    namespace filter = numeric::filter;

    NdArray<float, 2>
        a1({6, 3}, {
            0, 0, 0, 1, 1, 1,
            0, 0, 0, 1, 1, 1,
            0, 0, 0, 1, 1, 1 }),
        b1({6, 3}, {
            -0.5, 0, 0.5, 0.5, 0, -0.5,
            -0.5, 0, 0.5, 0.5, 0, -0.5,
            -0.5, 0, 0.5, 0.5, 0, -0.5 }),
        b2({6, 3});
    std::fill(b2.begin(), b2.end(), 0.0);

    assert_array_equal(filter::sobel(a1, 0), b1);
    assert_array_equal(filter::sobel(a1, 1), b2);
}
