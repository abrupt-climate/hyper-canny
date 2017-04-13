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
