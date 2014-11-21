#!/usr/bin/env bash
set -x
set -e

# Prepare environment
rm -Rf ~/.ndnx ~/.ndn

echo $NODE_LABELS
IS_OSX=$( python -c "print 'yes' if 'OSX' in '$NODE_LABELS'.strip().split(' ') else 'no'" )

if [[ $IS_OSX == "yes" ]]; then
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
