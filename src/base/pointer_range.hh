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

/*! \file base/pointer_range.hh
 *  \brief Non-owner view on raw pointer data.
 */

#include <type_traits>

namespace HyperCanny {
    template <typename T>
    class pointer_range
    {
        T *m_data;
        size_t m_size;

        public:
            using iterator = T *;
            using const_iterator = typename std::add_const<T>::type *;
            using pointer = T *;
            using const_pointer = typename std::add_const<T>::type *;
            using reference = T &;
            using const_reference = typename std::add_const<T>::type &;
            using value_type = T;

            pointer_range() {}

            pointer_range(T *data, size_t size)
                : m_data(data)
                , m_size(size)
            {}

            pointer data() { return m_data; }
            const_pointer data() const { return m_data; }
            iterator begin() { return m_data; }
            iterator end() { return m_data + m_size; }
            const_iterator cbegin() const { return m_data; }
            const_iterator cend() const { return m_data + m_size; }
            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); }
            reference operator[](size_t idx) { return m_data[idx]; }
            const_reference operator[](size_t idx) const { return m_data[idx]; }

            void set(T *ptr, size_t size)
            {
                m_data = ptr;
                m_size = size;
            }
    };
}
