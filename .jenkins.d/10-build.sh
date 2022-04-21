#!/usr/bin/env bash
set -ex

git submodule sync
git submodule update --init

if [[ -z $DISABLE_ASAN ]]; then
    ASAN="--with-sanitizer=address"
fi
if [[ $JOB_NAME == *"code-coverage" ]]; then
    COVERAGE="--with-coverage"
fi
if has CentOS-8 $NODE_LABELS; then
    # https://bugzilla.redhat.com/show_bug.cgi?id=1721553
    PCH="--without-pch"
fi

if [[ $JOB_NAME != *"code-coverage" && $JOB_NAME != *"limited-build" ]]; then
    # Build static library in release mode with tests and without precompiled headers
    ./waf --color=yes configure --enable-static --disable-shared --with-tests --without-pch
    ./waf --color=yes build -j$WAF_JOBS

    # Cleanup
    ./waf --color=yes distclean

    # Build static and shared library in release mode without tests
    ./waf --color=yes configure --enable-static --enable-shared $PCH
    ./waf --color=yes build -j$WAF_JOBS

    # Cleanup
    ./waf --color=yes distclean
fi

# Build shared library in debug mode with tests and examples
./waf --color=yes configure --disable-static --enable-shared --debug --with-tests --with-examples $ASAN $COVERAGE $PCH
./waf --color=yes build -j$WAF_JOBS

# (tests will be run against the debug version)

# Install
sudo_preserve_env PATH -- ./waf --color=yes install

if has CentOS-8 $NODE_LABELS; then
    sudo tee /etc/ld.so.conf.d/ndn.conf >/dev/null <<< /usr/local/lib64
fi
if has Linux $NODE_LABELS; then
    sudo ldconfig
fi
