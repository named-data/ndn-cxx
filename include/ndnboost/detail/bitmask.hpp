//  ndnboost/detail/bitmask.hpp  ------------------------------------------------//

//  Copyright Beman Dawes 2006

//  Distributed under the Boost Software License, Version 1.0
//  http://www.boost.org/LICENSE_1_0.txt

//  Usage:  enum foo { a=1, b=2, c=4 };
//          NDNBOOST_BITMASK( foo );
//
//          void f( foo arg );
//          ...
//          f( a | c );

#ifndef NDNBOOST_BITMASK_HPP
#define NDNBOOST_BITMASK_HPP

#include <ndnboost/cstdint.hpp>

#define NDNBOOST_BITMASK(Bitmask)                                            \
                                                                          \
  inline Bitmask operator| (Bitmask x , Bitmask y )                       \
  { return static_cast<Bitmask>( static_cast<ndnboost::int_least32_t>(x)     \
      | static_cast<ndnboost::int_least32_t>(y)); }                          \
                                                                          \
  inline Bitmask operator& (Bitmask x , Bitmask y )                       \
  { return static_cast<Bitmask>( static_cast<ndnboost::int_least32_t>(x)     \
      & static_cast<ndnboost::int_least32_t>(y)); }                          \
                                                                          \
  inline Bitmask operator^ (Bitmask x , Bitmask y )                       \
  { return static_cast<Bitmask>( static_cast<ndnboost::int_least32_t>(x)     \
      ^ static_cast<ndnboost::int_least32_t>(y)); }                          \
                                                                          \
  inline Bitmask operator~ (Bitmask x )                                   \
  { return static_cast<Bitmask>(~static_cast<ndnboost::int_least32_t>(x)); } \
                                                                          \
  inline Bitmask & operator&=(Bitmask & x , Bitmask y)                    \
  { x = x & y ; return x ; }                                              \
                                                                          \
  inline Bitmask & operator|=(Bitmask & x , Bitmask y)                    \
  { x = x | y ; return x ; }                                              \
                                                                          \
  inline Bitmask & operator^=(Bitmask & x , Bitmask y)                    \
  { x = x ^ y ; return x ; }                                              

#endif // NDNBOOST_BITMASK_HPP

