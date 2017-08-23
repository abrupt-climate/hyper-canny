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

/*! \file numeric/pointer_range.hh
 *  \brief Non-owner view on raw pointer data.
 */

#include "base/pointer_range.hh"

namespace HyperCanny {
namespace numeric
{
    template <typename T>
    using PointerRange = pointer_range<T>;

    template <typename T>
    using ConstPointerRange = pointer_range<T const>;
}}
