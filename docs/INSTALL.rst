.. _Getting Started with ndn-cxx:

Getting started with ndn-cxx
============================

Supported platforms
-------------------

ndn-cxx uses continuous integration and has been tested on the following platforms:

-  Ubuntu 16.04 (amd64)
-  Ubuntu 18.04 (amd64, armhf, i386)
-  Ubuntu 19.10 (amd64)
-  macOS 10.13
-  macOS 10.14
-  macOS 10.15
-  CentOS 8

ndn-cxx is known to work on the following platforms, although they are not officially
supported:

-  Debian >= 9
-  Fedora >= 29
-  Gentoo Linux
-  Raspbian >= 2017-08-16
-  FreeBSD >= 11.3

Prerequisites
-------------

Required:
~~~~~~~~~

-  GCC >= 5.3, or clang >= 3.6
-  Python >= 3.5
-  ``pkg-config``
-  Boost >= 1.58
-  OpenSSL >= 1.0.2
-  SQLite 3.x

Following are the detailed steps for each platform to install the compiler and all necessary
development tools and libraries that are required to build ndn-cxx from source.

- Ubuntu

    In a terminal, enter::

        sudo apt install g++ pkg-config python3-minimal libboost-all-dev libssl-dev libsqlite3-dev

- CentOS and Fedora

    In a terminal, enter::

        sudo dnf install gcc-c++ pkgconf-pkg-config python3 boost-devel openssl-devel sqlite-devel

- macOS

    * Install either Xcode (from the App Store) or the Command Line Tools
      (with ``xcode-select --install``)

    * If using Homebrew (recommended), enter the following in a terminal::

        brew install boost openssl pkg-config

      .. note::

        If a major OS upgrade is performed after installing the dependencies
        with Homebrew, remember to reinstall all packages.

- FreeBSD

    In a terminal, enter::

        sudo pkg install pkgconf python3 boost-libs openssl sqlite3

Optional:
~~~~~~~~~

To build tutorials, manpages, and API documentation the following
dependencies need to be installed:

-  ``doxygen``
-  ``graphviz``
-  ``sphinx``
-  ``sphinxcontrib-doxylink``

The following lists the steps to install these prerequisites on various common platforms.

- On Ubuntu::

    sudo apt install doxygen graphviz python3-pip
    sudo pip3 install sphinx sphinxcontrib-doxylink

- On CentOS and Fedora::

    sudo dnf config-manager --enable PowerTools  # on CentOS only
    sudo dnf install doxygen graphviz python3-pip
    pip3 install --user sphinx sphinxcontrib-doxylink

- On macOS::

    brew install doxygen graphviz
    sudo pip install sphinx sphinxcontrib-doxylink

- On FreeBSD::

    sudo pkg install doxygen graphviz py37-sphinx


.. _build:

Build
-----

.. note::
  These are instructions for regular builds of ndn-cxx (release mode).
  To do development of ndn-cxx code itself, see "Development build" below.

To build in a terminal, change directory to the ndn-cxx root, then enter::

    ./waf configure  # on CentOS, add --without-pch
    ./waf
    sudo ./waf install

By default, only the shared version of ndn-cxx library is built.  To build the static library,
use ``--enable-static`` option for ``./waf configure`` command::

    ./waf configure --enable-static

To disable build of the shared library and build only the static library, use additional
``--disable-shared`` option.  Note that at least one version of the library needs to be
enabled.

::

    ./waf configure --enable-static --disable-shared

After the shared library is installed, on Linux it is also necessary to run::

    sudo ldconfig

.. note::
  When the library is installed in a non-standard path (in general: not in ``/usr/lib``
  or ``/usr/local/lib``; on some Linux distros including Fedora: not in ``/usr/lib``),
  additional actions may be necessary.

  The installation path should be added to ``/etc/ld.so.conf`` (or in
  ``/etc/ld.so.conf.d``) **before** running ``sudo ldconfig``. For example::

      echo /usr/local/lib | sudo tee /etc/ld.so.conf.d/ndn-cxx.conf

  Alternatively, the ``LD_LIBRARY_PATH`` environment variable can be set to point to
  the installation directory of the shared library::

      export LD_LIBRARY_PATH=/usr/local/lib

The ``./waf install`` command installs the following files:

-  ``<LIBPATH>/libndn-cxx.a``: static NDN C++ library (if enabled).
-  ``<LIBPATH>/libndn-cxx.so``, ``<LIBPATH>/libndn-cxx.so.<VERSION>`` (on Linux),
   ``<LIBPATH>/libndn-cxx.dylib``, ``<LIBPATH>/libndn-cxx.<VERSION>.dylib`` (on macOS):
   shared NDN C++ library (if enabled).
-  ``<LIBPATH>/pkgconfig/libndn-cxx.pc``: pkgconfig file storing all
   neccessary flags to build against the library. For example, if
   pkg-config or pkgconf package is installed and ``PKG_CONFIG_PATH`` is
   configured properly (or ``<LIBPATH>/pkgconfig`` is a default path),
   ``pkgconfig --libs --clflags libndn-cxx`` will return all necessary
   compile and link flags for the library.
-  ``<BINPATH>/ndnsec``: tool to manage NDN keys and certificates.
-  ``<BINPATH>/ndnsec-*``: convenience aliases for ``ndnsec`` tools.

If configured with tests (``./waf configure --with-tests``), the above
commands will also produce:

-  ``build/unit-tests``: a unit test binary for the library.

1.5GB available memory per CPU core is necessary for efficient compilation.
On a multi-core machine with less than 1.5GB available memory per CPU core,
limit the objects being compiled in parallel with ``./waf -jN`` where N is the amount
of available memory divided by 1.5GB (eg. ``./waf -j1`` for 1.5GB memory),
which should usually avoid memory thrashing and result in faster compilation.

Build with examples
-------------------

By default, examples in ``examples/`` are not built.  To enable them, use the
``--with-examples`` configure option::

    ./waf configure --with-examples
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only

To run examples::

    # trivial producer app
    ./build/examples/producer

    # trivial consumer app
    ./build/examples/consumer

    # trivial consumer app with timers
    ./build/examples/consumer-with-timer

If you want to test out a sample application, just create a ``.cpp`` file in ``examples/``
folder and it will be compiled on the next run on ``./waf``.  For example::

    cp examples/consumer.cpp examples/my-new-consumer-app.cpp
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only
    ./build/examples/my-new-consumer-app

Debug symbols
~~~~~~~~~~~~~

The default compiler flags enable debug symbols to be included in binaries (i.e., ``-g``
flag for ``./waf configure`` and ``-g3`` for ``./waf configure --debug``).  This
potentially allows more meaningful debugging information if your application crashes.

The default build flags can easily be overridden::

    CXXFLAGS="-O2" ./waf configure --prefix=/usr --sysconfdir=/etc
    ./waf
    sudo ./waf install


Documentation
-------------

ndn-cxx tutorials and API documentation can be built using the following
commands::

    # Full set of documentation (tutorials + API) in build/docs
    ./waf docs

    # Only tutorials in build/docs
    ./waf sphinx

    # Only API docs in build/docs/doxygen
    ./waf doxygen

Manpages are automatically created and installed during the normal build
process (e.g., during ``./waf`` and ``./waf install``), if
``python-sphinx`` module is detected during ``./waf configure`` stage.
By default, manpages are installed into ``${PREFIX}/share/man`` (where
default value for ``PREFIX`` is ``/usr/local``). This location can be
changed during ``./waf configure`` stage using ``--prefix``,
``--datarootdir``, or ``--mandir`` options.

For more details, refer to ``./waf --help``.


Development build
-----------------

The following is the suggested configure command for development builds::

    ./waf configure --debug --with-tests
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only

In the development build most compiler optimizations are disabled by
default and all warnings are treated as errors. The default behavior can
be overridden by setting ``CXXFLAGS`` environment variable before
running ``./waf configure``::

    CXXFLAGS="-O1 -g3" ./waf configure --debug --with-tests
    ...


Customizing the compiler
------------------------

To choose a custom C++ compiler for building ndn-cxx, set the ``CXX`` environment
variable to point to the compiler binary. For example, to build with clang on
Linux, use the following::

    CXX=clang++ ./waf configure
