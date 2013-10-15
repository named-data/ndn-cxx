//  (C) Copyright Gennadiy Rozental 2005-2008.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 49312 $
//
//  Description : input validation helpers implementation
// ***************************************************************************

#ifndef NDNBOOST_RT_CLA_VALIDATION_IPP_070604GER
#define NDNBOOST_RT_CLA_VALIDATION_IPP_070604GER

// Boost.Runtime.Parameter
#include <ndnboost/test/utils/runtime/config.hpp>

#include <ndnboost/test/utils/runtime/cla/argv_traverser.hpp>
#include <ndnboost/test/utils/runtime/cla/validation.hpp>
#include <ndnboost/test/utils/runtime/validation.hpp> // NDNBOOST_RT_PARAM_NAMESPACE::logic_error

// Boost
#include <ndnboost/test/utils/basic_cstring/io.hpp>

// STL

namespace ndnboost {

namespace NDNBOOST_RT_PARAM_NAMESPACE {

namespace cla {

// ************************************************************************** //
// **************           runtime::cla::validation           ************** //
// ************************************************************************** //

NDNBOOST_RT_PARAM_INLINE void
report_input_error( argv_traverser const& tr, format_stream& msg )
{
    if( tr.eoi() )
        msg << NDNBOOST_RT_PARAM_LITERAL( " at the end of input" );
    else {
        msg << NDNBOOST_RT_PARAM_LITERAL( " in the following position: " );

        if( tr.input().size() > 5 )
            msg << tr.input().substr( 0, 5 ) << NDNBOOST_RT_PARAM_LITERAL( "..." );
        else
            msg << tr.input();
    }

    throw NDNBOOST_RT_PARAM_NAMESPACE::logic_error( msg.str() );
}

//____________________________________________________________________________//

} // namespace cla

} // namespace NDNBOOST_RT_PARAM_NAMESPACE

} // namespace ndnboost

#endif // NDNBOOST_RT_CLA_VALIDATION_IPP_070604GER
