// Boost Lambda Library - is_instance_of.hpp ---------------------

// Copyright (C) 2001 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org

// ---------------------------------------------------------------

#ifndef NDNBOOST_LAMBDA_IS_INSTANCE_OF
#define NDNBOOST_LAMBDA_IS_INSTANCE_OF

#include "ndnboost/config.hpp" // for NDNBOOST_STATIC_CONSTANT
#include "ndnboost/type_traits/conversion_traits.hpp" // for is_convertible
#include "ndnboost/preprocessor/enum_shifted_params.hpp"
#include "ndnboost/preprocessor/repeat_2nd.hpp"

// is_instance_of --------------------------------
// 
// is_instance_of_n<A, B>::value is true, if type A is 
// an instantiation of a template B, or A derives from an instantiation 
// of template B
//
// n is the number of template arguments for B
// 
// Example:
// is_instance_of_2<std::istream, basic_stream>::value == true

// The original implementation was somewhat different, with different versions
// for different compilers. However, there was still a problem
// with gcc.3.0.2 and 3.0.3 compilers, which didn't think regard
// is_instance_of_N<...>::value was a constant.
// John Maddock suggested the way around this problem by building 
// is_instance_of templates using ndnboost::is_convertible.
// Now we only have one version of is_instance_of templates, which delagate
// all the nasty compiler tricks to is_convertible. 

#define NDNBOOST_LAMBDA_CLASS(z, N,A) NDNBOOST_PP_COMMA_IF(N) class
#define NDNBOOST_LAMBDA_CLASS_ARG(z, N,A) NDNBOOST_PP_COMMA_IF(N) class A##N 
#define NDNBOOST_LAMBDA_ARG(z, N,A) NDNBOOST_PP_COMMA_IF(N) A##N 

#define NDNBOOST_LAMBDA_CLASS_LIST(n, NAME) NDNBOOST_PP_REPEAT(n, NDNBOOST_LAMBDA_CLASS, NAME)

#define NDNBOOST_LAMBDA_CLASS_ARG_LIST(n, NAME) NDNBOOST_PP_REPEAT(n, NDNBOOST_LAMBDA_CLASS_ARG, NAME)

#define NDNBOOST_LAMBDA_ARG_LIST(n, NAME) NDNBOOST_PP_REPEAT(n, NDNBOOST_LAMBDA_ARG, NAME)

namespace ndnboost {
namespace lambda {

#define NDNBOOST_LAMBDA_IS_INSTANCE_OF_TEMPLATE(INDEX)                         \
                                                                            \
namespace detail {                                                          \
                                                                            \
template <template<NDNBOOST_LAMBDA_CLASS_LIST(INDEX,T)> class F>               \
struct NDNBOOST_PP_CAT(conversion_tester_,INDEX) {                             \
  template<NDNBOOST_LAMBDA_CLASS_ARG_LIST(INDEX,A)>                            \
  NDNBOOST_PP_CAT(conversion_tester_,INDEX)                                    \
    (const F<NDNBOOST_LAMBDA_ARG_LIST(INDEX,A)>&);                             \
};                                                                          \
                                                                            \
} /* end detail */                                                          \
                                                                            \
template <class From, template <NDNBOOST_LAMBDA_CLASS_LIST(INDEX,T)> class To> \
struct NDNBOOST_PP_CAT(is_instance_of_,INDEX)                                  \
{                                                                           \
 private:                                                                   \
   typedef ::ndnboost::is_convertible<                                         \
     From,                                                                  \
     NDNBOOST_PP_CAT(detail::conversion_tester_,INDEX)<To>                     \
   > helper_type;                                                           \
                                                                            \
public:                                                                     \
  NDNBOOST_STATIC_CONSTANT(bool, value = helper_type::value);                  \
};


#define NDNBOOST_LAMBDA_HELPER(z, N, A) NDNBOOST_LAMBDA_IS_INSTANCE_OF_TEMPLATE( NDNBOOST_PP_INC(N) )

// Generate the traits for 1-4 argument templates

NDNBOOST_PP_REPEAT_2ND(4,NDNBOOST_LAMBDA_HELPER,FOO)

#undef NDNBOOST_LAMBDA_HELPER
#undef NDNBOOST_LAMBDA_IS_INSTANCE_OF_TEMPLATE
#undef NDNBOOST_LAMBDA_CLASS
#undef NDNBOOST_LAMBDA_ARG
#undef NDNBOOST_LAMBDA_CLASS_ARG
#undef NDNBOOST_LAMBDA_CLASS_LIST
#undef NDNBOOST_LAMBDA_ARG_LIST
#undef NDNBOOST_LAMBDA_CLASS_ARG_LIST

} // lambda
} // boost

#endif





