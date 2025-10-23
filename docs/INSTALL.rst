Getting started with ndn-cxx
============================

Supported platforms
-------------------

ndn-cxx is built against a continuous integration system and has been tested on the
following platforms:

- Ubuntu 22.04 (jammy)
- Ubuntu 24.04 (noble)
- Debian 12 (bookworm)
- Debian 13 (trixie)
- CentOS Stream 9
- macOS 14 / 15 / 26

ndn-cxx should also work on the following platforms, although they are not officially
supported:

- Any other recent version of Ubuntu not listed above
- Fedora >= 34
- Alpine >= 3.14
- Any version of Raspberry Pi OS based on Debian 12 (bookworm) or later
- macOS >= 11
- FreeBSD >= 12.2

Prerequisites
-------------

Required
~~~~~~~~

- GCC >= 10.2 or clang >= 10.0 (if you are on Linux or FreeBSD)
- Xcode >= 13.0 or corresponding version of Command Line Tools (if you are on macOS)
- Boost >= 1.74.0
- OpenSSL >= 1.1.1
- SQLite >= 3.31
- pkgconf >= 1.6
- Python >= 3.8

To build ndn-cxx from source, one must first install a C++ compiler and all necessary
development tools and libraries:

- On **Debian** and **Ubuntu**, run in a terminal:

  .. code-block:: shell

    sudo apt install build-essential libboost-all-dev libssl-dev libsqlite3-dev pkgconf python3

- On **CentOS** and **Fedora**, run in a terminal:

  .. code-block:: shell

    sudo dnf install gcc-c++ boost-devel openssl-devel sqlite-devel pkgconf python3

- On **macOS**

  * Install either Xcode (from the App Store) or the Command Line Tools
    (with ``xcode-select --install``)

  * If using Homebrew (recommended), enter the following in a terminal:

    .. code-block:: shell

      brew install boost openssl pkgconf

- On **FreeBSD**, run in a terminal:

  .. code-block:: shell

    sudo pkg install boost-libs openssl sqlite3 pkgconf python3

Optional
~~~~~~~~

To build tutorials, man pages, and API documentation the following additional dependencies
need to be installed:

- doxygen
- graphviz
- `sphinx <https://www.sphinx-doc.org/>`__
- `sphinxcontrib-doxylink <https://github.com/sphinx-contrib/doxylink>`__
- `furo <https://pradyunsg.me/furo/>`__

The following lists the steps to install these prerequisites on various common platforms.

.. tip::
  On Linux, you may need to add ``$HOME/.local/bin`` to the ``PATH`` environment variable
  for your user, for example:

  .. code-block:: shell

    export PATH="${HOME}/.local/bin${PATH:+:}${PATH}"

- On **Debian** and **Ubuntu**:

  .. code-block:: shell

    sudo apt install doxygen graphviz python3-pip
    python3 -m pip install --user -r docs/requirements.txt

- On **CentOS** and **Fedora**:

  .. code-block:: shell

    sudo dnf install doxygen graphviz python3-pip
    python3 -m pip install --user -r docs/requirements.txt

- On **macOS**:

  .. code-block:: shell

    brew install doxygen graphviz
    python3 -m pip install --user -r docs/requirements.txt

- On **FreeBSD**:

  .. code-block:: shell

    sudo pkg install doxygen graphviz py39-sphinx

Building
--------

.. note::
  These are the instructions for a regular build of ndn-cxx (release mode). If you are planning
  to develop or modify the ndn-cxx code itself, you should do a :ref:`Development build`.

To build in a terminal, change directory to the ndn-cxx root, then enter:

.. code-block:: shell

    ./waf configure
    ./waf
    sudo ./waf install

By default, only the shared variant of the ndn-cxx library will be built. To build the
static library, pass ``--enable-static`` to the ``./waf configure`` command:

.. code-block:: shell

    ./waf configure --enable-static

To disable the build of the shared library and build only the static library, use the
additional ``--disable-shared`` option.  Note that at least one variant of the library
needs to be enabled.

.. code-block:: shell

    ./waf configure --enable-static --disable-shared

On Linux, it is necessary to run the following command after the shared library has
been installed:

.. code-block:: shell

    sudo ldconfig

.. tip::
  On Linux, when the library is installed in a non-default location (generally, not in
  ``/usr/lib`` or ``/usr/local/lib``), the following additional actions may be necessary.

  The library installation path should be added to ``/etc/ld.so.conf`` or to
  ``/etc/ld.so.conf.d/*.conf`` before running ``ldconfig``. For example:

  .. code-block:: shell

      echo /usr/local/lib64 | sudo tee /etc/ld.so.conf.d/ndn-cxx.conf
      sudo ldconfig

  Alternatively, the ``LD_LIBRARY_PATH`` environment variable can be set to point to
  the installation directory of the shared library:

  .. code-block:: shell

      export LD_LIBRARY_PATH=/usr/local/lib64

The ``./waf install`` command installs the following files:

-  ``<LIBDIR>/libndn-cxx.a``: static NDN C++ library (if enabled).
-  ``<LIBDIR>/libndn-cxx.so``, ``<LIBDIR>/libndn-cxx.so.<VERSION>`` (on Linux),
   ``<LIBDIR>/libndn-cxx.dylib``, ``<LIBDIR>/libndn-cxx.<VERSION>.dylib`` (on macOS):
   shared NDN C++ library (if enabled).
-  ``<LIBDIR>/pkgconfig/libndn-cxx.pc``: pkgconfig file storing all necessary flags
   to build against the library. For example, if ``pkgconf`` is installed and
   ``PKG_CONFIG_PATH`` is configured properly (or if ``<LIBDIR>/pkgconfig`` is a default
   search path), the command ``pkgconf --cflags --libs libndn-cxx`` will return all
   necessary compile and link flags for the ndn-cxx library.
-  ``<BINDIR>/ndnsec``: command-line tool to manage NDN keys and certificates used by
   ndn-cxx applications.
-  ``<BINDIR>/ndnsec-*``: convenience aliases for ``ndnsec`` tools.

If configured with tests (``./waf configure --with-tests``), the above commands
will also produce:

- ``build/unit-tests``: a unit test binary for the library; it will not be installed.

1.5 GB available memory per CPU core is necessary for efficient compilation. On a
multi-core machine with less than 1.5 GB available memory per CPU core, limit the
objects being compiled in parallel with ``./waf -jN``, where N is the amount of
available memory divided by 1.5 GB (e.g., ``./waf -j2`` for 3 GB of memory). This
should avoid memory thrashing and result in faster compilation.

Building with examples
----------------------

By default, the examples in the ``examples/`` directory will not be built. To enable
them, pass ``--with-examples`` during the configuration step:

.. code-block:: shell

    ./waf configure --with-examples
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only

To run the examples:

.. code-block:: shell

    # trivial producer app
    ./build/examples/producer

    # trivial consumer app
    ./build/examples/consumer

    # trivial consumer app with timers
    ./build/examples/consumer-with-timer

If you want to make a new sample application, just create a ``.cpp`` file inside the
``examples/`` directory and it will be compiled during the next run of ``./waf``:

.. code-block:: shell

    cp examples/consumer.cpp examples/my-new-app.cpp
    ... # edit examples/my-new-app.cpp with your preferred editor
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only
    ./build/examples/my-new-app

Building the documentation
--------------------------

Tutorials and API documentation can be built using the following commands:

.. code-block:: shell

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

Debug symbols
-------------

The default compiler flags include debug symbols in binaries. This should provide
more meaningful debugging information if ndn-cxx or your application crashes.

If this is not desired, the default flags can be overridden to disable debug symbols.
The following example shows how to completely disable debug symbols and configure
ndn-cxx to be installed into ``/usr`` with configuration in the ``/etc`` directory.

.. code-block:: shell

    CXXFLAGS="-O2" ./waf configure --prefix=/usr --sysconfdir=/etc
    ./waf
    sudo ./waf install

Customizing the compiler
------------------------

To build ndn-cxx with a different compiler (rather than the platform default), set the
``CXX`` environment variable to point to the compiler binary. For example, to build
with clang on Linux, use the following:

.. code-block:: shell

    CXX=clang++ ./waf configure

.. _Development build:

Development build
-----------------

The following is the suggested build procedure for development builds:

.. code-block:: shell

    ./waf configure --debug --with-tests
    ./waf
    sudo ./waf install
    sudo ldconfig  # on Linux only

In a development build, most compiler optimizations will be disabled and all warnings
will be treated as errors. This default behavior can be overridden by setting the
``CXXFLAGS`` environment variable before running ``./waf configure``, for example:

.. code-block:: shell

    CXXFLAGS="-O1 -g3 -Wall" ./waf configure --debug --with-tests
    ./waf
