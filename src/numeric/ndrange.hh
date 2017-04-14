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
#include "types.hh"
#include "support.hh"

#include <limits>

namespace HyperCanny { namespace numeric
{
    constexpr size_t range_end = std::numeric_limits<size_t>::max();

    template <unsigned D>
    class NdRange //: public Expression<shape_t<D>, NdRange<D>>
    {
        size_t address_;
        shape_t<D> index_, shape_;
        stride_t<D> semi_stride_;

        public:
            size_t address() const { return address_; }
            uint32_t index(unsigned i) const { return index_[i]; }

            /*! default constructor is initialised to an index of 0xffffffff,
             * which signifies the end of a range.
             */
            NdRange(): address_(range_end) {}

            NdRange(shape_t<D> const &shape):
                address_(0), shape_(shape)
            {
                index_.fill(0);
                calc_semi_stride(calc_stride(shape_));
            }

            NdRange(size_t offset,
                    shape_t<D> const &shape,
                    stride_t<D> const &stride):
                address_(offset), shape_(shape)
            {
                index_.fill(0);
                calc_semi_stride(stride);
            }

            NdRange &operator++()
            {
                ++index_[0];
                address_ += semi_stride_[0];

                unsigned i = 0;
                while (index_[i] == shape_[i])
                {
                    if (++i == D)
                    {
                        address_ = range_end;
                        break;
                    }

                    index_[i-1] = 0;
                    ++index_[i];
                    address_ += semi_stride_[i];
                }

                return *this;
            }

            NdRange const &operator*() const
            {
                return *this;
            }

            uint32_t operator[](size_t i) const
            {
                return index_[i];
            }

            bool operator!=(NdRange const &other) const
            {
                return address_ != other.address_;
            }

        private:
            void calc_semi_stride(stride_t<D> const &stride)
            {
                semi_stride_[0] = stride[0];

                for (unsigned i = 1; i < shape_.size(); ++i)
                {
                    semi_stride_[i] = stride[i] - shape_[i-1] * stride[i-1];
                }
            }
    };
}} // namespace numeric
