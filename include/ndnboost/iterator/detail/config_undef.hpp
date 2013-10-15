// (C) Copyright Thomas Witt    2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// no include guard multiple inclusion intended

//
// This is a temporary workaround until the bulk of this is
// available in boost config.
// 23/02/03 thw
//

#undef NDNBOOST_NO_IS_CONVERTIBLE
#undef NDNBOOST_NO_IS_CONVERTIBLE_TEMPLATE
#undef NDNBOOST_NO_STRICT_ITERATOR_INTEROPERABILITY
#undef NDNBOOST_ARG_DEPENDENT_TYPENAME
#undef NDNBOOST_NO_LVALUE_RETURN_DETECTION
#undef NDNBOOST_NO_ONE_WAY_ITERATOR_INTEROP

#ifdef NDNBOOST_ITERATOR_CONFIG_DEF
# undef NDNBOOST_ITERATOR_CONFIG_DEF
#else
# error missing or nested #include config_def
#endif 
