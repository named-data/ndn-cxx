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
./build/unit-tests -l test_suite
