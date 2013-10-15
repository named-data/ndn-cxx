
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

// no include guards, this file is guarded externally

#ifdef __WAVE__
// this file has been generated from the master.hpp file in the same directory
#   pragma wave option(preserve: 0)
#endif

#if !defined(NDNBOOST_PP_VALUE)
#   include <ndnboost/preprocessor/slot/slot.hpp>
#   include <ndnboost/preprocessor/iteration/self.hpp>

#   include <ndnboost/function_types/detail/encoding/def.hpp>
#   include <ndnboost/function_types/detail/encoding/aliases_def.hpp>

namespace ndnboost { namespace function_types {

namespace detail
{
  template<class Tag, class RefTag> struct selector_bits
  {
#   define  NDNBOOST_PP_VALUE non_member|member|non_variadic|variadic
#   include NDNBOOST_PP_ASSIGN_SLOT(1)

    NDNBOOST_STATIC_CONSTANT(bits_t, value = (
        (::ndnboost::function_types::detail::bits<Tag>::value & NDNBOOST_FT_default_cc) 
      | (::ndnboost::function_types::detail::bits<RefTag>::value & NDNBOOST_PP_SLOT(1))
    ));
  };

  template<bits_t SelectorBits> struct default_cc_tag; 
  
  template<class Tag, class RefTag> struct retag_default_cc
    : detail::compound_tag
      < Tag, detail::default_cc_tag< 
          ::ndnboost::function_types::detail::selector_bits<Tag,RefTag>::value > >
  { };

  template<bits_t SelectorBits> struct default_cc_tag
  {
    typedef null_tag::bits bits;
    typedef null_tag::mask mask;
  };

  class test_class;
  typedef constant<NDNBOOST_FT_cc_mask> cc_mask_constant;

#   define NDNBOOST_FT_self \
      <ndnboost/function_types/detail/pp_retag_default_cc/master.hpp>

#   define  default_cc_ NDNBOOST_FT_default_cc

#   define  NDNBOOST_PP_VALUE default_cc_|non_member|non_variadic
#   define  NDNBOOST_FT_tester void (*tester)()
#   define  NDNBOOST_PP_INDIRECT_SELF NDNBOOST_FT_self
#   include NDNBOOST_PP_INCLUDE_SELF()

#   define  NDNBOOST_PP_VALUE default_cc_|non_member|variadic
#   define  NDNBOOST_FT_tester void (*tester)(...)
#   define  NDNBOOST_PP_INDIRECT_SELF NDNBOOST_FT_self
#   include NDNBOOST_PP_INCLUDE_SELF()

#   define  NDNBOOST_PP_VALUE default_cc_|member|non_variadic
#   define  NDNBOOST_FT_tester void (test_class::*tester)()
#   define  NDNBOOST_PP_INDIRECT_SELF NDNBOOST_FT_self
#   include NDNBOOST_PP_INCLUDE_SELF()

#   define  NDNBOOST_PP_VALUE default_cc_|member|variadic
#   define  NDNBOOST_FT_tester void (test_class::*tester)(...)
#   define  NDNBOOST_PP_INDIRECT_SELF NDNBOOST_FT_self
#   include NDNBOOST_PP_INCLUDE_SELF()

#   undef   default_cc_

#   undef NDNBOOST_FT_self

} } } // namespace ::ndnboost::function_types::detail

#   include <ndnboost/function_types/detail/encoding/aliases_undef.hpp>
#   include <ndnboost/function_types/detail/encoding/undef.hpp>

#else // if defined(NDNBOOST_PP_VALUE)

#   include NDNBOOST_PP_ASSIGN_SLOT(1)

  template<> struct default_cc_tag<NDNBOOST_PP_SLOT(1)> 
  {
    typedef NDNBOOST_FT_tester;
    typedef mpl::bitand_<components<tester>::bits,cc_mask_constant> bits;
    typedef cc_mask_constant mask;
  };

#   undef NDNBOOST_FT_tester

#endif

