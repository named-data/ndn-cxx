Notes for ndn-cxx developers
============================

If you are new to the NDN community of software generally, read the
[Contributor's Guide](https://github.com/named-data/NFD/blob/master/CONTRIBUTING.md).

Code style
----------

ndn-cxx code is subject to [ndn-cxx code style](https://named-data.net/doc/ndn-cxx/current/code-style.html).

Licensing
---------

Contributions to the library must be licensed under the LGPL v3 or compatible license.  If
you are choosing LGPL v3, please use the following license boilerplate in all `.hpp` and
`.cpp` files:

    /* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
    /*
     * Copyright (c) [Year(s)] [Copyright Holder(s)].
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
     ////// [optional part] //////
     *
     * @author Author's Name <email@domain-or-homepage://url>
     * @author Other Author's Name <another.email@domain-or-homepage://url>
     ////// [end of optional part] //////
     */

If you are affiliated to an NSF-supported NDN project institution, please use the [NDN Team License
Boilerplate](https://redmine.named-data.net/projects/ndn-cxx/wiki/NDN_Team_License_Boilerplate_(ndn-cxx)).

Running unit tests
------------------

To run the unit tests, ndn-cxx needs to be built with unit test support and installed
into the configured location.  For example:

    ./waf configure --with-tests # --debug is also strongly recommended while developing
    ./waf
    sudo ./waf install

**Note**: On Linux you also need to run `sudo ldconfig` to reconfigure dynamic loader
run-time bindings.

The simplest way to run the tests is to launch the compiled binary without any parameters:

    ./build/unit-tests

[Boost.Test framework](https://www.boost.org/doc/libs/1_58_0/libs/test/doc/html/index.html)
is very flexible and allows a number of run-time customization of what tests should be run.
For example, it is possible to choose to run only a specific test suite, only a specific
test case within a suite, or specific test cases within specific test suites:

    # Run only Face test suite tests (tests/unit/face.t.cpp)
    ./build/unit-tests -t TestFace

    # Run only test case ExpressInterestData from the same test suite
    ./build/unit-tests -t TestFace/ExpressInterestData

    # Run Basic test case from all test suites
    ./build/unit-tests -t */Basic

By default, Boost.Test framework will produce verbose output only when a test case fails.
If it is desired to see verbose output (result of each test assertion), add `-l all`
option to `./build/unit-tests` command.  To see test progress, you can use `-l test_suite`,
or `-p` to show a progress bar:

    # Show report all log messages including the passed test notification
    ./build/unit-tests -l all

    # Show test suite messages
    ./build/unit-tests -l test_suite

    # Show nothing
    ./build/unit-tests -l nothing

    # Show progress bar
    ./build/unit-tests -p

There are many more command line options available, information about which can be obtained
either from the command line using `--help` switch, or online on
[Boost.Test library](https://www.boost.org/doc/libs/1_58_0/libs/test/doc/html/index.html)
website.

**Warning:** If you have customized parameters for NDN platform using `client.conf` in
`/etc/ndn` or `/usr/local/etc/ndn` (or other `@SYSCONFDIR@/etc` if it was configured to custom
path during `./waf configure`), Face-related test cases may fail.
