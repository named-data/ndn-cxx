/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#ifndef NDN_TESTS_TRANSPORT_DUMMY_FACE_HPP
#define NDN_TESTS_TRANSPORT_DUMMY_FACE_HPP

#include "face.hpp"

namespace ndn {

class DummyTransport : public Transport
{
public:
  void
  receive(const Block& block)
  {
    m_receiveCallback(block);
  }

  virtual void
  close()
  {
  }

  virtual void
  pause()
  {
  }

  virtual void
  resume()
  {
  }

  virtual void
  send(const Block& wire)
  {
    if (wire.type() == Tlv::Interest) {
      m_sentInterests->push_back(Interest(wire));
    }
    else if (wire.type() == Tlv::Data) {
      m_sentDatas->push_back(Data(wire));
    }
  }

  virtual void
  send(const Block& header, const Block& payload)
  {
    this->send(payload);
  }

public:
  std::vector<Interest>* m_sentInterests;
  std::vector<Data>*     m_sentDatas;
};


/** \brief a Face for unit testing
 */
class DummyFace : public Face
{
public:
  explicit
  DummyFace(shared_ptr<DummyTransport> transport)
    : Face(transport)
    , m_transport(transport)
  {
    m_transport->m_sentInterests = &m_sentInterests;
    m_transport->m_sentDatas     = &m_sentDatas;
  }

  /** \brief cause the Face to receive a packet
   */
  template<typename Packet>
  void
  receive(const Packet& packet)
  {
    m_transport->receive(packet.wireEncode());
  }

public:
  std::vector<Interest> m_sentInterests;
  std::vector<Data>     m_sentDatas;

private:
  shared_ptr<DummyTransport> m_transport;
};

inline shared_ptr<DummyFace>
makeDummyFace()
{
  return make_shared<DummyFace>(make_shared<DummyTransport>());
}

} // namespace ndn

#endif // NDN_TESTS_TRANSPORT_DUMMY_FACE_HPP
