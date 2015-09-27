ndn-cxx version 0.3.0
---------------------

Release date: February 2, 2015

Changes since version 0.2.0:

New features:
^^^^^^^^^^^^^

- **Build**

  + The code now requires C++11.  The minimum supported gcc version is 4.6, as earlier versions
    do not have proper support for C++11 features.

- **Base**

  + Enable detailed version information when built from shallow clone or tarball
    (:issue:`1915`)

  + Make default transport configurable (:issue:`2189`)

  + Add ability to do partial Name comparison (:issue:`2090`)

- **Security**

  + Add support for KeyDigest key locator (:issue:`1426`)

  + Add a method to get KeyDigest from a PublicKey instance (:issue:`1964`)

  + Add support for KeyChain to sign Interest using SHA256 digest (:issue:`2218`)

  + Add ``ndnsec-dsk-gen`` tool to generate a pair of Data-Signing-Key (DSK) for the specified
    identity  (:issue:`2246`)

  + Pair-up PIB and TPM in KeyChain: PIB remembers the last used TPM type/location, which is
    used during KeyChain construction (unless overriden) (:issue:`2242`)

- **Wire encoding**

  + Add support for application-defined meta information blocks (:issue:`2021`)

  + Introduce new ``name::Component::is*`` methods to allow checking if the name component
    follows the specific naming convention without relying on the exception handling
    (:issue:`2088`)

  + Add definition of producer-generated NACK (:issue:`2111`)

  + Add ``std::hash<Name>`` specialization to support ``unordered_map`` container
    (:issue:`2147`)

  + Add support for ImplicitSha256DigestComponent (:issue:`1640`)

  + Allow tagging Interest and Data packets with arbitrary information (:issue:`2336`)

- **Management**

  + Support NFD face query operation (:issue:`2085`)

- **Miscellaneous tools**

  + Add a new helper to calculate digests over dynamic inputs (:issue:`1934`)

  + Add DNS resolver helper (:issue:`1918`)

  + Import and re-license Ethernet and FaceUri classes from NFD codebase, add FaceUri
    canonization support for ``udp``, ``tcp``, and ``ether`` schemas (:issue:`1994`)

  + Add per-application in-memory storage with LRU, LFU, and FIFO replacement policies
    (:issue:`1940`)

  + Add ability to specialize system and steady clocks (e.g., for unit tests and simulation
    purposes) (:issue:`2158`)

  + Expose DummyClientFace as public API

  + New IndentedStream helper class to simplify writing to streams with the specified indents
    or prefixes (:issue:`2238`)

  + New Signal class template as an enhanced version of the deprecated EventEmitter
    (:issue:`2279`)

  + New scheduler::ScopedEventId helper to automatically cancel scheduled events upon
    destruction (:issue:`2442`)

  + New NetworkMonitor helper to detect network state changes (:issue:`2443`)

Updates and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^

- **Base**

  + Prevent flags being set for rib/unregister command (:issue:`1945`)

  + Fix segfault caused by Face::unregisterPrefix when registeredPrefixId does not exist

  + Add explicit internal KeyChain to Face class.  KeyChain previously existed in the Face as
    part of NFD controller class.  (:issue:`2039`)

  + Properly disable assertions in release builds (:issue:`2135`)

  + Connect to Transport during construction of Face instance (:issue:`2318`)

- **Wire encoding**

  + Fix segfault when decoding empty Exclude element (:issue:`1970`)

  + Provide EqualityComparable concept for Exclude element

  + Correct Marker in Name::appendVersion() (:issue:`2086`)

  + Ensure that wire-encoding errors are inherited from ``tlv::Error`` (:issue:`1983`)

- **Management**

  + Replace FaceFlags with individual fields in face management data structures (:issue:`1992`)

  + ``ndn::nfd::Controller`` extended to allow generating ControlCommand for remote prefix
    registration (:issue:`2039`)

  + Change ``ForwarderStatus.NfdVersion`` to string (:issue:`2003`)

- **Security**

  + Fix ``ndnsec-cert-gen`` to correctly infer prefix before KEY component (:issue:`2052`)

  + Help message corrections in ``ndnsec-cert-dump``, ``ndnsec-cert-gen``, and
    ``ndnsec-key-gen`` (:issue:`2052`)

  + Accept ``--sign-id`` and ``--cert-prefix`` in ``ndnsec-cert-revoke`` (:issue:`2058`)

  + Changes output format of ``ndnsec-cert-revoke`` to base64 encoding (:issue:`2059`)

  + Add a new (safer) option to ``ndnsec-cert-gen`` allowing inclusion of additional
    information blocks into the certificate.  The new ``--signed-info`` parameter makes minimum
    assumptions, treating text before first space as OID and the rest as the associated string
    with this OID.

  + Make Face optional in Validator class (:issue:`2124`)

  + Display signature information when printing a certificate (:issue:`2196`)

  + Handle certificate decoding error in validation process (:issue:`2212`)

  + Fix segmentation fault in ``ndnsec-sign-req`` (:issue:`2211`)

  + Allow verbose option in ``ndnsec-list`` (:issue:`2197`)

  + Allow generation of DSK using ``ndnsec-key-gen`` (:issue:`2247`)

  + Allow ``ndnsec-key-gen`` to generate ECDSA keys (:issue:`2248`)

  + Fix memory leak in SecPublicInfoSqlite3 (:issue:`2253`)

  + Properly report status of ``ndnsec-delete`` execution (:issue:`2275`)

- **Miscellaneous tools**

  + Fix incorrect tag map in ``tlvdump`` tool

  + Change to default resolver behavior in util::dns::Resolver (:issue:`2415`)

- **Documentation**

  + Update of tutorials and installation instructions

  + Rename manpage of ``client.conf`` to ``ndn-client.conf`` (:issue:`1967`)

  + Updated and extended ndn-cxx code style:

    - add rules for some C++11 constructs
    - prefer ``BOOST_ASSERT`` and ``static_assert``

  + Improve example applications (:issue:`1976`)

- **Tests**

  + Improve security unit test cases (:issue:`1683`)

  + Embedding CI build and test running scripts into the repository

  + Partial unit test coverage for Face class (previously covered only by integration tests)
    (:issue:`1947`)

  + Improve stability of unit tests by using UnitTestTime (wall clock independent time)
    (:issue:`2152`)

- **Build**

  + Redesign the way default compiler flags are determined (:issue:`2209`)

  + Support tools and examples with multiple translation units (:issue:`2344`)

  + Disable installation of internal headers and headers of the disabled components
    (:issue:`2266`, and :issue:`2269`)

  + Enable conditional compilation based on presence of ``getpass()`` function

- Other minor fixes and corrections

Deprecated:
^^^^^^^^^^^

- ``ndn::ptr_lib`` and ``ndn::func_lib`` namespace aliases

- The following ``nfd::Controller`` methods:

  + ``Controller::start`` overloads, except the overload taking ``CommandOptions``
  + ``Controller::Sign`` typedef
  + ``Controller::getDefaultCommandTimeout``
  + ``ControlCommand::getPrefix``
  + ``ControlCommand::getRequestName(ControlParameters)``
  + ``Controller::Controller(Face&)``

- ``MetaInfo::TYPE_*`` constants

- ``EventEmitter`` is deprecated in favor of ``Signal``

Removed:
^^^^^^^^

- FaceFlags APIs (:issue:`1992`)

- ``ControlCommand::makeCommandInterest`` (:issue:`2008`)

- namespace ``ndn::Tlv`` (:issue:`2079`)

- ``shared_ptr<io_service>`` constructor and getter in Face class

- ``tlv::ConentType`` (typo in the name)

- ``Selectors::Selectors(int, int, Exclude, int, bool)`` constructor

- ``Interest::Interest(Name, int, int, Exclude, int, bool, int, time::milliseconds, uint32_t)``
  constructor

- ``signature-sha256.hpp`` file

- unused ``encryption-manager.hpp`` file

- unused ``openssl`` dependency
