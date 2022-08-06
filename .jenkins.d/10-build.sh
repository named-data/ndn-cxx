#!/usr/bin/env bash
set -eo pipefail

if [[ -z $DISABLE_ASAN ]]; then
    ASAN="--with-sanitizer=address"
fi
if [[ $JOB_NAME == *"code-coverage" ]]; then
    COVERAGE="--with-coverage"
fi
if [[ $ID == macos && ${VERSION_ID%%.*} -ge 12 ]]; then
    KEYCHAIN="--without-osx-keychain"
fi
if [[ -n $DISABLE_PCH ]]; then
    PCH="--without-pch"
fi

set -x

if [[ $JOB_NAME != *"code-coverage" && $JOB_NAME != *"limited-build" ]]; then
    # Build static library in release mode with tests and without precompiled headers
    ./waf --color=yes configure --enable-static --disable-shared --with-tests --without-pch
    ./waf --color=yes build

    # Cleanup
    ./waf --color=yes distclean

    # Build static and shared library in release mode without tests
    ./waf --color=yes configure --enable-static --enable-shared $PCH
    ./waf --color=yes build

    # Cleanup
    ./waf --color=yes distclean
fi

# Build shared library in debug mode with tests and examples
./waf --color=yes configure --disable-static --enable-shared --debug --with-tests --with-examples $ASAN $COVERAGE $KEYCHAIN $PCH
./waf --color=yes build

# (tests will be run against the debug version)

# Install
sudo ./waf --color=yes install

if [[ $ID_LIKE == *fedora* ]]; then
    sudo tee /etc/ld.so.conf.d/ndn.conf >/dev/null <<< /usr/local/lib64
fi
if [[ $ID_LIKE == *linux* ]]; then
    sudo ldconfig
fi
