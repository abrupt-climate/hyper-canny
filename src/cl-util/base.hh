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

/*! \file cl-util/base.hh
 *  \brief Basic include for rest of module.
 */

#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION  120
#include "../base.hh"
#include <CL/cl2.hpp>

namespace HyperCanny
{
    template <typename ...Args>
    inline void checkErr(cl_int err, Args &&...args)
    {
        if (err != CL_SUCCESS)
        {
            Console::Log console;
            std::string msg = format(std::forward<Args>(args)...);
            console.error(err, msg);
            throw Exception(msg);
        }
    }
}
