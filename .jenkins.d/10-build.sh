#!/usr/bin/env bash
set -e

JDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source "$JDIR"/util.sh

set -x

sudo rm -Rf /usr/local/include/ndn-cxx
sudo rm -f /usr/local/lib/libndn-cxx*
sudo rm -f /usr/local/lib/pkgconfig/libndn-cxx*

# Cleanup
sudo ./waf -j1 --color=yes distclean

if [[ "$JOB_NAME" != *"limited-build" ]]; then
  # Configure/build static library in optimized mode with tests
  ./waf -j1 --color=yes configure --enable-static --disable-shared --with-tests
  ./waf -j1 --color=yes build

  # Cleanup
  sudo ./waf -j1 --color=yes distclean

  # Configure/build static and shared library in optimized mode without tests
  ./waf -j1 --color=yes configure --enable-static --enable-shared
  ./waf -j1 --color=yes build

  # Cleanup
  sudo ./waf -j1 --color=yes distclean
fi

# Configure/build shared library in debug mode with tests/examples and without precompiled headers
if [[ "$JOB_NAME" == *"code-coverage" ]]; then
    COVERAGE="--with-coverage"
elif ! has OSX-10.9 $NODE_LABELS && ! has OSX-10.11 $NODE_LABELS; then
    ASAN="--with-sanitizer=address"
fi
./waf -j1 --color=yes configure --disable-static --enable-shared --debug --with-tests --with-examples --without-pch $COVERAGE $ASAN
./waf -j1 --color=yes build

# (tests will be run against debug version)

# Install
sudo ./waf -j1 --color=yes install

if has Linux $NODE_LABELS; then
    sudo ldconfig
elif has FreeBSD $NODE_LABELS; then
    sudo ldconfig -a
fi
