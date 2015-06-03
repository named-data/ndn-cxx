#!/usr/bin/env bash
set -x
set -e

JDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source "$JDIR"/util.sh

if has code-coverage $JOB_NAME; then
  BASE="`pwd | sed -e 's|/|\\\/|g'`\\"
  (cd build && gcovr -x -f $BASE/src -r ../ -o coverage.xml -b ./)
fi
