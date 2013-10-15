// Copyright (C) 2004 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_MODIFIERS_HPP_INCLUDED
#define NDNBOOST_TYPEOF_MODIFIERS_HPP_INCLUDED

#include <ndnboost/typeof/encode_decode.hpp>
#include <ndnboost/preprocessor/facilities/identity.hpp>

#include NDNBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

// modifiers

#define NDNBOOST_TYPEOF_modifier_support(ID, Fun)\
    template<class V, class T> struct encode_type_impl<V, Fun(T)>\
    {\
        typedef\
            typename ndnboost::type_of::encode_type<\
            typename ndnboost::type_of::push_back<\
            V\
            , ndnboost::mpl::size_t<ID> >::type\
            , T>::type\
            type;\
    };\
    template<class Iter> struct decode_type_impl<ndnboost::mpl::size_t<ID>, Iter>\
    {\
        typedef ndnboost::type_of::decode_type<Iter> d1;\
        typedef Fun(typename d1::type) type;\
        typedef typename d1::iter iter;\
    }


#define NDNBOOST_TYPEOF_const_fun(T) const T
#define NDNBOOST_TYPEOF_volatile_fun(T) volatile T
#define NDNBOOST_TYPEOF_volatile_const_fun(T) volatile const T
#define NDNBOOST_TYPEOF_pointer_fun(T) T*
#define NDNBOOST_TYPEOF_reference_fun(T) T&

#if defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
//Borland incorrectly handles T const, T const volatile and T volatile.
//It drops the decoration no matter what, so we need to try to handle T* const etc. without loosing the top modifier.
#define NDNBOOST_TYPEOF_const_pointer_fun(T) T const *
#define NDNBOOST_TYPEOF_const_reference_fun(T) T const &
#define NDNBOOST_TYPEOF_volatile_pointer_fun(T) T volatile*
#define NDNBOOST_TYPEOF_volatile_reference_fun(T) T volatile&
#define NDNBOOST_TYPEOF_volatile_const_pointer_fun(T) T volatile const *
#define NDNBOOST_TYPEOF_volatile_const_reference_fun(T) T volatile const &
#endif

NDNBOOST_TYPEOF_BEGIN_ENCODE_NS

NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_const_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_volatile_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_volatile_const_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_pointer_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_reference_fun);

#if defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_const_pointer_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_const_reference_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_volatile_pointer_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_volatile_reference_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_volatile_const_pointer_fun);
NDNBOOST_TYPEOF_modifier_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_TYPEOF_volatile_const_reference_fun);
#endif

NDNBOOST_TYPEOF_END_ENCODE_NS

#undef NDNBOOST_TYPEOF_modifier_support
#undef NDNBOOST_TYPEOF_const_fun
#undef NDNBOOST_TYPEOF_volatile_fun
#undef NDNBOOST_TYPEOF_volatile_const_fun
#undef NDNBOOST_TYPEOF_pointer_fun
#undef NDNBOOST_TYPEOF_reference_fun

#if defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
#undef NDNBOOST_TYPEOF_const_pointer_fun
#undef NDNBOOST_TYPEOF_const_reference_fun
#undef NDNBOOST_TYPEOF_volatile_pointer_fun
#undef NDNBOOST_TYPEOF_volatile_reference_fun
#undef NDNBOOST_TYPEOF_volatile_const_pointer_fun
#undef NDNBOOST_TYPEOF_volatile_const_reference_fun
#endif

// arrays

#define NDNBOOST_TYPEOF_array_support(ID, Qualifier)\
    template<class V, class T, int N>\
    struct encode_type_impl<V, Qualifier() T[N]>\
    {\
        typedef\
            typename ndnboost::type_of::encode_type<\
            typename ndnboost::type_of::push_back<\
            typename ndnboost::type_of::push_back<\
            V\
            , ndnboost::mpl::size_t<ID> >::type\
            , ndnboost::mpl::size_t<N> >::type\
            , T>::type\
        type;\
    };\
    template<class Iter>\
    struct decode_type_impl<ndnboost::mpl::size_t<ID>, Iter>\
    {\
        enum{n = Iter::type::value};\
        typedef ndnboost::type_of::decode_type<typename Iter::next> d;\
        typedef typename d::type Qualifier() type[n];\
        typedef typename d::iter iter;\
    }

NDNBOOST_TYPEOF_BEGIN_ENCODE_NS

NDNBOOST_TYPEOF_array_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_PP_EMPTY);
NDNBOOST_TYPEOF_array_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_PP_IDENTITY(const));
NDNBOOST_TYPEOF_array_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_PP_IDENTITY(volatile));
NDNBOOST_TYPEOF_array_support(NDNBOOST_TYPEOF_UNIQUE_ID(), NDNBOOST_PP_IDENTITY(volatile const));
NDNBOOST_TYPEOF_END_ENCODE_NS

#undef NDNBOOST_TYPEOF_array_support

#endif//NDNBOOST_TYPEOF_MODIFIERS_HPP_INCLUDED
