//
//! Copyright (c) 2011-2012
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#if !defined(NDNBOOST_NUMERIC_CONVERSION_DONT_USE_PREPROCESSED_FILES)

    #include <ndnboost/numeric/conversion/detail/preprocessed/numeric_cast_traits_common.hpp>
	
	#if !defined(NDNBOOST_NO_LONG_LONG)
        #include <ndnboost/numeric/conversion/detail/preprocessed/numeric_cast_traits_long_long.hpp>
	#endif
	
#else
#if !NDNBOOST_PP_IS_ITERATING

    #include <ndnboost/preprocessor/seq/elem.hpp>
    #include <ndnboost/preprocessor/seq/size.hpp>
    #include <ndnboost/preprocessor/iteration/iterate.hpp>
    
    #if defined(__WAVE__) && defined(NDNBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
        #pragma wave option(preserve: 2, line: 0, output: "preprocessed/numeric_cast_traits_common.hpp")
    #endif
//
//! Copyright (c) 2011-2012
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
    #if defined(__WAVE__) && defined(NDNBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
        #pragma wave option(preserve: 1)
    #endif
	
	//! These are the assumed common built in fundamental types (not typedefs/macros.)
	#define NDNBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES() \
        (char)                                            \
        (signed char)                                     \
        (unsigned char)                                   \
        (short)                                           \
        (unsigned short)                                  \
        (int)                                             \
        (unsigned int)                                    \
        (long)                                            \
        (unsigned long)                                   \
        (float)                                           \
        (double)                                          \
        (long double)                                     \
    /***/
	
    #define NDNBOOST_NUMERIC_CONVERSION_SEQ_A() NDNBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES()
	#define NDNBOOST_NUMERIC_CONVERSION_SEQ_B() NDNBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES()

namespace ndnboost { namespace numeric {

    #define NDNBOOST_PP_ITERATION_PARAMS_1 (3, (0, NDNBOOST_PP_DEC(NDNBOOST_PP_SEQ_SIZE(NDNBOOST_NUMERIC_CONVERSION_SEQ_A())), <ndnboost/numeric/conversion/detail/numeric_cast_traits.hpp>))
    #include NDNBOOST_PP_ITERATE()    

}}//namespace ndnboost::numeric;

    #if defined(__WAVE__) && defined(NDNBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
        #pragma wave option(output: null)
    #endif   
	
	#if ( defined(__WAVE__) && defined(NDNBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES) ) || !defined(NDNBOOST_NO_LONG_LONG)
	
	    #undef NDNBOOST_NUMERIC_CONVERSION_SEQ_A
	    #undef NDNBOOST_NUMERIC_CONVERSION_SEQ_B

	    #if defined(__WAVE__) && defined(NDNBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
            #pragma wave option(preserve: 2, line: 0, output: "preprocessed/numeric_cast_traits_long_long.hpp")
        #endif

//
//! Copyright (c) 2011-2012
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
        #if defined(__WAVE__) && defined(NDNBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
            #pragma wave option(preserve: 1)
        #endif

namespace ndnboost { namespace numeric {

    #define NDNBOOST_NUMERIC_CONVERSION_SEQ_A() NDNBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES()(ndnboost::long_long_type)(ndnboost::ulong_long_type)
	#define NDNBOOST_NUMERIC_CONVERSION_SEQ_B() (ndnboost::long_long_type)(ndnboost::ulong_long_type)
    
    #define NDNBOOST_PP_ITERATION_PARAMS_1 (3, (0, NDNBOOST_PP_DEC(NDNBOOST_PP_SEQ_SIZE(NDNBOOST_NUMERIC_CONVERSION_SEQ_A())), <ndnboost/numeric/conversion/detail/numeric_cast_traits.hpp>))
    #include NDNBOOST_PP_ITERATE()    

}}//namespace ndnboost::numeric;

        #if defined(__WAVE__) && defined(NDNBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
            #pragma wave option(output: null)
        #endif   
	
	#endif
		
    #undef NDNBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES
	#undef NDNBOOST_NUMERIC_CONVERSION_SEQ_A
	#undef NDNBOOST_NUMERIC_CONVERSION_SEQ_B
    
#elif NDNBOOST_PP_ITERATION_DEPTH() == 1

    #define NDNBOOST_PP_ITERATION_PARAMS_2 (3, (0, NDNBOOST_PP_DEC(NDNBOOST_PP_SEQ_SIZE(NDNBOOST_NUMERIC_CONVERSION_SEQ_B())), <ndnboost/numeric/conversion/detail/numeric_cast_traits.hpp>))
    #include NDNBOOST_PP_ITERATE()

#elif NDNBOOST_PP_ITERATION_DEPTH() == 2

    //! Generate default traits for the specified source and target.
    #define NDNBOOST_NUMERIC_CONVERSION_A NDNBOOST_PP_FRAME_ITERATION(1)
    #define NDNBOOST_NUMERIC_CONVERSION_B NDNBOOST_PP_FRAME_ITERATION(2)

    template <>
    struct numeric_cast_traits
        <
            NDNBOOST_PP_SEQ_ELEM(NDNBOOST_NUMERIC_CONVERSION_A, NDNBOOST_NUMERIC_CONVERSION_SEQ_A())
          , NDNBOOST_PP_SEQ_ELEM(NDNBOOST_NUMERIC_CONVERSION_B, NDNBOOST_NUMERIC_CONVERSION_SEQ_B())
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<NDNBOOST_PP_SEQ_ELEM(NDNBOOST_NUMERIC_CONVERSION_B, NDNBOOST_NUMERIC_CONVERSION_SEQ_B())> rounding_policy;
    };     

    #undef NDNBOOST_NUMERIC_CONVERSION_A
    #undef NDNBOOST_NUMERIC_CONVERSION_B

#endif//! Depth 2.
#endif// NDNBOOST_NUMERIC_CONVERSION_DONT_USE_PREPROCESSED_FILES
