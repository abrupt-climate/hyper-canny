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
#include "info.hh"

using namespace HyperCanny;

/*! \brief Prints some information on the available OpenCL runtime.
 */
void HyperCanny::print_opencl_info(
    std::vector<cl::Platform> const &platform_list)
{
    Console::Log console;

    console.msg("Number of platforms: ", platform_list.size()).endl();

    for (unsigned i = 0; i < platform_list.size(); ++i)
    {
        Console::Log console(
            format(
                "== \033[1mPlatform Nº ", i+1,
			    "\033[m ========================================================="),
            "  ");

        auto const &platform = platform_list[i];
        console
            .msg("Name:      ", platform.getInfo<CL_PLATFORM_NAME>())
            .msg("Vendor:    ", platform.getInfo<CL_PLATFORM_VENDOR>());

        std::vector<cl::Device> device_list;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &device_list);

        console
			.msg("# devices: ", device_list.size())
			.msg();

        for (unsigned j = 0; j < device_list.size(); ++j)
        {
            Console::Log console(
				format(
                    "-- \033[1mDevice Nº ", j+1,
				    "\033[m ---------------------------------------------------------"),
				"  ");

            auto const &device = device_list[j];
            console
                .msg("Name:             ", device.getInfo<CL_DEVICE_NAME>())
                .msg("Device version:   ", device.getInfo<CL_DEVICE_VERSION>())
				.msg("Driver version:   ", device.getInfo<CL_DRIVER_VERSION>())
				.msg("OpenCL version:   ", device.getInfo<CL_DEVICE_OPENCL_C_VERSION>())
				.msg("# Compute units:  ", device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>());
        }
    }
}
