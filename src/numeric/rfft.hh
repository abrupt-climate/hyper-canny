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

/*! \file numeric/rfft.hh
 *  \brief Easy interface for FFTW real valued Fourier transforms.
 */

namespace HyperCanny {
namespace numeric {
namespace fourier
{
    template <typename real_t, unsigned D>
    class RFFT
    {
        using fftw = RFFT_traits<real_t>;
        using complex_t = typename fftw::complex_t;

        bool const m_in_place;
        Slice<D> m_real_slice;
        Slice<D> m_freq_slice;

        real_t *m_real_buffer;
        complex_t *m_freq_buffer;

        fftw_plan m_forward_plan;
        fftw_plan m_inverse_plan;

        public:
            RFFT() = delete;
            RFFT(RFFT const &) = delete;
            RFFT &operator=(RFFT const &) = delete;

            explicit RFFT(shape_t<D> const &shape, unsigned flags, bool in_place=true);
            ~RFFT() noexcept;

            NdArray<real_t,D>::View real_space();
            NdArray<std::complex<real_t>,D>::View freq_space();

            void forward();
            void inverse();
    };

    //=======================================================================//
    //                             IMPLEMENTATION                            //
    //=======================================================================//

    template <typename real_t, unsigned D>
    RFFT<real_t,D>::RFFT(shape_t<D> const &shape, unsigned flags, bool in_place=true)
        : m_in_place(in_place)
    {
        // see FFTW manual: Real-data DFT Array Format
        // FFTW stores in row-major order, we use column-major order.

        shape_t<D> real_shape = shape;
        shape_t<D> freq_shape = shape;
        freq_shape[0] = (shape[0]/2 + 1);
        m_freq_slice = Slice(freq_shape);

        if (m_in_place)
        {
            shape_t<D> real_mem_shape = shape;
            real_mem_shape[0] = 2 * (shape[0]/2 + 1);
            stride_t<D> real_stride = calc_stride(real_mem_shape);
            m_real_slice = Slice(0, real_shape, real_stride);

            m_real_buffer = fftw::alloc_real(m_real_slice.size);
            m_freq_buffer = reinterpret_cast<complex_t *>(m_real_buffer);
        }
        else
        {
            m_real_slice = Slice(shape); 
            m_real_buffer = fftw::alloc_real(m_real_slice.size);
            m_freq_buffer = fftw::alloc_complex(m_freq_slice.size);
        }

        int n[D];
        std::copy(m_shape.begin(), m_shape.end(), n);

        m_forward_plan = fftw::forward_plan(
            D, n, m_real_buffer, m_freq_buffer, flags);
        m_inverse_plan = fftw::inverse_plan(
            D, n, m_freq_buffer, m_real_buffer, flags);
    }

    template <typename real_t, unsigned D>
    RFFT<real_t, D>::~RFFT() noexcept
    {
        fftw::destroy_plan(m_forward_plan);
        fftw::destroy_plan(m_inverse_plan);

        if (m_in_place)
        {
            fftw::free(reinterpret_cast<void *>(m_real_buffer));
        }
        else
        {
            fftw::free(reinterpret_cast<void *>(m_real_buffer));
            fftw::free(reinterpret_cast<void *>(m_freq_buffer));
        }
    }

    template <typename real_t, unsigned D>
    NdArray<real_t,D>::View RFFT<real_t, D>::real_space()
    {
        return NdArray<real_t,D>::View(m_real_buffer, m_real_slice);
    }

    template <typename real_t, unsigned D>
    NdArray<std::complex<real_t>,D>::View RFFT<real_t, D>::freq_space()
    {
        return NdArray<real_t,D>::View(
            reinterpret_cast<std::complex<real_t>*>(m_freq_buffer),
            m_freq_slice);
    }

    template <typename real_t, unsigned D>
    void RFFT<real_t, D>::forward()
    {
        fftw::execute(m_forward_plan);
    }

    template <typename real_t, unsigned D>
    void RFFT<real_t, D>::inverse()
    {
        fftw::execute(m_inverse_plan);
    }
}}} // namespace HyperCanny::numeric::fourier
