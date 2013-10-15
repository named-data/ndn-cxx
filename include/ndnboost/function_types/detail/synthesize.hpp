
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_DETAIL_SYNTHESIZE_HPP_INCLUDED
#define NDNBOOST_FT_DETAIL_SYNTHESIZE_HPP_INCLUDED

#include <cstddef>

#include <ndnboost/mpl/at.hpp>
#include <ndnboost/mpl/size.hpp>

#include <ndnboost/function_types/config/config.hpp>
#include <ndnboost/function_types/property_tags.hpp>
#include <ndnboost/function_types/detail/cv_traits.hpp>
#include <ndnboost/function_types/detail/retag_default_cc.hpp>

namespace ndnboost { namespace function_types { namespace detail {

template<bits_t Flags, bits_t CCID, std::size_t Size>
struct synthesize_impl_o
{
  template<typename Seq> struct synthesize_impl_i { };
};

template<typename Seq, bits_t Bits>
struct synthesize_impl
  : detail::synthesize_impl_o
    < ::ndnboost::function_types::detail::decode_bits<Bits>::flags
    , ::ndnboost::function_types::detail::decode_bits<Bits>::cc_id
    , ::ndnboost::mpl::size<Seq>::value
    >
    ::template synthesize_impl_i<Seq>
{ };

template<typename Seq, typename Tag>
struct synthesize_func
  : detail::synthesize_impl
    < Seq
    , ::ndnboost::function_types::detail::bits
      < detail::retag_default_cc
        < function_types::tag<nv_dcc_func, Tag> > 
      >::value 
    >
{ };

template<typename Seq, typename Tag>
struct synthesize_mfp
  : detail::synthesize_impl
    < Seq 
    , ::ndnboost::function_types::detail::bits
      < detail::retag_default_cc
        < function_types::tag
          < typename detail::cv_traits< typename mpl::at_c<Seq,1>::type >::tag
          , nv_dcc_mfp, Tag
        > >
      >::value
    >
{ };

template<typename S, typename R = typename mpl::at_c<S,0>::type,
    typename C = typename mpl::at_c<S,1>::type>
struct synthesize_mop
{
  typedef R C::* type;
};

#define NDNBOOST_FT_variations NDNBOOST_FT_function|NDNBOOST_FT_member_pointer
#define NDNBOOST_FT_al_path ndnboost/function_types/detail/synthesize_impl
#include <ndnboost/function_types/detail/pp_loop.hpp>

} } } // namespace ::ndnboost::function_types::detail

#endif

