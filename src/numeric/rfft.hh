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

#include "fftw_traits.hh"
#include "support.hh"
#include <complex>

namespace HyperCanny {
namespace numeric {
namespace fourier
{
    template <typename real_t>
    class PointerRange
    {
        real_t *m_data;
        size_t m_size;

        public:
            using iterator = real_t *;
            using const_iterator = real_t const *;

            PointerRange() {}

            PointerRange(real_t *data, size_t size)
                : m_data(data)
                , m_size(size)
            {}

            real_t *data() { return m_data; }
            real_t *begin() { return m_data; }
            real_t *end() { return m_data + m_size; }
            real_t const *cbegin() const { return m_data; }
            real_t const *cend() const { return m_data + m_size; }
            real_t const *begin() const { return cbegin(); }
            real_t const *end() const { return cend(); }

            void set(real_t *ptr, size_t size)
            {
                m_data = ptr;
                m_size = size;
            }

            real_t operator[](size_t idx) { return m_data[idx]; }
    };

    template <typename real_t, unsigned D>
    class RFFT
    {
        using fftw = RFFT_traits<real_t>;
        using complex_t = typename fftw::complex_t;
        using plan_t = typename fftw::plan_t;

        bool m_in_place;
        Slice<D> m_real_slice;
        Slice<D> m_freq_slice;

        PointerRange<real_t> m_real_buffer;
        PointerRange<std::complex<real_t>> m_freq_buffer;

        NdArrayView<real_t, D, PointerRange<real_t>> m_real_space;
        NdArrayView<std::complex<real_t>, D, PointerRange<std::complex<real_t>>> m_freq_space;
        // real_t *m_real_buffer;
        // complex_t *m_freq_buffer;

        plan_t m_forward_plan;
        plan_t m_inverse_plan;

        public:
            RFFT() = delete;
            RFFT(RFFT const &) = delete;
            RFFT &operator=(RFFT const &) = delete;

            size_t size() const;

            explicit RFFT(shape_t<D> const &shape, unsigned flags=0, bool in_place=true);
            ~RFFT() noexcept;

            NdArrayView<real_t,D,PointerRange<real_t>> &real_space();
            NdArrayView<std::complex<real_t>,D,PointerRange<std::complex<real_t>>> &freq_space();

            void forward();
            void inverse();
    };

    //=======================================================================//
    //                             IMPLEMENTATION                            //
    //=======================================================================//

    template <unsigned long D>
    Slice<D> compute_freq_slice(shape_t<D> shape)
    {
        shape_t<D> freq_shape = shape;
        freq_shape[0] = (shape[0]/2 + 1);
        return Slice<D>(freq_shape);
    }

    template <unsigned long D>
    Slice<D> compute_real_slice(shape_t<D> shape, bool in_place)
    {
        shape_t<D> real_shape = shape;

        if (in_place)
        {
            shape_t<D> real_mem_shape = shape;
            real_mem_shape[0] = 2 * (shape[0]/2 + 1);
            stride_t<D> real_stride = calc_stride(real_mem_shape);
            return Slice<D>(0, real_shape, real_stride);
        }
        else
        {
            return Slice<D>(shape);
        }
    }

    template <typename real_t, unsigned D>
    RFFT<real_t,D>::RFFT(shape_t<D> const &shape, unsigned flags, bool in_place)
        : m_in_place(in_place)
        , m_real_slice(compute_real_slice(shape, in_place))
        , m_freq_slice(compute_freq_slice(shape))
        , m_real_space(m_real_slice, m_real_buffer)
        , m_freq_space(m_freq_slice, m_freq_buffer)
    {
        // see FFTW manual: Real-data DFT Array Format
        // FFTW stores in row-major order, we use column-major order.
        if (m_in_place)
        {
            size_t real_size = m_real_slice.stride[D-1] * m_real_slice.shape[D-1];
            m_real_buffer.set(fftw::alloc_real(real_size), m_real_slice.size);
            m_freq_buffer.set(
                reinterpret_cast<std::complex<real_t> *>(
                    m_real_buffer.data()),
                m_freq_slice.size);
        }
        else
        {
            m_real_buffer.set(fftw::alloc_real(m_real_slice.size), m_real_slice.size);
            m_freq_buffer.set(
                reinterpret_cast<std::complex<real_t> *>(
                    fftw::alloc_complex(m_freq_slice.size)),
                m_freq_slice.size);
        }

        int n[D];
        std::copy(shape.rbegin(), shape.rend(), n);

        m_forward_plan = fftw::forward_plan(
            D, n, m_real_buffer.data(), reinterpret_cast<complex_t *>(m_freq_buffer.data()), flags);
        m_inverse_plan = fftw::inverse_plan(
            D, n, reinterpret_cast<complex_t *>(m_freq_buffer.data()), m_real_buffer.data(), flags);
    }

    template <typename real_t, unsigned D>
    size_t RFFT<real_t, D>::size() const
    {
        return m_real_slice.size;
    }

    template <typename real_t, unsigned D>
    RFFT<real_t, D>::~RFFT() noexcept
    {
        fftw::destroy_plan(m_forward_plan);
        fftw::destroy_plan(m_inverse_plan);

        if (m_in_place)
        {
            fftw::free(reinterpret_cast<void *>(m_real_buffer.data()));
        }
        else
        {
            fftw::free(reinterpret_cast<void *>(m_real_buffer.data()));
            fftw::free(reinterpret_cast<void *>(m_freq_buffer.data()));
        }
    }

    template <typename real_t, unsigned D>
    NdArrayView<real_t,D,PointerRange<real_t>> &RFFT<real_t, D>::real_space()
    {
        return m_real_space;
    }

    template <typename real_t, unsigned D>
    NdArrayView<std::complex<real_t>,D,PointerRange<std::complex<real_t>>> &RFFT<real_t, D>::freq_space()
    {
        return m_freq_space;
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
