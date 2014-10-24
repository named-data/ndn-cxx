#!/usr/bin/env bash
set -x
set -e

# Cleanup
sudo ./waf distclean -j1 --color=yes

COVERAGE=$( python -c "print '--with-coverage' if 'code-coverage' in '$JOB_NAME' else ''" )

sudo rm -Rf /usr/local/include/ndn-cxx
sudo rm -f /usr/local/lib/libndn-cxx*
sudo rm -f /usr/local/lib/pkgconfig/libndn-cxx*

# Configure
CXXFLAGS="-std=c++03 -pedantic -Wall -Wno-long-long -O2 -g -Werror" \
  ./waf -j1 configure --color=yes --with-tests --without-pch $COVERAGE

# Build
./waf --color=yes -j1

# Install
sudo ./waf -j1 --color=yes install
sudo ldconfig || true
