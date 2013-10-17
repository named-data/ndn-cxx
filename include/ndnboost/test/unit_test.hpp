//  (C) Copyright Gennadiy Rozental 2001-2008.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 49312 $
//
//  Description : Entry point for the end user into the Unit Test Framework.
// ***************************************************************************

#ifndef NDNBOOST_TEST_UNIT_TEST_HPP_071894GER
#define NDNBOOST_TEST_UNIT_TEST_HPP_071894GER

// Boost.Test
#include <ndnboost/test/test_tools.hpp>
#include <ndnboost/test/unit_test_suite.hpp>

//____________________________________________________________________________//

// ************************************************************************** //
// **************                 Auto Linking                 ************** //
// ************************************************************************** //

#if !defined(NDNBOOST_ALL_NO_LIB) && !defined(NDNBOOST_TEST_NO_LIB) && \
    !defined(NDNBOOST_TEST_SOURCE) && !defined(NDNBOOST_TEST_INCLUDED)
#  define NDNBOOST_LIB_NAME ndnboost_unit_test_framework

#  if defined(NDNBOOST_ALL_DYN_LINK) || defined(NDNBOOST_TEST_DYN_LINK)
#    define NDNBOOST_DYN_LINK
#  endif

#  include <ndnboost/config/auto_link.hpp>

#endif  // auto-linking disabled

// ************************************************************************** //
// **************                  unit_test_main              ************** //
// ************************************************************************** //

namespace ndnboost { namespace unit_test {

int NDNBOOST_TEST_DECL unit_test_main( init_unit_test_func init_func, int argc, char* argv[] );

}}

#if defined(NDNBOOST_TEST_DYN_LINK) && defined(NDNBOOST_TEST_MAIN) && !defined(NDNBOOST_TEST_NO_MAIN)

// ************************************************************************** //
// **************        main function for tests using dll     ************** //
// ************************************************************************** //

int NDNBOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
    return ::ndnboost::unit_test::unit_test_main( &init_unit_test, argc, argv );
}

//____________________________________________________________________________//

#endif // NDNBOOST_TEST_DYN_LINK && NDNBOOST_TEST_MAIN && !NDNBOOST_TEST_NO_MAIN

#endif // NDNBOOST_TEST_UNIT_TEST_HPP_071894GER
