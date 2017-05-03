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
#include "save_png.hh"
#include "numeric/canny.hh"
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

    auto ai = a.begin();
    for (auto bi = b.begin(); bi != b.end(); ++bi, ++ai)
    {
        EXPECT_NEAR(*ai, *bi, eps);
    }
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

    auto s1 = filter::sobel(a1);
    auto c1 = s1.sel(0, 0).copy();
    c1 /= 2;
    assert_array_equal(c1, b1);

    auto mask = filter::edge_thinning(s1);
    NdArray<bool, 2>
        expected_mask({6, 3}, {
            true, false, true, true, false, true,
            true, false, true, true, false, true,
            true, false, true, true, false, true });
    EXPECT_EQ(mask, expected_mask);
}

TEST (Filters, Gaussian2D)
{
    using numeric::NdArray;
    namespace filter = numeric::filter;

    NdArray<float, 2>
        a1({9, 9}, {
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0 }),
        b1({9, 9}, {
            0.0, 0.0, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.0, 0.0,
            0.0, 0.0, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.0, 0.0,
            0.0, 0.0, 0.01214612, 0.02610994, 0.03369732, 0.02610994, 0.01214612, 0.0, 0.0,
            0.0, 0.0, 0.02610994, 0.05612730, 0.07243752, 0.05612730, 0.02610994, 0.0, 0.0,
            0.0, 0.0, 0.03369732, 0.07243752, 0.09348738, 0.07243752, 0.03369732, 0.0, 0.0,
            0.0, 0.0, 0.02610994, 0.05612730, 0.07243752, 0.05612730, 0.02610994, 0.0, 0.0,
            0.0, 0.0, 0.01214612, 0.02610994, 0.03369732, 0.02610994, 0.01214612, 0.0, 0.0,
            0.0, 0.0, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.0, 0.0,
            0.0, 0.0, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.0, 0.0 });

    assert_array_equal(b1, filter::gaussian(a1, 2, 1.4));
}

TEST (Filters, GaussianNormalised)
{
    using numeric::NdArray;
    namespace filter = numeric::filter;

    auto noise = std::bind(
        std::normal_distribution<double>(0.0, 1.0), std::mt19937());

    NdArray<double, 3>
        a1({16, 42, 23});
    std::generate(a1.begin(), a1.end(), noise);

    auto b1 = filter::gaussian(a1, 5, 2.0);
    ASSERT_NEAR(a1.sum(), b1.sum(), 1e-6);
}

TEST (Filters, Sobel3D)
{
    using numeric::NdArray;
    namespace fourier = numeric::fourier;
    namespace filter = numeric::filter;

    auto noise = std::bind(
        std::normal_distribution<float>(0.0, 1.0), std::mt19937());

    numeric::shape_t<3> shape = {32, 24, 40};
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

    auto apply_filter = [&fft_data, &fft_kernel] (auto const &data, auto const &kernel)
    {
        fft_data.real_space() = data;
        fft_kernel.real_space() = 0.0f;

        auto kernel_view = fft_kernel.real_space().periodic_view({-1, -1, -1}, {3, 3, 3});
        kernel_view = kernel;

        fft_kernel.forward();
        fft_data.forward();
        fft_data.freq_space() *= fft_kernel.freq_space();
        fft_data.inverse();
        fft_data.real_space() /= fft_data.size();
    };

    apply_filter(a1, kernel1);
    auto r1 = filter::sobel(a1, 2);
    ASSERT_NEAR(r1.std(), fft_data.real_space().std(), 1e-4);
    assert_array_equal(r1, fft_data.real_space());

    apply_filter(a1, kernel2);
    auto r2 = filter::sobel(a1, 1);
    ASSERT_NEAR(r2.std(), fft_data.real_space().std(), 1e-4);
    assert_array_equal(r2, fft_data.real_space());
}
