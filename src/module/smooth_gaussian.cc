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
#include <cassert>

using namespace HyperCanny;

template <typename real_t, unsigned D>
void do_smooth_gaussian(
        unsigned *i_shape_p, size_t i_offset, int *i_stride_p, size_t i_size, real_t *input_p,
        unsigned *o_shape_p, size_t o_offset, int *o_stride_p, size_t o_size, real_t *output_p,
        unsigned n, float sigma)
{
    using namespace numeric;
    using namespace filter;

    shape_t<D> i_shape, o_shape;
    stride_t<D> i_stride, o_stride;

    std::copy(i_shape_p, i_shape_p + D, i_shape.rbegin());
    std::copy(o_shape_p, o_shape_p + D, o_shape.rbegin());
    std::copy(i_stride_p, i_stride_p + D, i_stride.rbegin());
    std::copy(o_stride_p, o_stride_p + D, o_stride.rbegin());

    std::cerr << "smoothing: " << i_shape << ", " << i_offset << ", " << i_stride
              << " out " << o_shape << ", " << o_offset << ", " << o_stride << std::endl;

    assert(i_shape == o_shape);

    using output_type = NdArray<real_t, D, pointer_range<real_t>>;
    using input_type = NdArray<real_t, D, pointer_range<real_t>>;

    Slice<D> input_slice(i_offset, i_shape, i_stride),
             output_slice(o_offset, o_shape, o_stride);

    output_type output(
        output_slice, pointer_range<real_t>(output_p, o_size));
    input_type input(
        input_slice, pointer_range<real_t>(input_p, i_size));

    auto result = gaussian(input, n, sigma);
    std::copy(result.begin(), result.end(), output.begin());
}

extern "C" void smooth_gaussian(
    unsigned dim,
    unsigned *i_shape, size_t i_offset, int *i_stride, size_t i_size, float *input,
    unsigned *o_shape, size_t o_offset, int *o_stride, size_t o_size, float *output,
    unsigned filter_width, float sigma)
{
    switch (dim)
    {
        case 2: do_smooth_gaussian<float, 2>(
                    i_shape, i_offset, i_stride, i_size, input,
                    o_shape, o_offset, o_stride, o_size, output,
                    filter_width, sigma); break;
        case 3: do_smooth_gaussian<float, 3>(
                    i_shape, i_offset, i_stride, i_size, input,
                    o_shape, o_offset, o_stride, o_size, output,
                    filter_width, sigma); break;
        case 4: do_smooth_gaussian<float, 4>(
                    i_shape, i_offset, i_stride, i_size, input,
                    o_shape, o_offset, o_stride, o_size, output,
                    filter_width, sigma); break;
        case 5: do_smooth_gaussian<float, 5>(
                    i_shape, i_offset, i_stride, i_size, input,
                    o_shape, o_offset, o_stride, o_size, output,
                    filter_width, sigma); break;
        default: throw Exception("Invalid dimenension, must be number between 2 and 5.");
    }
}
