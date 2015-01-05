Trivial applications
====================

.. note::

    To successfully run the following examples, please make sure that NFD is properly
    configured and running.  For more information about NFD, refer to `NFD's official
    homepage <http://named-data.net/doc/NFD/>`_.

Trivial consumer
----------------

In the following trivial example, a consumer creates a :ndn-cxx:`Face` with default
transport (:ndn-cxx:`UnixTransport`) and sends an Interest for
``/localhost/testApp/randomData``.  While expressing Interest, the app specifies two
callbacks to be called when Data is retrieved or Interest times out.

.. literalinclude:: ../examples/consumer.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 24-27,39,43-46,50,57,67


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
   :emphasize-lines: 42,55-58,61,64,67,70,76


Consumer that uses ndn::Scheduler
---------------------------------

The following example demonstrates how to use :ndn-cxx:`ndn::Scheduler` to schedule arbitrary
events for execution at specific points of time.

The library internally uses `boost::asio::io_service
<http://www.boost.org/doc/libs/1_48_0/doc/html/boost_asio/reference/io_service.html>`_ to
implement fully asynchronous NDN operations (i.e., sending and receiving Interests and
Data).  In addition to network-related operations, ``boost::asio::io_service`` can be used
to execute any arbitrary callback within the processing thread (run either explicitly via
``io.run`` or implicitly via :ndn-cxx:`Face::processEvents` as in previous examples).
:ndn-cxx:`ndn::Scheduler` is just a wrapper on top of ``boost::asio::io_service``,
allowing simple interface to schedule tasks at specific times.

The highlighted lines in the example demonstrate all that is needed to express a second
Interest approximately 2 seconds after the first one.

.. literalinclude:: ../examples/consumer-with-timer.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 39-40,51-54,58-59,61-62,99-100

