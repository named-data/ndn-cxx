// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef NDNBOOST_CONCEPT_CHECK_MSVC_NDNBOOST_DWA2006429_HPP
# define NDNBOOST_CONCEPT_CHECK_MSVC_NDNBOOST_DWA2006429_HPP

# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/concept/detail/backward_compatibility.hpp>

# ifdef NDNBOOST_OLD_CONCEPT_SUPPORT
#  include <ndnboost/concept/detail/has_constraints.hpp>
#  include <ndnboost/mpl/if.hpp>
# endif


namespace ndnboost { namespace concepts {


template <class Model>
struct check
{
    virtual void failed(Model* x)
    {
        x->~Model();
    }
};

# ifndef NDNBOOST_NO_PARTIAL_SPECIALIZATION
struct failed {};
template <class Model>
struct check<failed ************ Model::************>
{
    virtual void failed(Model* x)
    {
        x->~Model();
    }
};
# endif

# ifdef NDNBOOST_OLD_CONCEPT_SUPPORT
  
namespace detail
{
  // No need for a virtual function here, since evaluating
  // not_satisfied below will have already instantiated the
  // constraints() member.
  struct constraint {};
}

template <class Model>
struct require
  : mpl::if_c<
        not_satisfied<Model>::value
      , detail::constraint
# ifndef NDNBOOST_NO_PARTIAL_SPECIALIZATION
      , check<Model>
# else
      , check<failed ************ Model::************>
# endif 
        >::type
{};
      
# else
  
template <class Model>
struct require
# ifndef NDNBOOST_NO_PARTIAL_SPECIALIZATION
    : check<Model>
# else
    : check<failed ************ Model::************>
# endif 
{};
  
# endif
    
# if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, == 1310)

//
// The iterator library sees some really strange errors unless we
// do things this way.
//
template <class Model>
struct require<void(*)(Model)>
{
    virtual void failed(Model*)
    {
        require<Model>();
    }
};

# define NDNBOOST_CONCEPT_ASSERT_FN( ModelFnPtr )      \
enum                                                \
{                                                   \
    NDNBOOST_PP_CAT(boost_concept_check,__LINE__) =    \
    sizeof(::ndnboost::concepts::require<ModelFnPtr>)    \
}
  
# else // Not vc-7.1
  
template <class Model>
require<Model>
require_(void(*)(Model));
  
# define NDNBOOST_CONCEPT_ASSERT_FN( ModelFnPtr )          \
enum                                                    \
{                                                       \
    NDNBOOST_PP_CAT(boost_concept_check,__LINE__) =        \
      sizeof(::ndnboost::concepts::require_((ModelFnPtr)0)) \
}
  
# endif
}}

#endif // NDNBOOST_CONCEPT_CHECK_MSVC_NDNBOOST_DWA2006429_HPP
