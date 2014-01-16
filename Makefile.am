include aminclude.am

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

lib_LTLIBRARIES = libndn-c.la libndn-cpp.la

# "make install" will also install the include headers.
SUBDIRS = include . tools examples

if COMPILE_TESTS
  SUBDIRS += tests
if HAVE_BOOST_UNIT_TEST_FRAMEWORK
  SUBDIRS += tests_boost
endif
endif

# Just the C code.
## To regenerate:
## for i in `find src/c -name '*.c' -or -name '*.h'`; do echo "  $i \\"; done
##
## Note that public header files do not need to be included here (handled by include/Makefile.am)
libndn_c_la_SOURCES = \
  src/c/data.h \
  src/c/encoding/binary-xml-data.c \
  src/c/encoding/binary-xml-data.h \
  src/c/encoding/binary-xml-decoder.c \
  src/c/encoding/binary-xml-decoder.h \
  src/c/encoding/binary-xml-element-reader.c \
  src/c/encoding/binary-xml-element-reader.h \
  src/c/encoding/binary-xml-encoder.c \
  src/c/encoding/binary-xml-encoder.h \
  src/c/encoding/binary-xml-forwarding-entry.c \
  src/c/encoding/binary-xml-forwarding-entry.h \
  src/c/encoding/binary-xml-interest.c \
  src/c/encoding/binary-xml-interest.h \
  src/c/encoding/binary-xml-key.c \
  src/c/encoding/binary-xml-key.h \
  src/c/encoding/binary-xml-name.c \
  src/c/encoding/binary-xml-name.h \
  src/c/encoding/binary-xml-publisher-public-key-digest.c \
  src/c/encoding/binary-xml-publisher-public-key-digest.h \
  src/c/encoding/binary-xml-structure-decoder.c \
  src/c/encoding/binary-xml-structure-decoder.h \
  src/c/encoding/binary-xml.h \
  src/c/errors.c \
  src/c/errors.h \
  src/c/forwarding-entry.h \
  src/c/forwarding-flags.c \
  src/c/interest.c \
  src/c/interest.h \
  src/c/key-locator.h \
  src/c/name.c \
  src/c/name.h \
  src/c/publisher-public-key-digest.h \
  src/c/transport/socket-transport.c \
  src/c/transport/socket-transport.h \
  src/c/transport/tcp-transport.h \
  src/c/transport/udp-transport.h \
  src/c/util/blob.h \
  src/c/util/crypto.c \
  src/c/util/dynamic-uint8-array.c \
  src/c/util/dynamic-uint8-array.h \
  src/c/util/ndn_memory.c \
  src/c/util/ndn_memory.h \
  src/c/util/ndn_realloc.c \
  src/c/util/ndn_realloc.h \
  src/c/util/time.c \
  src/c/util/time.h

libndn_c_la_LIBADD  = @OPENSSL_LIBS@
libndn_c_la_LDFLAGS = @OPENSSL_LDFLAGS@
libndn_c_la_CFLAGS  = @OPENSSL_INCLUDES@

# C++ code and also the C code.
## To regenerate:
## for i in `find src -name '*.cpp' -or -name '*.hpp' -or -name '*.h' -and -not -path 'src/c/*'`; do echo "  $i \\"; done
libndn_cpp_la_SOURCES = $(libndn_c_la_SOURCES) \
  src/common.cpp \
  src/data.cpp \
  src/encoding/binary-xml-decoder.hpp \
  src/encoding/binary-xml-encoder.hpp \
  src/encoding/binary-xml-structure-decoder.hpp \
  src/encoding/binary-xml-wire-format.cpp \
  src/encoding/block.cpp \
  src/encoding/cryptopp/asn_ext.cpp \
  src/encoding/cryptopp/asn_ext.hpp \
  src/encoding/element-listener.cpp \
  src/encoding/oid.cpp \
  src/encoding/wire-format.cpp \
  src/exclude.cpp \
  src/face.cpp \
  src/interest.cpp \
  src/name.cpp \
  src/node.cpp \
  src/security/certificate-extension.cpp \
  src/security/certificate-subject-description.cpp \
  src/security/certificate.cpp \
  src/security/identity-certificate.cpp \
  src/security/public-key.cpp \
  src/security/sec-public-info-sqlite3.cpp \
  src/security/sec-public-info-memory.cpp \
  src/security/sec-tpm-memory.cpp \
  src/security/verifier.cpp \
  src/security/sec-policy-no-verify.cpp \
  src/security/sec-policy-self-verify.cpp \
  src/transport/unix-transport.cpp \
  src/util/blob-stream.hpp \
  src/util/blob.cpp \
  src/util/changed-event.cpp \
  src/util/changed-event.hpp \
  src/util/dynamic-uint8-vector.cpp \
  src/util/dynamic-uint8-vector.hpp \
  src/util/logging.cpp \
  src/util/logging.hpp \
  src/util/ndnd-id-fetcher.hpp \
  src/util/string-helper.hpp \
  src/util/time.hpp

if HAVE_OSX_SECURITY
  libndn_cpp_la_SOURCES += src/security/sec-tpm-osx.cpp
else
  EXTRA_DIST = src/security/sec-tpm-osx.cpp
endif

libndn_cpp_la_LIBADD  = @OPENSSL_LIBS@     @CRYPTOPP_LIBS@     @OSX_SECURITY_LIBS@  @BOOST_SYSTEM_LIB@
libndn_cpp_la_LDFLAGS = @OPENSSL_LDFLAGS@  @CRYPTOPP_LDFLAGS@  @SQLITE3_LDFLAGS@    @BOOST_LDFLAGS@
libndn_cpp_la_CFLAGS  = @OPENSSL_INCLUDES@ @CRYPTOPP_INCLUDES@ @SQLITE3_CFLAGS@     @BOOST_CPPFLAGS@

dist_noinst_SCRIPTS = autogen.sh
