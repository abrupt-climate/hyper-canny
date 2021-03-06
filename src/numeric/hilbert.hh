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

/*! \file numeric/hilbert.hh
 *  \brief Creates index arrays for iterating an array by Hilbert curve.
 */

#include "ndarray.hh"
#include <vector>

namespace HyperCanny { namespace numeric
{
    extern NdArray<size_t, 2> hilbert_array(unsigned n);
    extern std::vector<size_t> hilbert_walk(unsigned n);
}}
