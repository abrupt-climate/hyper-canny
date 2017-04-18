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

/*! \file base/filesystem.hh
 *  \brief Compatibility with C++17 std::filesystem.
 *
 *  If you use functions in `std::filesystem` add them here from
 *  `std::experimental::filesystem`. This adds compatibility between
 *  the 2014 and 2017 C++ standards.
 */

#include <experimental/filesystem>

namespace std
{
    namespace filesystem
    {
        using std::experimental::filesystem::remove;
        using std::experimental::filesystem::create_directories;
    }
}
