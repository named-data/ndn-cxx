include aminclude.am

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LTLIBRARIES = libndn-c.la libndn-cpp.la

bin_PROGRAMS = bin/test-encode-decode-forwarding-entry bin/test-encode-decode-interest bin/test-encode-decode-data \
  bin/test-get-async bin/test-publish-async

libndn_c_la_SOURCES = \
  config.h \
  ndn-cpp/c/data.h \
  ndn-cpp/c/errors.c ndn-cpp/c/errors.h \
  ndn-cpp/c/forwarding-entry.h \
  ndn-cpp/c/interest.c ndn-cpp/c/interest.h \
  ndn-cpp/c/key.h \
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
  ndn-cpp/c/util/dynamic-uchar-array.c ndn-cpp/c/util/dynamic-uchar-array.h \
  ndn-cpp/c/util/ndn_memory.c ndn-cpp/c/util/ndn_memory.h \
  ndn-cpp/c/util/ndn_realloc.c ndn-cpp/c/util/ndn_realloc.h

libndn_cpp_la_SOURCES = \
  config.h \
  ndn-cpp/common.hpp ndn-cpp/common.cpp \
  ndn-cpp/data.cpp ndn-cpp/c/data.h ndn-cpp/data.hpp \
  ndn-cpp/face.cpp ndn-cpp/face.hpp \
  ndn-cpp/forwarding-entry.cpp ndn-cpp/c/forwarding-entry.h ndn-cpp/forwarding-entry.hpp \
  ndn-cpp/interest.cpp ndn-cpp/c/interest.h ndn-cpp/interest.hpp \
  ndn-cpp/key.cpp ndn-cpp/c/key.h ndn-cpp/key.hpp \
  ndn-cpp/name.cpp ndn-cpp/c/name.h ndn-cpp/name.hpp \
  ndn-cpp/node.cpp ndn-cpp/node.hpp \
  ndn-cpp/publisher-public-key-digest.hpp \
  ndn-cpp/sha256-with-rsa-signature.cpp ndn-cpp/sha256-with-rsa-signature.hpp \
  ndn-cpp/c/encoding/binary-xml.h \
  ndn-cpp/c/encoding/binary-xml-decoder.h ndn-cpp/encoding/binary-xml-decoder.hpp \
  ndn-cpp/c/encoding/binary-xml-element-reader.h ndn-cpp/encoding/binary-xml-element-reader.cpp ndn-cpp/encoding/binary-xml-element-reader.hpp \
  ndn-cpp/c/encoding/binary-xml-encoder.h ndn-cpp/encoding/binary-xml-encoder.hpp ndn-cpp/c/util/dynamic-uchar-array.h \
  ndn-cpp/c/encoding/binary-xml-structure-decoder.h ndn-cpp/encoding/binary-xml-structure-decoder.hpp \
  ndn-cpp/encoding/binary-xml-wire-format.cpp ndn-cpp/c/encoding/binary-xml-name.h ndn-cpp/encoding/binary-xml-wire-format.hpp \
  ndn-cpp/encoding/wire-format.cpp ndn-cpp/encoding/wire-format.hpp \
  ndn-cpp/security/security-common.hpp \
  ndn-cpp/security/key-chain.cpp ndn-cpp/security/key-chain.hpp \
  ndn-cpp/security/identity/identity-manager.cpp ndn-cpp/security/identity/identity-manager.hpp \
  ndn-cpp/security/identity/private-key-storage.cpp ndn-cpp/identity/security/private-key-storage.hpp \
  ndn-cpp/c/transport/socket-transport.h ndn-cpp/c/transport/tcp-transport.h ndn-cpp/transport/tcp-transport.cpp ndn-cpp/transport/tcp-transport.hpp \
  ndn-cpp/transport/transport.cpp ndn-cpp/transport/transport.hpp \
  ndn-cpp/c/transport/socket-transport.h ndn-cpp/c/transport/udp-transport.h ndn-cpp/transport/udp-transport.cpp ndn-cpp/transport/udp-transport.hpp \
  ndn-cpp/util/blob.hpp \
  ndn-cpp/util/changed-event.cpp ndn-cpp/util/changed-event.hpp \
  ndn-cpp/util/dynamic-uchar-vector.cpp ndn-cpp/util/dynamic-uchar-vector.hpp \
  ndn-cpp/util/logging.cpp ndn-cpp/util/logging.hpp \
  ndn-cpp/util/signed-blob.hpp

bin_test_encode_decode_forwarding_entry_SOURCES = tests/test-encode-decode-forwarding-entry.cpp
bin_test_encode_decode_forwarding_entry_LDADD = libndn-cpp.la libndn-c.la

bin_test_encode_decode_interest_SOURCES = tests/test-encode-decode-interest.cpp
bin_test_encode_decode_interest_LDADD = libndn-cpp.la libndn-c.la

bin_test_encode_decode_data_SOURCES = tests/test-encode-decode-data.cpp
bin_test_encode_decode_data_LDADD = libndn-cpp.la libndn-c.la

bin_test_get_async_SOURCES = tests/test-get-async.cpp
bin_test_get_async_LDADD = libndn-cpp.la libndn-c.la

bin_test_publish_async_SOURCES = tests/test-publish-async.cpp
bin_test_publish_async_LDADD = libndn-cpp.la libndn-c.la

dist_noinst_SCRIPTS = autogen.sh
