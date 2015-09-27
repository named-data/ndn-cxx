ndn-cxx version 0.3.3
---------------------

Release date: July 1, 2015

Changes since version 0.3.2:

New features:
^^^^^^^^^^^^^

- Allow compilation of ndn-cxx as a shared library (:issue:`2243`)

  In this release, by default, only a static library is compiled.  The next release will change
  the default behavior to compile only a shared library.

- Introduce concept of PartialName (:issue:`1962`)

  Semantically, PartialName abstraction represents an arbitrary sequence of name components,
  while Name represents an absolute name.  Currently, PartialName is typedef alias to Name, but
  it can be changed in the future releases.

- Introduce generalized signing API in KeyChain (:issue:`2871`, :issue:`1705`)

  A new API in KeyChain introduces a general interface to sign interest and data packets and
  supply necessary signing information, such as signing identity name, signing key name, or
  signing certificate name.  In addition, the supplied signing information can include additional
  information that signer wants to include in the SignatureInfo of the signed packet.

- Introduce helpers to create SigningInfo for the generalized signing API (:issue:`2922`)

- Sqlite3Statement utility helper that wraps an SQLite3 statements and provide automatic memory
  cleanup

- Introduce PibSqlite3 based on PibImpl (:issue:`2807`)

- Make public interface of Pib, Identity, and Key as read-only (:issue:`2928`)

- New encoding block helpers to simplify operations with ``std::string`` (:issue:`2951`):

  * ``prependStringBlock``
  * ``makeStringBlock``
  * ``readString``

Improvements and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Use C++11 lambda expression and smart pointers in Face implementation (:issue:`2112`)

- Fix compilation failure on OS X 10.10 with cryptopp from MacPorts

- Update client.conf manpage

- Change Name::compare to return negative, zero, or positive integer (instead of strictly -1,
  0, 1) when the first name comes before the other in canonical ordering, is equal, or comes
  after (:issue:`2445`)

- IdentityCertificate sets default FreshnessPeriod to 1 hour (:issue:`2872`)

- Unify TPM creation (:issue:`2722`)

- Allow negative start index in Name::getSubName method (:issue:`1962`)

- Improved documentation of PibImpl interfaces (:issue:`2896`, :issue:`2898`)

- Simplify CommandOptions with SigningInfo (:issue:`2893`)

- Refactor internal PIT to use scheduled events (:issue:`1372`, :issue:`2518`)

- Improve structure and documentation of block helpers (:issue:`2951`)

- Declare all move constructors as ``noexcept`` to ensure move operations are used even when a
  restrictive move operation (`move_if_noexcept`) is used (e.g., in STL library's containers)
  (:issue:`2966`)

Deprecated:
^^^^^^^^^^^

- The following ``KeyChain::sign*`` methods, in favor of generalized ``KeyChain::sign(..., SigningInfo)``

  * ``KeyChain::sign(Packet, Name)``
  * ``KeyChain::sign(uint8_t*, size_t, Name)``
  * ``KeyChain::signByIdentity(Packet, Name)``
  * ``KeyChain::signByIdentity(uint8_t*, size_t, Name)``
  * ``KeyChain::signWithSha256(Data)``
  * ``KeyChain::signWithSha256(Interest)``

- The following encoding block helper functions:

  * ``nonNegativeIntegerBlock`` (use ``makeNonNegativeIntegerBlock``)
  * ``prependBooleanBlock`` (use ``prependEmptyBlock``)
  * ``booleanBlock`` (use ``makeEmptyBlock``)
  * ``dataBlock`` (use ``makeBinaryBlock``)
  * ``nestedBlock`` (use ``makeNestedBlock``)

- The following methods of ``CommandOptions`` class, use ``setSigningInfo()`` instead (:issue:`2893`):

  * ``CommandOptions::getSigningParamsKind``
  * ``CommandOptions::getSigningIdentity``
  * ``CommandOptions::getSigningCertificate``
  * ``CommandOptions::setSigningDefault``
  * ``CommandOptions::setSigningIdentity``
  * ``CommandOptions::setSigningCertificate``

Removed:
^^^^^^^^

- Remove SCOPE from Interests and all references to it in the related code (:issue:`2345`)

- Direct FIB management in Face class (:issue:`2533`)

  ``Face::register`` and ``Face::setInterestFilter`` methods now only support NFD RIB management
  protocol.

  For special needs, FIB management can be implemented using ``nfd::Controller``
  (``start<FibAddNextHopCommand>``, and ``start<FibRemoveNextHopCommand>``)

Upcoming features (partially finished in development branches):
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- NDNLPv2 (http://redmine.named-data.net/projects/nfd/wiki/NDNLPv2, :issue:`2520`,
  :issue:`2879`, :issue:`2763`, :issue:`2883`, :issue:`2841`, :issue:`2866`)

- New NDN certificate format (:issue:`2861`, :issue:`2868`)

- NDN trust schema as a description of application trust model, which can help automate data
  and interest packet signing and authentication (:issue:`2829`)

- Refactored KeyChain abstraction (:issue:`2926`)
