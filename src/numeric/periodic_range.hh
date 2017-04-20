#pragma once

/*! \file numeric/periodic_slice.hh
 *  \brief Slice an array periodically
 */

#include "support.hh"

namespace HyperCanny {
namespace numeric {
    template <unsigned D>
    class PeriodicRange
    {
        NdRange<D> m_range;
        shape_t<D> m_shape, m_index;

        public:
            PeriodicRange(
                    NdRange<D> const &range,
                    shape_t<D> const &shape)
                : m_range(range)
                , m_shape(shape)
            {
                m_index.fill(0);
            }

            PeriodicRange()
                : m_range()
            {}

            PeriodicRange &operator++()
            {
                ++m_index[0];
                m_range.cycle(0);

                unsigned i = 0;
                while (m_index[i] == m_shape[i])
                {
                    if (++i == D)
                    {
                        m_range.set_end()
                        break;
                    }

                    m_index[i-1] = 0;
                    ++m_index[i];
                    m_range.cycle(i);
                }

                return *this;
            }

            NdRange<D> const &operator*() const
            {
                return m_range;
            }

            uint32_t operator[](size_t i) const
            {
                return m_range[i];
            }

            bool operator!=(PeriodicRange const &other) const
            {
                return m_range != other.m_range;
            }
    };
}} // HyperCanny::numeric
