#pragma once
#include "types.hh"
#include "support.hh"
#include "slice.hh"
#include <vector>

namespace HyperCanny { namespace numeric
{
    template <typename T, unsigned D>
    class NdIterator: public std::iterator<std::forward_iterator_tag, T>
    {
        T *data_;
        NdRange<D> loc_;

        public:
            NdIterator(T *data, NdRange<D> const &loc):
                data_(data), loc_(loc) {}

            NdIterator &operator++()
                { ++loc_; return *this; }

            T &operator*()
                { return data_[loc_.address()]; }

            bool operator!=(NdIterator const &other) const
                { return loc_ != other.loc_; }
    };

    template <typename T, unsigned D>
    class NdConstIterator: public std::iterator<std::forward_iterator_tag, T>
    {
        T const *data_;
        NdRange<D> loc_;

        public:
            NdConstIterator(T const *data, NdRange<D> const &loc):
                data_(data), loc_(loc) {}

            NdConstIterator &operator++()
                { ++loc_; return *this; }

            T const &operator*() const
                { return data_[loc_.address()]; }

            bool operator!=(NdConstIterator const &other) const
                { return loc_ != other.loc_; }
    };

    template <typename T, unsigned D>
    class NdArrayBase
    {
        Slice<D> slice_;

        public:
            class View;

            using iterator = NdIterator<T, D>;
            using const_iterator = NdConstIterator<T, D>;

            NdArrayBase(shape_t<D> const &shape):
                slice_(shape)
            {}

            NdArrayBase(size_t offset, shape_t<D> const &shape, stride_t<D> const &stride):
                slice_(offset, shape, stride)
            {}

            NdArrayBase(Slice<D> const &slice):
                slice_(slice)
            {}

            virtual ~NdArrayBase() {}

            Slice<D> const &slice() const { return slice_; }
            shape_t<D> const &shape() const { return slice_.shape; }
            size_t size() const { return slice_.size; }

            iterator begin() { return iterator(data(), slice_.begin()); }
            iterator end() { return iterator(data(), slice_.end()); }
            const_iterator cbegin() const { return const_iterator(data(), slice_.begin()); }
            const_iterator cend() const { return const_iterator(data(), slice_.end()); }

            template <unsigned axis>
            View reverse()
                { return View(slice_.reverse<axis>(), data()); }
            View transpose()
                { return View(slice_.transpose(), data()); }
            template <unsigned axis>
            View sub(size_t begin, size_t end, size_t step = 1)
                { return View(slice_.sub<axis>(begin, end, step), data()); }

            virtual T *data() = 0;
            virtual T const *data() const = 0;

            NdArrayBase &operator+=(T value)
            {
                for (auto i = begin(); i != end(); ++i)
                    *i += value;
                return *this;
            }

            NdArrayBase &operator=(NdArrayBase const &other)
            {
                if (shape() != other.shape())
                    throw "shapes of arrays do not match.";
                std::copy(other.cbegin(), other.cend(), begin());
                return *this;
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
        std::vector<T> data_;

        public:
            NdArray(shape_t<D> const &shape):
                NdArrayBase<T, D>(shape),
                data_(calc_size<D>(shape))
            {}

            NdArray(shape_t<D> const &shape, T value):
                NdArrayBase<T, D>(shape),
                data_(calc_size<D>(shape), value)
            {}

            T &operator[](size_t i) { return data_[i]; }
            T const &operator[](size_t i) const { return data_[i]; }

            virtual T *data() { return data_.data(); }
            virtual T const *data() const { return data_.data(); }
    };

    template <typename T, unsigned D>
    class NdArrayBase<T, D>::View: public NdArrayBase<T, D>
    {
        T *data_;

        public:
            View(Slice<D> slice, T *data):
                NdArrayBase<T, D>(slice),
                data_(data)
            {}

            virtual T *data() { return data_; }
            virtual T const *data() const { return data_; }

            View &operator=(View const &other) { NdArrayBase<T, D>::operator=(other); return *this; }
            View &operator=(NdArray<T, D> const &other) { NdArrayBase<T, D>::operator=(other); return *this; }
    };
}}

