# SYNOPSIS
#
#   AX_LIB_CRYPTOPP([ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the Crypto++ library
#
#   If no intallation prefix to the installed Crypto++ library is given the
#   macro searches under /usr, /usr/local, and /opt.
#
#   This macro calls:
#
#     AC_SUBST(CRYPTOPP_INCLUDES)
#     AC_SUBST(CRYPTOPP_LDFLAGS)
#     AC_SUBST(CRYPTOPP_LIBS)
#
#   And sets:
#
#     HAVE_CRYPTOPP

AC_DEFUN([AX_LIB_CRYPTOPP],
[
    AC_ARG_WITH([cryptopp],
        AS_HELP_STRING(
            [--with-cryptopp=@<:@ARG@:>@],
            [use Crypto++ library @<:@default=yes@:>@, optionally specify the prefix for the library]
        ),
        [
        if test "$withval" = "no"; then
            WANT_CRYPTOPP="no"
        elif test "$withval" = "yes"; then
            WANT_CRYPTOPP="yes"
            ac_cryptopp_path=""
        else
            WANT_CRYPTOPP="yes"
            ac_cryptopp_path="$withval"
        fi
        ],
        [WANT_CRYPTOPP="yes"]
    )

    CRYPTOPP_CFLAGS=""
    CRYPTOPP_LDFLAGS=""
    CRYPTOPP_LIBS=""

    if test "x$WANT_CRYPTOPP" = "xyes"; then

        AC_MSG_CHECKING([for Crypto++ library])

        ac_cryptopp_header="cryptopp/cryptlib.h"
        ac_cryptopp_libs="-lcryptopp"

        if test "$ac_cryptopp_path" != ""; then
            ac_cryptopp_ldflags="-L$ac_cryptopp_path/lib"
            ac_cryptopp_cppflags="-I$ac_cryptopp_path/include"
        else
            for ac_cryptopp_path_tmp in /usr /usr/local /opt ; do
                if test -f "$ac_cryptopp_path_tmp/include/$ac_cryptopp_header" \
                    && test -r "$ac_cryptopp_path_tmp/include/$ac_cryptopp_header"; then
                    ac_cryptopp_path=$ac_cryptopp_path_tmp
                    ac_cryptopp_cppflags="-I$ac_cryptopp_path_tmp/include"
                    ac_cryptopp_ldflags="-L$ac_cryptopp_path_tmp/lib"
                    break;
                fi
            done
        fi

        CRYPTOPP_INCLUDES="$ac_cryptopp_cppflags"
        CRYPTOPP_LDFLAGS="$ac_cryptopp_ldflags"
        CRYPTOPP_LIBS="$ac_cryptopp_libs"

        save_LIBS="$LIBS"
        save_LDFLAGS="$LDFLAGS"
        save_CPPFLAGS="$CPPFLAGS"

        LDFLAGS="$LDFLAGS $CRYPTOPP_LDFLAGS"
        LIBS="$CRYPTOPP_LIBS $LIBS"
        CPPFLAGS="$CRYPTOPP_INCLUDES $CPPFLAGS"
        AC_LANG_PUSH(C++)
        AC_LINK_IFELSE(
            [AC_LANG_PROGRAM([#include <cryptopp/cryptlib.h>], [])],
            [
                AC_MSG_RESULT([yes])
                $1
            ], [
                AC_MSG_RESULT([no])
                $2
            ])
        AC_LANG_POP([C++])
        CPPFLAGS="$save_CPPFLAGS"
        LDFLAGS="$save_LDFLAGS"
        LIBS="$save_LIBS"

        AC_SUBST(CRYPTOPP_INCLUDES)
        AC_SUBST(CRYPTOPP_LDFLAGS)
        AC_SUBST(CRYPTOPP_LIBS)

        AC_DEFINE([HAVE_CRYPTOPP], [], [Have the Crypto++ library])
    fi
])
