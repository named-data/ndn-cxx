# Notes for ndn-cxx developers

If you are new to the NDN software community, please read our
[Contributor's Guide](https://github.com/named-data/.github/blob/main/CONTRIBUTING.md).

## Code style

ndn-cxx code is subject to [ndn-cxx code style](https://docs.named-data.net/ndn-cxx/current/code-style.html).

## Licensing

Contributions to ndn-cxx must be licensed under the LGPL v3 or a compatible license.
If you choose the LGPL v3, please use the following license boilerplate in all `.hpp`
and `.cpp` files:

```cpp
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) [Year(s)], [Copyright Holder(s)].
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */
```

If you are affiliated to an NSF-supported NDN project institution, please use the [NDN Team License
Boilerplate](https://redmine.named-data.net/projects/ndn-cxx/wiki/NDN_Team_License_Boilerplate_(ndn-cxx)).

## Unit tests

To run the unit tests, ndn-cxx needs to be built with unit test support and installed
into the configured location.  For example:

```shell
./waf configure --with-tests # --debug is also strongly recommended while developing
./waf
sudo ./waf install
```

> [!TIP]
> On Linux you may also need to run `sudo ldconfig` to reconfigure the dynamic linker bindings.

The simplest way to run the tests is to launch the compiled binary without any parameters:

```shell
./build/unit-tests
```

The [Boost.Test framework](https://www.boost.org/doc/libs/1_74_0/libs/test/doc/html/index.html)
is very flexible and allows a number of run-time customization of what tests should be run.
For example, it is possible to choose to run only a specific test suite, only a specific
test case within a suite, specific test cases across multiple test suites, and so on:

```shell
# Run all the test cases inside the Face test suite (tests/unit/face.t.cpp)
./build/unit-tests -t TestFace

# Run only the test case "ExpressInterestData" from the previous test suite
./build/unit-tests -t TestFace/ExpressInterestData

# Run the "Basic" test case from all test suites
./build/unit-tests -t */Basic
```

By default, Boost.Test framework will produce verbose output only when a test case fails.
If it is desired to see verbose output (result of each test assertion), add `-l all`
option to `./build/unit-tests` command.  To see test progress, you can use `-l test_suite`,
or `-p` to show a progress bar:

```shell
# Show report all log messages including the passed test notification
./build/unit-tests -l all

# Show test suite messages
./build/unit-tests -l test_suite

# Show nothing
./build/unit-tests -l nothing

# Show progress bar
./build/unit-tests -p
```

There are many more command line options available, information about which can be obtained
either from the command line using the `--help` switch, or online on the
[Boost.Test website](https://www.boost.org/doc/libs/1_74_0/libs/test/doc/html/boost_test/runtime_config/summary.html).

> [!WARNING]
> If you have a customized `client.conf` in `~/.ndn`, `/etc/ndn`, or `/usr/local/etc/ndn`
> (or any other `SYSCONFDIR/ndn` if you passed `--sysconfdir` to `./waf configure`),
> Face-related test cases may fail.
