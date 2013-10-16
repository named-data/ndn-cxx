// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef NDNBOOST_CONCEPT_DETAIL_BORLAND_NDNBOOST_DWA2006429_HPP
# define NDNBOOST_CONCEPT_DETAIL_BORLAND_NDNBOOST_DWA2006429_HPP

# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/concept/detail/backward_compatibility.hpp>

namespace ndnboost { namespace concepts {

template <class ModelFnPtr>
struct require;

template <class Model>
struct require<void(*)(Model)>
{
    enum { instantiate = sizeof((((Model*)0)->~Model()), 3) };
};

#  define NDNBOOST_CONCEPT_ASSERT_FN( ModelFnPtr )         \
  enum                                                  \
  {                                                     \
      NDNBOOST_PP_CAT(boost_concept_check,__LINE__) =      \
      ndnboost::concepts::require<ModelFnPtr>::instantiate  \
  }

}} // namespace ndnboost::concept

#endif // NDNBOOST_CONCEPT_DETAIL_BORLAND_NDNBOOST_DWA2006429_HPP
