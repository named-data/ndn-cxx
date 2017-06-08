#!/usr/bin/env bash

# It's intentional not to use `set -x`, because this script explicitly prints useful information
# and should not run in trace mode.
# It's intentional not to use `set -e`, because this script wants to check all headers
# (similar to running all test cases), instead of failing at the first error.

CXX=${CXX:-g++}
STD=-std=c++11
CXXFLAGS="-O2 -Wall -Wno-unused-local-typedef $(pkg-config --cflags libndn-cxx)"
INCLUDEDIR="$(pkg-config --variable=includedir libndn-cxx)"/ndn-cxx

echo "Using: $CXX $STD $CXXFLAGS"

NCHECKED=0
NERRORS=0
while IFS= read -r -d '' H; do
  echo "Verifying standalone header compilation for ${H#${INCLUDEDIR}/}"
  "$CXX" -xc++ $STD $CXXFLAGS -c -o /dev/null "$H"
  [[ $? -eq 0 ]] || ((NERRORS++))
  ((NCHECKED++))
done < <(find "$INCLUDEDIR" -name '*.hpp' -type f -print0 2>/dev/null)

if [[ $NCHECKED -eq 0 ]]; then
  echo 'No header found. Is ndn-cxx installed?'
  exit 1
else
  echo "$NCHECKED headers checked."
fi

if [[ $NERRORS -gt 0 ]]; then
  echo "$NERRORS headers could not be compiled."
  exit 1
fi
