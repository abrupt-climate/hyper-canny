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
#include <utility>

namespace HyperCanny
{
    template <unsigned N>
    void set_args_n(cl::Kernel &k) {}

    template <unsigned N, typename First, typename ...Rest>
    void set_args_n(cl::Kernel &k, First &&first, Rest &&...rest)
    {
        k.setArg(N, first);
        set_args_n<N+1>(k, std::forward<Rest>(rest)...);
    }

    template <typename ...Args>
    void set_args(cl::Kernel &k, Args &&...args)
    {
        set_args_n<0>(k, std::forward<Args>(args)...);
    }
}
