ndn-cxx version 0.9.0
---------------------

*Release date: July 28, 2024*

Important changes and new features
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **C++17** is now *required* to build ndn-cxx and when including any of its header files

- The build dependencies have been increased as follows:

  - GCC >= 9.3 or Clang >= 7.0 are strongly *recommended* on Linux; GCC 8.x is also known
    to work but is not officially supported
  - Xcode 13 or later is *recommended* on macOS; older versions may still work but are not
    officially supported
  - Boost >= 1.71.0 is *required* on all platforms

- Ubuntu 24.04 (Noble), Debian 12 (Bookworm), and macOS 14 (Sonoma) are now officially supported

- Added an official Dockerfile to the repository. From this Dockerfile, two images are currently
  published to the GitHub container registry:

  - `named-data/ndn-cxx-build <https://github.com/named-data/ndn-cxx/pkgs/container/ndn-cxx-build>`__
  - `named-data/ndn-cxx-runtime <https://github.com/named-data/ndn-cxx/pkgs/container/ndn-cxx-runtime>`__

  Both images are available for *linux/amd64* and *linux/arm64* platforms.

- The default forwarder socket path for Unix transports changed to ``/run/nfd/nfd.sock`` on Linux
  and to ``/var/run/nfd/nfd.sock`` on all other platforms (:issue:`5304`)

- On macOS, the default TPM backend is now **tpm-file** (the same as all other platforms)

- :ndn-cxx:`Interest::matchesData` no longer considers ``MustBeFresh`` and ``FreshnessPeriod``
  (:issue:`5270`)

- Modernized several APIs to use ``std::string_view``

- Added :ndn-cxx:`Interest::setApplicationParameters(std::string_view)` and
  :ndn-cxx:`Data::setContent(std::string_view)` overloads for convenience

- :ndn-cxx:`SegmentFetcher` gained an option to omit ``MustBeFresh`` from the first Interest sent

- Moved :ndn-cxx:`DummyClientFace`, :ndn-cxx:`Segmenter`, and :ndn-cxx:`SegmentFetcher` to
  namespace ``ndn``

- Moved all :ndn-cxx:`Signal`-related classes to namespace ``ndn::signal``

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Check the TLV type when constructing :ndn-cxx:`Name` from :ndn-cxx:`Block`

- Provide relational operators for :ndn-cxx:`FaceUri`, to make it usable as a container key

- Generalize and simplify :ndn-cxx:`time::fromUnixTimestamp` and :ndn-cxx:`time::toUnixTimestamp`

- Prevent overflows in ``InterestLifetime`` and ``FreshnessPeriod`` (:issue:`4997`)

- Make HMAC signing and verification compatible with OpenSSL 3.x (:issue:`5251`)

- Better handling of large dates in :ndn-cxx:`ValidityPeriod` (:issue:`5176`)

- :ndn-cxx:`NetworkMonitor` now ignore AWDL interfaces on macOS (:issue:`5074`)

- Avoid dependency on *boost_program_options* when tools are disabled (:issue:`5210`)

- Add fine-grained options to ``./waf configure`` to enable/disable the compilation of
  benchmarks, integration tests, and unit tests independently

- Fix building the documentation with Python 3.12 (:issue:`5298`)

- Reduce amount of debugging information produced in compiled binaries by default (:issue:`5279`)

- Update waf build system to version 2.0.27

- Various code cleanups, modernizations, and documentation improvements

Deprecations
^^^^^^^^^^^^

- ``Face::getIoService()``. Use ``Face::getIoContext()`` instead.

- ``Component::fromEscapedString()``. Use :ndn-cxx:`Component::fromUri` instead.

- TPM unlock and related functionality. This includes ``Tpm::isTpmLocked()``,
  ``Tpm::unlockTpm()``, ``Tpm::isTerminalMode()``, ``Tpm::setTerminalMode()``,
  and the ``ndnsec-unlock-tpm`` command-line tool (:issue:`4754`)

Removals
^^^^^^^^

- ``Block::wire()``. Use :ndn-cxx:`Block::data` instead.

- :ndn-cxx:`Component` constructors that take a raw ``const uint8_t*``. Use the ``span``-based
  constructors instead.

- ``Component::fromImplicitSha256Digest()`` and ``Component::fromParametersSha256Digest()``

- Previously deprecated overloads of ``Name::append()``

- ``Name::push_back()``. Use :ndn-cxx:`Name::append` instead.

- ``ndn::escape()`` and ``ndn::unescape()`` overloads that take raw strings as ``const char*`` +
  ``size_t``. Use the corresponding ``std::string_view`` versions instead.

- ``ndn::optional`` and ``ndn::variant``. Use C++17 ``std::optional`` and ``std::variant``
  directly instead.

- ``ndn::to_string()``. Use the standard C++ function ``std::to_string()`` instead.

- Previously deprecated ``ndnsec`` command aliases (:issue:`5163`)
