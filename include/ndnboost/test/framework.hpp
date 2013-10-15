//  (C) Copyright Gennadiy Rozental 2005-2008.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 54633 $
//
//  Description : defines framework interface
// ***************************************************************************

#ifndef NDNBOOST_TEST_FRAMEWORK_HPP_020805GER
#define NDNBOOST_TEST_FRAMEWORK_HPP_020805GER

// Boost.Test
#include <ndnboost/test/detail/global_typedef.hpp>
#include <ndnboost/test/detail/fwd_decl.hpp>
#include <ndnboost/test/utils/trivial_singleton.hpp>

#include <ndnboost/test/detail/suppress_warnings.hpp>

// STL
#include <stdexcept>

//____________________________________________________________________________//

namespace ndnboost {

namespace unit_test {

// ************************************************************************** //
// **************              init_unit_test_func             ************** //
// ************************************************************************** //

#ifdef NDNBOOST_TEST_ALTERNATIVE_INIT_API
typedef bool        (*init_unit_test_func)();
#else
typedef test_suite* (*init_unit_test_func)( int, char* [] );
#endif

// ************************************************************************** //
// **************                   framework                  ************** //
// ************************************************************************** //

namespace framework {

// initialization
NDNBOOST_TEST_DECL void    init( init_unit_test_func init_func, int argc, char* argv[] );
NDNBOOST_TEST_DECL bool    is_initialized();

// mutation access methods
NDNBOOST_TEST_DECL void    register_test_unit( test_case* tc );
NDNBOOST_TEST_DECL void    register_test_unit( test_suite* ts );
NDNBOOST_TEST_DECL void    deregister_test_unit( test_unit* tu );
NDNBOOST_TEST_DECL void    clear();

NDNBOOST_TEST_DECL void    register_observer( test_observer& );
NDNBOOST_TEST_DECL void    deregister_observer( test_observer& );
NDNBOOST_TEST_DECL void    reset_observers();

NDNBOOST_TEST_DECL master_test_suite_t& master_test_suite();

// constant access methods
NDNBOOST_TEST_DECL test_case const&    current_test_case();

NDNBOOST_TEST_DECL test_unit&  get( test_unit_id, test_unit_type );
template<typename UnitType>
UnitType&               get( test_unit_id id )
{
    return static_cast<UnitType&>( get( id, static_cast<test_unit_type>(UnitType::type) ) );
}

// test initiation
NDNBOOST_TEST_DECL void    run( test_unit_id = INV_TEST_UNIT_ID, bool continue_test = true );
NDNBOOST_TEST_DECL void    run( test_unit const*, bool continue_test = true );

// public test events dispatchers
NDNBOOST_TEST_DECL void    assertion_result( bool passed );
NDNBOOST_TEST_DECL void    exception_caught( execution_exception const& );
NDNBOOST_TEST_DECL void    test_unit_aborted( test_unit const& );

// ************************************************************************** //
// **************                framework errors              ************** //
// ************************************************************************** //

struct internal_error : std::runtime_error {
    internal_error( const_string m ) : std::runtime_error( std::string( m.begin(), m.size() ) ) {}
};

struct setup_error : std::runtime_error {
    setup_error( const_string m ) : std::runtime_error( std::string( m.begin(), m.size() ) ) {}
};

#define NDNBOOST_TEST_SETUP_ASSERT( cond, msg ) if( cond ) {} else throw unit_test::framework::setup_error( msg )

struct nothing_to_test {}; // not really an error

} // namespace framework

} // unit_test

} // namespace ndnboost

//____________________________________________________________________________//

#include <ndnboost/test/detail/enable_warnings.hpp>

#endif // NDNBOOST_TEST_FRAMEWORK_HPP_020805GER

