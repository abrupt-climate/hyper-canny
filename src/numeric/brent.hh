#pragma once

namespace eScatter { namespace numeric {
    /*! \brief Root finding by Brent's method.
     *
     * Brent's method employs Newton-Raphson method to converge to a root,
     * but falls back to a simpler but more robust bi-section method when
     * the guess by Newton method falls outside the known bounds.
     *
     * \param f The function to solve for.
     * \param df Derivative of f.
     * \param x1 Left bound of interval to search.
     * \param x2 Right bound of interval to search.
     * \param y0 Value function f should atain.
     * \param epsilon Absolute error.
     *
     * Newton's method uses the derivative of f to guess the next value:
     *      \f[ x_{i+1} = x_{i} - \Delta y / f'(x_{i}) \f]
     * If the function is not so nice to us -- it may be rapidly changing,
     * but more importantly, we could be mucking about in the exponential tail
     * of a distribution -- this method will behave bad, so we back it up
     * with a bisection method, which always converges.
     */
    template <typename real_t, typename Fn1, typename Fn2>
    real_t find_root_brent(Fn1 f, Fn2 df, real_t x1, real_t x2, real_t y0, real_t epsilon)
    {
        real_t b = (x1 + x2) / 2,           // initial guess
               w = f(x1) - y0,
               y = f(b) - y0,
               a = (w * y < 0 ? x1 : x2);   // set other bound

        while (epsilon < fabs(y))
        {
            // try Newton method
            real_t c = b - y / df(b);

            // keep if within bounds otherwise bisection
            c = ((b - c) * (a - c) < 0 ? c : (a + b) / 2);
               
            // update bounds 
            w = f(c) - y0;
            a = (w * y < 0 ? b : a);
            b = c;
            y = w;
        }

        return b;
    }
}}

