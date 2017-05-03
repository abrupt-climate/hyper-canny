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
#pragma once

/*! \file numeric/netcdf.hh
 *  \brief Provides functions to save/load arrays from NetCDF files.
 */

#include "base.hh"
#include "ndarray.hh"
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

    template <>
    struct type_traits<size_t>
    {
        static NcType nc_type;
    };

    NcType type_traits<float>::nc_type = ncFloat;
    NcType type_traits<int>::nc_type = ncInt;
    NcType type_traits<size_t>::nc_type = ncUint64;
}

namespace HyperCanny {
namespace numeric
{
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

    template <typename T_from, typename T_to>
    void netcdf_convert_helper(
            netCDF::NcVar &nc_var,
            T_to *data,
            size_t size)
    {
        T_from *temp = new T_from[size];
        nc_var.getVar(temp);
        std::copy(temp, temp + size, data);
    }

    template <typename T>
    void netcdf_convert(
            netCDF::NcVar &nc_var,
            T *data,
            size_t size)
    {
        using namespace netCDF;
        switch (nc_var.getType().getTypeClass())
        {
            case NcType::nc_FLOAT:
                netcdf_convert_helper<float, T>(nc_var, data, size);
                break;
            case NcType::nc_INT:
                netcdf_convert_helper<int, T>(nc_var, data, size);
                break;
            case NcType::nc_UINT64:
                netcdf_convert_helper<uint64_t, T>(nc_var, data, size);
                break;
            default:
                throw Exception(format(
                        "Could not convert NetCDF data: ",
                        nc_var.getType().getTypeClassName()));
        }
    }

    template <typename T, unsigned D, typename ...DimArgs>
    void netcdf_add_array(
            netCDF::NcFile &file,
            NdArray<T,D> const &data,
            std::string const &var_name,
            DimArgs &&...dim_args)
    {
        using namespace netCDF;
        static_assert(
            sizeof...(DimArgs) == D,
            "Number of dimension arguments should match dimension of array.");

        std::vector<NcDim> dims;
        variadic_enumerated_for_each(
            [&file, &dims, &data] (unsigned idx, std::string const &dimname)
        {
            dims.push_back(file.addDim(dimname, data.shape()[D-idx-1]));
        }, std::forward<DimArgs>(dim_args)...);

        NcVar nc_data = file.addVar(var_name, type_traits<T>::nc_type, dims);
        nc_data.putVar(data.const_container().data());
    }

    template <typename T, unsigned D>
    std::unique_ptr<NdArray<T,D>> netcdf_read_array(
            netCDF::NcFile &file,
            std::string const &var_name)
    {
        using namespace netCDF;
        Console::Log console;

        NcVar nc_var = file.getVar(var_name);
        if (nc_var.isNull())
            throw Exception(format(
                "NetCDF file does not contain requested variable: ", var_name));

        if (nc_var.getDimCount() != D)
            throw Exception(format(
                "NetCDF variable ", var_name,
                " does not have the expected number of ", D, " dimensions: ",
                "got ", nc_var.getDimCount(), "."));

        shape_t<D> shape;
        for (unsigned i = 0; i < D; ++i)
            shape[D-i-1] = nc_var.getDim(i).getSize();

        auto data = std::make_unique<NdArray<T,D>>(shape);

        if (nc_var.getType() == type_traits<T>::nc_type)
        {
            nc_var.getVar(data->container().data());
        } else {
            console.warning(
                "converting data from ", nc_var.getType().getTypeClassName(),
                " to ", type_traits<T>::nc_type.getTypeClassName());
            netcdf_convert<T>(nc_var, data->container().data(), data->size());
        }

        return std::move(data);
    }
}}
