ndn-cxx: NDN C++ library with eXperimental eXtensions
=====================================================

**ndn-cxx** is a C++14 library implementing Named Data Networking (NDN) primitives
that can be used to write various NDN applications. The library is currently being
used by the following projects:

* `NFD - NDN Forwarding Daemon <https://github.com/named-data/NFD>`_
* `NLSR - Named-data Link-State Routing protocol <https://github.com/named-data/NLSR>`_
* `ndn-tools - Essential NDN command-line tools <https://github.com/named-data/ndn-tools>`_
* `repo-ng - Next generation of NDN repository <https://github.com/named-data/repo-ng>`_
* `ChronoSync - Sync library for multiuser realtime applications <https://github.com/named-data/ChronoSync>`_
* `PSync - Partial and full synchronization library <https://github.com/named-data/PSync>`_
* `ndn-traffic-generator - Traffic generator for NDN <https://github.com/named-data/ndn-traffic-generator>`_
* `NAC - Name-based Access Control <https://github.com/named-data/name-based-access-control>`_
* `NDNS - Domain Name Service for NDN <https://github.com/named-data/ndns>`_

Please submit any bugs or feature requests to the `ndn-cxx issue tracker
<https://redmine.named-data.net/projects/ndn-cxx/issues>`_.

ndn-cxx Documentation
---------------------

.. toctree::
   :hidden:
   :maxdepth: 3

   INSTALL
   examples
   tutorials
   specs
   manpages

.. toctree::
   :hidden:
   :maxdepth: 1

   code-style
   RELEASE_NOTES
   releases

- :doc:`INSTALL`

- :doc:`examples`

- :doc:`tutorials`

   + `NDN Software Contributor's Guide <https://github.com/named-data/NFD/blob/master/CONTRIBUTING.md>`_ (guide for newcomers to the NDN community of software generally)

   + :doc:`tutorials/utils-ndn-regex`
   + :doc:`tutorials/security-validator-config`

- :doc:`specs`

   + :doc:`specs/signed-interest`
   + :doc:`specs/certificate-format`
   + :doc:`specs/safe-bag`
   + :doc:`specs/validation-error-code`

- :doc:`manpages`

**Additional documentation**

- `API documentation (doxygen) <doxygen/annotated.html>`_

- :doc:`code-style`

- :doc:`RELEASE_NOTES`

- :doc:`releases`

License
-------

ndn-cxx is an open source project licensed under the LGPL version 3. For more information about
the license, refer to `COPYING.md <https://github.com/named-data/ndn-cxx/blob/master/COPYING.md>`_.
