#!/usr/bin/env bash
set -e

JDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source "$JDIR"/util.sh

set -x

if [[ $JOB_NAME == *"code-coverage" ]]; then
    gcovr --object-directory=build \
          --output=build/coverage.xml \
          --filter="$PWD/src" \
          --root=. \
          --xml

    # Generate also a detailed HTML output, but using lcov (better results)
    lcov --quiet \
         --capture --no-external \
         --directory . \
         --rc lcov_branch_coverage=1 \
         --output-file build/coverage-with-tests.info

    lcov --quiet \
         --remove build/coverage-with-tests.info "$PWD/tests/*" \
         --rc lcov_branch_coverage=1 \
         --output-file build/coverage.info

    genhtml --legend \
            --rc genhtml_branch_coverage=1 \
            build/coverage.info \
            --output-directory build/coverage
fi
