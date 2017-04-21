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

/*! \file numeric/nditerator.hh
 *  \brief N-dimensional iterator object.
 *
 *  This provides the basic arithmetic to iterate arrays, as such it
 *  provides an iterator-like interface, but it doesn't iterate anything.
 *  Maybe `NdIndex` is a better name.
 */

#include "types.hh"
#include "support.hh"

namespace HyperCanny {
namespace numeric
{
    /*!
     * \addtogroup NdArrays
     * @{
     */
    template <typename LinearIterator, unsigned D>
    class NdIterator
    {
        LinearIterator m_address;
        shape_t<D> m_index;
        shape_t<D> m_shape;
        stride_t<D> m_stride;
        stride_t<D> m_semi_stride;

        public:
            using value_type = typename std::iterator_traits<LinearIterator>::value_type;
            using
            LinearIterator const &address() const { return m_address; }

            NdIterator(
                    LinearIterator const &start,
                    shape_t<D> const &shape,
                    stride_t<D> const &stride)
                : m_address(start)
                , m_shape(shape)
                , m_stride(stride)
                , m_semi_stride(calc_semi_stride(shape, stride))
            {
                m_index.fill(0);
            }

            NdIterator(
                    LinearIterator const &start,
                    shape_t<D> const &shape,
                    stride_t<D> const &stride,
                    shape_t<D> const &index)
                : m_address(start)
                , m_index(index)
                , m_shape(shape)
                , m_stride(stride)
                , m_semi_stride(calc_semi_stride(shape, stride))
            {}

            NdIterator &cycle(unsigned axis)
            {
                ++m_index[axis];
                m_address += m_stride[axis];

                if (m_index[axis] == m_shape[axis])
                {
                    m_index[axis] = 0;
                    m_address -= m_stride[axis] * m_shape[axis];
                }

                return *this;
            }

            NdIterator &operator++()
            {
                ++m_index[0];
                m_address += m_semi_stride[0];

                unsigned i = 0;
                while (m_index[i] == m_shape[i])
                {
                    if (++i == D)
                    {
                        set_end();
                        break;
                    }

                    m_index[i-1] = 0;
                    ++m_index[i];
                    m_address += m_semi_stride[i];
                }

                return *this;
            }

            value_type &operator*() const
            {
                return m_address.operator*();
            }

            value_type *operator->() const
            {
                return m_address.operator->();
            }

            size_t operator[](size_t i) const
            {
                return m_index[i];
            }

            bool operator!=(NdRange const &other) const
            {
                return m_address != other.m_address;
            }
    };
    /*! @} */
}} // namespace numeric

