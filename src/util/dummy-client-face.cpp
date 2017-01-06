/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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
#include "../detail/lp-field-tag.hpp"
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

  void
  close() override
  {
  }

  void
  pause() override
  {
  }

  void
  resume() override
  {
  }

  void
  send(const Block& wire) override
  {
    onSendBlock(wire);
  }

  void
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

struct DummyClientFace::BroadcastLink
{
  std::vector<DummyClientFace*> faces;
};

DummyClientFace::AlreadyLinkedError::AlreadyLinkedError()
  : Error("Face has already been linked to another face")
{
}

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

DummyClientFace::~DummyClientFace()
{
  unlink();
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
        addTagFromField<lp::CongestionMarkTag, lp::CongestionMarkField>(*nack, lpPacket);
        onSendNack(*nack);
      }
      else {
        addTagFromField<lp::NextHopFaceIdTag, lp::NextHopFaceIdField>(*interest, lpPacket);
        addTagFromField<lp::CongestionMarkTag, lp::CongestionMarkField>(*interest, lpPacket);
        onSendInterest(*interest);
      }
    }
    else if (block.type() == tlv::Data) {
      shared_ptr<Data> data = make_shared<Data>(block);
      addTagFromField<lp::CachePolicyTag, lp::CachePolicyField>(*data, lpPacket);
      addTagFromField<lp::CongestionMarkTag, lp::CongestionMarkField>(*data, lpPacket);
      onSendData(*data);
    }
  });

  if (options.enablePacketLogging)
    this->enablePacketLogging();

  if (options.enableRegistrationReply)
    this->enableRegistrationReply();

  m_processEventsOverride = options.processEventsOverride;

  enableBroadcastLink();
}

void
DummyClientFace::enableBroadcastLink()
{
  this->onSendInterest.connect([this] (const Interest& interest) {
      if (m_bcastLink != nullptr) {
        for (auto otherFace : m_bcastLink->faces) {
          if (otherFace != this) {
            otherFace->receive(interest);
          }
        }
      }
    });
  this->onSendData.connect([this] (const Data& data) {
      if (m_bcastLink != nullptr) {
        for (auto otherFace : m_bcastLink->faces) {
          if (otherFace != this) {
            otherFace->receive(data);
          }
        }
      }
    });
  this->onSendNack.connect([this] (const lp::Nack& nack) {
      if (m_bcastLink != nullptr) {
        for (auto otherFace : m_bcastLink->faces) {
          if (otherFace != this) {
            otherFace->receive(nack);
          }
        }
      }
    });
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
    params.setOrigin(nfd::ROUTE_ORIGIN_APP);
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

void
DummyClientFace::receive(const Interest& interest)
{
  lp::Packet lpPacket(interest.wireEncode());

  addFieldFromTag<lp::IncomingFaceIdField, lp::IncomingFaceIdTag>(lpPacket, interest);
  addFieldFromTag<lp::NextHopFaceIdField, lp::NextHopFaceIdTag>(lpPacket, interest);
  addFieldFromTag<lp::CongestionMarkField, lp::CongestionMarkTag>(lpPacket, interest);

  static_pointer_cast<Transport>(getTransport())->receive(lpPacket.wireEncode());
}

void
DummyClientFace::receive(const Data& data)
{
  lp::Packet lpPacket(data.wireEncode());

  addFieldFromTag<lp::IncomingFaceIdField, lp::IncomingFaceIdTag>(lpPacket, data);
  addFieldFromTag<lp::CongestionMarkField, lp::CongestionMarkTag>(lpPacket, data);

  static_pointer_cast<Transport>(getTransport())->receive(lpPacket.wireEncode());
}

void
DummyClientFace::receive(const lp::Nack& nack)
{
  lp::Packet lpPacket;
  lpPacket.add<lp::NackField>(nack.getHeader());
  Block interest = nack.getInterest().wireEncode();
  lpPacket.add<lp::FragmentField>(make_pair(interest.begin(), interest.end()));

  addFieldFromTag<lp::IncomingFaceIdField, lp::IncomingFaceIdTag>(lpPacket, nack);
  addFieldFromTag<lp::CongestionMarkField, lp::CongestionMarkTag>(lpPacket, nack);

  static_pointer_cast<Transport>(getTransport())->receive(lpPacket.wireEncode());
}

void
DummyClientFace::linkTo(DummyClientFace& other)
{
  if (m_bcastLink != nullptr && other.m_bcastLink != nullptr) {
    if (m_bcastLink != other.m_bcastLink) {
      // already on different links
      BOOST_THROW_EXCEPTION(AlreadyLinkedError());
    }
  }
  else if (m_bcastLink == nullptr && other.m_bcastLink != nullptr) {
    m_bcastLink = other.m_bcastLink;
    m_bcastLink->faces.push_back(this);
  }
  else if (m_bcastLink != nullptr && other.m_bcastLink == nullptr) {
    other.m_bcastLink = m_bcastLink;
    m_bcastLink->faces.push_back(&other);
  }
  else {
    m_bcastLink = other.m_bcastLink = make_shared<BroadcastLink>();
    m_bcastLink->faces.push_back(this);
    m_bcastLink->faces.push_back(&other);
  }
}

void
DummyClientFace::unlink()
{
  if (m_bcastLink == nullptr) {
    return;
  }

  auto it = std::find(m_bcastLink->faces.begin(), m_bcastLink->faces.end(), this);
  BOOST_ASSERT(it != m_bcastLink->faces.end());
  m_bcastLink->faces.erase(it);

  if (m_bcastLink->faces.size() == 1) {
    m_bcastLink->faces[0]->m_bcastLink = nullptr;
    m_bcastLink->faces.clear();
  }
  m_bcastLink = nullptr;
}

void
DummyClientFace::doProcessEvents(time::milliseconds timeout, bool keepThread)
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
