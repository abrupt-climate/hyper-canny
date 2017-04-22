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
#include "numeric/ndarray.hh"
#include "numeric/hilbert.hh"
#include "numeric/netcdf.hh"

#include <cmath>

using namespace HyperCanny;

int main(int argc, char **argv)
{
    using namespace numeric;

    Console::Log console("testing write and read to NetCDF file");
    auto long_data = numeric::hilbert_array(10);
    auto data = NdArray<int,2>(long_data.shape());
    std::copy(long_data.begin(), long_data.end(), data.begin());

    {   // write a test file
        console.msg("writing ...");
        netCDF::NcFile data_file("data/test/netcdf-test.nc", netCDF::NcFile::replace);
        netcdf_add_array(data_file, data, "hilbert_walk", "x", "y");
    }

    {   // read the same data with a different type
        console.msg("Reading, should give a warning ...");
        netCDF::NcFile data_file("data/test/netcdf-test.nc", netCDF::NcFile::read);
        auto float_data = netcdf_read_array<float,2>(data_file, "hilbert_walk");

        // check they're the same
        bool eq = std::equal(data.begin(), data.end(), float_data->begin(),
            [] (size_t i, float f)
        {
            return i == static_cast<size_t>(round(f));
        });

        console.msg("Comparing ... ", (eq ? "success" : "fail"));
    }

    console.finish("done");
    return EXIT_SUCCESS;
}

