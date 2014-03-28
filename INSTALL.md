ndn-cpp-dev:  A dev version of Named Data Networking client library for C++
===========================================================================

Supported platforms
-------------------

ndn-cpp-dev is tested on the following platforms:

* Ubuntu 12.04 (64 bit and 32 bit)
* Ubuntu 13.10 (64 bit and 32 bit)
* Mac OS X 10.8
* Mac OS X 10.9


Prerequisites
-------------

(These are prerequisites to build ndn-cpp-dev.  To do development of ndn-cpp-dev code and update the build system,
 see Development Prerequisites.)

Required:

* `python` >= 2.6
* `libcrypto`
* `libsqlite3`
* `libcrypto++`
* `pkg-config`
* Boost libraries >= 1.48
* OSX Security framework (on OSX platform only)

Following are the detailed steps for each platform to install the prerequisites.

* Mac OS X

    Install Xcode.
    In Xcode Preferences > Downloads, install "Command Line Tools".

    If using macports, dependencies can be installed using the following commands:

        sudo port install pkgconfig boost sqlite3 libcryptopp

* Ubuntu 12.04, Ubuntu 13.10

    In a terminal, enter:

        sudo apt-get install build-essential
        sudo apt-get install libssl-dev libsqlite3-dev libcrypto++-dev

        # For Ubuntu 12.04
        sudo apt-get install libboost1.48-all-dev

        # For Ubuntu 13.10
        sudo apt-get install libboost-all-dev

Build
-----

(These are instructions to build ndn-cpp-dev. To do development of ndn-cpp-dev
code and update the build system, see Development.)

To build in a terminal, change directory to the ndn-cpp-dev root.  Enter:

    ./waf configure
    ./waf
    sudo ./waf install

This builds and installs the following items:

* `<LIBPATH>/libndn-cpp-dev.a`: static NDN C++ library
* `<LIBPATH>/pkgconfig/libndn-cpp-dev.pc`: pkgconfig file storing all neccessary flags
  to build against the library.  For example, if pkgconfig or pkgconf package is
  installed and `PKG_CONFIG_PATH` is configured properly (or `<LIBPATH>/pkgconfig` is a
  default path), `pkgconfig --libs --clflags libndn-cpp-dev` will return all necessary
  compile and link flags for the library.
* `<BINPATH>/tlvdump`: a simple tool to dump contents of TLV-formatted data
* `<BINPATH>/ndncatchunks3`: a simplified equivalent to ndncatchunks2 in NDNx package
* `<BINPATH>/ndnputchunks3`: a simplified equivalent to ndnputchunks2 in NDNx package
* `<BINPATH>/ndnsec`: tool to manage NDN keys and certificates
* `<BINPATH>/ndnsec-*`: convenience scripts for `ndnsec` tools

If configured with tests: `./waf configure --with-tests`), the above commands will
also produce:

* `build/unit-tests`: A unit test binary for the library

Documentation
-------------

To make documentation, enter:

    ./waf doxygen

This will produce doxygen API code documentation in:

* `build/doc/html`

Development Prerequisites
-------------------------

These steps are only needed to do development of ndn-cpp-dev code and update the build system.
First follow the Prerequisites above for your platforms.

* Mac OS X 10.7.3, Mac OS X 10.8.4, Mac OS X 10.9

    Install MacPorts from http://www.macports.org/install.php
    In a terminal, enter:

        sudo port install doxygen

* Ubuntu 12.04 (64 bit and 32 bit), Ubuntu 13.04 (64 bit)

    In a terminal, enter:

        sudo apt-get install doxygen

Development Build
-----------------

The following is the suggested configure commands for development build.

    ./waf configure --debug --with-tests
    ./waf
    sudo ./waf install

In the development build all compiler optimizations are disabled by default and all warnings are treated as error.
The default behavior can be overridden by setting CXXFLAGS environment variable before running `./waf configure`:

    CXXFLAGS="-O1 -g3" ./waf configure --debug --with-tests
    ...
