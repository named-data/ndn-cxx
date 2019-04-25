ndn-cxx version 0.6.6
---------------------

Release date: April 27, 2019

Note that this is the last release that encodes to `NDN packet format version 0.2.1
<https://named-data.net/doc/NDN-packet-spec/0.2.1/>`__. A future release will continue to
decode v0.2.1 format, but will encode to `v0.3 format
<https://named-data.net/doc/NDN-packet-spec/0.3/>`__.

New features:
^^^^^^^^^^^^^

- More support for `NDN packet format version
  0.3 <https://named-data.net/doc/NDN-packet-spec/0.3/>`__ (:issue:`4527`)

  * Support new naming conventions to encode/decode segment numbers, byte offsets, versions,
    timestamps, and sequence numbers based on typed name components (:issue:`4777`)

  * Stop using ``ChildSelector`` in ``CertificateBundleFetcher``  (:issue:`4665`)

- ``NDN_THROW`` macro to throw exceptions including file position and runtime context of the
  throw statement (:issue:`4834`)

- Ensure that a ``Block`` with TLV-TYPE zero is treated as invalid (:issue:`4726`, :issue:`4895`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Avoid directly using memory address as ``PendingInterestId`` to prevent potential false
  removal of an entry (:issue:`2865`)

- Follow up packet specification changes to rename the ``Parameters`` element to
  ``ApplicationParameters`` and to change its number to be a non-critical element type
  (:issue:`4658`, :issue:`4780`)

- Add option to disable infrastructure interest in ``CertificateFetcherDirectFetch`` (:issue:`4879`)

- Fix compilation against recent versions of Boost libraries and issues with Xcode 10.2 and
  older versions of Boost libraries (:issue:`4890`, :issue:`4923`, :issue:`4860`)

- Improve the "CanBePrefix unset" warning (:issue:`4581`)

- Improve documentation

Deprecated
~~~~~~~~~~

- ``PendingInterestId``, ``RegisteredPrefixId``, and ``InterestFilterId`` types in favor of
  ``PendingInterestHandle``, ``RegisteredPrefixHandle``, and ``InterestFilterHandle``
  (:issue:`4316`, :issue:`3919`)

- ``Block::empty`` in favor of ``Block::isValid`` (with inverted semantics)

- ``Scheduler::scheduleEvent`` and ``Scheduler::cancelEvent`` in favor of ``Scheduler::schedule``
  and ``EventId::cancel`` (or use ``ScopedEventId`` to automatically cancel the event when
  the object goes out of scope) (:issue:`4883`)

- ``ndn::util::Scheduler`` (use ``ndn::Scheduler`` or ``ndn::scheduler::Scheduler``) and
  ``ndn::EventId`` (use ``ndn::scheduler::EventId``) (:issue:`4883`)
