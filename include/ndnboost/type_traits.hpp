//  (C) Copyright John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

//  See boost/type_traits/*.hpp for full copyright notices.

#ifndef BOOST_TYPE_TRAITS_HPP
#define BOOST_TYPE_TRAITS_HPP

#include "ndnboost/type_traits/add_const.hpp"
#include "ndnboost/type_traits/add_cv.hpp"
#include "ndnboost/type_traits/add_lvalue_reference.hpp"
#include "ndnboost/type_traits/add_pointer.hpp"
#include "ndnboost/type_traits/add_reference.hpp"
#include "ndnboost/type_traits/add_rvalue_reference.hpp"
#include "ndnboost/type_traits/add_volatile.hpp"
#include "ndnboost/type_traits/aligned_storage.hpp"
#include "ndnboost/type_traits/alignment_of.hpp"
#include "ndnboost/type_traits/common_type.hpp"
#include "ndnboost/type_traits/conditional.hpp"
#include "ndnboost/type_traits/decay.hpp"
#include "ndnboost/type_traits/extent.hpp"
#include "ndnboost/type_traits/floating_point_promotion.hpp"
#include "ndnboost/type_traits/function_traits.hpp"
#if !defined(__BORLANDC__) && !defined(__CUDACC__)
#include "ndnboost/type_traits/has_new_operator.hpp"
#endif
#include "ndnboost/type_traits/has_nothrow_assign.hpp"
#include "ndnboost/type_traits/has_nothrow_constructor.hpp"
#include "ndnboost/type_traits/has_nothrow_copy.hpp"
#include "ndnboost/type_traits/has_nothrow_destructor.hpp"
#include <ndnboost/type_traits/has_operator.hpp>
#include "ndnboost/type_traits/has_trivial_assign.hpp"
#include "ndnboost/type_traits/has_trivial_constructor.hpp"
#include "ndnboost/type_traits/has_trivial_copy.hpp"
#include "ndnboost/type_traits/has_trivial_destructor.hpp"
#include "ndnboost/type_traits/has_trivial_move_assign.hpp"
#include "ndnboost/type_traits/has_trivial_move_constructor.hpp"
#include "ndnboost/type_traits/has_virtual_destructor.hpp"
#include "ndnboost/type_traits/is_abstract.hpp"
#include "ndnboost/type_traits/is_arithmetic.hpp"
#include "ndnboost/type_traits/is_array.hpp"
#include "ndnboost/type_traits/is_base_and_derived.hpp"
#include "ndnboost/type_traits/is_base_of.hpp"
#include "ndnboost/type_traits/is_class.hpp"
#include <ndnboost/type_traits/is_complex.hpp>
#include "ndnboost/type_traits/is_compound.hpp"
#include "ndnboost/type_traits/is_const.hpp"
#include "ndnboost/type_traits/is_convertible.hpp"
#include "ndnboost/type_traits/is_empty.hpp"
#include "ndnboost/type_traits/is_enum.hpp"
#include "ndnboost/type_traits/is_float.hpp"
#include "ndnboost/type_traits/is_floating_point.hpp"
#include "ndnboost/type_traits/is_function.hpp"
#include "ndnboost/type_traits/is_fundamental.hpp"
#include "ndnboost/type_traits/is_integral.hpp"
#include "ndnboost/type_traits/is_lvalue_reference.hpp"
#include "ndnboost/type_traits/is_member_function_pointer.hpp"
#include "ndnboost/type_traits/is_member_object_pointer.hpp"
#include "ndnboost/type_traits/is_member_pointer.hpp"
#include "ndnboost/type_traits/is_nothrow_move_assignable.hpp"
#include "ndnboost/type_traits/is_nothrow_move_constructible.hpp"
#include "ndnboost/type_traits/is_object.hpp"
#include "ndnboost/type_traits/is_pod.hpp"
#include "ndnboost/type_traits/is_polymorphic.hpp"
#include "ndnboost/type_traits/is_pointer.hpp"
#include "ndnboost/type_traits/is_reference.hpp"
#include "ndnboost/type_traits/is_rvalue_reference.hpp"
#include "ndnboost/type_traits/is_signed.hpp"
#include "ndnboost/type_traits/is_same.hpp"
#include "ndnboost/type_traits/is_scalar.hpp"
#include "ndnboost/type_traits/is_stateless.hpp"
#include "ndnboost/type_traits/is_union.hpp"
#include "ndnboost/type_traits/is_unsigned.hpp"
#include "ndnboost/type_traits/is_void.hpp"
#include "ndnboost/type_traits/is_virtual_base_of.hpp"
#include "ndnboost/type_traits/is_volatile.hpp"
#include <ndnboost/type_traits/make_unsigned.hpp>
#include <ndnboost/type_traits/make_signed.hpp>
#include "ndnboost/type_traits/rank.hpp"
#include "ndnboost/type_traits/remove_bounds.hpp"
#include "ndnboost/type_traits/remove_extent.hpp"
#include "ndnboost/type_traits/remove_all_extents.hpp"
#include "ndnboost/type_traits/remove_const.hpp"
#include "ndnboost/type_traits/remove_cv.hpp"
#include "ndnboost/type_traits/remove_pointer.hpp"
#include "ndnboost/type_traits/remove_reference.hpp"
#include "ndnboost/type_traits/remove_volatile.hpp"
#include "ndnboost/type_traits/type_with_alignment.hpp"
#if !(defined(__sgi) && defined(__EDG_VERSION__) && (__EDG_VERSION__ == 238))
#include "ndnboost/type_traits/integral_promotion.hpp"
#include "ndnboost/type_traits/promote.hpp"
#endif

#include "ndnboost/type_traits/ice.hpp"

#endif // BOOST_TYPE_TRAITS_HPP
