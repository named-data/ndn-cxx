# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  *
#  * See http://www.boost.org for most recent version.
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# ifndef NDNBOOST_PREPROCESSOR_LIST_ADT_HPP
# define NDNBOOST_PREPROCESSOR_LIST_ADT_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/detail/is_binary.hpp>
# include <ndnboost/preprocessor/logical/compl.hpp>
# include <ndnboost/preprocessor/tuple/eat.hpp>
#
# /* NDNBOOST_PP_LIST_CONS */
#
# define NDNBOOST_PP_LIST_CONS(head, tail) (head, tail)
#
# /* NDNBOOST_PP_LIST_NIL */
#
# define NDNBOOST_PP_LIST_NIL NDNBOOST_PP_NIL
#
# /* NDNBOOST_PP_LIST_FIRST */
#
# define NDNBOOST_PP_LIST_FIRST(list) NDNBOOST_PP_LIST_FIRST_D(list)
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_LIST_FIRST_D(list) NDNBOOST_PP_LIST_FIRST_I list
# else
#    define NDNBOOST_PP_LIST_FIRST_D(list) NDNBOOST_PP_LIST_FIRST_I ## list
# endif
#
# define NDNBOOST_PP_LIST_FIRST_I(head, tail) head
#
# /* NDNBOOST_PP_LIST_REST */
#
# define NDNBOOST_PP_LIST_REST(list) NDNBOOST_PP_LIST_REST_D(list)
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_LIST_REST_D(list) NDNBOOST_PP_LIST_REST_I list
# else
#    define NDNBOOST_PP_LIST_REST_D(list) NDNBOOST_PP_LIST_REST_I ## list
# endif
#
# define NDNBOOST_PP_LIST_REST_I(head, tail) tail
#
# /* NDNBOOST_PP_LIST_IS_CONS */
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_BCC()
#    define NDNBOOST_PP_LIST_IS_CONS(list) NDNBOOST_PP_LIST_IS_CONS_D(list)
#    define NDNBOOST_PP_LIST_IS_CONS_D(list) NDNBOOST_PP_LIST_IS_CONS_ ## list
#    define NDNBOOST_PP_LIST_IS_CONS_(head, tail) 1
#    define NDNBOOST_PP_LIST_IS_CONS_NDNBOOST_PP_NIL 0
# else
#    define NDNBOOST_PP_LIST_IS_CONS(list) NDNBOOST_PP_IS_BINARY(list)
# endif
#
# /* NDNBOOST_PP_LIST_IS_NIL */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_BCC()
#    define NDNBOOST_PP_LIST_IS_NIL(list) NDNBOOST_PP_COMPL(NDNBOOST_PP_IS_BINARY(list))
# else
#    define NDNBOOST_PP_LIST_IS_NIL(list) NDNBOOST_PP_COMPL(NDNBOOST_PP_LIST_IS_CONS(list))
# endif
#
# endif
