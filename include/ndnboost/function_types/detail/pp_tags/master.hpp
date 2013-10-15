
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

#if !defined(NDNBOOST_FT_PREPROCESSING_MODE) || defined(NDNBOOST_FT_CONFIG_HPP_INCLUDED)
#   error "this file used with two-pass preprocessing, only"
#endif

#include <ndnboost/preprocessor/slot/slot.hpp>
#include <ndnboost/function_types/detail/encoding/def.hpp>

namespace ndnboost { namespace function_types {

typedef detail::property_tag<NDNBOOST_FT_non_variadic,NDNBOOST_FT_variadic_mask> non_variadic;
typedef detail::property_tag<NDNBOOST_FT_variadic,NDNBOOST_FT_variadic_mask>     variadic;
                                                                       
typedef detail::property_tag<0,NDNBOOST_FT_const>                     non_const;
typedef detail::property_tag<NDNBOOST_FT_const,NDNBOOST_FT_const>        const_qualified;
                                                                       
typedef detail::property_tag<0,NDNBOOST_FT_volatile>                  non_volatile;
typedef detail::property_tag<NDNBOOST_FT_volatile,NDNBOOST_FT_volatile>  volatile_qualified; 

typedef detail::property_tag<NDNBOOST_FT_default_cc,NDNBOOST_FT_cc_mask> default_cc;

#define NDNBOOST_PP_VALUE NDNBOOST_FT_const|NDNBOOST_FT_volatile 
#include NDNBOOST_PP_ASSIGN_SLOT(1)

typedef detail::property_tag<0                , NDNBOOST_PP_SLOT(1)> non_cv;
typedef detail::property_tag<NDNBOOST_FT_const   , NDNBOOST_PP_SLOT(1)> const_non_volatile;
typedef detail::property_tag<NDNBOOST_FT_volatile, NDNBOOST_PP_SLOT(1)> volatile_non_const;
typedef detail::property_tag<NDNBOOST_PP_SLOT(1) , NDNBOOST_PP_SLOT(1)> cv_qualified;

namespace detail {

  typedef constant<NDNBOOST_FT_full_mask> full_mask;

  template <bits_t Flags, bits_t CCID> struct encode_bits_impl
  {
    NDNBOOST_STATIC_CONSTANT( bits_t, value = 
      Flags | (NDNBOOST_FT_default_cc * CCID) << 1 );
  };

  template <bits_t Flags, bits_t CCID, std::size_t Arity> 
  struct encode_charr_impl
  {
    NDNBOOST_STATIC_CONSTANT(std::size_t, value = (std::size_t)(1+
      Flags | (NDNBOOST_FT_default_cc * CCID) << 1 | Arity << NDNBOOST_FT_arity_shift
    ));
  };

  template <bits_t Bits> struct decode_bits
  {
    NDNBOOST_STATIC_CONSTANT(bits_t, flags = Bits & NDNBOOST_FT_flags_mask);

    NDNBOOST_STATIC_CONSTANT(bits_t, cc_id = 
      ( (Bits & NDNBOOST_FT_full_mask) / NDNBOOST_FT_default_cc) >> 1 
    );

    NDNBOOST_STATIC_CONSTANT(bits_t, tag_bits = (Bits & NDNBOOST_FT_full_mask));

    NDNBOOST_STATIC_CONSTANT(std::size_t, arity = (std::size_t)
      (Bits >> NDNBOOST_FT_arity_shift) 
    );
  };

  template <bits_t LHS_bits, bits_t LHS_mask, bits_t RHS_bits, bits_t RHS_mask>
  struct tag_ice
  {
    NDNBOOST_STATIC_CONSTANT(bool, match =
      RHS_bits == (LHS_bits & RHS_mask & (RHS_bits |~NDNBOOST_FT_type_mask))
    );

    NDNBOOST_STATIC_CONSTANT(bits_t, combined_bits = 
      (LHS_bits & ~RHS_mask) | RHS_bits
    );

    NDNBOOST_STATIC_CONSTANT(bits_t, combined_mask =
      LHS_mask | RHS_mask
    );

    NDNBOOST_STATIC_CONSTANT(bits_t, extracted_bits =
      LHS_bits & RHS_mask
    );

  };

#define NDNBOOST_FT_mask NDNBOOST_FT_type_mask
  typedef property_tag<NDNBOOST_FT_callable_builtin,NDNBOOST_FT_mask>            callable_builtin_tag;
  typedef property_tag<NDNBOOST_FT_non_member_callable_builtin,NDNBOOST_FT_mask> nonmember_callable_builtin_tag;
  typedef property_tag<NDNBOOST_FT_function,NDNBOOST_FT_mask>       function_tag;
  typedef property_tag<NDNBOOST_FT_reference,NDNBOOST_FT_mask>      reference_tag;
  typedef property_tag<NDNBOOST_FT_pointer,NDNBOOST_FT_mask>        pointer_tag;
  typedef property_tag<NDNBOOST_FT_member_function_pointer,NDNBOOST_FT_mask> member_function_pointer_tag;
  typedef property_tag<NDNBOOST_FT_member_object_pointer,NDNBOOST_FT_mask> member_object_pointer_tag;
  typedef property_tag<NDNBOOST_FT_member_object_pointer_flags,NDNBOOST_FT_full_mask> member_object_pointer_base;
  typedef property_tag<NDNBOOST_FT_member_pointer,NDNBOOST_FT_mask> member_pointer_tag;
#undef  NDNBOOST_FT_mask 

#define NDNBOOST_PP_VALUE NDNBOOST_FT_function|NDNBOOST_FT_non_variadic|NDNBOOST_FT_default_cc
#include NDNBOOST_PP_ASSIGN_SLOT(1)
#define NDNBOOST_PP_VALUE NDNBOOST_FT_type_mask|NDNBOOST_FT_variadic_mask|NDNBOOST_FT_cc_mask
#include NDNBOOST_PP_ASSIGN_SLOT(2)

  typedef property_tag< NDNBOOST_PP_SLOT(1) , NDNBOOST_PP_SLOT(2) > nv_dcc_func;

#define NDNBOOST_PP_VALUE \
    NDNBOOST_FT_member_function_pointer|NDNBOOST_FT_non_variadic|NDNBOOST_FT_default_cc
#include NDNBOOST_PP_ASSIGN_SLOT(1)

  typedef property_tag< NDNBOOST_PP_SLOT(1) , NDNBOOST_PP_SLOT(2) > nv_dcc_mfp;

} // namespace detail

} } // namespace ::ndnboost::function_types

