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
#include "get_gpu_context.hh"
#include "../base.hh"
#include <iostream>

using namespace HyperCanny;

std::tuple<std::vector<cl::Device>, cl::Context>
    HyperCanny::get_default_gpu_context()
{
    Console::Log console(
        "\033[32m☷ \033[m OpenCL initialisation ...",
        "\033[32m│\033[m  ");

    std::vector<cl::Platform> platform_list;
    cl::Platform::get(&platform_list);
    checkErr(platform_list.size() != 0 ? CL_SUCCESS : -1,
        " cl::Platform::get failed to retrieve a platform;\n"
        "  is OpenCL correctly installed?"
        "\n(fail)\n");

    cl::Platform default_platform = platform_list[0];
    console.message(
        "Using platform: ",
        default_platform.getInfo<CL_PLATFORM_NAME>());

    std::vector<cl::Device> device_list;
    default_platform.getDevices(CL_DEVICE_TYPE_GPU, &device_list);
    checkErr(device_list.size() != 0 ? CL_SUCCESS : -1,
        " cl::Platform::getDevices failed to find a GPU device;\n"
        "  do you have the correct device loader installed?"
        "\n(fail)\n");

    cl::Device default_device = device_list[0];
    console.message(
        "Using device:   ",
        default_device.getInfo<CL_DEVICE_NAME>());

    console.finish("\033[32m┕\033[m (ok)");

    cl::Context context({default_device});
    return std::make_tuple(device_list, context);
}
