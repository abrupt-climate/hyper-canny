#include "../../base.hh"
#include "../../save_png.hh"

#include <cstdlib>
#include <H5Cpp.h>

using namespace HyperCanny;

void convert_hdf5_to_png()
{
    auto subcon = console.sublog("Converting HDF5 floating point data to a PNG image.");
    console.msg("Opening ./data/test/cute.hdf5 ...");
    H5::H5File f("./data/test/cute.hdf5", H5F_ACC_RDONLY);
    H5::DataSet dataset = f.openDataSet("image");
    H5T_class_t type_class = dataset.getTypeClass();

    if (type_class != H5T_FLOAT)
        throw Exception("Expected a field with floating-point values.");

    subcon.finish("Done");
}

int main(int argc, char **argv)
{
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

