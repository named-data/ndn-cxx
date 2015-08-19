#!/usr/bin/env bash
set -x
set -e

JDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source "$JDIR"/util.sh

sudo rm -Rf /usr/local/include/ndn-cxx
sudo rm -f /usr/local/lib/libndn-cxx*
sudo rm -f /usr/local/lib/pkgconfig/libndn-cxx*

# Cleanup
sudo ./waf -j1 --color=yes distclean

# Configure/build static library in debug mode with precompiled headers
./waf -j1 --color=yes configure --enable-static --disable-shared --with-tests --debug
./waf -j1 --color=yes build

# Cleanup
sudo ./waf -j1 --color=yes distclean

# Configure/build static and shared library in optimized mode without tests with precompiled headers
./waf -j1 --color=yes configure --enable-shared --enable-static
./waf -j1 --color=yes build

# Cleanup
sudo ./waf -j1 --color=yes distclean

# Configure/build shared library in debug mode with examples without precompiled headers
if [[ "$JOB_NAME" == *"code-coverage" ]]; then
    COVERAGE="--with-coverage"
fi
./waf -j1 --color=yes configure --debug --enable-shared --disable-static --with-tests --without-pch --with-examples $COVERAGE
./waf -j1 --color=yes build

# (tests will be run against debug version)

# Install
sudo ./waf -j1 --color=yes install

if has Linux $NODE_LABELS; then
    sudo ldconfig
elif has FreeBSD $NODE_LABELS; then
    sudo ldconfig -a
fi
