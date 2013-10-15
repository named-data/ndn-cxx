// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef NDNBOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP
# define NDNBOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP

# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/concept/detail/backward_compatibility.hpp>

# ifdef NDNBOOST_OLD_CONCEPT_SUPPORT
#  include <ndnboost/concept/detail/has_constraints.hpp>
#  include <ndnboost/mpl/if.hpp>
# endif

// This implementation works on Comeau and GCC, all the way back to
// 2.95
namespace ndnboost { namespace concepts {

template <class ModelFn>
struct requirement_;

namespace detail
{
  template <void(*)()> struct instantiate {};
}

template <class Model>
struct requirement
{
    static void failed() { ((Model*)0)->~Model(); }
};

struct failed {};

template <class Model>
struct requirement<failed ************ Model::************>
{
    static void failed() { ((Model*)0)->~Model(); }
};

# ifdef NDNBOOST_OLD_CONCEPT_SUPPORT

template <class Model>
struct constraint
{
    static void failed() { ((Model*)0)->constraints(); }
};
  
template <class Model>
struct requirement_<void(*)(Model)>
  : mpl::if_<
        concepts::not_satisfied<Model>
      , constraint<Model>
      , requirement<failed ************ Model::************>
    >::type
{};
  
# else

// For GCC-2.x, these can't have exactly the same name
template <class Model>
struct requirement_<void(*)(Model)>
    : requirement<failed ************ Model::************>
{};
  
# endif

#  define NDNBOOST_CONCEPT_ASSERT_FN( ModelFnPtr )             \
    typedef ::ndnboost::concepts::detail::instantiate<          \
    &::ndnboost::concepts::requirement_<ModelFnPtr>::failed>    \
      NDNBOOST_PP_CAT(boost_concept_check,__LINE__)

}}

#endif // NDNBOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP
