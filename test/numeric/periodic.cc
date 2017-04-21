#include "base.hh"
#include "numeric/ndarray.hh"
#include "numeric/nditerator.hh"
#include "numeric/slice.hh"

#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

using namespace HyperCanny;

class NumberIterator: public std::iterator<std::forward_iterator_tag, int>
{
    int i;

    public:
        NumberIterator(): i(0) {}
        NumberIterator(int i_): i(i_) {}

        NumberIterator &operator++() { ++i; return *this; }
        NumberIterator &operator+=(int j) { i += j; return *this; }
        NumberIterator &operator--() { --i; return *this; }
        NumberIterator &operator-=(int j) { i -= j; return *this; }
        NumberIterator operator+(int j) const { return NumberIterator(*this) += j; }

        int const &operator*() const { return i; }
        bool operator!=(NumberIterator const &other) const { return i != other.i; }
};

template <typename T, unsigned long D>
std::ostream &operator<<(std::ostream &out, std::array<T,D> const &x)
{
    return out << "(" << string_join(x, ", ") << ")";
}

TEST (NdArray, PeriodicIterator)
{
    using numeric::ConstNdIterator;
    using iterator = ConstNdIterator<NumberIterator,2>;
    using periodic_iterator = numeric::PeriodicIterator<iterator>;

    numeric::Slice<2> s({3, 3});
    std::vector<int> result,
                     expected { 4, 5, 3, 7, 8, 6, 1, 2, 0 };
    for (periodic_iterator i(iterator(NumberIterator(), s, {1, 1}), {3, 3}); i != periodic_iterator(); ++i)
        result.push_back(*i);
    ASSERT_EQ(result, expected);
}

TEST (NdArray, PeriodicRange)
{
    using numeric::NdArray;
    using numeric::PeriodicNdArrayView;

    NdArray<int,2> a1({2, 2}, {1, 2, 3, 4}),
                   b1({3, 3}, {4, 3, 4, 2, 1, 2, 4, 3, 4});

    PeriodicNdArrayView<int,2> r1(a1, {1, 1}, {3, 3});
    ASSERT_EQ(r1, b1);
}

