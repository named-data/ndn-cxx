// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef NDNBOOST_CONCEPT_USAGE_NDNBOOST_DWA2006919_HPP
# define NDNBOOST_CONCEPT_USAGE_NDNBOOST_DWA2006919_HPP

# include <ndnboost/concept/assert.hpp>
# include <ndnboost/detail/workaround.hpp>
# include <ndnboost/concept/detail/backward_compatibility.hpp>

namespace ndnboost { namespace concepts { 

# if NDNBOOST_WORKAROUND(__GNUC__, == 2)

#  define NDNBOOST_CONCEPT_USAGE(model) ~model()

# else 

template <class Model>
struct usage_requirements
{
    ~usage_requirements() { ((Model*)0)->~Model(); }
};

#  if NDNBOOST_WORKAROUND(__GNUC__, <= 3)

#   define NDNBOOST_CONCEPT_USAGE(model)                                    \
      model(); /* at least 2.96 and 3.4.3 both need this :( */           \
      NDNBOOST_CONCEPT_ASSERT((ndnboost::concepts::usage_requirements<model>)); \
      ~model()

#  else

#   define NDNBOOST_CONCEPT_USAGE(model)                                    \
      NDNBOOST_CONCEPT_ASSERT((ndnboost::concepts::usage_requirements<model>)); \
      ~model()

#  endif

# endif 

}} // namespace ndnboost::concepts

#endif // NDNBOOST_CONCEPT_USAGE_NDNBOOST_DWA2006919_HPP
