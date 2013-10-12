include aminclude.am

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

# "make install" will also install the include headers.
SUBDIRS = include

lib_LTLIBRARIES = libndn-c.la libndn-cpp.la

noinst_PROGRAMS = bin/test-encode-decode-forwarding-entry bin/test-encode-decode-interest bin/test-encode-decode-data \
  bin/test-get-async bin/test-publish-async

# Public C headers.
ndn_cpp_c_headers = \
  include/ndn-cpp/ndn-cpp-config.h \
  include/ndn-cpp/c/common.h \
  include/ndn-cpp/c/data-types.h \
  include/ndn-cpp/c/forwarding-flags.h \
  include/ndn-cpp/c/interest-types.h \
  include/ndn-cpp/c/key-types.h \
  include/ndn-cpp/c/encoding/element-listener.h

# Public C++ headers.
ndn_cpp_cpp_headers = \
  include/ndn-cpp/common.hpp \
  include/ndn-cpp/data.hpp \
  include/ndn-cpp/face.hpp \
  include/ndn-cpp/forwarding-entry.hpp \
  include/ndn-cpp/forwarding-flags.hpp \
  include/ndn-cpp/interest.hpp \
  include/ndn-cpp/key.hpp \
  include/ndn-cpp/name.hpp \
  include/ndn-cpp/node.hpp \
  include/ndn-cpp/publisher-public-key-digest.hpp \
  include/ndn-cpp/sha256-with-rsa-signature.hpp \
  include/ndn-cpp/encoding/binary-xml-wire-format.hpp \
  include/ndn-cpp/encoding/element-listener.hpp \
  include/ndn-cpp/encoding/oid.hpp \
  include/ndn-cpp/encoding/wire-format.hpp \
  include/ndn-cpp/security/security-common.hpp \
  include/ndn-cpp/security/key-chain.hpp \
  include/ndn-cpp/security/security-exception.hpp \
  include/ndn-cpp/security/certificate/certificate.hpp \
  include/ndn-cpp/security/certificate/identity-certificate.hpp \
  include/ndn-cpp/security/certificate/public-key.hpp \
  include/ndn-cpp/security/encryption/encryption-manager.hpp \
  include/ndn-cpp/security/identity/basic-identity-storage.hpp \
  include/ndn-cpp/security/identity/identity-manager.hpp \
  include/ndn-cpp/security/identity/identity-storage.hpp \
  include/ndn-cpp/security/identity/memory-identity-storage.hpp \
  include/ndn-cpp/security/identity/memory-private-key-storage.hpp \
  include/ndn-cpp/security/identity/osx-private-key-storage.hpp \
  include/ndn-cpp/security/identity/private-key-storage.hpp \
  include/ndn-cpp/security/policy/no-verify-policy-manager.hpp \
  include/ndn-cpp/security/policy/policy-manager.hpp \
  include/ndn-cpp/transport/tcp-transport.hpp \
  include/ndn-cpp/transport/transport.hpp \
  include/ndn-cpp/transport/udp-transport.hpp \
  include/ndn-cpp/util/blob.hpp \
  include/ndn-cpp/util/signed-blob.hpp

# Just the C code.
libndn_c_la_SOURCES = $(ndn_cpp_c_headers) \
  ndn-cpp/c/data.h \
  ndn-cpp/c/errors.c ndn-cpp/c/errors.h \
  ndn-cpp/c/forwarding-entry.h \
  ndn-cpp/c/forwarding-flags.c \
  ndn-cpp/c/interest.c ndn-cpp/c/interest.h \
  ndn-cpp/c/name.c ndn-cpp/c/name.h \
  ndn-cpp/c/publisher-public-key-digest.h \
  ndn-cpp/c/encoding/binary-xml.h \
  ndn-cpp/c/encoding/binary-xml-data.c ndn-cpp/c/encoding/binary-xml-data.h \
  ndn-cpp/c/encoding/binary-xml-decoder.c ndn-cpp/c/encoding/binary-xml-decoder.h \
  ndn-cpp/c/encoding/binary-xml-element-reader.c ndn-cpp/c/encoding/binary-xml-element-reader.h \
  ndn-cpp/c/encoding/binary-xml-encoder.c ndn-cpp/c/encoding/binary-xml-encoder.h \
  ndn-cpp/c/encoding/binary-xml-forwarding-entry.c ndn-cpp/c/encoding/binary-xml-forwarding-entry.h \
  ndn-cpp/c/encoding/binary-xml-interest.c ndn-cpp/c/encoding/binary-xml-interest.h \
  ndn-cpp/c/encoding/binary-xml-key.c ndn-cpp/c/encoding/binary-xml-key.h \
  ndn-cpp/c/encoding/binary-xml-name.c ndn-cpp/c/encoding/binary-xml-name.h \
  ndn-cpp/c/encoding/binary-xml-publisher-public-key-digest.c ndn-cpp/c/encoding/binary-xml-publisher-public-key-digest.h \
  ndn-cpp/c/encoding/binary-xml-structure-decoder.c ndn-cpp/c/encoding/binary-xml-structure-decoder.h \
  ndn-cpp/c/transport/socket-transport.c ndn-cpp/c/transport/socket-transport.h \
  ndn-cpp/c/transport/tcp-transport.h \
  ndn-cpp/c/transport/udp-transport.h \
  ndn-cpp/c/util/blob.h \
  ndn-cpp/c/util/crypto.c ndn-cpp/c/util/crypto.h \
  ndn-cpp/c/util/dynamic-uint8-array.c ndn-cpp/c/util/dynamic-uint8-array.h \
  ndn-cpp/c/util/ndn_memory.c ndn-cpp/c/util/ndn_memory.h \
  ndn-cpp/c/util/ndn_realloc.c ndn-cpp/c/util/ndn_realloc.h

# C++ code and also the C code.
libndn_cpp_la_SOURCES = $(libndn_c_la_SOURCES) $(ndn_cpp_cpp_headers) \
  ndn-cpp/common.cpp \
  ndn-cpp/data.cpp \
  ndn-cpp/face.cpp \
  ndn-cpp/forwarding-entry.cpp \
  ndn-cpp/interest.cpp \
  ndn-cpp/key.cpp \
  ndn-cpp/name.cpp \
  ndn-cpp/node.cpp \
  ndn-cpp/publisher-public-key-digest.cpp \
  ndn-cpp/sha256-with-rsa-signature.cpp \
  ndn-cpp/encoding/binary-xml-decoder.hpp \
  ndn-cpp/encoding/binary-xml-encoder.hpp \
  ndn-cpp/encoding/binary-xml-structure-decoder.hpp \
  ndn-cpp/encoding/binary-xml-wire-format.cpp \
  ndn-cpp/encoding/element-listener.cpp \
  ndn-cpp/encoding/oid.cpp \
  ndn-cpp/encoding/wire-format.cpp \
  ndn-cpp/security/key-chain.cpp \
  ndn-cpp/security/security-exception.cpp \
  ndn-cpp/security/certificate/public-key.cpp \
  ndn-cpp/security/identity/basic-identity-storage.cpp \
  ndn-cpp/security/identity/identity-manager.cpp \
  ndn-cpp/security/identity/memory-identity-storage.cpp \
  ndn-cpp/security/identity/memory-private-key-storage.cpp \
  ndn-cpp/security/identity/osx-private-key-storage.cpp \
  ndn-cpp/security/policy/no-verify-policy-manager.cpp \
  ndn-cpp/transport/tcp-transport.cpp \
  ndn-cpp/transport/transport.cpp \
  ndn-cpp/transport/udp-transport.cpp \
  ndn-cpp/util/blob.cpp \
  ndn-cpp/util/changed-event.cpp ndn-cpp/util/changed-event.hpp \
  ndn-cpp/util/dynamic-uint8-vector.cpp ndn-cpp/util/dynamic-uint8-vector.hpp \
  ndn-cpp/util/logging.cpp ndn-cpp/util/logging.hpp

bin_test_encode_decode_forwarding_entry_SOURCES = tests/test-encode-decode-forwarding-entry.cpp
bin_test_encode_decode_forwarding_entry_LDADD = libndn-cpp.la

bin_test_encode_decode_interest_SOURCES = tests/test-encode-decode-interest.cpp
bin_test_encode_decode_interest_LDADD = libndn-cpp.la

bin_test_encode_decode_data_SOURCES = tests/test-encode-decode-data.cpp
bin_test_encode_decode_data_LDADD = libndn-cpp.la

bin_test_get_async_SOURCES = tests/test-get-async.cpp
bin_test_get_async_LDADD = libndn-cpp.la

bin_test_publish_async_SOURCES = tests/test-publish-async.cpp
bin_test_publish_async_LDADD = libndn-cpp.la

dist_noinst_SCRIPTS = autogen.sh
