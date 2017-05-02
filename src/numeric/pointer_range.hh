#pragma once

/*! \file numeric/pointer_range.hh
 *  \brief Non-owner view on raw pointer data.
 */

namespace HyperCanny {
namespace numeric
{
    template <typename real_t>
    class ConstPointerRange
    {
        real_t const *m_data;
        size_t m_size;

        public:
            using reference = real_t const &;
            using const_reference = real_t const &;
            using iterator = real_t const *;
            using const_iterator = real_t const *;

            ConstPointerRange() {}

            ConstPointerRange(real_t const *data, size_t size)
                : m_data(data)
                , m_size(size)
            {}

            real_t const *data() const { return m_data; }
            real_t const *cbegin() const { return m_data; }
            real_t const *cend() const { return m_data + m_size; }
            real_t const *begin() const { return cbegin(); }
            real_t const *end() const { return cend(); }

            void set(real_t const *ptr, size_t size)
            {
                m_data = ptr;
                m_size = size;
            }

            reference operator[](size_t idx) { return m_data[idx]; }
            const_reference operator[](size_t idx) const { return m_data[idx]; }
    };

    template <typename real_t>
    class PointerRange
    {
        real_t *m_data;
        size_t m_size;

        public:
            using iterator = real_t *;
            using const_iterator = real_t const *;
            using reference = real_t &;
            using const_reference = real_t const &;

            PointerRange() {}

            PointerRange(real_t *data, size_t size)
                : m_data(data)
                , m_size(size)
            {}

            real_t *data() { return m_data; }
            real_t const *data() const { return m_data; }
            real_t *begin() { return m_data; }
            real_t *end() { return m_data + m_size; }
            real_t const *cbegin() const { return m_data; }
            real_t const *cend() const { return m_data + m_size; }
            real_t const *begin() const { return cbegin(); }
            real_t const *end() const { return cend(); }

            void set(real_t *ptr, size_t size)
            {
                m_data = ptr;
                m_size = size;
            }

            reference operator[](size_t idx) { return m_data[idx]; }
            const_reference operator[](size_t idx) const { return m_data[idx]; }
    };
}}
