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

#include "base.hh"
#include "numeric/convolution.hh"
#include "numeric/rfft.hh"
// #include "save_png.hh"

#include <random>
#include <cmath>

using namespace HyperCanny;

template <typename ...Args>
class Zip
{
    public:
        class iterator: public std::iterator<std::forward_iterator_tag, std::tuple<typename Args::value_type...>>
        {
            std::tuple<typename Args::iterator...> iterators;
            public:
                
        };
};

template <typename T1, typename T2>
inline void assert_array_equal(T1 const &a, T2 const &b, double eps=1e-5)
{
    ASSERT_EQ(a.shape(), b.shape()) << "arrays should have same shape.";
    ASSERT_TRUE(std::equal(a.begin(), a.end(), b.begin(),
        [eps] (auto k, auto l)
    {
        return abs(k - l) < eps;
    })) << "arrays should be equal to " << eps;
}

TEST (Fourier, Convolution)
{
    using numeric::NdArray;
    using numeric::convolve;
    using numeric::PeriodicNdArrayView;
    using numeric::fourier::RFFT;
    using numeric::shape_t;

    Timer timer;

    auto noise = std::bind(
        std::normal_distribution<float>(0.0, 1.0), std::mt19937());

    constexpr unsigned D = 2;
    shape_t<D> data_shape   {  64,  64 },
               kernel_shape {  16,  16 };
    size_t N = 64 * 64;

    RFFT<float, D> fft_data(data_shape);
    std::generate(fft_data.real_space().begin(), fft_data.real_space().end(), noise);

    NdArray<float, D> kernel(kernel_shape);
    std::generate(kernel.container().begin(), kernel.container().end(), noise);

    timer.start("Brute force convolution");
    auto result1 = convolve(fft_data.real_space(), kernel);
    timer.stop();

    RFFT<float, D> fft_kernel(data_shape);
    fft_kernel.real_space() = 0.0f;
    PeriodicNdArrayView<float, D, numeric::fourier::PointerRange<float>> kernel_view(
        fft_kernel.real_space(), {-7,-7}, {16,16});
    std::copy(kernel.begin(), kernel.end(), kernel_view.begin());

    timer.start("Fourier convolution");
    fft_data.forward();
    fft_kernel.forward();
    std::transform(
        fft_data.freq_space().container().begin(), fft_data.freq_space().container().end(),
        fft_kernel.freq_space().container().begin(),
        fft_data.freq_space().container().begin(),
        [] (auto a, auto b) { return a * b; });
    fft_data.inverse();
    std::transform(
        fft_data.real_space().begin(), fft_data.real_space().end(),
        fft_data.real_space().begin(),
        [N] (auto a) {
            return a / N;
        });
    timer.stop();

    assert_array_equal(fft_data.real_space(), result1);
}
