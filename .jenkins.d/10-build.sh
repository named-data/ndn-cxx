#!/usr/bin/env bash
set -eo pipefail

if [[ -z $DISABLE_ASAN ]]; then
    ASAN="--with-sanitizer=address"
fi
if [[ -n $GITHUB_ACTIONS && $ID == macos && ${VERSION_ID%%.*} -le 12 ]]; then
    KEYCHAIN="--with-osx-keychain"
fi

set -x

if [[ $JOB_NAME != *"code-coverage" && $JOB_NAME != *"limited-build" ]]; then
    # Build static library in release mode with tests and without precompiled headers
    ./waf --color=yes configure --enable-static --disable-shared --with-tests --without-pch
    ./waf --color=yes build

    # Cleanup
    ./waf --color=yes distclean

    # Build shared library in release mode with examples and benchmarks
    ./waf --color=yes configure --disable-static --enable-shared --with-examples --with-benchmarks
    ./waf --color=yes build

    # Cleanup
    ./waf --color=yes distclean
fi

if [[ $JOB_NAME == *"code-coverage" ]]; then
    # Build for coverage testing: enable instrumentation and unit tests only
    ./waf --color=yes configure --debug --with-coverage --with-unit-tests --without-tools
    ./waf --color=yes build
else
    # Build shared library in debug mode with tests
    ./waf --color=yes configure --disable-static --enable-shared --debug --with-tests $ASAN $KEYCHAIN
    ./waf --color=yes build
fi

# Install
sudo ./waf --color=yes install

if [[ $ID_LIKE == *linux* ]]; then
    if [[ $(uname -m) == x86_64 && -d /usr/lib64 ]]; then
        sudo tee /etc/ld.so.conf.d/ndn.conf >/dev/null <<< /usr/local/lib64
    fi
    sudo ldconfig
fi
