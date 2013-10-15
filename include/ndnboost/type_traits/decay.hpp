//  (C) Copyright John Maddock & Thorsten Ottosen 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef NDNBOOST_TT_DECAY_HPP_INCLUDED
#define NDNBOOST_TT_DECAY_HPP_INCLUDED

#include <ndnboost/type_traits/config.hpp>
#include <ndnboost/type_traits/is_array.hpp>
#include <ndnboost/type_traits/is_function.hpp>
#include <ndnboost/type_traits/remove_bounds.hpp>
#include <ndnboost/type_traits/add_pointer.hpp>
#include <ndnboost/type_traits/remove_reference.hpp>
#include <ndnboost/mpl/eval_if.hpp>
#include <ndnboost/mpl/identity.hpp>

namespace ndnboost 
{

    template< class T >
    struct decay
    {
    private:
        typedef NDNBOOST_DEDUCED_TYPENAME remove_reference<T>::type Ty;
    public:
        typedef NDNBOOST_DEDUCED_TYPENAME mpl::eval_if< 
            is_array<Ty>,
            mpl::identity<NDNBOOST_DEDUCED_TYPENAME remove_bounds<Ty>::type*>,
            NDNBOOST_DEDUCED_TYPENAME mpl::eval_if< 
                is_function<Ty>,
                add_pointer<Ty>,
                mpl::identity<Ty>
            >
        >::type type;
    };
    
} // namespace ndnboost


#endif // NDNBOOST_TT_DECAY_HPP_INCLUDED
