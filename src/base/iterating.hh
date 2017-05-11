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

/*! \file base/iterating.hh
 *  \brief Helper function to do generic iterations.
 */

#include <utility>
#include <functional>

namespace HyperCanny
{
    /*! \brief Contain begin and end iterators of any type.
     */
    template <typename Iterator>
    class Span: public std::pair<Iterator, Iterator>
    {
        using Base = std::pair<Iterator, Iterator>;

        public:
            using value_type = typename Iterator::value_type;
            using iterator = Iterator;
            using const_iterator = Iterator;

            using Base::Base;

            Iterator begin() const { return this->first; }
            Iterator end() const { return this->second; }
            Iterator cbegin() const { return begin(); }
            Iterator cend() const { return end(); }
    };

    /*! \brief get the head of a range
     * \param r a ranged object (having begin and end methods)
     * \return reference to first element of the range.
     */
    template <typename R>
    auto head(R const &r) -> decltype(*r.begin())
    {
        return *r.begin();
    }

    template <typename T>
    class Tail
    {
        public:
            typedef typename T::value_type value_type;
            typedef typename T::const_iterator const_iterator;

            explicit Tail(T const &a):
                b(a.begin()), e(a.end())
            {
                ++b;
            }

            const_iterator begin() const { return b; }
            const_iterator end() const { return e; }
            const_iterator cbegin() const { return b; }
            const_iterator cend() const { return e; }

        private:
            const_iterator b, e;
    };

    /*! \brief get the tail of a range
     * \param r a range object (having begin and end methods).
     * \return a Tail object combining begin()++ and end().
     */
    template <typename T>
    Tail<T> tail(T const &a) { return Tail<T>(a); }

    template <typename Iterator, typename To>
    class SelectIterator: public Iterator
    {
        using From = typename Iterator::value_type;
        std::function<To (From const &)> f;

        public:
            using value_type = To;
            using reference = To;
            using const_reference = To;

            template <typename Fn>
            SelectIterator(Fn _f, Iterator const &_i)
                : Iterator(_i)
                , f(_f)
            {}

            const_reference operator*() const
            {
                return f(Iterator::operator*());
            }
    };

    template <typename Fn, typename Range>
    auto select(Fn f, Range const &r)
        -> Span<SelectIterator<typename Range::const_iterator, decltype(f(*r.begin()))>>
    {
        using iterator =
            SelectIterator<typename Range::const_iterator, decltype(f(*r.begin()))>;

        return Span<iterator>(
            iterator(f, r.begin()),
            iterator(f, r.end()));
    }
}
