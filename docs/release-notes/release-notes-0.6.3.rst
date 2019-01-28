ndn-cxx version 0.6.3
---------------------

Release date: September 18, 2018

The build requirements have been upgraded to gcc >= 5.3 or clang >= 3.6, boost >= 1.58,
openssl >= 1.0.2. This effectively drops support for all versions of Ubuntu older than 16.04
that use distribution-provided compilers and packages.

The compilation now uses the C++14 standard.

New features:
^^^^^^^^^^^^^

- More support for `NDN packet format version
  0.3 <https://named-data.net/doc/NDN-packet-spec/0.3/>`__ (:issue:`4527`)

  - Allow applications to declare a default ``CanBePrefix`` setting (:issue:`4581`)

  - Accommodate typed name components in ``Name::getSuccessor`` (:issue:`4570`)

  - Support Parameters element (:issue:`4658`)

  - Recognize alternative type representations in URI syntax (:issue:`4690`)

  - Introduce ``ParametersSha256DigestComponent`` (:issue:`4658`, :issue:`4570`)

- Prefix announcement object (:issue:`4650`)

- MTU element in ``ControlParameters`` and ``FaceStatus`` (:issue:`4005`)

- Enable congestion control features in ``SegmentFetcher`` (:issue:`4364`)

- ``_block`` literal operator (:issue:`4722`)

- Add official support for CentOS 7 (:issue:`4610`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Fix a segfault in ``Face::satisfyPendingInterests`` when ``Face::put`` is
  called in DataCallback (:issue:`4596`)

- Allow specifying passphrase for ndnsec import/export on the command line (:issue:`4633`)

- Fix bug preventing customization of KeyChain's TPM on macOS (:issue:`4297`)

- Fix bug with handling Sha256-signed Command Interests (:issue:`4635`)

- Eliminate selector usage in ``SegmentFetcher`` (:issue:`4555`)

- Improvements in ``netlink`` message processing

- Gracefully handle ``NetworkMonitor`` backend initialization failure (:issue:`4668`)

- Add support 224-bit and 521-bit NIST elliptic curves, add support for SHA-3 (with
  openssl >= 1.1.1-pre1), and forbid RSA keys shorter than 2048 bits in security helpers

- Improve and simplify code with modern C++ features

- Properly declare move constructors

- Improve error handling

- Improve test cases

- Correct and improve documentation

Deprecated
~~~~~~~~~~

- ``SegmentFetcher::fetch()`` static functions in favor of ``start()`` (:issue:`4464`)

- ``ndn::ip::address{,V6}FromString`` as Boost.Asio >= 1.58 has proper implementation of
  ``addressFromString``

- Selectors (:issue:`4527`)

  NDN Packet Format v0.3 replaces Selectors with ``CanBePrefix`` and ``MustBeFresh`` elements.
  ``CanBePrefix`` and ``MustBeFresh`` are currently mapped to the closest v0.2 semantics and
  encoded as selectors.

- ``Data::get/setFinalBlockId()`` in favor of ``Data::get/setFinalBlock()``

Removed
~~~~~~~

- Dependency on Boost.Regex in favor of ``std::regex``
