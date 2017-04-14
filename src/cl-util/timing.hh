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
#include <tuple>
#include <string>

namespace HyperCanny
{
    extern std::tuple<unsigned long, unsigned long> get_runtime(cl::Event const &event);
    extern void print_runtime_msg(std::tuple<unsigned long, unsigned long> const &t, std::string const &msg);
}
