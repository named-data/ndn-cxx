#!/usr/bin/env bash
set -e

JDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source "$JDIR"/util.sh

set -x

sudo rm -f /usr/local/bin/ndnsec*
sudo rm -fr /usr/local/include/ndn-cxx
sudo rm -f /usr/local/lib/libndn-cxx*
sudo rm -f /usr/local/lib/pkgconfig/libndn-cxx.pc

# Cleanup
sudo env "PATH=$PATH" ./waf --color=yes distclean

if [[ $JOB_NAME != *"code-coverage" && $JOB_NAME != *"limited-build" ]]; then
  # Configure/build static library in optimized mode with tests
  ./waf --color=yes configure --enable-static --disable-shared --with-tests
  ./waf --color=yes build -j${WAF_JOBS:-1}

  # Cleanup
  sudo env "PATH=$PATH" ./waf --color=yes distclean

  # Configure/build static and shared library in optimized mode without tests
  ./waf --color=yes configure --enable-static --enable-shared
  ./waf --color=yes build -j${WAF_JOBS:-1}

  # Cleanup
  sudo env "PATH=$PATH" ./waf --color=yes distclean
fi

# Configure/build shared library in debug mode with tests/examples and without precompiled headers
if [[ $JOB_NAME == *"code-coverage" ]]; then
    COVERAGE="--with-coverage"
elif [[ -n $BUILD_WITH_ASAN || -z $TRAVIS ]]; then
    ASAN="--with-sanitizer=address"
fi
./waf --color=yes configure --disable-static --enable-shared --debug --with-tests --with-examples --without-pch $COVERAGE $ASAN
./waf --color=yes build -j${WAF_JOBS:-1}

# (tests will be run against debug version)

# Install
sudo env "PATH=$PATH" ./waf --color=yes install

if has Linux $NODE_LABELS; then
    sudo ldconfig
elif has FreeBSD $NODE_LABELS; then
    sudo ldconfig -a
fi
