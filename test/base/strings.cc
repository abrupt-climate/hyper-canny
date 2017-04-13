#include "base.hh"
#include <gtest/gtest.h>

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
