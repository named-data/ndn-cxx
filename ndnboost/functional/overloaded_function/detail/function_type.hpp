
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.boost.org/LICENSE_1_0.txt)
// Home at http://www.boost.org/libs/functional/overloaded_function

#ifndef BOOST_FUNCTIONAL_OVERLOADED_FUNCTION_DETAIL_FUNCTION_TYPE_HPP_
#define BOOST_FUNCTIONAL_OVERLOADED_FUNCTION_DETAIL_FUNCTION_TYPE_HPP_

#include <ndnboost/function_types/is_function.hpp>
#include <ndnboost/function_types/is_function_pointer.hpp>
#include <ndnboost/function_types/is_function_reference.hpp>
#include <ndnboost/function_types/function_type.hpp>
#include <ndnboost/function_types/parameter_types.hpp>
#include <ndnboost/function_types/result_type.hpp>
#include <ndnboost/type_traits/remove_pointer.hpp>
#include <ndnboost/type_traits/remove_reference.hpp>
#include <ndnboost/function.hpp>
#include <ndnboost/mpl/if.hpp>
#include <ndnboost/mpl/identity.hpp>
#include <ndnboost/mpl/pop_front.hpp>
#include <ndnboost/mpl/push_front.hpp>
#include <ndnboost/typeof/typeof.hpp>

// Do not use namespace ::detail because overloaded_function is already a class.
namespace ndnboost { namespace overloaded_function_detail {

// Requires: F is a monomorphic functor (i.e., has non-template `operator()`).
// Returns: F's function type `result_type (arg1_type, arg2_type, ...)`.
// It does not assume F typedef result_type, arg1_type, ... but needs typeof.
template<typename F>
class functor_type {
    // NOTE: clang does not accept extra parenthesis `&(...)`.
    typedef BOOST_TYPEOF_TPL(&F::operator()) call_ptr;
public:
    typedef
        typename ndnboost::function_types::function_type<
            typename ndnboost::mpl::push_front<
                  typename ndnboost::mpl::pop_front< // Remove functor type (1st).
                    typename ndnboost::function_types::parameter_types<
                            call_ptr>::type
                  >::type
                , typename ndnboost::function_types::result_type<call_ptr>::type
            >::type
        >::type
    type;
};

// NOTE: When using ndnboost::function in Boost.Typeof emulation mode, the user
// has to register ndnboost::functionN instead of ndnboost::function in oder to
// do TYPEOF(F::operator()). That is confusing, so ndnboost::function is handled
// separately so it does not require any Boost.Typeof registration at all.
template<typename F>
struct functor_type< ndnboost::function<F> > {
    typedef F type;
};

// Requires: F is a function type, pointer, reference, or monomorphic functor.
// Returns: F's function type `result_type (arg1_type, arg2_type, ...)`.
template<typename F>
struct function_type {
    typedef
        typename ndnboost::mpl::if_<ndnboost::function_types::is_function<F>,
            ndnboost::mpl::identity<F>
        ,
            typename ndnboost::mpl::if_<ndnboost::function_types::
                    is_function_pointer<F>,
                ndnboost::remove_pointer<F>
            ,
                typename ndnboost::mpl::if_<ndnboost::function_types::
                        is_function_reference<F>,
                    ndnboost::remove_reference<F>
                , // Else, requires that F is a functor.
                    functor_type<F>
                >::type
            >::type
        >::type
    ::type type;
};

} } // namespace

#endif // #include guard

