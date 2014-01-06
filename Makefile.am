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
SUBDIRS = include . tests

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
  src/c/util/crypto.h \
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
  src/encoding/der/der-exception.cpp \
  src/encoding/der/der-exception.hpp \
  src/encoding/der/der.cpp \
  src/encoding/der/der.hpp \
  src/encoding/der/visitor/certificate-data-visitor.cpp \
  src/encoding/der/visitor/certificate-data-visitor.hpp \
  src/encoding/der/visitor/no-arguments-visitor.cpp \
  src/encoding/der/visitor/no-arguments-visitor.hpp \
  src/encoding/der/visitor/print-visitor.cpp \
  src/encoding/der/visitor/print-visitor.hpp \
  src/encoding/der/visitor/public-key-visitor.cpp \
  src/encoding/der/visitor/public-key-visitor.hpp \
  src/encoding/der/visitor/simple-visitor.cpp \
  src/encoding/der/visitor/simple-visitor.hpp \
  src/encoding/der/visitor/visitor.hpp \
  src/encoding/der/visitor/void-no-arguments-visitor.hpp \
  src/encoding/der/visitor/void-visitor.cpp \
  src/encoding/der/visitor/void-visitor.hpp \
  src/encoding/element-listener.cpp \
  src/encoding/oid.cpp \
  src/encoding/wire-format.cpp \
  src/exclude.cpp \
  src/face.cpp \
  src/forwarding-entry.cpp \
  src/interest.cpp \
  src/name.cpp \
  src/node.cpp \
  src/security/certificate/certificate-extension.cpp \
  src/security/certificate/certificate-subject-description.cpp \
  src/security/certificate/certificate.cpp \
  src/security/certificate/identity-certificate.cpp \
  src/security/certificate/public-key.cpp \
  src/security/identity/basic-identity-storage.cpp \
  src/security/identity/identity-manager.cpp \
  src/security/identity/identity-storage.cpp \
  src/security/identity/memory-identity-storage.cpp \
  src/security/identity/memory-private-key-storage.cpp \
  src/security/identity/osx-private-key-storage.cpp \
  src/security/key-chain.cpp \
  src/security/policy/no-verify-policy-manager.cpp \
  src/security/policy/self-verify-policy-manager.cpp \
  src/security/security-exception.cpp \
  src/security/signature/sha256-with-rsa-handler.cpp \
  src/transport/tcp-transport.cpp \
  src/transport/transport.cpp \
  src/transport/udp-transport.cpp \
  src/util/blob-stream.hpp \
  src/util/blob.cpp \
  src/util/changed-event.cpp \
  src/util/changed-event.hpp \
  src/util/dynamic-uint8-vector.cpp \
  src/util/dynamic-uint8-vector.hpp \
  src/util/logging.cpp \
  src/util/logging.hpp

libndn_cpp_la_LIBADD  = @OPENSSL_LIBS@ @OSX_SECURITY_LIBS@
libndn_cpp_la_LDFLAGS = @OPENSSL_LDFLAGS@  @SQLITE3_LDFLAGS@ @BOOST_LDFLAGS@
libndn_cpp_la_CFLAGS  = @OPENSSL_INCLUDES@ @SQLITE3_CFLAGS@  @BOOST_CPPFLAGS@

dist_noinst_SCRIPTS = autogen.sh
