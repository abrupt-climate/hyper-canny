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

/*! \file numeric/ndarray.hh
 *  \brief N-dimensional arrays
 */

#include "types.hh"
#include "support.hh"
#include "slice.hh"
#include "nditerator.hh"
#include "periodic_iterator.hh"

#include <vector>
#include <initializer_list>

namespace HyperCanny {
namespace numeric
{
    /*! \addtogroup NdArrays
     * @{
     */
    template <typename T, unsigned D, typename Container>
    class NdArrayView;

    template <typename T, unsigned D, typename Container>
    class NdArray;

    template <typename T, unsigned D, typename Container = std::vector<T>>
    class NdArrayBase
    {
        protected:
            Slice<D> m_slice;

        public:
            using View = NdArrayView<T,D,Container>;
            using iterator = NdIterator<typename Container::iterator, D>;
            using const_iterator = ConstNdIterator<typename Container::const_iterator, D>;

            NdArrayBase() {}

            explicit NdArrayBase(shape_t<D> const &shape):
                m_slice(shape)
            {}

            NdArrayBase(size_t offset, shape_t<D> const &shape, stride_t<D> const &stride):
                m_slice(offset, shape, stride)
            {}

            explicit NdArrayBase(Slice<D> const &slice):
                m_slice(slice)
            {}

            virtual ~NdArrayBase() {}

            Slice<D> const &slice() const { return m_slice; }
            shape_t<D> const &shape() const { return m_slice.shape; }
            stride_t<D> const &stride() const { return m_slice.stride; }
            size_t offset() const { return m_slice.offset; }
            size_t shape(unsigned i) const { return m_slice.shape[i]; }

            size_t size() const { return m_slice.size; }

            iterator begin()
            {
                return iterator(
                    std::begin(container()) + m_slice.offset, m_slice.shape, m_slice.stride);
            }

            iterator end() { return iterator(); }

            const_iterator cbegin() const
            {
                return const_iterator(
                    std::cbegin(container()) + m_slice.offset, m_slice.shape, m_slice.stride);
            }

            const_iterator cend() const { return const_iterator(); }

            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }

            template <unsigned axis>
            View reverse()
                { return View(m_slice.reverse<axis>(), container()); }
            View reverse_all()
                { return View(m_slice.reverse_all(), container()); }
            View transpose()
                { return View(m_slice.transpose(), container()); }

            template <unsigned axis>
            View sub(size_t begin, size_t end, size_t step = 1)
                { return View(m_slice.sub<axis>(begin, end, step), container()); }

            template <unsigned axis>
            NdArrayView<T,D-1,Container> sel(size_t idx)
                { return NdArrayView<T,D-1,Container>(m_slice.sel<axis>(idx), container()); }

            Container &data() { return container(); }
            Container const &data () const { return container(); }
            virtual Container &container() = 0;
            virtual Container const &container() const = 0;

            NdArray<T, D, std::vector<T>> copy() const
            {
                NdArray<T, D, std::vector<T>> result(shape());
                std::copy(cbegin(), cend(), result.begin());
                return result;
            }

            NdArrayBase &operator+=(T value)
            {
                for (auto i = begin(); i != end(); ++i)
                    *i += value;
                return *this;
            }

            NdArrayBase &operator*=(T value)
            {
                for (auto i = begin(); i != end(); ++i)
                    *i *= value;
                return *this;
            }

            template <typename T2>
            NdArrayBase &operator=(T2 const &other)
            {
                if (shape() != other.shape())
                    throw "shapes of arrays do not match.";
                std::copy(other.cbegin(), other.cend(), begin());
                return *this;
            }

            NdArrayBase &operator=(T value)
            {
                std::fill(begin(), end(), value);
                return *this;
            }

            template <typename T2>
            bool operator==(T2 const &other) const
            {
                return (shape() == other.shape()) && std::equal(begin(), end(), other.begin());
            }

            template <typename T2>
            bool operator!=(T2 const &other) const
            {
                return (shape() != other.shape()) || !std::equal(begin(), end(), other.begin());
            }
    };

    /*! \brief Multi-dimensional numeric array.
     *
     * This allows us to use numeric data, stored in std::vector
     * in a NumPy fashion. The raw data is accompanied with shape
     * and stride information. The shape and stride allow us to change
     * the way in which we iterate the data.
     */
    template <typename T, unsigned D, typename Container = std::vector<T>>
    class NdArray: public NdArrayBase<T, D>
    {
        Container m_container;

        public:
            using View = NdArrayView<T,D,Container>;
            NdArray() {}

            explicit NdArray(shape_t<D> const &shape):
                NdArrayBase<T, D>(shape),
                m_container(calc_size<D>(shape))
            {}

            NdArray(shape_t<D> const &shape, T value):
                NdArrayBase<T, D>(shape),
                m_container(calc_size<D>(shape), value)
            {}

            NdArray(shape_t<D> const &shape, std::initializer_list<T> const &init)
                : NdArray(shape)
            {
                std::copy(init.begin(), init.end(), this->begin());
            }

            template <typename U>
            explicit NdArray(NdArray<U,D> const &other):
                NdArray(other.shape())
            {
                std::copy(other.begin(), other.end(), this->begin());
            }

            void resize(shape_t<D> shape)
            {
                this->m_slice = Slice<D>(shape);
                m_container.resize(this->m_slice.size);
            }

            T &operator[](size_t i) { return m_container[i]; }
            T const &operator[](size_t i) const { return m_container[i]; }

            virtual Container &container() { return m_container; }
            virtual Container const &container() const { return m_container; }

            template <typename T2>
            NdArray &operator=(NdArrayBase<T2, D> const &other) { NdArrayBase<T, D>::operator=(other); return *this; }
    };

    template <typename T, unsigned D, typename Container = std::vector<T>>
    class NdArrayView: public NdArrayBase<T, D, Container>
    {
        using Base = NdArrayBase<T, D, Container>;
        Container &m_container;

        public:
            NdArrayView() {}

            NdArrayView(Slice<D> const &slice, Container &data):
                Base(slice),
                m_container(data)
            {}

            virtual Container &container() { return m_container; }
            virtual Container const &container() const { return m_container; }

            template <typename T2>
            NdArrayView &operator*=(T2 const &other) { Base::operator*=(other); return *this; }
            NdArrayView &operator=(NdArrayView const &other) { Base::operator=(other); return *this; }
            NdArrayView &operator=(Base const &other) { Base::operator=(other); return *this; }
            template <typename T2>
            NdArrayView &operator=(T2 value) { Base::operator=(value); return *this; }

            //template <typename T2>
            //bool operator==(T2 const &other) const { return Base::operator==(other); }
    };

    template <typename T, unsigned D, typename Container = std::vector<T>>
    class PeriodicNdArrayView: public NdArrayView<T, D, Container>
    {
        using Base = NdArrayView<T, D, Container>;

        shape_t<D> m_offset;
        shape_t<D> m_shape;

        public:
            using base_iterator = typename Base::iterator;
            using iterator = PeriodicIterator<base_iterator>;
            using base_const_iterator = typename Base::const_iterator;
            using const_iterator = PeriodicIterator<base_const_iterator>;

            shape_t<D> const &shape() const { return m_shape; }

            PeriodicNdArrayView(
                    NdArrayBase<T, D, Container> &data,
                    stride_t<D> const &offset,
                    shape_t<D> const &shape)
                : NdArrayView<T, D, Container>(data.slice(), data.container())
                , m_offset(modulo(offset, data.shape()))
                , m_shape(shape)
            {}

            PeriodicNdArrayView &set_offset(stride_t<D> const &offset)
            {
                m_offset = modulo(offset, this->shape());
                return *this;
            }

            iterator begin()
            {
                return iterator(
                    base_iterator(this->container().begin(), this->slice(), m_offset),
                    m_shape);
            }
            iterator end() { return iterator(); }

            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }

            const_iterator cbegin() const
            {
                return const_iterator(
                    base_const_iterator(this->container().cbegin(), this->slice(), m_offset),
                    m_shape);
            }
            const_iterator cend() const { return const_iterator(); }

            template <typename Array>
            bool operator!=(Array const &other) const
            {
                return (shape() != other.shape()) || !std::equal(begin(), end(), other.begin());
            }

            template <typename Array>
            bool operator==(Array const &other) const
            {
                return (shape() == other.shape()) && std::equal(begin(), end(), other.begin());
            }
    };

    template <typename T, typename Container>
    class NdArrayView<T,0,Container>
    {
        public:
            NdArrayView(Slice<0> const &, Container &data) {}
    };

    /*! @} */
}} // namespace HyperCanny::numeric
