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
#include <gtest/gtest.h>

using namespace HyperCanny;

TEST (BaseLibrary, Exceptions)
{
    try
    {
        throw Exception("error test");
    } catch (Exception const &e)
    {
        ASSERT_EQ(std::string(e.what()), "error test");
    }
}

TEST (BaseLibrary, HeadsAndTails)
{
    std::vector<int> a = { 1, 2, 3, 4, 5 };

    EXPECT_EQ(head(a), 1);
    EXPECT_EQ(head(tail(a)), 2);

    auto b = tail(tail(a));
    std::vector<int> c = { 3, 4, 5 };
    EXPECT_TRUE(std::equal(b.begin(), b.end(), c.begin()));
    EXPECT_TRUE(std::equal(b.cbegin(), b.cend(), c.begin()));
}

