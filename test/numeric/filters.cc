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
#include <random>
#include "base.hh"
#include "numeric/filters.hh"
#include "numeric/rfft.hh"
#include "numeric/support.hh"

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

TEST (Filters, Sobel3D)
{
    using numeric::NdArray;
    namespace fourier = numeric::fourier;
    namespace filter = numeric::filter;

    auto noise = std::bind(
        std::normal_distribution<float>(0.0, 1.0), std::mt19937());

    numeric::shape_t<3> shape = {16, 16, 16};
    NdArray<float, 3> a1(shape);
    std::generate(a1.begin(), a1.end(), noise);

    NdArray<float, 3> kernel1({3, 3, 3}, {
             1.,  2.,  1.,  2.,  4.,  2.,  1.,  2.,  1.,
             0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,  0.,
            -1., -2., -1., -2., -4., -2., -1., -2., -1. });
    kernel1 /= 32;
    NdArray<float, 3> kernel2({3, 3, 3}, {
             1.,  2.,  1.,  0.,  0.,  0., -1., -2., -1.,
             2.,  4.,  2.,  0.,  0.,  0., -2., -4., -2.,
             1.,  2.,  1.,  0.,  0.,  0., -1., -2., -1. });
    kernel2 /= 32;

    fourier::RFFT<float, 3> fft_data(shape), fft_kernel(shape);

    fft_data.real_space() = a1;
    fft_kernel.real_space() = 0.0f;
    auto kernel_view = fft_kernel.real_space().periodic_view({-1, -1, -1}, {3, 3, 3});
    kernel_view = kernel1;
    fft_kernel.forward();
    fft_data.forward();
    fft_data.freq_space() *= fft_kernel.freq_space();
    fft_data.inverse();
    fft_data.real_space() /= fft_data.size();
    auto r1 = filter::sobel(a1, 2);
    auto r2 = numeric::convolve(a1, kernel1);
    auto r3 = filter::sobel(a1, 1);
    auto r4 = numeric::convolve(a1, kernel2);

    ASSERT_NEAR(r1.std(), r2.std(), 1e-4);
    ASSERT_NEAR(r3.std(), r4.std(), 1e-4);
    for (unsigned i = 1; i <= 10000; i *= 10)
    {
        ASSERT_NEAR(r1.container()[i], r2.container()[i], 1e-4);
        ASSERT_NEAR(r3.container()[i], r4.container()[i], 1e-4);
    }

    // ASSERT_NEAR(r1.std(), fft_data.real_space().std(), 1e-4);
}
