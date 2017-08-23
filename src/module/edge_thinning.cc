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
void do_edge_thinning(unsigned *shape_p, real_t *input_p, uint8_t *output_p)
{
    using namespace numeric;
    using namespace filter;

    shape_t<D> shape;
    std::copy(shape_p, shape_p + D, shape.rbegin());

    using output_type = NdArray<uint8_t, D, pointer_range<uint8_t>>;
    using input_type = NdArray<real_t, D+1, pointer_range<real_t>>;

    Slice<D+1> input_slice(extend_one(shape, D+1));
    Slice<D>   output_slice(shape);
    output_type output(
        output_slice, pointer_range<uint8_t>(output_p, output_slice.size));
    input_type input(
        input_slice, pointer_range<real_t>(input_p, input_slice.size));

    auto mask = filter::edge_thinning(input);
    std::copy(mask.begin(), mask.end(), output.begin());
}

extern "C" void thin_edges(
    unsigned dim, unsigned *shape, float *input, uint8_t *output)
{
    switch (dim)
    {
        case 2: do_edge_thinning<float, 2>(shape, input, output); break;
        case 3: do_edge_thinning<float, 3>(shape, input, output); break;
        case 4: do_edge_thinning<float, 4>(shape, input, output); break;
        case 5: do_edge_thinning<float, 5>(shape, input, output); break;
    }
}
