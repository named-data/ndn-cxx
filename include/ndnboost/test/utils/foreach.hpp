//  (C) Copyright Eric Niebler 2004-2005
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
//  Description : this is an abridged version of an excelent NDNBOOST_FOREACH facility
//  presented by Eric Niebler. I am so fond of it so I can't wait till it 
//  going to be accepted into Boost. Also I need version with less number of dependencies 
//  and more portable. This version doesn't support rvalues and will reeveluate it's 
//  parameters, but should be good enough for my purposes.
// ***************************************************************************

#ifndef NDNBOOST_TEST_FOREACH_HPP_021005GER
#define NDNBOOST_TEST_FOREACH_HPP_021005GER

// Boost.Test
#include <ndnboost/test/detail/config.hpp>

// Boost
#include <ndnboost/type.hpp>
#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/test/detail/workaround.hpp>

#include <ndnboost/type_traits/is_const.hpp>

#include <ndnboost/test/detail/suppress_warnings.hpp>

//____________________________________________________________________________//

namespace ndnboost {

namespace unit_test {

namespace for_each {

// ************************************************************************** //
// **************                  static_any                  ************** //
// ************************************************************************** //

struct static_any_base
{
    operator bool() const { return false; }
};

//____________________________________________________________________________//

template<typename Iter>
struct static_any : static_any_base
{
    static_any( Iter const& t ) : m_it( t ) {}

    mutable Iter m_it;
};

//____________________________________________________________________________//

typedef static_any_base const& static_any_t;

//____________________________________________________________________________//

template<typename Iter>
inline Iter&
static_any_cast( static_any_t a, Iter* = 0 )
{
    return static_cast<Iter&>( static_cast<static_any<Iter> const&>( a ).m_it );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************                   is_const                   ************** //
// ************************************************************************** //

template<typename C>
inline is_const<C>
is_const_coll( C& )
{
    return is_const<C>();
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************                     begin                    ************** //
// ************************************************************************** //

template<typename C>
inline static_any<NDNBOOST_DEDUCED_TYPENAME C::iterator>
begin( C& t, mpl::false_ )
{
    return static_any<NDNBOOST_DEDUCED_TYPENAME C::iterator>( t.begin() );
}

//____________________________________________________________________________//

template<typename C>
inline static_any<NDNBOOST_DEDUCED_TYPENAME C::const_iterator>
begin( C const& t, mpl::true_ )
{
    return static_any<NDNBOOST_DEDUCED_TYPENAME C::const_iterator>( t.begin() );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************                      end                     ************** //
// ************************************************************************** //

template<typename C>
inline static_any<NDNBOOST_DEDUCED_TYPENAME C::iterator>
end( C& t, mpl::false_ )
{
    return static_any<NDNBOOST_DEDUCED_TYPENAME C::iterator>( t.end() );
}

//____________________________________________________________________________//

template<typename C>
inline static_any<NDNBOOST_DEDUCED_TYPENAME C::const_iterator>
end( C const& t, mpl::true_ )
{
    return static_any<NDNBOOST_DEDUCED_TYPENAME C::const_iterator>( t.end() );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************                      done                    ************** //
// ************************************************************************** //

template<typename C>
inline bool
done( static_any_t cur, static_any_t end, C&, mpl::false_ )
{
    return  static_any_cast<NDNBOOST_DEDUCED_TYPENAME C::iterator>( cur ) ==
            static_any_cast<NDNBOOST_DEDUCED_TYPENAME C::iterator>( end );
}

//____________________________________________________________________________//

template<typename C>
inline bool
done( static_any_t cur, static_any_t end, C const&, mpl::true_ )
{
    return  static_any_cast<NDNBOOST_DEDUCED_TYPENAME C::const_iterator>( cur ) ==
            static_any_cast<NDNBOOST_DEDUCED_TYPENAME C::const_iterator>( end );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************                      next                    ************** //
// ************************************************************************** //

template<typename C>
inline void
next( static_any_t cur, C&, mpl::false_ )
{
    ++static_any_cast<NDNBOOST_DEDUCED_TYPENAME C::iterator>( cur );
}

//____________________________________________________________________________//

template<typename C>
inline void
next( static_any_t cur, C const&, mpl::true_ )
{
    ++static_any_cast<NDNBOOST_DEDUCED_TYPENAME C::const_iterator>( cur );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************                      deref                   ************** //
// ************************************************************************** //

template<class RefType,typename C>
inline RefType
deref( static_any_t cur, C&, ::ndnboost::type<RefType>, mpl::false_ )
{
    return *static_any_cast<NDNBOOST_DEDUCED_TYPENAME C::iterator>( cur );
}

//____________________________________________________________________________//

template<class RefType,typename C>
inline RefType
deref( static_any_t cur, C const&, ::ndnboost::type<RefType>, mpl::true_ )
{
    return *static_any_cast<NDNBOOST_DEDUCED_TYPENAME C::const_iterator>( cur );
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************              NDNBOOST_TEST_FOREACH              ************** //
// ************************************************************************** //

#define NDNBOOST_TEST_FE_ANY                   ::ndnboost::unit_test::for_each::static_any_t
#define NDNBOOST_TEST_FE_IS_CONST( COL )       ::ndnboost::unit_test::for_each::is_const_coll( COL )

#define NDNBOOST_TEST_FE_BEG( COL )            \
    ::ndnboost::unit_test::for_each::begin(    \
        COL,                                \
        NDNBOOST_TEST_FE_IS_CONST( COL ) )     \
/**/

#define NDNBOOST_TEST_FE_END( COL )            \
    ::ndnboost::unit_test::for_each::end(      \
        COL,                                \
        NDNBOOST_TEST_FE_IS_CONST( COL ) )     \
/**/

#define NDNBOOST_TEST_FE_DONE( COL )           \
    ::ndnboost::unit_test::for_each::done(     \
        NDNBOOST_TEST_FE_CUR_VAR,              \
        NDNBOOST_TEST_FE_END_VAR,              \
        COL,                                \
        NDNBOOST_TEST_FE_IS_CONST( COL ) )     \
/**/

#define NDNBOOST_TEST_FE_NEXT( COL )           \
    ::ndnboost::unit_test::for_each::next(     \
        NDNBOOST_TEST_FE_CUR_VAR,              \
        COL,                                \
        NDNBOOST_TEST_FE_IS_CONST( COL ) )     \
/**/

#define NDNBOOST_FOREACH_NOOP(COL)             \
    ((void)&(COL))

#define NDNBOOST_TEST_FE_DEREF( COL, RefType ) \
    ::ndnboost::unit_test::for_each::deref(    \
        NDNBOOST_TEST_FE_CUR_VAR,              \
        COL,                                \
        ::ndnboost::type<RefType >(),          \
        NDNBOOST_TEST_FE_IS_CONST( COL ) )     \
/**/

#if NDNBOOST_WORKAROUND( NDNBOOST_MSVC, == 1310 )
#define NDNBOOST_TEST_LINE_NUM
#else
#define NDNBOOST_TEST_LINE_NUM     __LINE__
#endif

#define NDNBOOST_TEST_FE_CUR_VAR   NDNBOOST_JOIN( _fe_cur_, NDNBOOST_TEST_LINE_NUM )
#define NDNBOOST_TEST_FE_END_VAR   NDNBOOST_JOIN( _fe_end_, NDNBOOST_TEST_LINE_NUM )
#define NDNBOOST_TEST_FE_CON_VAR   NDNBOOST_JOIN( _fe_con_, NDNBOOST_TEST_LINE_NUM )

#define NDNBOOST_TEST_FOREACH( RefType, var, COL )                                             \
if( NDNBOOST_TEST_FE_ANY NDNBOOST_TEST_FE_CUR_VAR = NDNBOOST_TEST_FE_BEG( COL ) ) {} else            \
if( NDNBOOST_TEST_FE_ANY NDNBOOST_TEST_FE_END_VAR = NDNBOOST_TEST_FE_END( COL ) ) {} else            \
for( bool NDNBOOST_TEST_FE_CON_VAR = true;                                                     \
          NDNBOOST_TEST_FE_CON_VAR && !NDNBOOST_TEST_FE_DONE( COL );                              \
          NDNBOOST_TEST_FE_CON_VAR ? NDNBOOST_TEST_FE_NEXT( COL ) : NDNBOOST_FOREACH_NOOP( COL ))    \
                                                                                            \
    if( (NDNBOOST_TEST_FE_CON_VAR = false, false) ) {} else                                    \
    for( RefType var = NDNBOOST_TEST_FE_DEREF( COL, RefType );                                 \
         !NDNBOOST_TEST_FE_CON_VAR; NDNBOOST_TEST_FE_CON_VAR = true )                             \
/**/

//____________________________________________________________________________//

} // namespace for_each

} // namespace unit_test

} // namespace ndnboost

//____________________________________________________________________________//

#include <ndnboost/test/detail/enable_warnings.hpp>

#endif // NDNBOOST_TEST_FOREACH_HPP_021005GER
