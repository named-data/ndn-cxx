/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "util/signal.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace util {
namespace signal {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestSignal)

class SignalOwner0
{
public:
  Signal<SignalOwner0> sig;

public:
  DECLARE_SIGNAL_EMIT(sig)

  bool
  isSigEmpty()
  {
    return sig.isEmpty();
  }
};

BOOST_AUTO_TEST_CASE(ZeroSlot)
{
  SignalOwner0 so;
  BOOST_CHECK_NO_THROW(so.emitSignal(sig));
}

BOOST_AUTO_TEST_CASE(TwoListeners)
{
  SignalOwner0 so;

  int hit1 = 0, hit2 = 0;
  so.sig.connect([&hit1] { ++hit1; });
  so.sig.connect([&hit2] { ++hit2; });

  so.emitSignal(sig);

  BOOST_CHECK_EQUAL(hit1, 1);
  BOOST_CHECK_EQUAL(hit2, 1);
}

class SignalOwner1
{
public:
  Signal<SignalOwner1, int> sig;

protected:
  DECLARE_SIGNAL_EMIT(sig)
};

class SignalEmitter1 : public SignalOwner1
{
public:
  void
  emitTestSignal()
  {
    this->emitSignal(sig, 8106);
  }
};

BOOST_AUTO_TEST_CASE(OneArgument)
{
  SignalEmitter1 se;

  int hit = 0;
  se.sig.connect([&hit] (int a) {
    ++hit;
    BOOST_CHECK_EQUAL(a, 8106);
  });
  se.emitTestSignal();

  BOOST_CHECK_EQUAL(hit, 1);
}

BOOST_AUTO_TEST_CASE(TwoArguments)
{
  Signal<std::remove_pointer<decltype(this)>::type, int, int> sig;

  int hit = 0;
  sig.connect([&hit] (int a, int b) {
    ++hit;
    BOOST_CHECK_EQUAL(a, 21);
    BOOST_CHECK_EQUAL(b, 22);
  });
  sig(21, 22);

  BOOST_CHECK_EQUAL(hit, 1);
}

class RefObject
{
public:
  RefObject()
  {
  }

  RefObject(const RefObject& other)
  {
    ++s_copyCount;
  }

public:
  static int s_copyCount;
};
int RefObject::s_copyCount = 0;

// Signal passes arguments by reference,
// but it also allows a handler that accept arguments by value
BOOST_AUTO_TEST_CASE(HandlerByVal)
{
  RefObject refObject;
  RefObject::s_copyCount = 0;

  Signal<std::remove_pointer<decltype(this)>::type, RefObject> sig;
  sig.connect([] (RefObject ro) {});
  sig(refObject);

  BOOST_CHECK_EQUAL(RefObject::s_copyCount, 1);
}

// Signal passes arguments by reference, and no copying
// is necessary when handler accepts arguments by reference
BOOST_AUTO_TEST_CASE(HandlerByRef)
{
  RefObject refObject;
  RefObject::s_copyCount = 0;

  Signal<std::remove_pointer<decltype(this)>::type, RefObject> sig;
  sig.connect([] (const RefObject& ro) {});
  sig(refObject);

  BOOST_CHECK_EQUAL(RefObject::s_copyCount, 0);
}

BOOST_AUTO_TEST_CASE(ManualDisconnect)
{
  SignalOwner0 so;

  int hit = 0;
  Connection c1 = so.sig.connect([&hit] { ++hit; });
  BOOST_CHECK_EQUAL(c1.isConnected(), true);

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler called

  Connection c2 = c1; // make a copy
  BOOST_CHECK_EQUAL(c2.isConnected(), true);
  BOOST_CHECK_EQUAL(c1.isConnected(), true);
  c2.disconnect();
  BOOST_CHECK_EQUAL(c2.isConnected(), false);
  BOOST_CHECK_EQUAL(c1.isConnected(), false);
  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler not called

  BOOST_CHECK_NO_THROW(c2.disconnect());
  BOOST_CHECK_NO_THROW(c1.disconnect());
}

BOOST_AUTO_TEST_CASE(ManualDisconnectDestructed)
{
  auto so = make_unique<SignalOwner0>();

  int hit = 0;
  Connection connection = so->sig.connect([&hit] { ++hit; });

  so->emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler called

  BOOST_CHECK_EQUAL(connection.isConnected(), true);
  so.reset(); // destruct Signal
  BOOST_CHECK_EQUAL(connection.isConnected(), false);
  BOOST_CHECK_NO_THROW(connection.disconnect());
}

BOOST_AUTO_TEST_CASE(AutoDisconnect)
{
  SignalOwner0 so;

  int hit = 0;
  {
    ScopedConnection sc = so.sig.connect([&hit] { ++hit; });

    BOOST_CHECK_EQUAL(sc.isConnected(), true);
    so.emitSignal(sig);
    BOOST_CHECK_EQUAL(hit, 1); // handler called

    // sc goes out of scope, disconnecting
  }

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler not called
}

BOOST_AUTO_TEST_CASE(AutoDisconnectAssign)
{
  SignalOwner0 so;

  int hit1 = 0, hit2 = 0;
  ScopedConnection sc = so.sig.connect([&hit1] { ++hit1; });
  BOOST_CHECK_EQUAL(sc.isConnected(), true);

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit1, 1); // handler1 called

  sc = so.sig.connect([&hit2] { ++hit2; }); // handler1 is disconnected
  BOOST_CHECK_EQUAL(sc.isConnected(), true);

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit1, 1); // handler1 not called
  BOOST_CHECK_EQUAL(hit2, 1); // handler2 called
}

BOOST_AUTO_TEST_CASE(AutoDisconnectAssignSame)
{
  SignalOwner0 so;

  int hit = 0;
  Connection c1 = so.sig.connect([&hit] { ++hit; });

  ScopedConnection sc(c1);
  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler called
  BOOST_CHECK_EQUAL(c1.isConnected(), true);
  BOOST_CHECK_EQUAL(sc.isConnected(), true);

  sc = c1; // assign same connection
  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 2); // handler called
  BOOST_CHECK_EQUAL(c1.isConnected(), true);
  BOOST_CHECK_EQUAL(sc.isConnected(), true);

  Connection c2 = c1;
  sc = c2; // assign a copy of same connection
  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 3); // handler called
  BOOST_CHECK_EQUAL(c1.isConnected(), true);
  BOOST_CHECK_EQUAL(c2.isConnected(), true);
  BOOST_CHECK_EQUAL(sc.isConnected(), true);
}

BOOST_AUTO_TEST_CASE(AutoDisconnectRelease)
{
  SignalOwner0 so;

  int hit = 0;
  {
    ScopedConnection sc = so.sig.connect([&hit] { ++hit; });

    so.emitSignal(sig);
    BOOST_CHECK_EQUAL(hit, 1); // handler called
    BOOST_CHECK_EQUAL(sc.isConnected(), true);

    sc.release();
    BOOST_CHECK_EQUAL(sc.isConnected(), false);
    // sc goes out of scope, but not disconnecting
  }

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 2); // handler called
}

BOOST_AUTO_TEST_CASE(AutoDisconnectMove)
{
  SignalOwner0 so;
  unique_ptr<ScopedConnection> sc2;

  int hit = 0;
  {
    ScopedConnection sc = so.sig.connect([&hit] { ++hit; });

    so.emitSignal(sig);
    BOOST_CHECK_EQUAL(hit, 1); // handler called
    BOOST_CHECK_EQUAL(sc.isConnected(), true);

    sc2.reset(new ScopedConnection(std::move(sc)));
    BOOST_CHECK_EQUAL(sc.isConnected(), false);
    BOOST_CHECK_EQUAL(sc2->isConnected(), true);

    // sc goes out of scope, but not disconnecting
  }

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 2); // handler called
}

BOOST_AUTO_TEST_CASE(ConnectSingleShot)
{
  SignalOwner0 so;

  int hit = 0;
  so.sig.connectSingleShot([&hit] { ++hit; });

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler called

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler not called
}

BOOST_AUTO_TEST_CASE(ConnectSingleShotDisconnected)
{
  SignalOwner0 so;

  int hit = 0;
  Connection conn = so.sig.connectSingleShot([&hit] { ++hit; });
  BOOST_CHECK_EQUAL(conn.isConnected(), true);
  conn.disconnect();
  BOOST_CHECK_EQUAL(conn.isConnected(), false);

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 0); // handler not called
}

BOOST_AUTO_TEST_CASE(ConnectSingleShot1)
{
  SignalEmitter1 se;

  int hit = 0;
  se.sig.connectSingleShot([&hit] (int) { ++hit; });

  se.emitTestSignal();
  BOOST_CHECK_EQUAL(hit, 1); // handler called

  se.emitTestSignal();
  BOOST_CHECK_EQUAL(hit, 1); // handler not called
}

BOOST_AUTO_TEST_CASE(ConnectInHandler)
{
  SignalOwner0 so;

  int hit1 = 0, hit2 = 0; bool hasHandler2 = false;
  so.sig.connect([&] {
    ++hit1;
    if (!hasHandler2) {
      so.sig.connect([&] { ++hit2; });
      hasHandler2 = true;
    }
  });

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit1, 1); // handler1 called
  BOOST_CHECK_EQUAL(hit2, 0); // handler2 not called

  // new subscription takes effect
  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit1, 2); // handler1 called
  BOOST_CHECK_EQUAL(hit2, 1); // handler2 called
}

BOOST_AUTO_TEST_CASE(DisconnectSelfInHandler)
{
  SignalOwner0 so;

  int hit = 0;
  Connection connection;
  BOOST_CHECK_EQUAL(connection.isConnected(), false);
  connection = so.sig.connect([&so, &connection, &hit] {
    ++hit;
    BOOST_CHECK_EQUAL(connection.isConnected(), true);
    connection.disconnect();
    BOOST_CHECK_EQUAL(connection.isConnected(), false);
    BOOST_CHECK_EQUAL(so.isSigEmpty(), false); // disconnecting hasn't taken effect
  });

  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler called
  BOOST_CHECK_EQUAL(connection.isConnected(), false);

  // disconnecting takes effect
  BOOST_CHECK_EQUAL(so.isSigEmpty(), true);
  so.emitSignal(sig);
  BOOST_CHECK_EQUAL(hit, 1); // handler not called
}

BOOST_AUTO_TEST_CASE(ThrowInHandler)
{
  SignalOwner0 so;

  struct HandlerError : public std::exception
  {
  };

  int hit = 0;
  so.sig.connect([&] {
    ++hit;
    BOOST_THROW_EXCEPTION(HandlerError());
  });

  BOOST_CHECK_THROW(so.emitSignal(sig), HandlerError);
  BOOST_CHECK_EQUAL(hit, 1); // handler called

  BOOST_CHECK_THROW(so.emitSignal(sig), HandlerError);
  BOOST_CHECK_EQUAL(hit, 2); // handler called
}

BOOST_AUTO_TEST_SUITE_END() // TestSignal
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace signal
} // namespace util
} // namespace ndn
