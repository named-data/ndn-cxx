ndn-cxx version 0.3.4
---------------------

Release date: August 31, 2015

Changes since version 0.3.3:

New features:
^^^^^^^^^^^^^

- By default, library is now compiled as a shared library (:issue:`2867`)

- Specification for the new NDN certificate format. The reference implementation is being
  developed and will be merged in a future release. (:issue:`2861`)

- Add new ValidityPeriod abstraction into SignatureInfo (:issue:`2868`)

- Add new optional AdditionalInfo abstraction for SignatureInfo (:issue:`3058`)

- Implementation of basic NDNLP2 abstractions (:issue:`2875`, :issue:`2878`,
  :issue:`2963`, :issue:`2879`)

- ``Face::registerPrefix`` and ``Face::setInterestFilter`` are now unified with the use
  of SigningInfo abstraction (:issue:`2932`)

- New Dispatcher class to simplify server-side implementation of NFD management protocol
  (:issue:`2107`)

Improvements and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Fix compilation error with Boost 1.59 (:issue:`3128`)

- Enhance exception throwing with Boost Exception library (:issue:`2997`)

- Improvements of string helpers (:issue:`3006`)

- Add API in ``Block`` class to add sub-elements (:issue:`2998`)

- Add constructor to create sub-elements using the underlying buffer (:issue:`3100`)

- Fix bug in ``Block::remove``, so it removes all sub-elements of the specified type
  (:issue:`2998`)

- Add ability to create ``util::Digest`` object from ``std::istream&`` (:issue:`3022`)

Deprecated:
^^^^^^^^^^^

- The following methods of ``CommandOptions`` class in favor of ``getSigningInfo`` and
  ``setSigningInfo`` methods (:issue:`2893`):

  * ``getSigningParamsKind``
  * ``getSigningIdentity``
  * ``getSigningCertificate``
  * ``setSigningDefault``
  * ``setSigningIdentity``
  * ``setSigningCertificate``
  * ``setSigningCertificate``

- Overloads of ``Face::registerPrefix`` and ``Face::setInterestFilter`` that do not accept
  SigningInfo parameter (:issue:`2932`)

Upcoming features (partially implemented in development branches):
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Introduce new API to simplify security transformations (:issue:`3009`)

- NDN trust schema as a description of application trust model, which can help automate data
  and interest packet signing and authentication (:issue:`2829`)

- Refactored KeyChain abstraction (:issue:`2926`)
