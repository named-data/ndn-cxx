// Copyright David Abrahams 2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef NDNBOOST_CONCEPT_BACKWARD_COMPATIBILITY_NDNBOOST_DWA200968_HPP
# define NDNBOOST_CONCEPT_BACKWARD_COMPATIBILITY_NDNBOOST_DWA200968_HPP

namespace ndnboost
{
  namespace concepts {}

# if defined(NDNBOOST_HAS_CONCEPTS) && !defined(NDNBOOST_CONCEPT_NO_BACKWARD_KEYWORD)
  namespace concept = concepts;
# endif 
} // namespace ndnboost::concept

#endif // NDNBOOST_CONCEPT_BACKWARD_COMPATIBILITY_NDNBOOST_DWA200968_HPP
