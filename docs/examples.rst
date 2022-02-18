Trivial applications
====================

.. note::

    To successfully run the following examples, please make sure that NFD is properly
    configured and running.  For more information about NFD, refer to `NFD's official
    homepage <https://named-data.net/doc/NFD/>`_.

Trivial consumer
----------------

In the following trivial example, a consumer creates a :ndn-cxx:`Face` with default
transport (:ndn-cxx:`UnixTransport`) and sends an Interest for
``/localhost/testApp/randomData``.  While expressing Interest, the app specifies three
callbacks to be called when Data/Nack is retrieved or Interest times out.

.. literalinclude:: ../examples/consumer.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 43-48,51-54,62,76,82

Trivial producer
----------------

The following example demonstrates how to write a simple producer application.

First, the application sets an Interest filter for ``/localhost/testApp`` to receive all
Interests that have this prefix.  The :ndn-cxx:`Face::setInterestFilter` call accepts two
callbacks; the first will be called when an Interest is received and the second if prefix
registration fails.

After an Interest is received, the producer creates a Data packet with the same name as
the received Interest, adds content, and signs it with the system-default identity.  It is
also possible to specify a particular key to be used during the signing.  For more
information, refer to :ndn-cxx:`KeyChain API documentation <KeyChain>`.

Finally, after Data packet has been created and signed, it is returned to the requester
using :ndn-cxx:`Face::put` method.

.. literalinclude:: ../examples/producer.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 39-42,55,62-64,79,87,91

Consumer that uses Scheduler
----------------------------

The following example demonstrates how to use :ndn-cxx:`Scheduler` to schedule arbitrary
events for execution at specific points of time.

The library internally uses `boost::asio::io_service
<https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/reference/io_service.html>`_ to
implement fully asynchronous NDN operations (i.e., sending and receiving Interests and
Data).  In addition to network-related operations, ``boost::asio::io_service`` can be used
to execute any arbitrary callback within the processing thread (run either explicitly via
``io_service::run()`` or implicitly via :ndn-cxx:`Face::processEvents` as in previous
examples). :ndn-cxx:`Scheduler` is just a wrapper on top of ``io_service``, providing a
simple interface to schedule tasks at specific times.

The highlighted lines in the example demonstrate all that is needed to express a second
Interest approximately 3 seconds after the first one.

.. literalinclude:: ../examples/consumer-with-timer.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 47-50,53,84,96-99,104-106
