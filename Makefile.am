## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LIBRARIES = ndn-cpp.a

bin_PROGRAMS = test1

ndn_cpp_a_SOURCES = ndn-cpp/fields/name.h ndn-cpp/fields/name.cc

test1_SOURCES = ndn-cpp.a test/test1.cpp

dist_noinst_SCRIPTS = autogen.sh
