ndn-cxx version 0.4.0
---------------------

Release date: TBD

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

Improvements and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

Removed:
^^^^^^^^

- Deprecated EventEmitter class (:issue:`2351`)

Planned features for future releases:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Introduce new API to simplify security transformations (:issue:`3009`)

- NDN trust schema as a description of application trust model, which can help automate data
  and interest packet signing and authentication (:issue:`2829`)

- Refactored KeyChain abstraction (:issue:`2926`)
