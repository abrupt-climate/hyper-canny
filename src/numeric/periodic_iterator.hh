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

/*! \file numeric/periodic_iterator.hh
 *  \brief Slice an array periodically
 */

#include "support.hh"
#include "nditerator.hh"

namespace HyperCanny {
namespace numeric {
    template <typename T>
    struct dimension {};

    template <typename LinearIterator, unsigned D, typename T>
    struct dimension<NdIteratorImpl<LinearIterator, D, T>>
    {
        static constexpr unsigned value = D;
    };

    template <typename NdIterator>
    class PeriodicIterator: public std::iterator<std::forward_iterator_tag, typename NdIterator::value_type>
    {
        public:
            static constexpr unsigned D = dimension<NdIterator>::value;
            using value_type = typename NdIterator::value_type;
        
        private:
            NdIterator m_iterator;
            shape_t<D> m_shape, m_index;

        public:
            shape_t<D> const &index() const { return m_iterator.index(); }
            size_t index(unsigned i) const { return m_iterator[i]; }

            PeriodicIterator(
                    NdIterator const &iterator,
                    shape_t<D> const &shape)
                : m_iterator(iterator)
                , m_shape(shape)
            {
                m_index.fill(0);
            }

            PeriodicIterator()
                : m_iterator()
            {}

            PeriodicIterator &operator++()
            {
                ++m_index[0];
                m_iterator.cycle(0);

                unsigned i = 0;
                while (m_index[i] == m_shape[i])
                {
                    m_iterator.cycle(i, -m_shape[i]);
                    
                    if (++i == D)
                    {
                        m_iterator.set_end();
                        break;
                    }

                    m_index[i-1] = 0;
                    ++m_index[i];
                    m_iterator.cycle(i);
                }

                return *this;
            }

            value_type &operator*() const
            {
                return *m_iterator;
            }

            bool operator!=(PeriodicIterator const &other) const
            {
                return m_iterator != other.m_iterator;
            }

            bool operator==(PeriodicIterator const &other) const
            {
                return m_iterator == other.m_iterator;
            }
    };
}} // HyperCanny::numeric
