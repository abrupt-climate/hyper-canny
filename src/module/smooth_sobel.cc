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
#include "module.hh"
#include "base/pointer_range.hh"
#include "numeric/canny.hh"

using namespace HyperCanny;

template <typename real_t, unsigned D>
void do_smooth_sobel(unsigned *shape_p, real_t *input_p, unsigned n, float sigma, real_t *output_p)
{
    using namespace numeric;
    using namespace filter;

    shape_t<D> shape;
    std::copy(shape_p, shape_p + D, shape.rbegin());

    using output_type = NdArray<real_t, D+1, pointer_range<real_t>>;
    using input_type = NdArray<real_t, D, pointer_range<real_t>>;

    auto G = gaussian_kernel<real_t>(n, sigma);
    auto smooth_kernel = convolve_padding_zero(
            NdArray<real_t, 1>({3}, {0.25, 0.50, 0.25}), G);
    auto gradient_kernel = convolve_padding_zero(
            NdArray<real_t, 1>({3}, {0.5, 0.0, -0.5}), G);

    Slice<D>   input_slice(shape);
    Slice<D+1> output_slice(extend_one(shape, D+1));
    output_type output(
        output_slice, pointer_range<real_t>(output_p, output_slice.size));
    input_type input(
        input_slice, pointer_range<real_t>(input_p, input_slice.size));

    for (unsigned k = 0; k < D; ++k)
    {
        auto outs = output.sel(0, k);
        outs = sobel(input, k, smooth_kernel, gradient_kernel);
    }

    normalize_homogeneous_vectors(output);
}

extern "C" void smooth_sobel(
    unsigned dim, unsigned *shape, float *input, unsigned filter_width, float sigma, float *output)
{
    switch (dim)
    {
        case 2: do_smooth_sobel<float, 2>(shape, input, filter_width, sigma, output); break;
        case 3: do_smooth_sobel<float, 3>(shape, input, filter_width, sigma, output); break;
        case 4: do_smooth_sobel<float, 4>(shape, input, filter_width, sigma, output); break;
        case 5: do_smooth_sobel<float, 5>(shape, input, filter_width, sigma, output); break;
        default: throw Exception("Invalid dimenension, must be number between 2 and 5.");
    }
}
