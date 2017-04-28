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
    template <typename LinearIterator, unsigned D, typename T>
    class NdIteratorImpl: public std::iterator<std::forward_iterator_tag, T>
    {
        LinearIterator m_address;
        shape_t<D> m_index;
        shape_t<D> m_shape;
        stride_t<D> m_stride;
        stride_t<D> m_semi_stride;
        bool m_end;

        public:
            using value_type = T;
            using reference = typename std::iterator_traits<LinearIterator>::reference;
            using pointer = typename std::iterator_traits<LinearIterator>::pointer;

            LinearIterator const &address() const { return m_address; }
            shape_t<D> const &index() const { return m_index; }

            NdIteratorImpl()
                : m_end(true)
            {}

            NdIteratorImpl(
                    LinearIterator const &start,
                    Slice<D> const &slice)
                : m_address(start)
                , m_shape(slice.shape)
                , m_stride(slice.stride)
                , m_semi_stride(calc_semi_stride(slice.shape, slice.stride))
                , m_end(false)
            {
                m_index.fill(0);
            }

            NdIteratorImpl(
                    LinearIterator const &start,
                    Slice<D> const &slice,
                    shape_t<D> const &index)
                : m_address(start + slice.flat_index(index))
                , m_index(index)
                , m_shape(slice.shape)
                , m_stride(slice.stride)
                , m_semi_stride(calc_semi_stride(slice.shape, slice.stride))
                , m_end(false)
            {}

            NdIteratorImpl(
                    LinearIterator const &start,
                    shape_t<D> const &shape,
                    stride_t<D> const &stride)
                : m_address(start)
                , m_shape(shape)
                , m_stride(stride)
                , m_semi_stride(calc_semi_stride(shape, stride))
                , m_end(false)
            {
                m_index.fill(0);
            }

            NdIteratorImpl(
                    LinearIterator const &start,
                    shape_t<D> const &shape,
                    stride_t<D> const &stride,
                    shape_t<D> const &index)
                : m_address(start)
                , m_index(index)
                , m_shape(shape)
                , m_stride(stride)
                , m_semi_stride(calc_semi_stride(shape, stride))
                , m_end(false)
            {}

            NdIteratorImpl &cycle(unsigned axis)
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

            NdIteratorImpl &cycle(unsigned axis, int delta)
            {
                ptrdiff_t new_index = modulo(m_index[axis] + delta, m_shape[axis]);
                m_address += (new_index - m_index[axis]) * m_stride[axis];
                m_index[axis] = new_index;

                return *this;
            }

            NdIteratorImpl &operator++()
            {
                ++m_index[0];
                m_address += m_semi_stride[0];

                unsigned i = 0;
                while (m_index[i] == m_shape[i])
                {
                    if (++i == D)
                    {
                        m_end = true;
                        break;
                    }

                    m_index[i-1] = 0;
                    ++m_index[i];
                    m_address += m_semi_stride[i];
                }

                return *this;
            }

            reference operator*() const
            {
                return *m_address;
            }

            pointer operator->() const
            {
                return m_address.operator->();
            }

            size_t operator[](size_t i) const
            {
                return m_index[i];
            }

            bool operator!=(NdIteratorImpl const &other) const
            {
                return (m_end != other.m_end)
                    || (!m_end && m_address != other.m_address);
            }

            bool operator==(NdIteratorImpl const &other) const
            {
                return (m_end && other.m_end)
                    || (!m_end && !other.m_end && m_address == other.m_address);
            }

            NdIteratorImpl &set_end()
            {
                m_end = true;
                return *this;
            }
    };

    template <typename LinearIterator, unsigned D>
    using ConstNdIterator = NdIteratorImpl<LinearIterator, D, typename std::iterator_traits<LinearIterator>::value_type const>;

    template <typename LinearIterator, unsigned D>
    using NdIterator = NdIteratorImpl<LinearIterator, D, typename std::iterator_traits<LinearIterator>::value_type>;

    /*! @} */
}} // namespace numeric

