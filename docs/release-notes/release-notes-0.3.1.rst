ndn-cxx version 0.3.1
---------------------

Release date: March 3, 2015

Changes since version 0.3.0:

New features:
^^^^^^^^^^^^^

- **Management**

  - Add isConnected() in :ndn-cxx:`signal::Connection` and :ndn-cxx:`signal::ScopedConnection`
    (:issue:`2308`)

  - Extend :ndn-cxx:`DummyClientFace` to process ``LocalControlHeader`` (:issue:`2510`)

  - Add CachingPolicy to ``LocalControlHeader`` (:issue:`2183`)

Updates and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^

- **Wire encoding**

  - Refactoring of :ndn-cxx:`EncodingImpl` class template (``EncodingBuffer`` and
    ``EncodingEstimator``) (:issue:`2494`, :issue:`2490`)

  - Change :ndn-cxx:`Block::fromBuffer` return type (:issue:`2553`)

- **Miscellaneous tools**

  - Mark ``Digest<Hash>::operator==`` as unsuitable in a security sensitive context
    (:issue:`2395`)

  - Restrict destruction of Signal from handler (:issue:`2313`)

- **Documentation**

  - Correct function names in security library tutorial (:issue:`2474`)

  - Remind users to reinstall ports after OSX upgrade (:issue:`2284`)

- **Tests**

  - Fix global buffer overflow in ``ImplicitSha256Digest`` test case (:issue:`2307`)

  - Change naming conventions for unit test files and test suite names (:issue:`2497`)

- **Build**

  - Explicitly define ``_GLIBCXX_USE_NANOSLEEP``, necessary for gcc < 4.8 in some environments
    (:issue:`2499`)

Removed:
^^^^^^^^

- Delete deprecated ``Block(std::istream&)`` constructor (:issue:`2241`)

- Delete deprecated ``ndn::ptr_lib``, ``ndn::func_lib`` (:issue:`2110`)

- Delete deprecated ``MetaInfo::TYPE_*`` (:issue:`2170`)
