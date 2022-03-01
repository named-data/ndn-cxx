Getting started with ndn-cxx
============================

Supported platforms
-------------------

ndn-cxx is built against a continuous integration system and has been tested on the
following platforms:

-  Ubuntu 18.04 (amd64, armhf, i386)
-  Ubuntu 20.04 (amd64)
-  Ubuntu 21.10 (amd64)
-  macOS 10.15
-  macOS 11 (Intel only)
-  CentOS 8

ndn-cxx is known to work on the following platforms, although they are not officially
supported:

-  Alpine >= 3.12
-  Debian >= 10
-  Fedora >= 29
-  Gentoo Linux
-  Raspberry Pi OS (formerly Raspbian) >= 2019-06-20
-  FreeBSD >= 12.0
-  macOS 10.14

Prerequisites
-------------

Required
~~~~~~~~

-  GCC >= 7.4 or clang >= 6.0 (if you are on Linux or FreeBSD)
-  Xcode >= 11.3 or corresponding version of Command Line Tools (if you are on macOS)
-  Python >= 3.6
-  pkg-config
-  Boost >= 1.65.1
-  OpenSSL >= 1.1.1
-  SQLite 3.x

To build ndn-cxx from source, one must first install a C++ compiler and all necessary
development tools and libraries:

- On **Ubuntu**

    In a terminal, enter::

        sudo apt install build-essential pkg-config python3-minimal libboost-all-dev libssl-dev libsqlite3-dev

- On **CentOS** and **Fedora**

    In a terminal, enter::

        sudo dnf install gcc-c++ pkgconf-pkg-config python3 boost-devel openssl-devel sqlite-devel

- On **macOS**

    * Install either Xcode (from the App Store) or the Command Line Tools
      (with ``xcode-select --install``)

    * If using Homebrew (recommended), enter the following in a terminal:

      .. code-block:: sh

        brew install boost openssl pkg-config
        brew install python  # only on macOS 10.14 and earlier

      .. warning::

        If a major OS upgrade is performed after installing the dependencies
        with Homebrew, remember to reinstall all packages.

- On **FreeBSD**

    In a terminal, enter::

        sudo pkg install pkgconf python3 boost-libs openssl sqlite3

Optional
~~~~~~~~

To build tutorials, man pages, and API documentation the following additional dependencies
need to be installed:

-  doxygen
-  graphviz
-  sphinx >= 1.3
-  sphinxcontrib-doxylink

The following lists the steps to install these prerequisites on various common platforms.

.. note::
  On Linux, you may need to add ``$HOME/.local/bin`` to the ``PATH`` environment variable
  for your user, for example:

  .. code-block:: sh

      export PATH="${HOME}/.local/bin${PATH:+:}${PATH}"

- On **Ubuntu**:

  .. code-block:: sh

    sudo apt install doxygen graphviz python3-pip
    pip3 install --user sphinx sphinxcontrib-doxylink

- On **CentOS** and **Fedora**:

  .. code-block:: sh

    sudo dnf config-manager --enable powertools  # on CentOS only
    sudo dnf install doxygen graphviz python3-pip
    pip3 install --user sphinx sphinxcontrib-doxylink

- On **macOS**:

  .. code-block:: sh

    brew install doxygen graphviz
    sudo pip3 install sphinx sphinxcontrib-doxylink

- On **FreeBSD**:

  .. code-block:: sh

    sudo pkg install doxygen graphviz py37-sphinx

Build
-----

.. note::
  These are instructions for regular builds of ndn-cxx (release mode). If you are
  planning to develop the ndn-cxx code itself, you should do a :ref:`Development build`.

To build in a terminal, change directory to the ndn-cxx root, then enter:

.. code-block:: sh

    ./waf configure  # on CentOS, add --without-pch
    ./waf
    sudo ./waf install

By default, only the shared variant of the ndn-cxx library will be built. To build the
static library, pass ``--enable-static`` to the ``./waf configure`` command:

.. code-block:: sh

    ./waf configure --enable-static

To disable the build of the shared library and build only the static library, use the
additional ``--disable-shared`` option.  Note that at least one variant of the library
needs to be enabled.

.. code-block:: sh

    ./waf configure --enable-static --disable-shared

On Linux, it is necessary to run the following command after the shared library has
been installed:

.. code-block:: sh

    sudo ldconfig

.. note::
  When the library is installed in a non-standard path (in general: not in ``/usr/lib``
  or ``/usr/local/lib``; on some Linux distros including Fedora: not in ``/usr/lib``),
  additional actions may be necessary.

  The installation path should be added to ``/etc/ld.so.conf`` (or in
  ``/etc/ld.so.conf.d``) **before** running ``sudo ldconfig``. For example:

  .. code-block:: sh

      echo /usr/local/lib | sudo tee /etc/ld.so.conf.d/ndn-cxx.conf

  Alternatively, the ``LD_LIBRARY_PATH`` environment variable can be set to point to
  the installation directory of the shared library:

  .. code-block:: sh

      export LD_LIBRARY_PATH=/usr/local/lib

The ``./waf install`` command installs the following files:

-  ``<LIBPATH>/libndn-cxx.a``: static NDN C++ library (if enabled).
-  ``<LIBPATH>/libndn-cxx.so``, ``<LIBPATH>/libndn-cxx.so.<VERSION>`` (on Linux),
   ``<LIBPATH>/libndn-cxx.dylib``, ``<LIBPATH>/libndn-cxx.<VERSION>.dylib`` (on macOS):
   shared NDN C++ library (if enabled).
-  ``<LIBPATH>/pkgconfig/libndn-cxx.pc``: pkgconfig file storing all necessary flags to
   build against the library. For example, if the ``pkg-config`` or ``pkgconf-pkg-config``
   package is installed and ``PKG_CONFIG_PATH`` is configured properly (or if
   ``<LIBPATH>/pkgconfig`` is a default search path), the command ``pkg-config --cflags
   --libs libndn-cxx`` will return all necessary compile and link flags for the library.
-  ``<BINPATH>/ndnsec``: tool to manage NDN keys and certificates.
-  ``<BINPATH>/ndnsec-*``: convenience aliases for ``ndnsec`` tools.

If configured with tests (``./waf configure --with-tests``), the above commands
will also produce:

-  ``build/unit-tests``: a unit test binary for the library.

1.5 GB available memory per CPU core is necessary for efficient compilation. On a
multi-core machine with less than 1.5 GB available memory per CPU core, limit the
objects being compiled in parallel with ``./waf -jN``, where N is the amount of
available memory divided by 1.5 GB (e.g., ``./waf -j2`` for 3 GB of memory). This
should avoid memory thrashing and result in faster compilation.

Build with examples
-------------------

By default, the examples in the ``examples/`` directory will not be built. To enable
them, pass ``--with-examples`` during the configuration step:

.. code-block:: sh

    ./waf configure --with-examples  # on CentOS, add --without-pch
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only

To run the examples:

.. code-block:: sh

    # trivial producer app
    ./build/examples/producer

    # trivial consumer app
    ./build/examples/consumer

    # trivial consumer app with timers
    ./build/examples/consumer-with-timer

If you want to make a new sample application, just create a ``.cpp`` file inside the
``examples/`` directory and it will be compiled during the next run of ``./waf``:

.. code-block:: sh

    cp examples/consumer.cpp examples/my-new-app.cpp
    ... # edit examples/my-new-app.cpp with your preferred editor
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only
    ./build/examples/my-new-app

Debug symbols
-------------

The default compiler flags include debug symbols in binaries. This should provide
more meaningful debugging information if ndn-cxx or your application crashes.

If this is not desired, the default flags can be overridden to disable debug symbols.
The following example shows how to completely disable debug symbols and configure
ndn-cxx to be installed into ``/usr`` with configuration in the ``/etc`` directory.

.. code-block:: sh

    CXXFLAGS="-O2" ./waf configure --prefix=/usr --sysconfdir=/etc
    ./waf
    sudo ./waf install

Customizing the compiler
------------------------

To build ndn-cxx with a different compiler (rather than the platform default), set the
``CXX`` environment variable to point to the compiler binary. For example, to build
with clang on Linux, use the following:

.. code-block:: sh

    CXX=clang++ ./waf configure

Building the documentation
--------------------------

Tutorials and API documentation can be built using the following commands:

.. code-block:: sh

    # Full set of documentation (tutorials + API) in build/docs
    ./waf docs

    # Only tutorials in build/docs
    ./waf sphinx

    # Only API docs in build/docs/doxygen
    ./waf doxygen

If ``sphinx-build`` is detected during ``./waf configure``, man pages will automatically
be built and installed during the normal build process (i.e., during ``./waf`` and
``./waf install``). By default, man pages will be installed into ``${PREFIX}/share/man``
(the default value for ``PREFIX`` is ``/usr/local``). This location can be changed
during the ``./waf configure`` stage using the ``--prefix``, ``--datarootdir``, or
``--mandir`` options.

For further details, please refer to ``./waf --help``.

.. _Development build:

Development build
-----------------

The following is the suggested build procedure for development builds:

.. code-block:: sh

    ./waf configure --debug --with-tests  # on CentOS, add --without-pch
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only

In a development build, most compiler optimizations will be disabled and all warnings
will be treated as errors. This default behavior can be overridden by setting the
``CXXFLAGS`` environment variable before running ``./waf configure``, for example:

.. code-block:: sh

    CXXFLAGS="-O1 -g3" ./waf configure --debug --with-tests
    ...
