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
#include <type_traits>
#include <algorithm>

namespace HyperCanny {
namespace numeric
{
    /*! \addtogroup NdArrays
     * @{
     */
    template <typename T, unsigned D, typename Container = std::vector<T>>
    class NdArrayBase
    {
        public:
            virtual ~NdArrayBase() {}

            virtual Container &container() = 0;
            virtual Container const &container() const = 0;

            virtual shape_t<D> const &shape() const = 0;
    };

    template <typename T, unsigned D, typename Container>
    class NdArrayView;

    template <typename T, unsigned D, typename Container>
    class ConstNdArrayView;

    template <typename T, unsigned D, typename Container>
    class PeriodicNdArrayView;

    template <typename T, unsigned D, typename Container>
    class NdArray;

    template <typename ArrayT>
    struct array_traits;

    template <typename T, unsigned D, typename Container>
    struct array_traits<NdArray<T, D, Container>>
    {
        static constexpr unsigned dimension = D;
        using container_type = Container;
        using value_type = T;
        using iterator = NdIterator<typename Container::iterator, D>;
        using const_iterator = ConstNdIterator<typename Container::const_iterator, D>;
        using base_type = NdArrayBase<T, D, Container>;
    };

    template <typename T, unsigned D, typename Container>
    struct array_traits<NdArrayView<T, D, Container>>
    {
        static constexpr unsigned dimension = D;
        using container_type = Container;
        using value_type = T;
        using iterator = NdIterator<typename Container::iterator, D>;
        using const_iterator = ConstNdIterator<typename Container::const_iterator, D>;
        using base_type = NdArrayBase<T, D, Container>;
    };

    template <typename T, unsigned D, typename Container>
    struct array_traits<ConstNdArrayView<T, D, Container>>
    {
        static constexpr unsigned dimension = D;
        using container_type = Container;
        using value_type = T;
        using iterator = ConstNdIterator<typename Container::const_iterator, D>;
        using const_iterator = ConstNdIterator<typename Container::const_iterator, D>;
        using base_type = NdArrayBase<T, D, Container>;
    };

    template <typename T, unsigned D, typename Container = std::vector<T>>
    class PeriodicNdArrayView: public NdArrayBase<T, D, Container>
    {
        using Base = NdArrayBase<T, D, Container>;

        Container &m_container;
        Slice<D>   m_slice;
        shape_t<D> m_offset;
        shape_t<D> m_shape;

        public:
            using base_iterator = NdIterator<typename Container::iterator, D>;
            using iterator = PeriodicIterator<base_iterator>;
            using base_const_iterator = ConstNdIterator<typename Container::const_iterator, D>;
            using const_iterator = PeriodicIterator<base_const_iterator>;

            shape_t<D> const &shape() const { return m_shape; }

            PeriodicNdArrayView(
                    Container &data,
                    Slice<D> const &slice,
                    stride_t<D> const &offset,
                    shape_t<D> const &shape)
                : m_container(data)
                , m_slice(slice)
                , m_offset(modulo(offset, slice.shape))
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
                    base_iterator(m_container.begin(), m_slice, m_offset),
                    m_shape);
            }
            iterator end() { return iterator(); }

            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }

            const_iterator cbegin() const
            {
                return const_iterator(
                    base_const_iterator(m_container.cbegin(), m_slice, m_offset),
                    m_shape);
            }
            const_iterator cend() const { return const_iterator(); }

            virtual Container &container() { return m_container; }
            virtual Container const &container() const { return m_container; }

            template <typename T2>
            bool operator==(T2 const &other) const
                // typename std::enable_if<std::is_base_of<nd_array_tag, T>::value, T>::type const &other) const
            {
                return (shape() == other.shape()) && std::equal(begin(), end(), other.begin());
            }

            template <typename T2>
            bool operator!=(T2 const &other) const
                // typename std::enable_if<std::is_base_of<nd_array_tag, T>::value, T>::type const &other) const
            {
                return (shape() != other.shape()) || !std::equal(begin(), end(), other.begin());
            }

            template <typename T2>
            PeriodicNdArrayView &operator=(T2 const &other)
            {
                if (shape() != other.shape())
                    throw "shapes of arrays do not match.";
                std::copy(other.cbegin(), other.cend(), begin());
                return *this;
            }
    };

    template <typename Derived>
    class NdArrayImpl: public array_traits<Derived>::base_type
    {
        using traits = array_traits<Derived>;
        static constexpr unsigned D = traits::dimension;

        protected:
            Slice<D> m_slice;

        public:
            using value_type = typename traits::value_type;
            using container_type = typename traits::container_type;
            using iterator = typename traits::iterator;
            using const_iterator = typename traits::const_iterator;
            using View = NdArrayView<value_type, D, container_type>;
            using ConstView = ConstNdArrayView<value_type, D, container_type>;

            Slice<D> const &slice() const { return m_slice; }
            size_t offset() const { return m_slice.offset; }
            size_t shape(unsigned i) const { return m_slice.shape[i]; }
            shape_t<D> const &shape() const { return m_slice.shape; }
            size_t size() const { return m_slice.size; }
            stride_t<D> const &stride() const { return m_slice.stride; }

            NdArrayImpl() {}

            explicit NdArrayImpl(shape_t<D> const &shape):
                m_slice(shape)
            {}

            NdArrayImpl(size_t offset, shape_t<D> const &shape, stride_t<D> const &stride):
                m_slice(offset, shape, stride)
            {}

            explicit NdArrayImpl(Slice<D> const &slice):
                m_slice(slice)
            {}

            iterator begin()
            {
                return iterator(
                    std::begin(this->container()) + offset(), shape(), stride());
            }
            iterator end() { return iterator(); }

            const_iterator cbegin() const
            {
                return const_iterator(
                    std::cbegin(this->container()) + offset(), shape(), stride());
            }
            const_iterator cend() const { return const_iterator(); }

            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }

            template <unsigned axis>
            View reverse()
                { return View(m_slice.reverse<axis>(), this->container()); }
            template <unsigned axis>
            ConstView reverse() const
                { return View(m_slice.reverse<axis>(), this->container()); }

            View reverse_all()
                { return View(m_slice.reverse_all(), this->container()); }
            ConstView reverse_all() const
                { return ConstView(m_slice.reverse_all(), this->container()); }

            View transpose()
                { return View(m_slice.transpose(), this->container()); }
            View transpose() const
                { return ConstView(m_slice.transpose(), this->container()); }

            template <unsigned axis>
            View sub(size_t begin, size_t end, size_t step = 1)
                { return View(m_slice.sub<axis>(begin, end, step), this->container()); }
            template <unsigned axis>
            ConstView sub(size_t begin, size_t end, size_t step = 1) const
                { return ConstView(m_slice.sub<axis>(begin, end, step), this->container()); }

            template <unsigned axis>
            NdArrayView<value_type, D-1, container_type> sel(size_t idx)
                { return NdArrayView<value_type, D-1, container_type>(
                        m_slice.sel<axis>(idx), this->container()); }
            template <unsigned axis>
            ConstNdArrayView<value_type, D-1, container_type> sel(size_t idx) const
                { return ConstNdArrayView<value_type, D-1, container_type>(
                        m_slice.sel<axis>(idx), this->container()); }

            PeriodicNdArrayView<value_type, D, container_type> periodic_view(
                    stride_t<D> const &offset, shape_t<D> const &shape)
            {
                return PeriodicNdArrayView<value_type, D, container_type>(
                    this->container(), slice(), offset, shape);
            }

            NdArray<value_type, D, std::vector<value_type>> copy() const
            {
                NdArray<value_type, D, std::vector<value_type>> result(shape());
                std::copy(cbegin(), cend(), result.begin());
                return result;
            }

            Derived &operator+=(value_type value)
            {
                for (auto i = begin(); i != end(); ++i)
                    *i += value;
                return static_cast<Derived &>(*this);
            }

            template <typename T>
            Derived &operator*=(T const &other)
            {
                std::transform(begin(), end(), other.cbegin(), begin(),
                    [] (auto a, auto b) { return a * b; });
                return static_cast<Derived &>(*this);
            }

            Derived &operator*=(value_type value)
            {
                std::transform(begin(), end(), begin(),
                    [value] (auto a) { return a * value; });
                return static_cast<Derived &>(*this);
            }

            Derived &operator/=(value_type value)
            {
                std::transform(begin(), end(), begin(),
                    [value] (auto a) { return a / value; });
                return static_cast<Derived &>(*this);
            }

            template <typename T>
            Derived &operator=(T const &other)
            {
                if (shape() != other.shape())
                    throw "shapes of arrays do not match.";
                std::copy(other.cbegin(), other.cend(), begin());
                return static_cast<Derived &>(*this);
            }

            Derived &operator=(value_type value)
            {
                std::fill(begin(), end(), value);
                return static_cast<Derived &>(*this);
            }

            template <typename T>
            bool operator==(T const &other) const
            {
                return (shape() == other.shape()) && std::equal(begin(), end(), other.begin());
            }

            template <typename T>
            bool operator!=(T const &other) const
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
    class NdArray: public NdArrayImpl<NdArray<T, D, Container>>
    {
        using Base = NdArrayImpl<NdArray>;
        Container m_container;

        public:
            using View = NdArrayView<T,D,Container>;
            NdArray() {}

            explicit NdArray(shape_t<D> const &shape):
                NdArrayImpl<NdArray>(shape),
                m_container(calc_size<D>(shape))
            {}

            NdArray(shape_t<D> const &shape, T value):
                NdArrayImpl<NdArray>(shape),
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

            using Base::operator=;
            //NdArray &operator=(NdArray const &other) { return Base::operator=(other); }
            //template <typename T2>
            //NdArray &operator=(T2 const &other) { return Base::operator=(other); }
    };

    template <typename T, unsigned D, typename Container = std::vector<T>>
    class NdArrayView: public NdArrayImpl<NdArrayView<T, D, Container>>
    {
        using Base = NdArrayImpl<NdArrayView>;

        Container &m_container;

        public:
            NdArrayView() {}

            NdArrayView(Slice<D> const &slice, Container &data):
                Base(slice),
                m_container(data)
            {}

            virtual Container &container() { return m_container; }
            virtual Container const &container() const { return m_container; }

            // using Base::operator=;
            NdArrayView &operator=(NdArrayView const &other) { return Base::operator=(other); }
            template <typename T2>
            NdArrayView &operator=(T2 const &other) { return Base::operator=(other); }
    };

    template <typename T, unsigned D, typename Container = std::vector<T>>
    class ConstNdArrayView: public NdArrayImpl<ConstNdArrayView<T, D, Container>>
    {
        Container const &m_container;

        public:
            ConstNdArrayView() {}

            ConstNdArrayView(Slice<D> const &slice, Container &data):
                NdArrayImpl<ConstNdArrayView>(slice),
                m_container(data)
            {}

            virtual Container const &container() const { return m_container; }

            virtual Container &container() = delete;
            ConstNdArrayView &operator=(ConstNdArrayView const &other) = delete;
    };

    template <typename T, typename Container>
    class NdArrayView<T,0,Container>
    {
        public:
            NdArrayView(Slice<0> const &, Container &data) {}
    };

    /*! @} */
}} // namespace HyperCanny::numeric
