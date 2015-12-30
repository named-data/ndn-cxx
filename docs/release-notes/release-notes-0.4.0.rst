ndn-cxx version 0.4.0
---------------------

Release date: December 31, 2015

.. note::
   Version 0.4.0 introduces several breaking changes to API and wire format of management protocols

Changes since version 0.3.4:

New features:
^^^^^^^^^^^^^

- **(breaking change)** LocalControlHeader for special signaling between application and NFD has
  been replaced with NDNLPv2 signaling (:issue:`2879`, :issue:`2930`)

- NDNLPv2 Network NACK support in Face abstraction (:issue:`2930`)

- **(breaking change)** FacePersistency field added to API and wire format of ControlParameters abstraction (:issue:`2991`)

- **(breaking change)** NACK counters added to ForwarderStatus and FaceStatus datasets (:issue:`3174`)

- Backport C++14 ``std::make_unique`` (:issue:`3093`)

- Emulate ``std::to_string`` on platforms that do not provide it (:issue:`2743`)

- New API in Face class to remove all pending Interests (:issue:`3300`)

- Ability to choose signing parameters (a specific certificate, the default certificate for
  a key, or the default certificate for the default key) using a specially URI string
  (:issue:`3281`)

- Ability to create Face with custom IO service and default transport (:issue:`2500`)

Improvements and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **(breaking change)** SegmentFetcher helper changed to use asynchronous data validation (:issue:`2734`)

- Support Interest Nack in nfd::Controller (:issue:`3264`)

- Fix bug causing failure of Face if there is a large delay between construction of Face and calling processEvents
  (:issue:`2742`, :issue:`2526`)

- Cache Link object after the first call to getLink (:issue:`3158`)

- Minor documentation updates (:issue:`3207`, :issue:`3210`, :issue:`3221`)

Deprecated:
^^^^^^^^^^^

- Overloads of ``Face::registerPrefix`` and ``Face::setInterestFilter`` that do not accept
  SigningInfo parameter (:issue:`2932`)

- tlvdump tool (:issue:`3196`).  Use ``ndn-dissect`` program from ndn-tools repository
  `<https://github.com/named-data/ndn-tools>`__

- ndn::util::makeDummyClientFace, use ndn::util::DummyClientFace constructors directly
  (:issue:`3146`)

- ndn::util::DummyClientFace::sentDatas, use ndn::util::DummyClientFace.sentData instead
  (:issue:`3146`)

Removed:
^^^^^^^^

- Deprecated EventEmitter class (:issue:`2351`)

Planned features for future releases:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Introduce new API to simplify security transformations (:issue:`3009`)

- NDN trust schema as a description of application trust model, which can help automate data
  and interest packet signing and authentication (:issue:`2829`)

- Refactored KeyChain abstraction (:issue:`2926`)
