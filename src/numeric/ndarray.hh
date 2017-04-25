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

    // # Base class and forward declarations {{{1
    /*! \brief Base class for all NdArray classes.
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
    class ConstPeriodicNdArrayView;

    template <typename T, unsigned D, typename Container>
    class NdArray;
    // }}}1

    // # Traits class {{{1
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
        using reduced_view = NdArrayView<T, D-1, Container>;
        using const_reduced_view = ConstNdArrayView<T, D-1, Container>;
        using periodic_view = PeriodicNdArrayView<T, D, Container>;
        using const_periodic_view = ConstPeriodicNdArrayView<T, D, Container>;
        using copy_type = NdArray<T, D, std::vector<T>>;
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
        using reduced_view = NdArrayView<T, D-1, Container>;
        using const_reduced_view = ConstNdArrayView<T, D-1, Container>;
        using periodic_view = PeriodicNdArrayView<T, D, Container>;
        using const_periodic_view = ConstPeriodicNdArrayView<T, D, Container>;
        using copy_type = NdArray<T, D, std::vector<T>>;
    };

    template <typename T, typename Container>
    struct array_traits<NdArrayView<T, 0, Container>> {};

    template <typename T, unsigned D, typename Container>
    struct array_traits<ConstNdArrayView<T, D, Container>>
    {
        static constexpr unsigned dimension = D;
        using container_type = Container;
        using value_type = T;
        using iterator = ConstNdIterator<typename Container::const_iterator, D>;
        using const_iterator = ConstNdIterator<typename Container::const_iterator, D>;
        using base_type = NdArrayBase<T, D, Container>;
        using reduced_view = NdArrayView<T, D-1, Container>;
        using const_reduced_view = ConstNdArrayView<T, D-1, Container>;
        using periodic_view = PeriodicNdArrayView<T, D, Container>;
        using const_periodic_view = ConstPeriodicNdArrayView<T, D, Container>;
        using copy_type = NdArray<T, D, std::vector<T>>;
    };

    template <typename T, typename Container>
    struct array_traits<ConstNdArrayView<T, 0, Container>> {};
    // }}}1

    // # Class interfaces {{{1
    // class PeriodicNdArrayView {{{2
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
            using base_const_iterator =
                ConstNdIterator<typename Container::const_iterator, D>;
            using const_iterator = PeriodicIterator<base_const_iterator>;

            shape_t<D> const &shape() const { return m_shape; }

            /*! \brief Constructor
             */
            PeriodicNdArrayView(
                    Container &data,
                    Slice<D> const &slice,
                    stride_t<D> const &offset,
                    shape_t<D> const &shape);

            /*! \brief Set the offset into the parent NdArray, this accepts
             *  negative values to wrap around bounds.
             */
            PeriodicNdArrayView &set_offset(stride_t<D> const &offset);

            iterator begin();
            iterator end() { return iterator(); }
            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }
            const_iterator cbegin() const;
            const_iterator cend() const { return const_iterator(); }

            virtual Container &container() { return m_container; }
            virtual Container const &container() const { return m_container; }

            // TODO merge these operators with those in NdArrayImpl
            template <typename T2>
            bool operator==(T2 const &other) const;

            template <typename T2>
            bool operator!=(T2 const &other) const;

            template <typename T2>
            PeriodicNdArrayView &operator=(T2 const &other);
    };
    // }}}2
    // class ConstPeriodicNdArrayView {{{2
    template <typename T, unsigned D, typename Container = std::vector<T>>
    class ConstPeriodicNdArrayView: public NdArrayBase<T, D, Container>
    {
        using Base = NdArrayBase<T, D, Container>;

        Container const &m_container;
        Slice<D>   m_slice;
        shape_t<D> m_offset;
        shape_t<D> m_shape;

        public:
            using base_iterator = NdIterator<typename Container::iterator, D>;
            using iterator = PeriodicIterator<base_iterator>;
            using base_const_iterator =
                ConstNdIterator<typename Container::const_iterator, D>;
            using const_iterator = PeriodicIterator<base_const_iterator>;

            shape_t<D> const &shape() const { return m_shape; }

            /*! \brief Constructor
             */
            ConstPeriodicNdArrayView(
                    Container const &data,
                    Slice<D> const &slice,
                    stride_t<D> const &offset,
                    shape_t<D> const &shape);

            /*! \brief Set the offset into the parent NdArray, this accepts
             *  negative values to wrap around bounds.
             */
            ConstPeriodicNdArrayView &set_offset(stride_t<D> const &offset);

            // iterator begin();
            // iterator end() { return iterator(); }
            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }
            const_iterator cbegin() const;
            const_iterator cend() const { return const_iterator(); }

            virtual Container &container();
            virtual Container const &container() const { return m_container; }

            // TODO merge these operators with those in NdArrayImpl
            template <typename T2>
            bool operator==(T2 const &other) const;

            template <typename T2>
            bool operator!=(T2 const &other) const;

            template <typename T2>
            ConstPeriodicNdArrayView &operator=(T2 const &other);
    };
    /// }}}2
    // CRTP class NdArrayImpl {{{2
    template <typename Derived>
    class NdArrayImpl: public array_traits<Derived>::base_type
    {
        using traits = array_traits<Derived>;
        static constexpr unsigned D = traits::dimension;

        protected:
            Slice<D> m_slice;

        public:
            using slice_type = Slice<D>;
            using stride_type = stride_t<D>;
            using shape_type = shape_t<D>;
            using value_type = typename traits::value_type;
            using container_type = typename traits::container_type;
            using iterator = typename traits::iterator;
            using const_iterator = typename traits::const_iterator;
            using View = NdArrayView<value_type, D, container_type>;
            using ConstView = ConstNdArrayView<value_type, D, container_type>;

            Slice<D> const &slice() const;
            size_t offset() const;
            shape_t<D> const &shape() const;
            size_t size() const;
            stride_t<D> const &stride() const;

            NdArrayImpl();
            explicit NdArrayImpl(shape_t<D> const &shape);
            NdArrayImpl(size_t offset, shape_t<D> const &shape, stride_t<D> const &stride);
            explicit NdArrayImpl(Slice<D> const &slice);

            iterator begin();
            iterator end();
            const_iterator cbegin() const;
            const_iterator cend() const;
            const_iterator begin() const;
            const_iterator end() const;

            template <unsigned Reduced>
            NdArrayView<value_type, Reduced, container_type>
            view(Slice<Reduced> const &slice)
            {
                return NdArrayView<value_type, Reduced, container_type>(
                        slice, this->container());
            }

            template <unsigned Reduced>
            ConstNdArrayView<value_type, Reduced, container_type>
            view(Slice<Reduced> const &slice) const
            {
                return ConstNdArrayView<value_type, Reduced, container_type>(
                        slice, this->container());
            }

            template <unsigned axis>
            View reverse();
            template <unsigned axis>
            ConstView reverse() const;

            View reverse_all();
            ConstView reverse_all() const;

            View transpose();
            ConstView transpose() const;

            template <unsigned axis>
            View sub(size_t begin, size_t end, size_t step = 1);
            template <unsigned axis>
            ConstView sub(size_t begin, size_t end, size_t step = 1) const;

            typename array_traits<Derived>::reduced_view sel(unsigned axis, size_t idx);
            typename array_traits<Derived>::const_reduced_view sel(unsigned axis, size_t idx) const;

            template <unsigned axis>
            typename array_traits<Derived>::reduced_view sel(size_t idx);
            template <unsigned axis>
            typename array_traits<Derived>::const_reduced_view sel(size_t idx) const;

            typename array_traits<Derived>::periodic_view periodic_view(
                    stride_t<D> const &offset, shape_t<D> const &shape);
            typename array_traits<Derived>::const_periodic_view periodic_view(
                    stride_t<D> const &offset, shape_t<D> const &shape) const;

            typename array_traits<Derived>::copy_type copy() const;

            Derived &operator+=(value_type value);
            template <typename T>
            Derived &operator*=(T const &other);
            Derived &operator*=(value_type value);
            Derived &operator/=(value_type value);
            template <typename T>
            Derived &operator=(T const &other);
            Derived &operator=(value_type value);
            template <typename T>
            bool operator==(T const &other) const;
            template <typename T>
            bool operator!=(T const &other) const;
    };
    // }}}2
    // class NdArray {{{2
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

            NdArray &operator=(NdArray const &other)
                { return Base::operator=(other); }
            template <typename T2>
            NdArray &operator=(T2 const &other)
                { return Base::operator=(other); }
    };
    // }}}2
    // class NdArrayView {{{2
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

    template <typename T, typename Container>
    class NdArrayView<T,0,Container>
    {
        public:
            NdArrayView(Slice<0> const &, Container &data) {}
    };
    // }}}2
    // class ConstNdArrayView {{{2
    template <typename T, unsigned D, typename Container = std::vector<T>>
    class ConstNdArrayView: public NdArrayImpl<ConstNdArrayView<T, D, Container>>
    {
        using Base = NdArrayImpl<ConstNdArrayView<T, D, Container>>;

        Container const &m_container;

        public:
            using iterator = typename Base::const_iterator;
            iterator begin() const { return this->cbegin(); }
            iterator end() const { return this->cend(); }

            ConstNdArrayView() {}

            ConstNdArrayView(Slice<D> const &slice, Container const &data):
                NdArrayImpl<ConstNdArrayView>(slice),
                m_container(data)
            {}

            virtual Container const &container() const { return m_container; }

            ConstNdArrayView &operator=(ConstNdArrayView const &other) = delete;


        private:
            virtual Container &container();
    };
    // }}}2
    // }}}1

    /*! @} */

    // # Implementations {{{1
    // ConstPeriodicNdArrayView {{{2
    template <typename value_type, unsigned D, typename Container>
    ConstPeriodicNdArrayView<value_type, D, Container>::ConstPeriodicNdArrayView(
            Container const &data,
            Slice<D> const &slice,
            stride_t<D> const &offset,
            shape_t<D> const &shape)
        : m_container(data)
        , m_slice(slice)
        , m_offset(modulo(offset, slice.shape))
        , m_shape(shape)
    {}

    template <typename value_type, unsigned D, typename Container>
    ConstPeriodicNdArrayView<value_type, D, Container> &
    ConstPeriodicNdArrayView<value_type, D, Container>::set_offset(
            stride_t<D> const &offset)
    {
        m_offset = modulo(offset, this->shape());
        return *this;
    }

    template <typename value_type, unsigned D, typename Container>
    typename ConstPeriodicNdArrayView<value_type, D, Container>::const_iterator
    ConstPeriodicNdArrayView<value_type, D, Container>::cbegin() const
    {
        return const_iterator(
            base_const_iterator(m_container.cbegin(), m_slice, m_offset),
            m_shape);
    }

    template <typename value_type, unsigned D, typename Container>
    template <typename T2>
    bool ConstPeriodicNdArrayView<value_type, D, Container>::operator==(
            T2 const &other) const
    {
        return (shape() == other.shape())
            && std::equal(begin(), end(), other.begin());
    }

    template <typename value_type, unsigned D, typename Container>
    template <typename T2>
    bool ConstPeriodicNdArrayView<value_type, D, Container>::operator!=(
            T2 const &other) const
    {
        return (shape() != other.shape())
            || !std::equal(begin(), end(), other.begin());
    }

    template <typename value_type, unsigned D, typename Container>
    template <typename T2>
    ConstPeriodicNdArrayView<value_type, D, Container> &
    ConstPeriodicNdArrayView<value_type, D, Container>::operator=(
            T2 const &other)
    {
        if (shape() != other.shape())
            throw "shapes of arrays do not match.";
        std::copy(other.cbegin(), other.cend(), begin());
        return *this;
    }
    // }}}2
    // PeriodicNdArrayView {{{2
    template <typename value_type, unsigned D, typename Container>
    PeriodicNdArrayView<value_type, D, Container>::PeriodicNdArrayView(
            Container &data,
            Slice<D> const &slice,
            stride_t<D> const &offset,
            shape_t<D> const &shape)
        : m_container(data)
        , m_slice(slice)
        , m_offset(modulo(offset, slice.shape))
        , m_shape(shape)
    {}

    template <typename value_type, unsigned D, typename Container>
    PeriodicNdArrayView<value_type, D, Container> &
    PeriodicNdArrayView<value_type, D, Container>::set_offset(
            stride_t<D> const &offset)
    {
        m_offset = modulo(offset, this->shape());
        return *this;
    }

    template <typename value_type, unsigned D, typename Container>
    typename PeriodicNdArrayView<value_type, D, Container>::iterator
    PeriodicNdArrayView<value_type, D, Container>::begin()
    {
        return iterator(
            base_iterator(m_container.begin(), m_slice, m_offset),
            m_shape);
    }

    template <typename value_type, unsigned D, typename Container>
    typename PeriodicNdArrayView<value_type, D, Container>::const_iterator
    PeriodicNdArrayView<value_type, D, Container>::cbegin() const
    {
        return const_iterator(
            base_const_iterator(m_container.cbegin(), m_slice, m_offset),
            m_shape);
    }

    template <typename value_type, unsigned D, typename Container>
    template <typename T2>
    bool PeriodicNdArrayView<value_type, D, Container>::operator==(
            T2 const &other) const
    {
        return (shape() == other.shape())
            && std::equal(begin(), end(), other.begin());
    }

    template <typename value_type, unsigned D, typename Container>
    template <typename T2>
    bool PeriodicNdArrayView<value_type, D, Container>::operator!=(
            T2 const &other) const
    {
        return (shape() != other.shape())
            || !std::equal(begin(), end(), other.begin());
    }

    template <typename value_type, unsigned D, typename Container>
    template <typename T2>
    PeriodicNdArrayView<value_type, D, Container> &
    PeriodicNdArrayView<value_type, D, Container>::operator=(
            T2 const &other)
    {
        if (shape() != other.shape())
            throw "shapes of arrays do not match.";
        std::copy(other.cbegin(), other.cend(), begin());
        return *this;
    }
    // }}}2
    // NdArrayImpl {{{2
    // properties {{{3
    template <typename Derived>
    typename NdArrayImpl<Derived>::slice_type const &
    NdArrayImpl<Derived>::slice() const { return m_slice; }
    template <typename Derived>
    size_t NdArrayImpl<Derived>::offset() const { return m_slice.offset; }
    template <typename Derived>
    typename NdArrayImpl<Derived>::shape_type const &
    NdArrayImpl<Derived>::shape() const { return m_slice.shape; }
    template <typename Derived>
    size_t NdArrayImpl<Derived>::size() const { return m_slice.size; }
    template <typename Derived>
    typename NdArrayImpl<Derived>::stride_type const &
    NdArrayImpl<Derived>::stride() const { return m_slice.stride; }
    // }}}3
    // constructors {{{3
    template <typename Derived>
    NdArrayImpl<Derived>::NdArrayImpl() {}

    template <typename Derived>
    NdArrayImpl<Derived>::NdArrayImpl(
            typename NdArrayImpl<Derived>::shape_type const &shape):
        m_slice(shape)
    {}

    template <typename Derived>
    NdArrayImpl<Derived>::NdArrayImpl(
            size_t offset,
            typename NdArrayImpl<Derived>::shape_type const &shape,
            typename NdArrayImpl<Derived>::stride_type const &stride):
        m_slice(offset, shape, stride)
    {}

    template <typename Derived>
    NdArrayImpl<Derived>::NdArrayImpl(
            typename NdArrayImpl<Derived>::slice_type const &slice):
        m_slice(slice)
    {}
    // }}}3
    // iterators {{{3
    template <typename Derived>
    typename array_traits<Derived>::iterator NdArrayImpl<Derived>::begin()
    {
        return iterator(
            std::begin(this->container()) + offset(), shape(), stride());
    }
    template <typename Derived>
    typename array_traits<Derived>::iterator NdArrayImpl<Derived>::end()
    { return iterator(); }

    template <typename Derived>
    typename array_traits<Derived>::const_iterator
    NdArrayImpl<Derived>::cbegin() const
    {
        return const_iterator(
            std::cbegin(this->container()) + offset(), shape(), stride());
    }
    template <typename Derived>
    typename array_traits<Derived>::const_iterator
    NdArrayImpl<Derived>::cend() const { return const_iterator(); }

    template <typename Derived>
    typename array_traits<Derived>::const_iterator
    NdArrayImpl<Derived>::begin() const { return cbegin(); }
    template <typename Derived>
    typename array_traits<Derived>::const_iterator
    NdArrayImpl<Derived>::end() const { return cend(); }
    // }}}3
    // slice methods {{{3
    template <typename Derived>
    template <unsigned axis>
    typename NdArrayImpl<Derived>::View NdArrayImpl<Derived>::reverse()
        { return View(m_slice.reverse<axis>(), this->container()); }
    template <typename Derived>
    template <unsigned axis>
    typename NdArrayImpl<Derived>::ConstView NdArrayImpl<Derived>::reverse() const
        { return View(m_slice.reverse<axis>(), this->container()); }

    template <typename Derived>
    typename NdArrayImpl<Derived>::View NdArrayImpl<Derived>::reverse_all()
        { return View(m_slice.reverse_all(), this->container()); }
    template <typename Derived>
    typename NdArrayImpl<Derived>::ConstView NdArrayImpl<Derived>::reverse_all() const
        { return ConstView(m_slice.reverse_all(), this->container()); }

    template <typename Derived>
    typename NdArrayImpl<Derived>::View NdArrayImpl<Derived>::transpose()
        { return View(m_slice.transpose(), this->container()); }
    template <typename Derived>
    typename NdArrayImpl<Derived>::ConstView NdArrayImpl<Derived>::transpose() const
        { return ConstView(m_slice.transpose(), this->container()); }

    template <typename Derived>
    template <unsigned axis>
    typename NdArrayImpl<Derived>::View
    NdArrayImpl<Derived>::sub(size_t begin, size_t end, size_t step)
        { return View(m_slice.sub<axis>(begin, end, step), this->container()); }
    template <typename Derived>
    template <unsigned axis>
    typename NdArrayImpl<Derived>::ConstView
    NdArrayImpl<Derived>::sub(size_t begin, size_t end, size_t step) const
        { return ConstView(m_slice.sub<axis>(begin, end, step), this->container()); }

    template <typename Derived>
    typename array_traits<Derived>::reduced_view
    NdArrayImpl<Derived>::sel(unsigned axis, size_t idx)
    {
        using return_type = typename array_traits<Derived>::reduced_view;
        return return_type(m_slice.sel(axis, idx), this->container());
    }

    template <typename Derived>
    typename array_traits<Derived>::const_reduced_view
    NdArrayImpl<Derived>::sel(unsigned axis, size_t idx) const
    {
        using return_type = typename array_traits<Derived>::reduced_view;
        return return_type(m_slice.sel(axis, idx), this->container());
    }

    template <typename Derived>
    template <unsigned axis>
    typename array_traits<Derived>::reduced_view
    NdArrayImpl<Derived>::sel(size_t idx)
        { return NdArrayView<value_type, D-1, container_type>(
                m_slice.sel<axis>(idx), this->container()); }
    template <typename Derived>
    template <unsigned axis>
    typename array_traits<Derived>::const_reduced_view
    NdArrayImpl<Derived>::sel(size_t idx) const
        { return ConstNdArrayView<value_type, D-1, container_type>(
                m_slice.sel<axis>(idx), this->container()); }

    template <typename Derived>
    typename array_traits<Derived>::periodic_view
    NdArrayImpl<Derived>::periodic_view(
            typename NdArrayImpl<Derived>::stride_type const &offset,
            typename NdArrayImpl<Derived>::shape_type const &shape)
    {
        return PeriodicNdArrayView<value_type, D, container_type>(
            this->container(), slice(), offset, shape);
    }
    template <typename Derived>
    typename array_traits<Derived>::const_periodic_view
    NdArrayImpl<Derived>::periodic_view(
            typename NdArrayImpl<Derived>::stride_type const &offset,
            typename NdArrayImpl<Derived>::shape_type const &shape) const
    {
        return ConstPeriodicNdArrayView<value_type, D, container_type>(
            this->container(), slice(), offset, shape);
    }
    // }}}3
    // operators {{{3
    template <typename Derived>
    typename array_traits<Derived>::copy_type
    NdArrayImpl<Derived>::copy() const
    {
        NdArray<value_type, D, std::vector<value_type>> result(shape());
        std::copy(cbegin(), cend(), result.begin());
        return result;
    }

    template <typename Derived>
    Derived &NdArrayImpl<Derived>::operator+=(
            typename array_traits<Derived>::value_type value)
    {
        for (auto i = begin(); i != end(); ++i)
            *i += value;
        return static_cast<Derived &>(*this);
    }

    template <typename Derived>
    template <typename T>
    Derived &NdArrayImpl<Derived>::operator*=(T const &other)
    {
        std::transform(begin(), end(), other.cbegin(), begin(),
            [] (auto a, auto b) { return a * b; });
        return static_cast<Derived &>(*this);
    }

    template <typename Derived>
    Derived &NdArrayImpl<Derived>::operator*=(
            typename array_traits<Derived>::value_type value)
    {
        std::transform(begin(), end(), begin(),
            [value] (auto a) { return a * value; });
        return static_cast<Derived &>(*this);
    }

    template <typename Derived>
    Derived &NdArrayImpl<Derived>::operator/=(
            typename array_traits<Derived>::value_type value)
    {
        std::transform(begin(), end(), begin(),
            [value] (auto a) { return a / value; });
        return static_cast<Derived &>(*this);
    }

    template <typename Derived>
    template <typename T>
    Derived &NdArrayImpl<Derived>::operator=(T const &other)
    {
        if (shape() != other.shape())
            throw "shapes of arrays do not match.";
        std::copy(other.cbegin(), other.cend(), begin());
        return static_cast<Derived &>(*this);
    }

    template <typename Derived>
    Derived &NdArrayImpl<Derived>::operator=(
            typename array_traits<Derived>::value_type value)
    {
        std::fill(begin(), end(), value);
        return static_cast<Derived &>(*this);
    }

    template <typename Derived>
    template <typename T>
    bool NdArrayImpl<Derived>::operator==(T const &other) const
    {
        return (shape() == other.shape())
            && std::equal(begin(), end(), other.begin());
    }

    template <typename Derived>
    template <typename T>
    bool NdArrayImpl<Derived>::operator!=(T const &other) const
    {
        return (shape() != other.shape())
            || !std::equal(begin(), end(), other.begin());
    }
    // }}}3
    // }}}2 }}}1

}} // namespace HyperCanny::numeric
