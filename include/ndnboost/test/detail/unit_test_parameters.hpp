//  (C) Copyright Gennadiy Rozental 2001-2008.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 57992 $
//
//  Description : storage for unit test framework parameters information
// ***************************************************************************

#ifndef NDNBOOST_TEST_UNIT_TEST_PARAMETERS_HPP_071894GER
#define NDNBOOST_TEST_UNIT_TEST_PARAMETERS_HPP_071894GER

#include <ndnboost/test/detail/global_typedef.hpp>
#include <ndnboost/test/detail/log_level.hpp>

#include <ndnboost/test/detail/suppress_warnings.hpp>

// STL
#include <iosfwd>

//____________________________________________________________________________//

namespace ndnboost {

namespace unit_test {

// ************************************************************************** //
// **************                 runtime_config               ************** //
// ************************************************************************** //

namespace runtime_config {

NDNBOOST_TEST_DECL void                     init( int& argc, char** argv );

NDNBOOST_TEST_DECL unit_test::log_level     log_level();
NDNBOOST_TEST_DECL bool                     no_result_code();
NDNBOOST_TEST_DECL unit_test::report_level  report_level();
NDNBOOST_TEST_DECL const_string             test_to_run();
NDNBOOST_TEST_DECL const_string             break_exec_path();
NDNBOOST_TEST_DECL bool                     save_pattern();
NDNBOOST_TEST_DECL bool                     show_build_info();
NDNBOOST_TEST_DECL bool                     show_progress();
NDNBOOST_TEST_DECL bool                     catch_sys_errors();
NDNBOOST_TEST_DECL bool                     auto_start_dbg();
NDNBOOST_TEST_DECL bool                     use_alt_stack();
NDNBOOST_TEST_DECL bool                     detect_fp_exceptions();
NDNBOOST_TEST_DECL output_format            report_format();
NDNBOOST_TEST_DECL output_format            log_format();
NDNBOOST_TEST_DECL std::ostream*            report_sink();
NDNBOOST_TEST_DECL std::ostream*            log_sink();
NDNBOOST_TEST_DECL long                     detect_memory_leaks();
NDNBOOST_TEST_DECL int                      random_seed();

} // namespace runtime_config

} // namespace unit_test

} // namespace ndnboost

//____________________________________________________________________________//

#include <ndnboost/test/detail/enable_warnings.hpp>

#endif // NDNBOOST_TEST_UNIT_TEST_PARAMETERS_HPP_071894GER
