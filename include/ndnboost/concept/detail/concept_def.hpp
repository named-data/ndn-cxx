// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef NDNBOOST_CONCEPT_DETAIL_CONCEPT_DEF_DWA200651_HPP
# define NDNBOOST_CONCEPT_DETAIL_CONCEPT_DEF_DWA200651_HPP
# include <ndnboost/preprocessor/seq/for_each_i.hpp>
# include <ndnboost/preprocessor/seq/enum.hpp>
# include <ndnboost/preprocessor/comma_if.hpp>
# include <ndnboost/preprocessor/cat.hpp>
#endif // NDNBOOST_CONCEPT_DETAIL_CONCEPT_DEF_DWA200651_HPP

// NDNBOOST_concept(SomeName, (p1)(p2)...(pN))
//
// Expands to "template <class p1, class p2, ...class pN> struct SomeName"
//
// Also defines an equivalent SomeNameConcept for backward compatibility.
// Maybe in the next release we can kill off the "Concept" suffix for good.
#if NDNBOOST_WORKAROUND(__GNUC__, <= 3)
# define NDNBOOST_concept(name, params)                                            \
    template < NDNBOOST_PP_SEQ_FOR_EACH_I(NDNBOOST_CONCEPT_typename,~,params) >       \
    struct name; /* forward declaration */                                      \
                                                                                \
    template < NDNBOOST_PP_SEQ_FOR_EACH_I(NDNBOOST_CONCEPT_typename,~,params) >       \
    struct NDNBOOST_PP_CAT(name,Concept)                                           \
      : name< NDNBOOST_PP_SEQ_ENUM(params) >                                       \
    {                                                                           \
        /* at least 2.96 and 3.4.3 both need this */                            \
        NDNBOOST_PP_CAT(name,Concept)();                                           \
    };                                                                          \
                                                                                \
    template < NDNBOOST_PP_SEQ_FOR_EACH_I(NDNBOOST_CONCEPT_typename,~,params) >       \
    struct name                                                                
#else
# define NDNBOOST_concept(name, params)                                            \
    template < NDNBOOST_PP_SEQ_FOR_EACH_I(NDNBOOST_CONCEPT_typename,~,params) >       \
    struct name; /* forward declaration */                                      \
                                                                                \
    template < NDNBOOST_PP_SEQ_FOR_EACH_I(NDNBOOST_CONCEPT_typename,~,params) >       \
    struct NDNBOOST_PP_CAT(name,Concept)                                           \
      : name< NDNBOOST_PP_SEQ_ENUM(params) >                                       \
    {                                                                           \
    };                                                                          \
                                                                                \
    template < NDNBOOST_PP_SEQ_FOR_EACH_I(NDNBOOST_CONCEPT_typename,~,params) >       \
    struct name                                                                
#endif
    
// Helper for NDNBOOST_concept, above.
# define NDNBOOST_CONCEPT_typename(r, ignored, index, t) \
    NDNBOOST_PP_COMMA_IF(index) typename t

