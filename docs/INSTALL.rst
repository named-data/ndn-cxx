.. _Getting Started with ndn-cxx:

Getting started with ndn-cxx
============================

Supported platforms
-------------------

ndn-cxx uses continuous integration and has been tested on the following
platforms:

-  Ubuntu 12.04 (64-bit and 32-bit)
-  Ubuntu 14.04 (64-bit and 32-bit)
-  Ubuntu 14.10 (64-bit and 32-bit)
-  OS X 10.8
-  OS X 10.9
-  OS X 10.10

ndn-cxx is known to work on the following platforms, although they are not officially
supported:

-  Fedora >= 20
-  CentOS >= 6.2
-  Gentoo Linux
-  FreeBSD >= 10.0
-  Raspbian >= 3.12

Prerequisites
-------------

Required:
~~~~~~~~~

-  ``python`` >= 2.6
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

   .. note::

      If a major OS X system upgrade is performed after installing dependencies with MacPorts,
      remember to `reinstall all ports <https://trac.macports.org/wiki/Migration>`__.

-  Ubuntu

   In a terminal, enter::

       sudo apt-get install build-essential
       sudo apt-get install libsqlite3-dev libcrypto++-dev

       # For Ubuntu 12.04
       sudo apt-get install libboost1.48-all-dev

       # For all other Ubuntu versions
       sudo apt-get install libboost-all-dev

- Fedora

   In a terminal, enter::

       sudo yum install gcc-g++ git
       sudo yum install sqlite-devel cryptopp-devel boost-devel

Optional:
~~~~~~~~~

To build tutorials, manpages, and API documentation the following
dependencies need to be installed:

-  ``doxygen``
-  ``graphviz``
-  ``python-sphinx`` and sphinx extensions ``sphinxcontrib-doxylink``,
   ``sphinxcontrib-googleanalytics``

The following lists steps for common platforms to install these prerequisites:

-  On OS X with MacPorts::

       sudo port install doxygen graphviz py27-sphinx sphinx_select
       sudo port select sphinx py27-sphinx

       # Install sphinx extensions
       sudo port install py27-pip
       sudo port select pip pip27
       sudo pip install sphinxcontrib-doxylink sphinxcontrib-googleanalytics

-  On Ubuntu::

       sudo apt-get install doxygen graphviz python-sphinx python-pip
       sudo pip install sphinxcontrib-doxylink sphinxcontrib-googleanalytics

-  On Fedora::

       sudo yum install doxygen graphviz python-sphinx
       sudo pip install sphinxcontrib-doxylink sphinxcontrib-googleanalytics

.. _build:

Build
-----

(These are instructions to build ndn-cxx. To do development of ndn-cxx
code and update the build system, see Development.)

To build in a terminal, change directory to the ndn-cxx root. Enter:

::

    ./waf configure
    ./waf
    sudo ./waf install

By default, only the shared version of ndn-cxx library is built.  To build the static library,
use ``--enable-static`` option for ``./waf configure`` command.  For example::

    ./waf configure --enable-static

To disable build of the shared library and build only the static library, use additional
``--disable-shared`` option.  Note that at least one version of the library needs to be
enabled.

::

    ./waf configure --enable-static --disable-shared


After the shared library is built and installed, some systems require additional actions.

  - on Linux::

      sudo ldconfig

  - on FreeBSD::

      sudo ldconfig -m

  .. note::
     When library is installed in a non-standard path (in general: not in ``/usr/lib`` or
     ``/usr/local/lib``; on some Linux distros including Fedora: not in ``/usr/lib``),
     additional actions may be necessary.

     The installation path should be added to ``/etc/ld.so.conf`` (or in
     ``/etc/ld.so.conf.d``) **before** running ``sudo ldconfig``. For example::

         echo /usr/local/lib | sudo tee /etc/ld.so.conf.d/ndn-cxx.conf

     Alternatively, ``LD_LIBRARY_PATH`` environment variable should be set to the location of
     the library::

         export LD_LIBRARY_PATH=/usr/local/lib

This builds and installs the following items:

-  ``<LIBPATH>/libndn-cxx.a``: static NDN C++ library (if enabled)
-  ``<LIBPATH>/libndn-cxx.so``, ``<LIBPATH>/libndn-cxx.so.<VERSION>`` (on Linux),
   ``<LIBPATH>/libndn-cxx.dylib``, ``<LIBPATH>/libndn-cxx.<VERSION>.dylib`` (on OS X):
   shared NDN C++ library (if enabled)
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

1.5GB available memory per CPU core is necessary for efficient compilation.
On a multi-core machine with less than 1.5GB available memory per CPU core,
limit the objects being compiled in parallel with ``./waf -jN`` where N is the amount
of available memory divided by 1.5GB (eg. ``./waf -j1`` for 1.5GB memory),
which could usually avoid memory thrashing and result in faster compilation.

Build with examples
-------------------

By default, examples in ``examples/`` are not build.  To enable them, use
``--with-examples`` configure option:

::

    ./waf configure --with-examples
    ./waf
    sudo ./waf install

:ref:`Additional step <build>`:

  - on Linux::

      sudo ldconfig

  - on FreeBSD::

      sudo ldconfig -m

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
    sudo ./waf install
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

:ref:`Additional step <build>`:

  - on Linux::

      sudo ldconfig

  - on FreeBSD::

      sudo ldconfig -m

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

:ref:`Additional step <build>`:

  - on Linux::

      sudo ldconfig

  - on FreeBSD::

      sudo ldconfig -m

In the development build all compiler optimizations are disabled by
default and all warnings are treated as error. The default behavior can
be overridden by setting ``CXXFLAGS`` environment variable before
running ``./waf configure``:

::

    CXXFLAGS="-O1 -g3" ./waf configure --debug --with-tests
    ...

Customize Compiler
------------------

To customize compiler, set ``CXX`` environment variable to point to compiler binary and, in
some case, specify type of the compiler using ``--check-cxx-compiler``.  For example, when
using clang compiler on Linux system, use the following:

::

    CXX=clang++ ./waf configure --check-cxx-compiler=clang++
