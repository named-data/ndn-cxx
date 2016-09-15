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

#include "dummy-client-face.hpp"
#include "../lp/packet.hpp"
#include "../lp/tags.hpp"
#include "../mgmt/nfd/controller.hpp"
#include "../mgmt/nfd/control-response.hpp"
#include "../transport/transport.hpp"

#include <boost/asio/io_service.hpp>

namespace ndn {
namespace util {

class DummyClientFace::Transport : public ndn::Transport
{
public:
  void
  receive(Block block) const
  {
    block.encode();
    if (m_receiveCallback) {
      m_receiveCallback(block);
    }
  }

  virtual void
  close() override
  {
  }

  virtual void
  pause() override
  {
  }

  virtual void
  resume() override
  {
  }

  virtual void
  send(const Block& wire) override
  {
    onSendBlock(wire);
  }

  virtual void
  send(const Block& header, const Block& payload) override
  {
    EncodingBuffer encoder(header.size() + payload.size(), header.size() + payload.size());
    encoder.appendByteArray(header.wire(), header.size());
    encoder.appendByteArray(payload.wire(), payload.size());

    this->send(encoder.block());
  }

  boost::asio::io_service&
  getIoService()
  {
    return *m_ioService;
  }

public:
  Signal<Transport, Block> onSendBlock;
};

DummyClientFace::DummyClientFace(const Options& options/* = DummyClientFace::DEFAULT_OPTIONS*/)
  : Face(make_shared<DummyClientFace::Transport>())
  , m_internalKeyChain(new KeyChain)
  , m_keyChain(*m_internalKeyChain)
{
  this->construct(options);
}

DummyClientFace::DummyClientFace(KeyChain& keyChain,
                                 const Options& options/* = DummyClientFace::DEFAULT_OPTIONS*/)
  : Face(make_shared<DummyClientFace::Transport>(), keyChain)
  , m_keyChain(keyChain)
{
  this->construct(options);
}

DummyClientFace::DummyClientFace(boost::asio::io_service& ioService,
                                 const Options& options/* = DummyClientFace::DEFAULT_OPTIONS*/)
  : Face(make_shared<DummyClientFace::Transport>(), ioService)
  , m_internalKeyChain(new KeyChain)
  , m_keyChain(*m_internalKeyChain)
{
  this->construct(options);
}

DummyClientFace::DummyClientFace(boost::asio::io_service& ioService, KeyChain& keyChain,
                                 const Options& options/* = DummyClientFace::DEFAULT_OPTIONS*/)
  : Face(make_shared<DummyClientFace::Transport>(), ioService, keyChain)
  , m_keyChain(keyChain)
{
  this->construct(options);
}

void
DummyClientFace::construct(const Options& options)
{
  static_pointer_cast<Transport>(getTransport())->onSendBlock.connect([this] (const Block& blockFromDaemon) {
    Block packet(blockFromDaemon);
    packet.encode();
    lp::Packet lpPacket(packet);

    Buffer::const_iterator begin, end;
    std::tie(begin, end) = lpPacket.get<lp::FragmentField>();
    Block block(&*begin, std::distance(begin, end));

    if (block.type() == tlv::Interest) {
      shared_ptr<Interest> interest = make_shared<Interest>(block);
      if (lpPacket.has<lp::NackField>()) {
        shared_ptr<lp::Nack> nack = make_shared<lp::Nack>(std::move(*interest));
        nack->setHeader(lpPacket.get<lp::NackField>());
        if (lpPacket.has<lp::NextHopFaceIdField>()) {
          nack->setTag(make_shared<lp::NextHopFaceIdTag>(lpPacket.get<lp::NextHopFaceIdField>()));
        }
        onSendNack(*nack);
      }
      else {
        if (lpPacket.has<lp::NextHopFaceIdField>()) {
          interest->setTag(make_shared<lp::NextHopFaceIdTag>(lpPacket.get<lp::NextHopFaceIdField>()));
        }
        onSendInterest(*interest);
      }
    }
    else if (block.type() == tlv::Data) {
      shared_ptr<Data> data = make_shared<Data>(block);

      if (lpPacket.has<lp::CachePolicyField>()) {
        data->setTag(make_shared<lp::CachePolicyTag>(lpPacket.get<lp::CachePolicyField>()));
      }

      onSendData(*data);
    }
  });

  if (options.enablePacketLogging)
    this->enablePacketLogging();

  if (options.enableRegistrationReply)
    this->enableRegistrationReply();

  m_processEventsOverride = options.processEventsOverride;
}

void
DummyClientFace::enablePacketLogging()
{
  onSendInterest.connect([this] (const Interest& interest) {
    this->sentInterests.push_back(interest);
  });
  onSendData.connect([this] (const Data& data) {
    this->sentData.push_back(data);
  });
  onSendNack.connect([this] (const lp::Nack& nack) {
    this->sentNacks.push_back(nack);
  });
}

void
DummyClientFace::enableRegistrationReply()
{
  onSendInterest.connect([this] (const Interest& interest) {
    static const Name localhostRegistration("/localhost/nfd/rib");
    if (!localhostRegistration.isPrefixOf(interest.getName()))
      return;

    nfd::ControlParameters params(interest.getName().get(-5).blockFromValue());
    params.setFaceId(1);
    params.setOrigin(0);
    if (interest.getName().get(3) == name::Component("register")) {
      params.setCost(0);
    }

    nfd::ControlResponse resp;
    resp.setCode(200);
    resp.setBody(params.wireEncode());

    shared_ptr<Data> data = make_shared<Data>(interest.getName());
    data->setContent(resp.wireEncode());

    m_keyChain.sign(*data, security::SigningInfo(security::SigningInfo::SIGNER_TYPE_SHA256));

    this->getIoService().post([this, data] { this->receive(*data); });
  });
}

template<typename Packet>
void
DummyClientFace::receive(const Packet& packet)
{
  lp::Packet lpPacket(packet.wireEncode());

  shared_ptr<lp::IncomingFaceIdTag> incomingFaceIdTag =
    static_cast<const TagHost&>(packet).getTag<lp::IncomingFaceIdTag>();
  if (incomingFaceIdTag != nullptr) {
    lpPacket.add<lp::IncomingFaceIdField>(*incomingFaceIdTag);
  }

  shared_ptr<lp::NextHopFaceIdTag> nextHopFaceIdTag =
    static_cast<const TagHost&>(packet).getTag<lp::NextHopFaceIdTag>();
  if (nextHopFaceIdTag != nullptr) {
    lpPacket.add<lp::NextHopFaceIdField>(*nextHopFaceIdTag);
  }
  static_pointer_cast<Transport>(getTransport())->receive(lpPacket.wireEncode());
}

template void
DummyClientFace::receive<Interest>(const Interest& packet);

template void
DummyClientFace::receive<Data>(const Data& packet);

template<>
void
DummyClientFace::receive<lp::Nack>(const lp::Nack& nack)
{
  lp::Packet lpPacket;
  lpPacket.add<lp::NackField>(nack.getHeader());
  Block interest = nack.getInterest().wireEncode();
  lpPacket.add<lp::FragmentField>(make_pair(interest.begin(), interest.end()));

  shared_ptr<lp::IncomingFaceIdTag> incomingFaceIdTag = nack.getTag<lp::IncomingFaceIdTag>();
  if (incomingFaceIdTag != nullptr) {
    lpPacket.add<lp::IncomingFaceIdField>(*incomingFaceIdTag);
  }

  static_pointer_cast<Transport>(getTransport())->receive(lpPacket.wireEncode());
}

void
DummyClientFace::doProcessEvents(const time::milliseconds& timeout, bool keepThread)
{
  if (m_processEventsOverride != nullptr) {
    m_processEventsOverride(timeout);
  }
  else {
    this->Face::doProcessEvents(timeout, keepThread);
  }
}

} // namespace util
} // namespace ndn
