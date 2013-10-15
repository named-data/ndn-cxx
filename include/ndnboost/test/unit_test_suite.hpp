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
//  Description : defines Unit Test Framework public API
// ***************************************************************************

#ifndef NDNBOOST_TEST_UNIT_TEST_SUITE_HPP_071894GER
#define NDNBOOST_TEST_UNIT_TEST_SUITE_HPP_071894GER

// Boost.Test
#include <ndnboost/test/unit_test_suite_impl.hpp>
#include <ndnboost/test/framework.hpp>

//____________________________________________________________________________//

// ************************************************************************** //
// **************    Non-auto (explicit) test case interface   ************** //
// ************************************************************************** //

#define NDNBOOST_TEST_CASE( test_function ) \
ndnboost::unit_test::make_test_case( ndnboost::unit_test::callback0<>(test_function), NDNBOOST_TEST_STRINGIZE( test_function ) )
#define NDNBOOST_CLASS_TEST_CASE( test_function, tc_instance ) \
ndnboost::unit_test::make_test_case((test_function), NDNBOOST_TEST_STRINGIZE( test_function ), tc_instance )

// ************************************************************************** //
// **************               NDNBOOST_TEST_SUITE               ************** //
// ************************************************************************** //

#define NDNBOOST_TEST_SUITE( testsuite_name ) \
( new ndnboost::unit_test::test_suite( testsuite_name ) )

// ************************************************************************** //
// **************             NDNBOOST_AUTO_TEST_SUITE            ************** //
// ************************************************************************** //

#define NDNBOOST_AUTO_TEST_SUITE( suite_name )                             \
namespace suite_name {                                                  \
NDNBOOST_AUTO_TU_REGISTRAR( suite_name )( NDNBOOST_STRINGIZE( suite_name ) ); \
/**/

// ************************************************************************** //
// **************            NDNBOOST_FIXTURE_TEST_SUITE          ************** //
// ************************************************************************** //

#define NDNBOOST_FIXTURE_TEST_SUITE( suite_name, F )                       \
NDNBOOST_AUTO_TEST_SUITE( suite_name )                                     \
typedef F NDNBOOST_AUTO_TEST_CASE_FIXTURE;                                 \
/**/

// ************************************************************************** //
// **************           NDNBOOST_AUTO_TEST_SUITE_END          ************** //
// ************************************************************************** //

#define NDNBOOST_AUTO_TEST_SUITE_END()                                     \
NDNBOOST_AUTO_TU_REGISTRAR( NDNBOOST_JOIN( end_suite, __LINE__ ) )( 1 );      \
}                                                                       \
/**/

// ************************************************************************** //
// **************    NDNBOOST_AUTO_TEST_CASE_EXPECTED_FAILURES    ************** //
// ************************************************************************** //

#define NDNBOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( test_name, n )          \
struct NDNBOOST_AUTO_TC_UNIQUE_ID( test_name );                            \
                                                                        \
static struct NDNBOOST_JOIN( test_name, _exp_fail_num_spec )               \
: ndnboost::unit_test::ut_detail::                                         \
  auto_tc_exp_fail<NDNBOOST_AUTO_TC_UNIQUE_ID( test_name ) >               \
{                                                                       \
    NDNBOOST_JOIN( test_name, _exp_fail_num_spec )()                       \
    : ndnboost::unit_test::ut_detail::                                     \
      auto_tc_exp_fail<NDNBOOST_AUTO_TC_UNIQUE_ID( test_name ) >( n )      \
    {}                                                                  \
} NDNBOOST_JOIN( test_name, _exp_fail_num_spec_inst );                     \
                                                                        \
/**/

// ************************************************************************** //
// **************            NDNBOOST_FIXTURE_TEST_CASE           ************** //
// ************************************************************************** //

#define NDNBOOST_FIXTURE_TEST_CASE( test_name, F )                         \
struct test_name : public F { void test_method(); };                    \
                                                                        \
static void NDNBOOST_AUTO_TC_INVOKER( test_name )()                        \
{                                                                       \
    test_name t;                                                        \
    t.test_method();                                                    \
}                                                                       \
                                                                        \
struct NDNBOOST_AUTO_TC_UNIQUE_ID( test_name ) {};                         \
                                                                        \
NDNBOOST_AUTO_TU_REGISTRAR( test_name )(                                   \
    ndnboost::unit_test::make_test_case(                                   \
        &NDNBOOST_AUTO_TC_INVOKER( test_name ), #test_name ),              \
    ndnboost::unit_test::ut_detail::auto_tc_exp_fail<                      \
        NDNBOOST_AUTO_TC_UNIQUE_ID( test_name )>::instance()->value() );   \
                                                                        \
void test_name::test_method()                                           \
/**/

// ************************************************************************** //
// **************             NDNBOOST_AUTO_TEST_CASE             ************** //
// ************************************************************************** //

#define NDNBOOST_AUTO_TEST_CASE( test_name )                               \
NDNBOOST_FIXTURE_TEST_CASE( test_name, NDNBOOST_AUTO_TEST_CASE_FIXTURE )
/**/

// ************************************************************************** //
// **************       NDNBOOST_FIXTURE_TEST_CASE_TEMPLATE       ************** //
// ************************************************************************** //

#define NDNBOOST_FIXTURE_TEST_CASE_TEMPLATE( test_name, type_name, TL, F ) \
template<typename type_name>                                            \
struct test_name : public F                                             \
{ void test_method(); };                                                \
                                                                        \
struct NDNBOOST_AUTO_TC_INVOKER( test_name ) {                             \
    template<typename TestType>                                         \
    static void run( ndnboost::type<TestType>* = 0 )                       \
    {                                                                   \
        test_name<TestType> t;                                          \
        t.test_method();                                                \
    }                                                                   \
};                                                                      \
                                                                        \
NDNBOOST_AUTO_TU_REGISTRAR( test_name )(                                   \
    ndnboost::unit_test::ut_detail::template_test_case_gen<                \
        NDNBOOST_AUTO_TC_INVOKER( test_name ),TL >(                        \
          NDNBOOST_STRINGIZE( test_name ) ) );                             \
                                                                        \
template<typename type_name>                                            \
void test_name<type_name>::test_method()                                \
/**/

// ************************************************************************** //
// **************        NDNBOOST_AUTO_TEST_CASE_TEMPLATE         ************** //
// ************************************************************************** //

#define NDNBOOST_AUTO_TEST_CASE_TEMPLATE( test_name, type_name, TL )       \
NDNBOOST_FIXTURE_TEST_CASE_TEMPLATE( test_name, type_name, TL, NDNBOOST_AUTO_TEST_CASE_FIXTURE )

// ************************************************************************** //
// **************           NDNBOOST_TEST_CASE_TEMPLATE           ************** //
// ************************************************************************** //

#define NDNBOOST_TEST_CASE_TEMPLATE( name, typelist )                          \
    ndnboost::unit_test::ut_detail::template_test_case_gen<name,typelist >(    \
        NDNBOOST_TEST_STRINGIZE( name ) )                                      \
/**/

// ************************************************************************** //
// **************      NDNBOOST_TEST_CASE_TEMPLATE_FUNCTION       ************** //
// ************************************************************************** //

#define NDNBOOST_TEST_CASE_TEMPLATE_FUNCTION( name, type_name )    \
template<typename type_name>                                    \
void NDNBOOST_JOIN( name, _impl )( ndnboost::type<type_name>* );      \
                                                                \
struct name {                                                   \
    template<typename TestType>                                 \
    static void run( ndnboost::type<TestType>* frwrd = 0 )         \
    {                                                           \
       NDNBOOST_JOIN( name, _impl )( frwrd );                      \
    }                                                           \
};                                                              \
                                                                \
template<typename type_name>                                    \
void NDNBOOST_JOIN( name, _impl )( ndnboost::type<type_name>* )       \
/**/

// ************************************************************************** //
// **************              NDNBOOST_GLOBAL_FIXURE             ************** //
// ************************************************************************** //

#define NDNBOOST_GLOBAL_FIXTURE( F ) \
static ndnboost::unit_test::ut_detail::global_fixture_impl<F> NDNBOOST_JOIN( gf_, F ) ; \
/**/

// ************************************************************************** //
// **************         NDNBOOST_AUTO_TEST_CASE_FIXTURE         ************** //
// ************************************************************************** //

namespace ndnboost { namespace unit_test { namespace ut_detail {

struct nil_t {};

} // namespace ut_detail
} // unit_test
} // namespace ndnboost

// Intentionally is in global namespace, so that FIXURE_TEST_SUITE can reset it in user code.
typedef ::ndnboost::unit_test::ut_detail::nil_t NDNBOOST_AUTO_TEST_CASE_FIXTURE;

// ************************************************************************** //
// **************   Auto registration facility helper macros   ************** //
// ************************************************************************** //

#define NDNBOOST_AUTO_TU_REGISTRAR( test_name )    \
static ndnboost::unit_test::ut_detail::auto_test_unit_registrar NDNBOOST_JOIN( NDNBOOST_JOIN( test_name, _registrar ), __LINE__ )
#define NDNBOOST_AUTO_TC_INVOKER( test_name )      NDNBOOST_JOIN( test_name, _invoker )
#define NDNBOOST_AUTO_TC_UNIQUE_ID( test_name )    NDNBOOST_JOIN( test_name, _id )

// ************************************************************************** //
// **************                NDNBOOST_TEST_MAIN               ************** //
// ************************************************************************** //

#if defined(NDNBOOST_TEST_MAIN)

#ifdef NDNBOOST_TEST_ALTERNATIVE_INIT_API
bool init_unit_test()                   {
#else
::ndnboost::unit_test::test_suite*
init_unit_test_suite( int, char* [] )   {
#endif

#ifdef NDNBOOST_TEST_MODULE
    using namespace ::ndnboost::unit_test;
    assign_op( framework::master_test_suite().p_name.value, NDNBOOST_TEST_STRINGIZE( NDNBOOST_TEST_MODULE ).trim( "\"" ), 0 );
    
#endif

#ifdef NDNBOOST_TEST_ALTERNATIVE_INIT_API
    return true;
}
#else
    return 0;
}
#endif

#endif

//____________________________________________________________________________//

#endif // NDNBOOST_TEST_UNIT_TEST_SUITE_HPP_071894GER

