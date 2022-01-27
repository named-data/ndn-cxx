ndn-cxx version 0.8.0
---------------------

Release date: February 15, 2022

Important changes
^^^^^^^^^^^^^^^^^

- Flip default value of ``CanBePrefix`` to **false** (:issue:`4582`). This means that
  Interests created without explicitly specifying ``CanBePrefix`` option, will be treaded
  as Interests for exact Data name.

- Change default name component encoding convention to typed (:issue:`5044`)

- Update TLV-TYPE numbers to revision 3 of the `naming conventions`_

.. _naming conventions:
   https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/

- Change TLV-TYPE number of ``IncomingFaceId`` field in NDNLP (:issue:`5185`)

- Update ``Interest::ForwardingHint`` format (:issue:`5187`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Support certificate name, versionless certificate name, and key name options for
  KeyLocator name in ``ValidatorConfig`` (:issue:`5142`)

- Fix ``ValidatorConfig`` rule processing bug (:issue:`5145`)

- Generalize signature verification to allow ``digest-sha256``, effectively adding
  support of digest integrity verification in ``ValidatorConfig``

- Enforce ``sig-type`` check in ``ValidatorConfig`` (:issue:`4524`)

- Refactor and cleanup of ``StatusDatasetContext``, including increase of max payload size
  of each segment produced by ``StatusDatasetContext`` to 8,000 bytes

- Prevent potential dereferencing of past-the-end iterator in ``Block::value()``

- Fail early in ``Block::blockFromValue()``, if TLV length is zero

- Improve error reporting when loading a ``Certificate`` or ``SafeBag`` fails

- Add API to get the keyword name component for Metadata and Prefix Announcements

- Backport C++20 ``std::span`` and convert most APIs to use it

- Introduce ``time::{to,from}IsoExtendedString()`` utility functions

- Add support for OpenSSL 3.0. Note that because of the API changes, HMAC implementation
  does not currently work when compiled with OpenSSL 3.0, use older version of OpenSSL
  when needed.

- Various build system and documentation extension and fixes

Deprecations
^^^^^^^^^^^^

- ``Interest::setDefaultCanBePrefix()``

- ``Name::append(Block)`` overload, as it has confusing semantics (:issue:`5186`)

- ``ndnsec`` tool aliases: ``ndnsec-certgen``, ``ndnsec-dump-certificate``,
  ``ndnsec-install-cert``, ``ndnsec-keygen``, ``ndnsec-ls-identity``

Removals
^^^^^^^^

- Previously deprecated ``Signature`` class, its subclasses, and deprecated methods from
  ``Data``, ``KeyChain``, ``SignatureInfo``, and ``SigningInfo``

- Redundant overloads of ``verifySignature()`` and ``verifyDigest()``

- Previously deprecated ``security/v2`` headers

- Previously deprecated ``CommandInterestSigner``

- Unused ``Transport::send()`` overload

- Remove HTTP download feature in ``ndnsec cert-install``, as it does not support HTTPS
  retrieval.  The recommended way of installing certificates published over HTTP/HTTPS has
  been changed to use ``curl`` or ``wget`` (:issue:`4506`)
