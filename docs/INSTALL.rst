.. _Getting Started with ndn-cxx:

Getting started with ndn-cxx
============================

Supported platforms
-------------------

ndn-cxx uses continuous integration and has been tested on the following
platforms:

-  Ubuntu 12.04 (64-bit and 32-bit)
-  Ubuntu 13.10 (64-bit and 32-bit)
-  OS X 10.8
-  OS X 10.9

ndn-cxx is known to work on the following platforms, although they are not officially
supported:

-  Ubuntu 14.04
-  Fedora >= 20
-  FreeBSD >= 10.0
-  Raspberry Pi

Prerequisites
-------------

Required:
~~~~~~~~~

-  ``python`` >= 2.6
-  ``libcrypto``
-  ``libsqlite3``
-  ``libcrypto++``
-  ``pkg-config``
-  Boost libraries >= 1.48
-  OSX Security framework (on OSX platform only)

Following are the detailed steps for each platform to install the compiler, all necessary
development tools and libraries, and ndn-cxx prerequisites.

-  OS X

   Install Xcode. In Xcode Preferences > Downloads, install "Command
   Line Tools".

   If using MacPorts, dependencies can be installed using the following
   commands::

       sudo port install pkgconfig boost sqlite3 libcryptopp

-  Ubuntu 12.04, Ubuntu 13.10

   In a terminal, enter::

       sudo apt-get install build-essential
       sudo apt-get install libssl-dev libsqlite3-dev libcrypto++-dev

       # For Ubuntu 12.04
       sudo apt-get install libboost1.48-all-dev

       # For Ubuntu 13.10
       sudo apt-get install libboost-all-dev

- Fedora >=20

   In a terminal, enter::

       sudo yum install gcc-g++ git
       sudo yum install openssl-devel sqlite-devel cryptopp-devel boost-devel

Optional:
~~~~~~~~~

To build tutorials, manpages, and API documentation the following
dependencies need to be installed:

-  ``doxygen``
-  ``graphviz``
-  ``python-sphinx`` and sphinx extensions ``sphinxcontrib-doxylink``,
   ``sphinxcontrib-googleanalytics``

The following lists steps for common platforms to install these prerequisites:

-  On OS X 10.8 and 10.9 with MacPorts::

       sudo port install doxygen graphviz py27-sphinx sphinx_select
       sudo port select sphinx py27-sphinx

       # Install sphinx extensions
       sudo port install py27-pip
       sudo port select pip pip27
       sudo pip install sphinxcontrib-doxylink sphinxcontrib-googleanalytics

-  On Ubuntu >= 12.04::

       sudo apt-get install doxygen graphviz python-sphinx python-pip
       sudo pip install sphinxcontrib-doxylink sphinxcontrib-googleanalytics

-  On Fedora >= 20::

       sudp yum install doxygen graphviz python-sphinx
       sudo pip install sphinxcontrib-doxylink sphinxcontrib-googleanalytics

Build
-----

(These are instructions to build ndn-cxx. To do development of ndn-cxx
code and update the build system, see Development.)

To build in a terminal, change directory to the ndn-cxx root. Enter:

::

    ./waf configure
    ./waf
    sudo ./waf install

This builds and installs the following items:

-  ``<LIBPATH>/libndn-cxx.a``: static NDN C++ library
-  ``<LIBPATH>/pkgconfig/libndn-cxx.pc``: pkgconfig file storing all
   neccessary flags to build against the library. For example, if
   pkgconfig or pkgconf package is installed and ``PKG_CONFIG_PATH`` is
   configured properly (or ``<LIBPATH>/pkgconfig`` is a default path),
   ``pkgconfig --libs --clflags libndn-cxx`` will return all necessary
   compile and link flags for the library.
-  ``<BINPATH>/tlvdump``: a simple tool to dump contents of
   TLV-formatted data
-  ``<BINPATH>/ndncatchunks3``: a simplified equivalent to ndncatchunks2
   in NDNx package
-  ``<BINPATH>/ndnputchunks3``: a simplified equivalent to ndnputchunks2
   in NDNx package
-  ``<BINPATH>/ndnsec``: tool to manage NDN keys and certificates
-  ``<BINPATH>/ndnsec-*``: convenience scripts for ``ndnsec`` tools

If configured with tests: ``./waf configure --with-tests``), the above
commands will also produce:

-  ``build/unit-tests``: A unit test binary for the library

Build with examples
-------------------

By default, examples in ``examples/`` are not build.  To enable them, use
``--with-examples`` configure option:

::

    ./waf configure --with-examples
    ./waf

To run examples:

::

    # trivial producer app
    ./build/examples/producer

    # trivial consumer app
    ./build/examples/consumer

    # trivial consumer app with timers
    ./build/examples/consumer-with-timer

If you want to test out a sample application, just create a ``.cpp`` file in ``examples/``
folder and it will be compiled on the next run on ``./waf``.  For example:

::

    cp examples/consumer.cpp examples/my-new-consumer-app.cpp
    ./waf
    ./build/examples/my-new-consumer-app


Debug symbols
~~~~~~~~~~~~~

The default compiler flags enable debug symbols to be included in binaries (i.e., ``-g``
flag for ``./waf configure`` and ``-g3`` for ``./waf configure --debug``).  This
potentially allows more meaningful debugging information if your application crashes.

If it is undesirable, default flags can be easily overridden:

::

    CXXFLAGS="-O2" ./waf configure --prefix=/usr --sysconfdir=/etc
    ./waf
    sudo ./waf install

Documentation
-------------

ndn-cxx tutorials and API documentation can be built using the following
commands:

::

    # Full set of documentation (tutorials + API) in build/docs
    ./waf docs

    # Only tutorials in `build/docs`
    ./waf sphinx

    # Only API docs in `build/docs/doxygen`
    ./waf doxgyen

Manpages are automatically created and installed during the normal build
process (e.g., during ``./waf`` and ``./waf install``), if
``python-sphinx`` module is detected during ``./waf configure`` stage.
By default, manpages are installed into ``${PREFIX}/share/man`` (where
default value for ``PREFIX`` is ``/usr/local``). This location can be
changed during ``./waf configure`` stage using ``--prefix``,
``--datarootdir``, or ``--mandir`` options.

For more details, refer to ``./waf --help``.

Development Build
-----------------

The following is the suggested configure commands for development build.

::

    ./waf configure --debug --with-tests
    ./waf
    sudo ./waf install

In the development build all compiler optimizations are disabled by
default and all warnings are treated as error. The default behavior can
be overridden by setting ``CXXFLAGS`` environment variable before
running ``./waf configure``:

::

    CXXFLAGS="-O1 -g3" ./waf configure --debug --with-tests
    ...
