ndn-cxx version 0.7.0
---------------------

Release date: January 13, 2020

**Breaking Change**

This release features support of only `NDN packet format version 0.3
<https://docs.named-data.net/NDN-packet-spec/0.3/>`__ (:issue:`4527`, :issue:`4567`,
:issue:`4709`, :issue:`4913`). The library encodes and interprets Interest and Data
packets only in 0.3 format; support for version 0.2 has been completely removed. In
addition, the URI representation of Interest packets has been updated to follow the
packet format changes.

New features
^^^^^^^^^^^^

- HMAC signing support (:issue:`3075`)

- Support for ``ParametersSha256DigestComponent`` in :ndn-cxx:`Name` and :ndn-cxx:`Interest`
  (:issue:`4658`)

- Encoding and decoding of ``HopLimit`` field in Interest (:issue:`4806`)

- Support for encoding and decoding PIT token as NDNLPv2 field (:issue:`4532`).

  PIT token is a hop-by-hop header field that identifies an Interest-Data exchange. The
  downstream node can assign an opaque token to an outgoing Interest, and the upstream node
  is expected to return the same token on the Data or Nack in reply to that Interest. This
  would allow the downstream node to accelerate its processing, especially in PIT lookup.

- :ndn-cxx:`io::loadBuffer` and :ndn-cxx:`io::saveBuffer` helper functions

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Various improvements in the Linux implementation of :ndn-cxx:`NetworkMonitor`

- Rework of ``RttEstimator`` class:

  * split into two classes: :ndn-cxx:`RttEstimator` and :ndn-cxx:`RttEstimatorWithStats`
    (:issue:`4887`)
  * add a getter for the smoothed RTT value (:issue:`4892`)
  * switch to use ``time::nanoseconds`` (:issue:`4887`)

- Make use of attributes in logging facilities and generalize logger backend support
  (:issue:`4969`, :issue:`3782`)

- Silently accept an empty validation policy instead of throwing an exception (:issue:`5049`)

- Introduce alternative URI syntax for component types used in naming conventions, with
  ability to choose between canonical and alternate format (:issue:`4777`)

- Don't force the generation of an Interest nonce during decoding (:issue:`4685`)

- Various documentation improvements

Removals
^^^^^^^^

- ``ndn::util::Scheduler`` (use ``ndn::Scheduler`` or ``ndn::scheduler::Scheduler``) and
  ``ndn::EventId`` (use ``ndn::scheduler::EventId``) (:issue:`4883`)

- Unused ``KeyClass`` and ``AclType`` enums

- Unused ``v2::PublicKey`` alias of ``transform::PublicKey``

- ``HmacFilter`` class, use :ndn-cxx:`SignerFilter` and :ndn-cxx:`VerifierFilter` instead

- Ill-defined equality operators for ``Interest``, ``MetaInfo``, ``Signature`` (:issue:`4569`)

- Implicit conversion from the ``xyzHandle`` types to ``const xyzId*`` (where ``xyz`` is
  ``PendingInterest``, ``RegisteredPrefixId``, and ``InterestFilterId``)

- Deprecated ``KeyLocator::Type`` enum

- Private header files of concrete PIB, TPM, and KeyHandle implementations are no longer
  installed (:issue:`4782`)

- Renamed ``util/backports-ostream-joiner.hpp`` to ``util/ostream-joiner.hpp``
