ndn-cxx: NDN C++ library with eXperimental eXtensions
===========================================================================

Supported platforms
-------------------

ndn-cxx uses continuous integration testing and has been tested on the following platforms:

* Ubuntu 12.04 (64 bit and 32 bit)
* Ubuntu 13.10 (64 bit and 32 bit)
* Mac OS X 10.8
* Mac OS X 10.9


Prerequisites
-------------

### Required:

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

### Optional:

To build tutorials, manpages, and API documentation the following
dependencies need to be installed:

* `doxygen`
* `graphviz`
* `python-sphinx`

The following lists steps for common platforms to install these prerequisites:

* On OS X 10.8 and 10.9 with macports:

        sudo port install doxygen graphviz py27-sphinx sphinx_select
        sudo port select sphinx py27-sphinx

* On Ubuntu >= 12.04:

        sudo apt-get install doxygen graphviz python-sphinx


Build
-----

(These are instructions to build ndn-cxx. To do development of ndn-cxx
code and update the build system, see Development.)

To build in a terminal, change directory to the ndn-cxx root.  Enter:

    ./waf configure
    ./waf
    sudo ./waf install

This builds and installs the following items:

* `<LIBPATH>/libndn-cxx.a`: static NDN C++ library
* `<LIBPATH>/pkgconfig/libndn-cxx.pc`: pkgconfig file storing all neccessary flags
  to build against the library.  For example, if pkgconfig or pkgconf package is
  installed and `PKG_CONFIG_PATH` is configured properly (or `<LIBPATH>/pkgconfig` is a
  default path), `pkgconfig --libs --clflags libndn-cxx` will return all necessary
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

ndn-cxx tutorials and API documentation can be built using the following commands:

    # Full set of documentation (tutorials + API) in build/docs
    ./waf docs

    # Only tutorials in `build/docs`
    ./waf sphinx

    # Only API docs in `build/docs/doxygen`
    ./waf doxgyen


Manpages are automatically created and installed during the normal build process
(e.g., during `./waf` and `./waf install`), if `python-sphinx` module is detected during
`./waf configure` stage.  By default, manpages are installed into `${PREFIX}/share/man`
(where default value for `PREFIX` is `/usr/local`).  This location can be changed during
`./waf configure` stage using `--prefix`, `--datarootdir`, or `--mandir` options.

For more details, refer to `./waf --help`.




Development Build
-----------------

The following is the suggested configure commands for development build.

    ./waf configure --debug --with-tests
    ./waf
    sudo ./waf install

In the development build all compiler optimizations are disabled by default and all
warnings are treated as error.  The default behavior can be overridden by setting `CXXFLAGS`
environment variable before running `./waf configure`:

    CXXFLAGS="-O1 -g3" ./waf configure --debug --with-tests
    ...
