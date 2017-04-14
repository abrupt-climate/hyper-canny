#pragma once

namespace HyperCanny
{
    /* generic string utilities ========================================== */
    /*! \brief get the head of a range
     * \param r a ranged object (having begin and end methods)
     * \return reference to first element of the range.
     */
    template <typename R>
    auto head(R const &r) -> decltype(*r.begin())
    {
        return *r.begin();
    }

    template <typename T>
    class Tail
    {
        public:
            typedef typename T::value_type value_type;
            typedef typename T::const_iterator const_iterator;

            Tail(T const &a):
                b(a.begin()), e(a.end())
            {
                ++b;
            }

            const_iterator begin() const { return b; }
            const_iterator end() const { return e; }
            const_iterator cbegin() const { return b; }
            const_iterator cend() const { return e; }

        private:
            const_iterator b, e;
    };

    /*! \brief get the tail of a range
     * \param r a range object (having begin and end methods).
     * \return a Tail object combining begin()++ and end().
     */
    template <typename T>
    Tail<T> tail(T const &a) { return Tail<T>(a); }
}
