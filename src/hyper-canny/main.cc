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

template <typename Array>
void print_array(Array const &a)
{
    auto o = a.begin();
    for (unsigned j = 0; j < a.shape()[1]; ++j)
    {
        for (unsigned i = 0; i < a.shape()[0]; ++i, ++o)
        {
            std::cout << *o << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char **argv)
{
    Console::Log console("Hyper Canny!");
    console.message("reading ./data/test/vermeer.nc ...");
    netCDF::NcFile input_file("data/test/vermeer.nc", netCDF::NcFile::read);
    auto data = numeric::netcdf_read_array<float, 2>(input_file, "vermeer");

    Timer timer;
    timer.start("Sobel operator");
    auto sobel_filtered = filter::smooth_sobel(*data, 5, 2.4);
    timer.stop();
    //print_array(sobel_filtered.sel(0, 2));
    //std::cout << "\n\n";
    timer.start("Thinning edges");
    auto thinned_mask = filter::edge_thinning(sobel_filtered);
    timer.stop();
    //print_array(thinned_mask);
    //std::cout << "\n\n";
    timer.start("Hysteresis threshold");
    auto thresholded_mask = filter::double_threshold(sobel_filtered, thinned_mask, 100.0, 200.0);
    timer.stop();
    //print_array(thresholded_mask);

    console.finish("done idling");
    return EXIT_SUCCESS;
}
