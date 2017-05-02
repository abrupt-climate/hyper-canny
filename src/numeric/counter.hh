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

/*! \file numeric/counter.hh
 *  \brief Iterator that counts.
 */

#include "nditerator.hh"

namespace HyperCanny {
namespace numeric
{
    template <typename value_type=int>
    class Counter: public std::iterator<std::forward_iterator_tag, const value_type>
    {
        value_type i;

        public:
            Counter(): i(0) {}
            Counter(value_type i_): i(i_) {}

            Counter &operator++() { ++i; return *this; }
            Counter &operator+=(value_type j) { i += j; return *this; }
            Counter &operator--() { --i; return *this; }
            Counter &operator-=(value_type j) { i -= j; return *this; }
            Counter operator+(value_type j) const { return Counter(*this) += j; }

            value_type const &operator*() const { return i; }
            bool operator!=(Counter const &other) const { return i != other.i; }
    };

    template <typename T=size_t>
    class Integers
    {
        size_t m_size;

        public:
            using value_type = T;
            using reference = T;
            using const_reference = T;
            using iterator = Counter<value_type>;
            using const_iterator = iterator;

            Integers(size_t size = 0)
                : m_size(size)
            {}

            void resize(size_t size) { m_size = size; }
            value_type operator[](size_t idx) const { return idx; }
            iterator begin() const { return iterator(); }
            iterator end() const { return iterator(m_size); }
            iterator cbegin() const { return begin(); }
            iterator cend() const { return end(); }
    };

    template <unsigned D>
    using Grid = NdArray<size_t, D, Integers<size_t>>;

    template <unsigned D>
    class NdCounter: public ConstNdIterator<Counter<int>, D>
    {
        using Base = ConstNdIterator<Counter<int>, D>;

        public:
            NdCounter()
                : Base()
            {}

            NdCounter(Slice<D> const &slice)
                : Base(Counter<int>(), slice)
            {}

            NdCounter(Slice<D> const &slice, shape_t<D> const &index)
                : Base(Counter<int>(), slice, index)
            {}

            NdCounter(int start, Slice<D> const &slice)
                : Base(Counter<int>(start), slice)
            {}
    };

    template <unsigned D>
    struct dimension<NdCounter<D>>
    {
        static constexpr unsigned value = D;
    };
}}
