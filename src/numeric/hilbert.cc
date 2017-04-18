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
#include "hilbert.hh"

#include <iostream>
#include <iomanip>
#include <vector>

using namespace HyperCanny::numeric;

NdArray<size_t, 2> HyperCanny::numeric::hilbert_array(unsigned n)
{
    if (n == 0)
        return NdArray<size_t, 2>({1, 1}, 0);

    auto p = hilbert_array(n - 1);
    uint32_t N = p.shape()[0],
             M = p.size();

    NdArray<size_t, 2> q({2*N, 2*N}, 0);

    q.sub<0>(0,   N).sub<1>(0,   N) = p.transpose();

    p += M;
    q.sub<0>(0,   N).sub<1>(N, 2*N) = p;

    p += M;
    q.sub<0>(N, 2*N).sub<1>(N, 2*N) = p;

    p += M;
    q.sub<0>(N, 2*N).sub<1>(0,   N) = p.reverse<0>().reverse<1>().transpose();

    return q;
}


std::vector<size_t> HyperCanny::numeric::hilbert_walk(unsigned n)
{
    auto a = hilbert_array(n);
    std::vector<size_t> result(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        result[a[i]] = i;
    return result;
}
