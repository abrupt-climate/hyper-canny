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
#include "base.hh"
#include "numeric/ndarray.hh"
#include "numeric/nditerator.hh"
#include "numeric/slice.hh"
#include "numeric/counter.hh"

#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

using namespace HyperCanny;

TEST (NdArray, PeriodicIterator)
{
    using numeric::NdCounter;
    using numeric::ConstNdIterator;
    using iterator = NdCounter<2>;
    using periodic_iterator = numeric::PeriodicIterator<iterator>;

    numeric::Slice<2> s({3, 3});
    std::vector<int> result,
                     expected { 4, 5, 3, 7, 8, 6, 1, 2, 0 };
    for (periodic_iterator i(iterator(s, {1, 1}), {3, 3}); i != periodic_iterator(); ++i)
        result.push_back(*i);
    ASSERT_EQ(result, expected);
}

TEST (NdArray, PeriodicRange)
{
    using numeric::NdArray;

    NdArray<int,2> a1({2, 2}, {1, 2, 3, 4}),
                   b1({3, 3}, {4, 3, 4, 2, 1, 2, 4, 3, 4});

    auto r1 = a1.periodic_view({1, 1}, {3, 3});
    ASSERT_EQ(r1, b1);
}
