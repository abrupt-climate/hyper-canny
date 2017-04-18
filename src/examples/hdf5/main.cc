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
#include "../../base.hh"
#include "../../save_png.hh"

#include <cstdlib>
#include <H5Cpp.h>

using namespace HyperCanny;

void convert_hdf5_to_png()
{
    Console::Log console("Converting HDF5 floating point data to a PNG image.");

    console.msg("Opening ./data/test/cute.hdf5 ...");
    H5::H5File f("./data/test/cute.hdf5", H5F_ACC_RDONLY);
    H5::DataSet dataset = f.openDataSet("image");
    H5T_class_t type_class = dataset.getTypeClass();

    if (type_class != H5T_FLOAT)
        throw Exception("Expected a field with floating-point values.");

    H5::FloatType type = dataset.getFloatType();

    std::string endianness;
    switch (type.getOrder())
    {
        case H5T_ORDER_LE: endianness = "little endian"; break;
        case H5T_ORDER_BE: endianness = "big endian"; break;
        case H5T_ORDER_VAX: endianness = "vax"; break;
        default: endianness = "unknown endianness"; break;
    }

    console.msg(type.getPrecision(), " bit, ", endianness, ", floating point data.");

    H5::DataSpace dataspace = dataset.getSpace();
    if (not dataspace.isSimple())
        throw Exception("data space is not simple.");
    int dim = dataspace.getSimpleExtentNdims();
    std::vector<hsize_t> shape(dim);
    dataspace.getSimpleExtentDims(shape.data(), nullptr);
    console.msg("simple data with shape (", string_join(shape, ", "), ")");
    console.finish("Done");
}

int main(int argc, char **argv)
{
    Console::Log console;

    try
    {
        convert_hdf5_to_png();
    }
    catch (Exception const &e)
    {
        console.error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
