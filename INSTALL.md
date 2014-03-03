NDN-CPP-dev:  A dev version of Named Data Networking client library for C++
===========================================================================

Prerequisites
-------------

(These are prerequisites to build NDN-CPP.  To do development of NDN-CPP code and update the build system, 
 see Development Prerequisites.)

Required: 

* libcrypto
* libsqlite3 
* libcrypto++
* boost libraries (>= 1.48)
* OSX Security framework (on OSX platform only)

Following are the detailed steps for each platform to install the prerequisites.

* Mac OS X 10.7.3, Mac OS X 10.8.4

    Install Xcode.
    In Xcode Preferences > Downloads, install "Command Line Tools".

* Mac OS X 10.9

    Install Xcode.  (Xcode on OS X 10.9 seems to already have the Command Line Tools.)

    If using macports, dependencies can be installed using the following commands:

        sudo port install boost sqlite3 libcryptopp

* Ubuntu 12.04, Ubuntu 13.10

    In a terminal, enter:

        sudo apt-get install build-essential
        # For Ubuntu 12.04
        sudo apt-get install libboost1.48-all-dev
        # For Ubuntu 13.10
        sudo apt-get install libboost-all-dev
        sudo apt-get install libssl-dev libsqlite3-dev libcrypto++-dev

* Windows Cygwin

    Cygwin is tested on Windows 7 64-bit with the "Devel" packages selected to install at the top level of the
    cygwin installer.  This includes libcrypto and libsqlite3.

Build
-----

(These are instructions to build NDN-CPP. To do development of NDN-CPP code and update the build system, see Development.)

To build in a terminal, change directory to the NDN-CPP root.  Enter:

    ./waf configure
    ./waf
    sudo ./waf install

This makes and installs the following items:

* ``<LIBPATH>/libndn-cpp-dev.*``: static and dynamic NDN C++ libraries
* ``<LIBPATH>/pkgconfig/libndn-cpp-dev.pc``: pkgconfig file storing all neccessary flags
  to build against the library.  For example, if pkgconfig or pkgconf package is
  installed and PKG_CONFIG_PATH is configured properly (or ``<LIBPATH>/pkgconfig`` is a
  default path), ``pkgconfig --libs --clflags libndn-cpp-dev`` will return all necessary
  compile and link flags for the library.
* ``<BINPATH>/tlvdump``: a simple tool to dump contents of TLV-formatted data
* ``<BINPATH>/ndncatchunks3``: a simplified equivalent to ndncatchunks2 in NDNx package
* ``<BINPATH>/ndnputchunks3``: a simplified equivalent to ndnputchunks2 in NDNx package

If configured with tests: ``./waf configure --with-tests``), the above commands will
also produce:

* ``build/tests/unit-tests``: A tool to run unit tests for the library

Documentation
-------------

To make documentation, enter:

    ./waf doxygen

This will produce doxygen API code documentation in:

* build/doc/html

Supported platforms
-------------------

(to be confirmed)

NDN-CPP is tested on the following platforms:

* Ubuntu 12.04 (64 bit and 32 bit)
* Ubuntu 13.10 (64 bit and 32 bit)
* Mac OS X 10.8
* Mac OS X 10.9

Development Prerequisites
-------------------------

These steps are only needed to do development of NDN-CPP code and update the build system.
First follow the Prerequisites above for your platforms.

* Mac OS X 10.7.3, Mac OS X 10.8.4, Mac OS X 10.9

    Install MacPorts from http://www.macports.org/install.php
    In a terminal, enter:

        sudo port install doxygen

* Ubuntu 12.04 (64 bit and 32 bit), Ubuntu 13.04 (64 bit)

    In a terminal, enter:

        sudo apt-get install doxygen

Development
-----------

Follow Development Prerequisites above for your platform.
