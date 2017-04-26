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

#include <random>

using namespace HyperCanny;

TEST (BaseLibrary, StringSplit)
{
    std::vector<std::string> lst;
    string_split("Hello, World!", ' ', std::back_inserter(lst));
    ASSERT_EQ(lst.size(), 2u);
    ASSERT_EQ(lst[0], "Hello,");
    ASSERT_EQ(lst[1], "World!");
}

TEST (BaseLibrary, StringFormat)
{
    ASSERT_EQ(format(1, 2, 3), "123");
    ASSERT_EQ(format("Hello", 42), "Hello42");
}

TEST (BaseLibrary, StringJoin)
{
    std::vector<int> x = { 1, 2, 3 };
    ASSERT_EQ(string_join(x, ", "), "1, 2, 3");
}

TEST (BaseLibrary, FromString)
{
    ASSERT_EQ(3.2f, from_string<float>("3.2"));
    ASSERT_EQ(from_string<int>("not-an-int"), std::nullopt);
}

TEST (BaseLibrary, ToString)
{
    ASSERT_EQ("3.4", to_string(3.4));
    ASSERT_EQ("123456789", to_string(123456789L));
}

TEST (BaseLibrary, ToAndFromStringDouble)
{
    auto noise = std::bind(
        std::normal_distribution<double>(0.0, 1.0), std::mt19937());

    for (unsigned i = 0; i < 10000; ++i)
    {
        double x = noise();
        ASSERT_NEAR(x, *from_string<double>(to_string(x)), 1e-10);
    }
}

TEST (BaseLibrary, FromStringFail)
{
    EXPECT_FALSE(from_string<int>("hello"));
    EXPECT_FALSE(from_string<int>("4.5e23"));
    EXPECT_FALSE(from_string<float>("1q84"));
    EXPECT_FALSE(from_string<int>(""));
    EXPECT_TRUE(from_string<float>("4 \t  "));
}

