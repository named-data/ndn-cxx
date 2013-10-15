// Copyright (C) 2003, Fernando Luis Cacciola Carballal.
// Copyright (C) 2007, Tobias Schwinger.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#ifndef NDNBOOST_UTILITY_INPLACE_FACTORY_04APR2007_HPP
#ifndef NDNBOOST_PP_IS_ITERATING

#include <ndnboost/utility/detail/in_place_factory_prefix.hpp>

namespace ndnboost {

class in_place_factory_base {} ;

#define  NDNBOOST_PP_ITERATION_LIMITS (0, NDNBOOST_MAX_INPLACE_FACTORY_ARITY)
#define  NDNBOOST_PP_FILENAME_1 <ndnboost/utility/in_place_factory.hpp>
#include NDNBOOST_PP_ITERATE()

} // namespace ndnboost

#include <ndnboost/utility/detail/in_place_factory_suffix.hpp>

#define NDNBOOST_UTILITY_INPLACE_FACTORY_04APR2007_HPP
#else
#define N NDNBOOST_PP_ITERATION()

#if N
template< NDNBOOST_PP_ENUM_PARAMS(N, class A) >
#endif
class NDNBOOST_PP_CAT(in_place_factory,N)
  : 
  public in_place_factory_base
{
public:

  explicit NDNBOOST_PP_CAT(in_place_factory,N)
      ( NDNBOOST_PP_ENUM_BINARY_PARAMS(N,A,const& a) )
#if N > 0
    : NDNBOOST_PP_ENUM(N, NDNBOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_INIT, _)
#endif
  {}

  template<class T>
  void* apply(void* address
      NDNBOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(T)) const
  {
    return new(address) T( NDNBOOST_PP_ENUM_PARAMS(N, m_a) );
  }

  template<class T>
  void* apply(void* address, std::size_t n
      NDNBOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(T)) const
  {
    for(char* next = address = this->NDNBOOST_NESTED_TEMPLATE apply<T>(address);
        !! --n;)
      this->NDNBOOST_NESTED_TEMPLATE apply<T>(next = next+sizeof(T));
    return address; 
  }

  NDNBOOST_PP_REPEAT(N, NDNBOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_DECL, _)
};

#if N > 0
template< NDNBOOST_PP_ENUM_PARAMS(N, class A) >
inline NDNBOOST_PP_CAT(in_place_factory,N)< NDNBOOST_PP_ENUM_PARAMS(N, A) >
in_place( NDNBOOST_PP_ENUM_BINARY_PARAMS(N, A, const& a) )
{
  return NDNBOOST_PP_CAT(in_place_factory,N)< NDNBOOST_PP_ENUM_PARAMS(N, A) >
      ( NDNBOOST_PP_ENUM_PARAMS(N, a) );
}
#else
inline in_place_factory0 in_place()
{
  return in_place_factory0();
}
#endif

#undef N
#endif
#endif

