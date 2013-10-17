/* boost random/detail/integer_log2.hpp header file
 *
 * Copyright Steven Watanabe 2011
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id: integer_log2.hpp 83381 2013-03-09 22:55:05Z eric_niebler $
 *
 */

#ifndef NDNBOOST_RANDOM_DETAIL_INTEGER_LOG2_HPP
#define NDNBOOST_RANDOM_DETAIL_INTEGER_LOG2_HPP

#include <ndnboost/config.hpp>
#include <ndnboost/limits.hpp>
#include <ndnboost/pending/integer_log2.hpp>

namespace ndnboost {
namespace random {
namespace detail {

#if !defined(NDNBOOST_NO_CXX11_CONSTEXPR)
#define NDNBOOST_RANDOM_DETAIL_CONSTEXPR constexpr
#elif defined(NDNBOOST_MSVC)
#define NDNBOOST_RANDOM_DETAIL_CONSTEXPR __forceinline
#elif defined(__GNUC__) && __GNUC__ >= 4
#define NDNBOOST_RANDOM_DETAIL_CONSTEXPR inline __attribute__((const)) __attribute__((always_inline))
#else
#define NDNBOOST_RANDOM_DETAIL_CONSTEXPR inline
#endif

template<int Shift>
struct integer_log2_impl
{
#if defined(NDNBOOST_NO_CXX11_CONSTEXPR)
    template<class T>
    NDNBOOST_RANDOM_DETAIL_CONSTEXPR static int apply(T t, int accum)
    {
        int update = ((t >> Shift) != 0) * Shift;
        return integer_log2_impl<Shift / 2>::apply(t >> update, accum + update);
    }
#else
    template<class T>
    NDNBOOST_RANDOM_DETAIL_CONSTEXPR static int apply2(T t, int accum, int update)
    {
        return integer_log2_impl<Shift / 2>::apply(t >> update, accum + update);
    }

    template<class T>
    NDNBOOST_RANDOM_DETAIL_CONSTEXPR static int apply(T t, int accum)
    {
        return apply2(t, accum, ((t >> Shift) != 0) * Shift);
    }
#endif
};

template<>
struct integer_log2_impl<1>
{
    template<class T>
    NDNBOOST_RANDOM_DETAIL_CONSTEXPR static int apply(T t, int accum)
    {
        return int(t >> 1) + accum;
    }
};

template<class T>
NDNBOOST_RANDOM_DETAIL_CONSTEXPR int integer_log2(T t)
{
    return integer_log2_impl<
        ::ndnboost::detail::max_pow2_less<
            ::std::numeric_limits<T>::digits, 4
        >::value
    >::apply(t, 0);
}

} // namespace detail
} // namespace random
} // namespace ndnboost

#endif // NDNBOOST_RANDOM_DETAIL_INTEGER_LOG2_HPP
