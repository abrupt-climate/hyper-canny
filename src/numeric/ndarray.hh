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
#include <vector>
#include <initializer_list>

namespace HyperCanny {
namespace numeric
{
    /*! \addtogroup NdArrays
     * @{
     */
    template <typename T, unsigned D, typename Container = std::vector<T>>
    class NdArrayBase
    {
        protected:
            Slice<D> m_slice;

        public:
            using iterator = NdIterator<typename Container::iterator, D>;
            using const_iterator = NdIterator<typename Container::const_iterator, D>;

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
            size_t shape(unsigned i) const { return m_slice.shape[i]; }

            size_t size() const { return m_slice.size; }

            iterator begin()
            {
                return iterator(
                    container().begin(), m_slice.shape, m_slice.stride);
            }

            iterator end()
            {
                return iterator(
                    container().end(), m_slice.shape, m_slice.stride, m_slice.shape);
            }

            const_iterator cbegin() const
            {
                return const_iterator(
                    container().cbegin(), m_slice.shape, m_slice.stride);
            }

            const_iterator cend() const
            {
                return const_iterator(
                    container().cend(), m_slice.shape, m_slice.stride, m_slice.shape);
            }

            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }

            template <unsigned axis>
            View reverse()
                { return View(m_slice.reverse<axis>(), container()); }
            View transpose()
                { return View(m_slice.transpose(), container()); }

            template <unsigned axis>
            View sub(size_t begin, size_t end, size_t step = 1)
                { return View(m_slice.sub<axis>(begin, end, step), container()); }

            template <unsigned axis>
            typename NdArrayBase<T,D-1>::View sel(size_t idx)
                { return typename NdArrayBase<T,D-1>::View(m_slice.sel<axis>(idx), container()); }

            virtual Container &container() = 0;
            virtual Container const &container() const = 0;

            NdArrayBase &operator+=(T value)
            {
                for (auto i = begin(); i != end(); ++i)
                    *i += value;
                return *this;
            }

            template <typename T2>
            NdArrayBase &operator=(NdArrayBase<T2,D> const &other)
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

            bool operator==(NdArrayBase const &other) const
            {
                return (shape() == other.shape()) && std::equal(begin(), end(), other.begin());
            }

            bool operator!=(NdArrayBase const &other) const
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
    template <typename T, unsigned D>
    class NdArray: public NdArrayBase<T, D>
    {
        std::vector<T> m_data;

        public:
            using View = typename NdArrayBase<T,D>::View;
            NdArray() {}

            explicit NdArray(shape_t<D> const &shape):
                NdArrayBase<T, D>(shape),
                m_data(calc_size<D>(shape))
            {}

            NdArray(shape_t<D> const &shape, T value):
                NdArrayBase<T, D>(shape),
                m_data(calc_size<D>(shape), value)
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
                m_data.resize(this->m_slice.size);
            }

            T &operator[](size_t i) { return m_data[i]; }
            T const &operator[](size_t i) const { return m_data[i]; }

            virtual T *data() { return m_data.data(); }
            virtual T const *data() const { return m_data.data(); }

            template <typename T2>
            NdArray &operator=(NdArrayBase<T2, D> const &other) { NdArrayBase<T, D>::operator=(other); return *this; }
    };

    template <typename T, unsigned D>
    class NdArrayBase<T, D>::View: public NdArrayBase<T, D>
    {
        T *m_data;

        public:
            View(Slice<D> const &slice, T *data):
                NdArrayBase<T, D>(slice),
                m_data(data)
            {}

            virtual T *data() { return m_data; }
            virtual T const *data() const { return m_data; }

            View &operator=(View const &other) { NdArrayBase<T, D>::operator=(other); return *this; }
            View &operator=(NdArrayBase<T, D> const &other) { NdArrayBase<T, D>::operator=(other); return *this; }
            View &operator=(T value) { NdArrayBase<T, D>::operator=(value); return *this; }
    };

    template <typename T, unsigned D>
    class PeriodicNdArrayView: public NdArrayBase<T, D>
    {
        T *m_data;
        PeriodicRange<D> m_range;

        public:
            PeriodicNdArrayView(
                    NdArrayBase<T, D> &data
                    stride_t<D> offset,
                    shape_t<D> shape):
                NdArrayBase<T, D>(data.slice()),
                m_data(data.data()),
                m_range(data.slice().periodic_begin())
            {}

            virtual T *data() { return m_data; }
            virtual T const *data() const { return m_data; }

            virtual iterator begin() { return iterator(data(), m_slice.begin()); }
            virtual iterator end() { return iterator(data(), m_slice.end()); }
            virtual const_iterator begin() const { return const_iterator(data(), m_slice.begin()); }
            virtual const_iterator end() const { return const_iterator(data(), m_slice.end()); }
            virtual const_iterator cbegin() const { return const_iterator(data(), m_slice.begin()); }
            virtual const_iterator cend() const { return const_iterator(data(), m_slice.end()); }
    };

    template <typename T>
    class NdArrayBase<T,0>
    {
        public:
            class View {
                public:
                    View(Slice<0> const &, T *data) {}
            };
    };

    /*! @} */
}} // namespace HyperCanny::numeric
