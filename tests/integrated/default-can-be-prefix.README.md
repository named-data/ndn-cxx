# DefaultCanBePrefix test

`default-can-be-prefix-*.cpp` verifies the effect of `Interest::setDefaultCanBePrefix`.
They are written as integration tests because ndn-cxx unit tests are prohibited from calling `Interest::setDefaultCanBePrefix`.

Manual verification steps:

1. `default-can-be-prefix-unset` program should print a "CanBePrefix unset" warning to stderr.
2. `default-can-be-prefix-0` and `default-can-be-prefix-1` test cases should not print that warning.

