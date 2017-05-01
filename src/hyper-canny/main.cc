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
#include "numeric/numeric.hh"
#include "numeric/netcdf.hh"
#include "numeric/ndarray.hh"
#include "numeric/filters.hh"

#include <cstdlib>

using namespace HyperCanny;
namespace filter = numeric::filter;

int main(int argc, char **argv)
{
    Console::Log console("Hyper Canny!");
    console.message("reading ./data/test/mi.nc ...");
    netCDF::NcFile input_file("data/test/mi.nc", netCDF::NcFile::read);
    auto data = numeric::netcdf_read_array<float, 2>(input_file, "mi");

    console.message("applying sobel");
    auto sobel_filtered = filter::sobel(filter::gaussian(*data, 5, 1.4));
    console.message("thinning edges");
    auto thinned_mask = filter::edge_thinning(sobel_filtered);

    auto o = thinned_mask.begin();
    for (unsigned j = 0; j < thinned_mask.shape()[1]; ++j)
    {
        for (unsigned i = 0; i < thinned_mask.shape()[0]; ++i, ++o)
        {
            std::cout << *o << " ";
        }
        std::cout << "\n";
    }

    console.finish("done idling");
    return EXIT_SUCCESS;
}
