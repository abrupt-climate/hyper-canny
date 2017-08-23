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
#include "misc/argparse.hh"

#include "numeric/numeric.hh"
#include "numeric/netcdf.hh"
#include "numeric/ndarray.hh"
#include "numeric/canny.hh"

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
    using argparse::Args;
    using argparse::Option;
    Console::Log console;
    console.message(
        "Hyper Canny, Copyright 2017 "
        "\033[47;30m Netherlands\033[48;2;0;174;239;37m▌"
        "\033[38;2;255;255;255me\u20d2Science\u20d2\033[37m▐"
        "\033[47;30mcenter \033[m");
    console.message(
        "                    and"
        " \033[47;30m"
        " \033[38;2;42;158;41;4m⊓ \033[24m"
        "\033[38;2;0;74;109m Wageningen" // 0, 74, 109
        "\033[38;2;42;158;41m University & Research \033[m"); // 52, 178, 51
    console.message(
        "Licensed under the Apache License, Version 2.0");
    console.endl();

    Args args(
        "hyper-canny",
        "Edge detection, any way you like.",
        {
            Option("-h", "print help"),
            Option("-f", "input file", "", false),
            Option("-lower", "lower threshold", "100.0"),
            Option("-upper", "upper threshold", "200.0"),
            Option("-sigma", "pre smoothing width", "2.4")
        });

    try
    {
        args.parse(argc, argv);
    }
    catch (argparse::Exception const &e)
    {
        console.msg(e.what());
        args.usage();
        exit(EXIT_FAILURE);
    }

    if (*args.get<bool>("-h"))
    {
        args.usage();
        exit(EXIT_SUCCESS);
    }

    std::string filename = *args.get<std::string>("-f");
    console.msg("reading ", filename, " ...");
    netCDF::NcFile input_file(filename, netCDF::NcFile::read);
    auto vars = input_file.getVars();

    console.msg("found variables: ", string_join(
            select([] (auto const &p) { return p.first; }, vars), ", "));
    console.msg("reading ", vars.begin()->first);
    auto data = numeric::netcdf_read_array<float, 2>(vars.begin()->second);

    Timer timer;
    timer.start("Sobel operator");
    float sigma = args.get<float>("-sigma", 2.4);
    int filter_width = static_cast<int>(ceil(2 * sigma));
    auto sobel_filtered = filter::smooth_sobel(
        *data, filter_width, sigma);
    timer.stop();

    timer.start("Thinning edges");
    auto thinned_mask = filter::edge_thinning(sobel_filtered);
    timer.stop();

    float lower = args.get<float>("-lower", 100.0);
    float upper = args.get<float>("-upper", 200.0);
    timer.start("Hysteresis threshold");
    auto thresholded_mask = filter::double_threshold(
        sobel_filtered, thinned_mask, lower, upper);
    timer.stop();
    print_array(thresholded_mask);

    return EXIT_SUCCESS;
}
