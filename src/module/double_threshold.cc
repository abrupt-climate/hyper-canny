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
void do_double_threshold(unsigned *shape_p, real_t *input_p, uint8_t *mask_p, float a, float b, uint8_t *output_p)
{
    using namespace numeric;
    using namespace filter;

    shape_t<D> shape;
    std::copy(shape_p, shape_p + D, shape.rbegin());

    using input_type = NdArray<real_t, D+1, pointer_range<real_t>>;

    Slice<D+1> input_slice(extend_one(shape, D+1));
    Slice<D>   output_slice(shape);

    input_type input(
        input_slice, pointer_range<real_t>(input_p, input_slice.size));

    NdArray<bool, D> mask(shape);
    std::copy(mask_p, mask_p + output_slice.size, mask.begin());
    auto result = double_threshold(input, mask, a, b);
    std::copy(result.begin(), result.end(), output_p);
}

extern "C" void double_threshold(
    unsigned dim, unsigned *shape, float *input, uint8_t *mask, float a, float b, uint8_t *output)
{
    switch (dim)
    {
        case 2: do_double_threshold<float, 2>(shape, input, mask, a, b, output); break;
        case 3: do_double_threshold<float, 3>(shape, input, mask, a, b, output); break;
        case 4: do_double_threshold<float, 4>(shape, input, mask, a, b, output); break;
        case 5: do_double_threshold<float, 5>(shape, input, mask, a, b, output); break;
    }
}
