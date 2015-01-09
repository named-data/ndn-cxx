#!/usr/bin/env bash

# It's intentional not to use `set -x`, because this script explicitly prints useful information
# and should not run in trace mode.
# It's intentional not to use `set -e`, because this script wants to check all headers
# (similar to running all test cases), instead of failing at the first error.

CXX=${CXX:-g++}

STD=
if $CXX -xc++ /dev/null -o /dev/null -c -std=c++11 &>/dev/null; then
  STD=-std=c++11
elif $CXX  -xc++ /dev/null -o /dev/null -c -std=c++0x &>/dev/null; then
  STD=-std=c++0x
fi

STDLIB=
if $CXX -xc++ /dev/null -o /dev/null -c $STD -stdlib=libc++ &>/dev/null; then
  STDLIB=-stdlib=libc++
fi

echo 'Compiler flags:' $CXX $STD $STDLIB

INCLUDEDIR=/usr/local/include/ndn-cxx
NCHECKED=0
NERRORS=0
while IFS= read -r -d '' H; do
  echo 'Verifying standalone header compilation for' $H
  $CXX -xc++ "$INCLUDEDIR/$H" -o /dev/null -c $STD $STDLIB $(pkg-config --cflags libndn-cxx)
  [[ $? -eq 0 ]] || ((NERRORS++))
  ((NCHECKED++))
done < <(cd "$INCLUDEDIR" && find * -name '*.hpp' -type f -print0 2>/dev/null)

if [[ $NCHECKED -eq 0 ]]; then
  echo 'No header found. Is ndn-cxx installed?'
  exit 1
fi

if [[ $NERRORS -gt 0 ]]; then
  echo $NERRORS 'headers cannot compile on its own'
  exit 1
fi
