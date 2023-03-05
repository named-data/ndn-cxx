#!/usr/bin/env bash
set -eo pipefail

# https://github.com/google/sanitizers/wiki/AddressSanitizerFlags
ASAN_OPTIONS="color=always"
ASAN_OPTIONS+=":check_initialization_order=1"
ASAN_OPTIONS+=":detect_stack_use_after_return=1"
ASAN_OPTIONS+=":strict_init_order=1"
ASAN_OPTIONS+=":strict_string_checks=1"
ASAN_OPTIONS+=":detect_invalid_pointer_pairs=2"
ASAN_OPTIONS+=":strip_path_prefix=${PWD}/"
export ASAN_OPTIONS

# https://www.boost.org/doc/libs/release/libs/test/doc/html/boost_test/runtime_config/summary.html
export BOOST_TEST_BUILD_INFO=1
export BOOST_TEST_COLOR_OUTPUT=1
export BOOST_TEST_DETECT_MEMORY_LEAK=0
export BOOST_TEST_LOGGER=HRF,test_suite,stdout:XML,all,build/xunit-log.xml

set -x

# Prepare environment
rm -rf ~/.ndn

# Run unit tests
./build/unit-tests
