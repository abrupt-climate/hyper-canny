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

#include <netcdf>
#include <cstddef>

namespace netCDF
{
    template <typename T>
    struct type_traits;

    template <>
    struct type_traits<float>
    {
        static NcType nc_type;
    };

    template <>
    struct type_traits<int>
    {
        static NcType nc_type;
    };

    NcType type_traits<float>::nc_type = ncFloat;
    NcType type_traits<int>::nc_type = ncInt;
}

namespace HyperCanny
{
    using numeric::NdArray;

    template <typename F>
    void variadic_for_each_helper(F func, unsigned idx) {}

    template <typename F, typename Arg, typename ...Args>
    void variadic_for_each_helper(F func, unsigned idx, Arg &&arg1, Args &&...args)
    {
        func(idx, std::forward<Arg>(arg1));
        variadic_for_each_helper(func, idx + 1, std::forward<Args>(args)...);
    }

    template <typename F, typename ...Args>
    void variadic_enumerated_for_each(F func, Args &&...args)
    {
        variadic_for_each_helper(func, 0, std::forward<Args>(args)...);
    }

    template <typename T, unsigned D, typename ...DimArgs>
    void netcdf_add_array(
            netCDF::NcFile &file,
            NdArray<T,D> const &data,
            std::string const &var_name,
            DimArgs &&...dim_args)
    {
        using namespace netCDF;

        std::vector<NcDim> dims;
        variadic_enumerated_for_each(
            [&file, &dims, &data] (unsigned idx, std::string const &dimname)
        {
            dims.push_back(file.addDim(dimname, data.shape(idx)));
        }, std::forward<DimArgs>(dim_args)...);

        NcVar nc_data = file.addVar(var_name, type_traits<T>::nc_type, dims);
        nc_data.putVar(data.data());
    }
}

using namespace HyperCanny;

int main(int argc, char **argv)
{
    using numeric::NdArray;

    auto data = numeric::hilbert_array(10);
    NdArray<int,2> data_converted(data);

    netCDF::NcFile data_file("data/test/netcdf-test.nc", netCDF::NcFile::replace);
    netcdf_add_array(data_file, data_converted, "hilbert_walk", "x", "y");

    return EXIT_SUCCESS;
}

