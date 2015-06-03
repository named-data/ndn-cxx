#!/usr/bin/env bash
set -x
set -e

JDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source "$JDIR"/util.sh

# Prepare environment
rm -Rf ~/.ndn

if has OSX $NODE_LABELS; then
  echo "Unlocking OSX Keychain"
  security unlock-keychain -p "named-data"
fi

ndnsec-keygen "/tmp/jenkins/$NODE_NAME" | ndnsec-install-cert -

# Run unit tests
if [[ -n "$XUNIT" ]]; then
    ./build/unit-tests --log_format=XML --log_sink=build/xunit-report.xml --log_level=all --report_level=no
else
    ./build/unit-tests -l test_suite
fi
