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

BOOST_VERSION=$(python -c "import sys; sys.path.append('build/c4che'); import _cache; print(_cache.BOOST_VERSION_NUMBER);")

if (( BOOST_VERSION < 106200 )); then
    if [[ -n "$XUNIT" ]]; then
        UNIT_TEST_PARAMS1="--log_level=all"
        UNIT_TEST_PARAMS2="--log_format2=XML --log_sink2=build/xunit-report.xml"
        if (( BOOST_VERSION < 106000 )); then
            UNIT_TEST_PARAMS="$UNIT_TEST_PARAMS1 $UNIT_TEST_PARAMS2"
        else
            UNIT_TEST_PARAMS="$UNIT_TEST_PARAMS1 -- $UNIT_TEST_PARAMS2"
        fi
    else
        UNIT_TEST_PARAMS="--log_level=test_suite"
    fi
else
    UNIT_TEST_PARAMS="--logger=HRF,test_suite,stdout:XML,all,build/xunit-report.xml"
fi


# Run unit tests
./build/unit-tests $UNIT_TEST_PARAMS
