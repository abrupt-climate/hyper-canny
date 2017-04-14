#include "numeric/netcdf.hh"
#include "numeric/hilbert.hh"
#include "numeric/ndarray.hh"
#include <experimental/filesystem>
#include <algorithm>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace HyperCanny;
using namespace numeric;

class NetCDFTest: public ::testing::Test
{
    protected:
        NetCDFTest() {}
        virtual ~NetCDFTest() {}

        virtual void SetUp()
        {
            auto long_data = numeric::hilbert_array(10);
            data.resize(long_data.shape());
            data = long_data;
            std::experimental::filesystem::create_directories("./data/test");
        }

        virtual void TearDown()
        {
            std::experimental::filesystem::remove("./data/test/netcdf-test.nc");
        }

        NdArray<int, 2> data;
};

TEST_F (NetCDFTest, ReadingAndWriting)
{
    using testing::HasSubstr;
    using testing::Not;

    {
        netCDF::NcFile output_file("data/test/netcdf-test.nc", netCDF::NcFile::replace);
        netcdf_add_array(output_file, data, "hilbert_walk", "x", "y");
    }
    // | netcdf-c++4 version 4.2 seems to have no NcFile::close() yet.
    // | instead we trigger descruction by closing the scope.
    // output_file.close();

    {
        CaptureOutput capture(std::clog);
        netCDF::NcFile input_file("data/test/netcdf-test.nc", netCDF::NcFile::read);
        auto float_data = netcdf_read_array<float,2>(input_file, "hilbert_walk");
        capture.restore();

        EXPECT_THAT(capture.str(), HasSubstr("[warning]"));

        // check they're the same
        bool eq = std::equal(data.begin(), data.end(), float_data->begin(),
            [] (size_t i, float f)
        {
            return i == static_cast<size_t>(round(f));
        });

        ASSERT_TRUE(eq) << "array read from file does not match original.";
    }
}

