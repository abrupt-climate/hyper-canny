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

/*! \file numeric/fftw_traits.hh
 *  \brief Traits class for single/double precision FFTW interface.
 */

#include <fftw3.h>

namespace HyperCanny {
namespace numeric {
namespace fourier
{
    template <typename T>
    struct RFFT_traits;

    template <>
    struct RFFT_traits<double>
    {
        using complex_t = ::fftw_complex;
        using plan_t = ::fftw_plan;

        using forward_planner_t = fftw_plan(*)(
                int, const int *, double *, fftw_complex *, unsigned);
        using inverse_planner_t = fftw_plan(*)(
                int, const int *, fftw_complex *, double *, unsigned);

        static forward_planner_t forward_plan;
        static inverse_planner_t inverse_plan;
        static void (*destroy_plan)(plan_t);
        static void (*execute)(plan_t);

        static double *(*alloc_real)(size_t);
        static complex_t *(*alloc_complex)(size_t);
        static void (*free)(void *);
    };

    template <>
    struct RFFT_traits<float>
    {
        using complex_t = fftwf_complex;
        using plan_t = fftwf_plan;

        using forward_planner_t = fftwf_plan(*)(
                int, const int *, float *, fftwf_complex *, unsigned);
        using inverse_planner_t = fftwf_plan(*)(
                int, const int *, fftwf_complex *, float *, unsigned);

        static forward_planner_t forward_plan;
        static inverse_planner_t inverse_plan;
        static void (*destroy_plan)(plan_t);
        static void (*execute)(plan_t);

        static float *(*alloc_real)(size_t);
        static complex_t *(*alloc_complex)(size_t);
        static void (*free)(void *);
    };
}}} // namespace HyperCanny::numeric::fourier
