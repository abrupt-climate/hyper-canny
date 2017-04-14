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
#include "compile.hh"
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <iostream>

using namespace HyperCanny;

std::string HyperCanny::load_source(std::string const &filename)
{
    std::ifstream file(filename);
    checkErr(file.is_open() ? CL_SUCCESS : -1,
        "could not open file: " + filename);

    std::string program(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));

    return program;
}

cl::Program HyperCanny::get_program_from_file(
    cl::Context &cx,
    std::vector<cl::Device> const &devices,
    std::string const &filename)
{
    Console::Log console;

    console.msg("Compiling ", filename, " ...");
    cl::Program::Sources sources;
    sources.push_back(load_source(filename));
    cl::Program program(cx, sources);

    auto err_code = program.build(devices, "");
    checkErr(err_code,
        program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]));

    return program;
}

cl::Program HyperCanny::get_program_from_sources(
    cl::Context &cx,
    std::vector<cl::Device> const &devices,
    std::vector<std::string> const &filenames,
    std::string const &compiler_args)
{
    Console::Log console;

    console.msg("Compiling kernels ... ");
    cl::Program::Sources sources;
    std::transform(filenames.begin(), filenames.end(),
        std::back_inserter(sources), load_source);

    cl::Program program(cx, sources);

    auto err_code = program.build(devices, compiler_args.c_str());
    checkErr(err_code,
        program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]));

    return program;
}
