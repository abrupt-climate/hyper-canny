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
#include "base.hh"
#include <string>

namespace HyperCanny
{
    extern std::string load_source(std::string const &filename);

    extern cl::Program get_program_from_file(
        cl::Context &cx,
        std::vector<cl::Device> const &devices,
        std::string const &filename);

    extern cl::Program get_program_from_sources(
        cl::Context &cx,
        std::vector<cl::Device> const &devices,
        std::vector<std::string> const &filenames,
        std::string const &compiler_args = "");
}
