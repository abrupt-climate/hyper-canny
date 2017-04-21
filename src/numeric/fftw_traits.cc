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
#include "fftw_traits.hh"

namespace HyperCanny {
namespace numeric {
namespace fourier
{
    //=======================================================================//
    //                          Double precision                             //
    //=======================================================================//
    typename RFFT_traits<double>::forward_planner_t
        RFFT_traits<double>::forward_plan
        = fftw_plan_dft_r2c;

    typename RFFT_traits<double>::inverse_planner_t
        RFFT_traits<double>::inverse_plan
        = fftw_plan_dft_c2r;

    double *(*RFFT_traits<double>::alloc_real)(size_t)
        = fftw_alloc_real;

    typename RFFT_traits<double>::complex_t *(*RFFT_traits<double>::alloc_complex)(size_t)
        = fftw_alloc_complex;

    void (*RFFT_traits<double>::free)(void *)
        = fftw_free;

    void (*RFFT_traits<double>::execute)(typename RFFT_traits<double>::plan_t)
        = fftw_execute;

    void (*RFFT_traits<double>::destroy_plan)(typename RFFT_traits<double>::plan_t)
        = fftw_destroy_plan;

    //=======================================================================//
    //                          Single precision                             //
    //=======================================================================//
    typename RFFT_traits<float>::forward_planner_t
        RFFT_traits<float>::forward_plan
        = fftwf_plan_dft_r2c;

    typename RFFT_traits<float>::inverse_planner_t
        RFFT_traits<float>::inverse_plan
        = fftwf_plan_dft_c2r;

    float *(*RFFT_traits<float>::alloc_real)(size_t)
        = fftwf_alloc_real;

    typename RFFT_traits<float>::complex_t *(*RFFT_traits<float>::alloc_complex)(size_t)
        = fftwf_alloc_complex;

    void (*RFFT_traits<float>::free)(void *)
        = fftwf_free;

    void (*RFFT_traits<float>::execute)(typename RFFT_traits<float>::plan_t)
        = fftwf_execute;
    
    void (*RFFT_traits<float>::destroy_plan)(typename RFFT_traits<float>::plan_t)
        = fftwf_destroy_plan;
}}} // namespace HyperCanny::numeric::fourier
