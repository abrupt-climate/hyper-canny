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

/*! \file module/module.hh
 *  \brief C shared library
 */

#include "numeric/ndarray.hh"
#include "numeric/convolution.hh"
#include "numeric/canny.hh"

extern "C" void smooth_gaussian(
    unsigned dim,
    unsigned *i_shape, size_t i_offset, int *i_stride, size_t i_size, float *input,
    unsigned *o_shape, size_t o_offset, int *o_stride, size_t o_size, float *output,
    unsigned filter_width, float sigma);

extern "C" void smooth_sobel(
    unsigned dim, unsigned *shape, float *input, unsigned filter_width, float sigma, float *output);

extern "C" void thin_edges(
    unsigned dim, unsigned *shape, float *input, uint8_t *output);

extern "C" void double_threshold(
    unsigned dim, unsigned *shape, float *input, uint8_t *mask, float a, float b, uint8_t *output);
