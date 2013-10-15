//  (C) Copyright Gennadiy Rozental 2005-2008.
//  Use, modification, and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 54633 $
//
//  Description : implements parser - public interface for CLA parsing and accessing
// ***************************************************************************

#ifndef NDNBOOST_RT_CLA_PARSER_IPP_062904GER
#define NDNBOOST_RT_CLA_PARSER_IPP_062904GER

// Boost.Runtime.Parameter
#include <ndnboost/test/utils/runtime/config.hpp>
#include <ndnboost/test/utils/runtime/trace.hpp>
#include <ndnboost/test/utils/runtime/argument.hpp>

#include <ndnboost/test/utils/runtime/cla/argv_traverser.hpp>
#include <ndnboost/test/utils/runtime/cla/parameter.hpp>
#include <ndnboost/test/utils/runtime/cla/modifier.hpp>
#include <ndnboost/test/utils/runtime/cla/validation.hpp>
#include <ndnboost/test/utils/runtime/cla/parser.hpp>

// Boost.Test
#include <ndnboost/test/utils/basic_cstring/io.hpp>
#include <ndnboost/test/utils/foreach.hpp>

// Boost
#include <ndnboost/lexical_cast.hpp>

namespace ndnboost {

namespace NDNBOOST_RT_PARAM_NAMESPACE {

namespace cla {

// ************************************************************************** //
// **************             runtime::cla::parser             ************** //
// ************************************************************************** //

NDNBOOST_RT_PARAM_INLINE
parser::parser( cstring program_name )
{
    assign_op( m_program_name, program_name, 0 );
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE parser::param_iterator
parser::first_param() const
{
    return m_parameters.begin();
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE parser::param_iterator
parser::last_param() const
{
    return m_parameters.end();
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE argument const&
parser::valid_argument( cstring string_id ) const
{
    const_argument_ptr arg = (*this)[string_id];

    NDNBOOST_RT_PARAM_VALIDATE_LOGIC( !!arg, "Actual argument for parameter " << string_id << " is not present" );

    return *arg;
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE parser&
parser::operator<<( parameter_ptr new_param )
{
    NDNBOOST_TEST_FOREACH( parameter_ptr, old_param, m_parameters ) {
        NDNBOOST_RT_PARAM_VALIDATE_LOGIC( !old_param->conflict_with( *new_param ),
            NDNBOOST_RT_PARAM_LITERAL( "Definition of parameter " )                << new_param->id_2_report() << 
            NDNBOOST_RT_PARAM_LITERAL( " conflicts with defintion of parameter " ) << old_param->id_2_report() );
    }

    m_parameters.push_back( new_param );

    return *this;
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE void
parser::parse( int& argc, char_type** argv )
{
    if( m_program_name.empty() ) {
        m_program_name.assign( argv[0] );
        dstring::size_type pos = m_program_name.find_last_of( NDNBOOST_RT_PARAM_LITERAL( "/\\" ) );

        if( pos != static_cast<dstring::size_type>(cstring::npos) )
            m_program_name.erase( 0, pos+1 );
    }

    m_traverser.init( argc, argv );

    try {
        while( !m_traverser.eoi() ) {
            parameter_ptr found_param;

            NDNBOOST_RT_PARAM_TRACE( "Total " << m_parameters.size() << " parameters registered" );

            NDNBOOST_TEST_FOREACH( parameter_ptr const&, curr_param, m_parameters ) {
                NDNBOOST_RT_PARAM_TRACE( "Try parameter " << curr_param->id_2_report() );

                if( curr_param->matching( m_traverser, !found_param ) ) {
                    NDNBOOST_RT_PARAM_TRACE( "Match found" );
                    NDNBOOST_RT_CLA_VALIDATE_INPUT( !found_param, (m_traverser.rollback(),m_traverser), "Ambiguous input" );

                    found_param = curr_param;
                }

                m_traverser.rollback();
            }

            if( !found_param ) {
                NDNBOOST_RT_PARAM_TRACE( "No match found" );
                NDNBOOST_RT_CLA_VALIDATE_INPUT( m_traverser.handle_mismatch(), m_traverser,
                                             NDNBOOST_RT_PARAM_LITERAL( "Unexpected input" ) );

                continue;
            }

            NDNBOOST_RT_PARAM_TRACE( "Parse argument value" );
            found_param->produce_argument( m_traverser );

            m_traverser.commit();
        }

        NDNBOOST_TEST_FOREACH( parameter_ptr const&, curr_param, m_parameters ) {
            if( !curr_param->p_optional && !curr_param->actual_argument() ) {
                curr_param->produce_argument( *this );

                NDNBOOST_RT_PARAM_VALIDATE_LOGIC( curr_param->actual_argument(),
                    NDNBOOST_RT_PARAM_LITERAL( "Required argument for parameter " ) << curr_param->id_2_report()
                        << NDNBOOST_RT_PARAM_LITERAL( " is missing" ) );
            }
        }
    }
    catch( bad_lexical_cast const& ) {
        NDNBOOST_RT_PARAM_REPORT_LOGIC_ERROR( 
            NDNBOOST_RT_PARAM_LITERAL( "String to value convertion error during input parsing" ) );
    }

    m_traverser.remainder( argc, argv );
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE const_argument_ptr
parser::operator[]( cstring string_id ) const
{
    parameter_ptr found_param;

    NDNBOOST_TEST_FOREACH( parameter_ptr const&, curr_param, m_parameters ) {
        if( curr_param->responds_to( string_id ) ) {
            NDNBOOST_RT_PARAM_VALIDATE_LOGIC( !found_param,
                                           NDNBOOST_RT_PARAM_LITERAL( "Ambiguous parameter string id: " ) << string_id );

            found_param = curr_param;
        }
    }

    return found_param ? found_param->actual_argument() : argument_ptr();
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE cstring
parser::get( cstring string_id ) const
{
    return get<cstring>( string_id );
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE void
parser::usage( out_stream& ostr )
{
    if( m_program_name.empty() )
        assign_op( m_program_name, NDNBOOST_RT_PARAM_CSTRING_LITERAL( "<program>" ), 0 );

    format_stream fs;

    fs << m_program_name;

    NDNBOOST_TEST_FOREACH( parameter_ptr const&, curr_param, m_parameters ) {
        fs << NDNBOOST_RT_PARAM_LITERAL( ' ' );

        if( curr_param->p_optional )
            fs << NDNBOOST_RT_PARAM_LITERAL( '[' );

        curr_param->usage_info( fs );

        if( curr_param->p_optional )
            fs << NDNBOOST_RT_PARAM_LITERAL( ']' );

        if( curr_param->p_multiplicable ) {
            fs << NDNBOOST_RT_PARAM_CSTRING_LITERAL( " ... " );
            
            if( curr_param->p_optional )
                fs << NDNBOOST_RT_PARAM_LITERAL( '[' );

            curr_param->usage_info( fs );

            if( curr_param->p_optional )
                fs << NDNBOOST_RT_PARAM_LITERAL( ']' );
        }
    }

    ostr << NDNBOOST_RT_PARAM_CSTRING_LITERAL( "Usage:\n" ) << fs.str() << std::endl;
}

//____________________________________________________________________________//

NDNBOOST_RT_PARAM_INLINE void
parser::help( out_stream& ostr )
{
    usage( ostr );

    bool need_where = true;

    NDNBOOST_TEST_FOREACH( parameter_ptr const&, curr_param, m_parameters ) {
        if( curr_param->p_description->empty() )
            continue;

        if( need_where ) {
            ostr << NDNBOOST_RT_PARAM_CSTRING_LITERAL( "where:\n" );
            need_where = false;
        }

        ostr << curr_param->id_2_report() << NDNBOOST_RT_PARAM_CSTRING_LITERAL( " - " ) << curr_param->p_description << std::endl;
    }
}

//____________________________________________________________________________//

} // namespace cla

} // namespace NDNBOOST_RT_PARAM_NAMESPACE

} // namespace ndnboost

#endif // NDNBOOST_RT_CLA_PARSER_IPP_062904GER
