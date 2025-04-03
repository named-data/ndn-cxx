#!/usr/bin/env bash
set -eo pipefail

[[ $JOB_NAME == *code-coverage ]] || exit 0

export FORCE_COLOR=1
export UV_NO_MANAGED_PYTHON=1

set -x

# Generate a detailed HTML report and an XML report in Cobertura format using gcovr
# Note: trailing slashes are important in the paths below. Do not remove them!
uvx --from 'git+https://github.com/gcovr/gcovr@99b82e7' gcovr \
    --decisions \
    --filter ndn-cxx/ \
    --exclude ndn-cxx/detail/nonstd/ \
    --exclude-throw-branches \
    --exclude-unreachable-branches \
    --cobertura build/coverage.xml \
    --html-details build/gcovr/ \
    --txt-summary \
    build

# Generate a detailed HTML report using lcov
lcov \
    --quiet \
    --capture \
    --directory . \
    --include "$PWD/ndn-cxx/*" \
    --exclude "$PWD/ndn-cxx/detail/nonstd/*" \
    --ignore-errors count,inconsistent \
    --branch-coverage \
    --rc no_exception_branch=1 \
    --output-file build/coverage.info

genhtml \
    --quiet \
    --branch-coverage \
    --demangle-cpp \
    --legend \
    --missed \
    --show-proportion \
    --title "ndn-cxx $(cat VERSION.info)" \
    --output-directory build/lcov \
    build/coverage.info
