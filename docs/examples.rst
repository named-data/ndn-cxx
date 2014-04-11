Basic examples
==============

Trivial consumer
----------------

In the following trivial example, a consumer creates a Face with default transport (UnixSocket transport) and sends an Interest for ``/localhost/testApp/randomData``.
While expressing Interest, the app specifies two callbacks to be called when Data is retrieved or Interest times out.

``ndn::bind`` is an alias for either `boost::bind <http://www.boost.org/doc/libs/1_55_0/libs/bind/bind.html>`_ or `std::bind <http://en.cppreference.com/w/cpp/utility/functional/bind>`_ when the library is compiled in C++11 mode.

.. literalinclude:: ../examples/consumer.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 17-19,25-27,41-44,47


Trivial producer
----------------

The following example demonstrates how to write a simple producer application.

First, application sets interset filter for ``/localhost/testApp`` to receive all Interests that have this prefix.
``setInterestFilter`` call accepts two callbacks, one which will be called when an Interest is received, and the other if prefix registration (i.e., configuring proper FIB entry in NFD) fails.

After Interest is received, a producer creates a Data packet with the same name as in the received Interest, adds a silly content, and signs the Data packet with the system-default identity.
It is possible to specify a particular key to be used during the signing.
For more information, refer to KeyChain API documentation.

Finally, after Data packet has been created and signed, it is returned to the requester using ``Face::put`` method.

.. literalinclude:: ../examples/producer.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 37-40,43,49,63-65


Consumer that uses ndn::Scheduler
---------------------------------

The following example demonstrates use for ``ndn::Scheduler`` to schedule an arbitrary events for execution at specific points of time.

The library internally uses `boost::asio::io_service <http://www.boost.org/doc/libs/1_48_0/doc/html/boost_asio/reference/io_service.html>`_ to implement fully asynchronous NDN operations (i.e., sending and receiving Interests and Data).
In addition to network-related operations, ``boost::asio::io_service`` can be used to execute any arbitrary callback within the processing thread (run either explicitly via ``io->run`` or implicitly via ``Face::processEvents`` as in previous examples).
``ndn::Scheduler`` is just a wrapper on top of ``boost::asio::io_service``, allowing simple interface to schedule tasks at specific times.

The highlighted lines in the example demonstrate all that is needed to express a second interest approximately 2 seconds after the first one.

.. literalinclude:: ../examples/consumer-with-timer.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 10,62,69,72-73,76
