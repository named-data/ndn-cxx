#!/usr/bin/env bash
set -e

JDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source "$JDIR"/util.sh

set -x

sudo rm -f /usr/local/bin/ndnsec*
sudo rm -fr /usr/local/include/ndn-cxx
sudo rm -f /usr/local/lib{,64}/libndn-cxx*
sudo rm -f /usr/local/lib{,64}/pkgconfig/libndn-cxx.pc

if [[ $JOB_NAME == *"code-coverage" ]]; then
    COVERAGE="--with-coverage"
elif [[ -z $DISABLE_ASAN ]]; then
    ASAN="--with-sanitizer=address"
fi

# Cleanup
sudo_preserve_env PATH -- ./waf --color=yes distclean

if [[ $JOB_NAME != *"code-coverage" && $JOB_NAME != *"limited-build" ]]; then
    # Build static library in release mode with tests and without precompiled headers
    ./waf --color=yes configure --enable-static --disable-shared --with-tests --without-pch
    ./waf --color=yes build -j${WAF_JOBS:-1}

    # Cleanup
    sudo_preserve_env PATH -- ./waf --color=yes distclean

    # Build static and shared library in release mode without tests
    ./waf --color=yes configure --enable-static --enable-shared
    ./waf --color=yes build -j${WAF_JOBS:-1}

    # Cleanup
    sudo_preserve_env PATH -- ./waf --color=yes distclean
fi

# Build shared library in debug mode with tests and examples
./waf --color=yes configure --disable-static --enable-shared --debug --with-tests --with-examples $ASAN $COVERAGE
./waf --color=yes build -j${WAF_JOBS:-1}

# (tests will be run against the debug version)

# Install
sudo_preserve_env PATH -- ./waf --color=yes install

if has Linux $NODE_LABELS; then
    sudo ldconfig
fi
