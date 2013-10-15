//  (C) Copyright Gennadiy Rozental 2005-2008.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 49312 $
//
//  Description : simple helpers for creating cusom output manipulators
// ***************************************************************************

#ifndef NDNBOOST_TEST_TRIVIAL_SIGNLETON_HPP_020505GER
#define NDNBOOST_TEST_TRIVIAL_SIGNLETON_HPP_020505GER

#include <ndnboost/config.hpp>
#include <ndnboost/detail/workaround.hpp>

#include <ndnboost/noncopyable.hpp>

#include <ndnboost/test/detail/suppress_warnings.hpp>

//____________________________________________________________________________//

namespace ndnboost {

namespace unit_test {

// ************************************************************************** //
// **************                   singleton                  ************** //
// ************************************************************************** //

template<typename Derived>
class singleton : private ndnboost::noncopyable {
public:
    static Derived& instance() { static Derived the_inst; return the_inst; }    
protected:
    singleton()  {}
    ~singleton() {}
};

} // namespace unit_test

#define NDNBOOST_TEST_SINGLETON_CONS( type )       \
friend class ndnboost::unit_test::singleton<type>; \
type() {}                                       \
/**/

#if NDNBOOST_WORKAROUND(__DECCXX_VER, NDNBOOST_TESTED_AT(60590042))

#define NDNBOOST_TEST_SINGLETON_INST( inst ) \
template class unit_test::singleton< NDNBOOST_JOIN( inst, _t ) > ; \
namespace { NDNBOOST_JOIN( inst, _t)& inst = NDNBOOST_JOIN( inst, _t)::instance(); }

#elif defined(__APPLE_CC__) && defined(__GNUC__) && __GNUC__ < 4
#define NDNBOOST_TEST_SINGLETON_INST( inst ) \
static NDNBOOST_JOIN( inst, _t)& inst = NDNBOOST_JOIN (inst, _t)::instance();

#else

#define NDNBOOST_TEST_SINGLETON_INST( inst ) \
namespace { NDNBOOST_JOIN( inst, _t)& inst = NDNBOOST_JOIN( inst, _t)::instance(); }

#endif

} // namespace ndnboost

//____________________________________________________________________________//

#include <ndnboost/test/detail/enable_warnings.hpp>

#endif // NDNBOOST_TEST_TRIVIAL_SIGNLETON_HPP_020505GER
