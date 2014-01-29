/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FACE_INSTANCE_HPP
#define NDN_FACE_INSTANCE_HPP

#include "encoding/tlv-face-management.hpp"
#include "name.hpp"
#include "encoding/block.hpp"

namespace ndn {

/**
 * An FaceInstance holds an action and  Name prefix and other fields for an forwarding entry.
 */
class FaceInstance {
public:    
  FaceInstance(const std::string &action,
               int64_t     faceId,
               uint32_t    ipProto,
               const std::string &host,
               const std::string &port,
               const std::string &multicastInterface,
               uint32_t    multicastTtl,
               Milliseconds freshnessPeriod) 
    : action_(action)
    , faceId_(faceId)
    , ipProto_(ipProto)
    , host_(host)
    , port_(port)
    , multicastInterface_(multicastInterface)
    , multicastTtl_(multicastTtl)
    , freshnessPeriod_(freshnessPeriod)
  {
  }

  FaceInstance()
    : faceId_(-1)
    , ipProto_(-1)
    , multicastTtl_(-1)
    , freshnessPeriod_(-1)
  {
  }

  // Action
  const std::string& 
  getAction() const { return action_; }

  void 
  setAction(const std::string& action) { action_ = action; wire_.reset(); }

  // FaceID
  int64_t
  getFaceId() const { return faceId_; }

  void 
  setFaceId(int64_t faceId) { faceId_ = faceId; wire_.reset(); }

  // IPProto
  int32_t 
  getIpProto() const { return ipProto_; }

  void 
  setIpProto(int32_t ipProto) { ipProto_ = ipProto; wire_.reset(); }

  // Host
  const std::string& 
  getHost() const { return host_; }

  void 
  setHost(const std::string& host) { host_ = host; wire_.reset(); }

  // Port
  const std::string&
  getPort() const { return port_; }

  void 
  setPort(const std::string &port) { port_ = port; wire_.reset(); }

  // MulticastInterface
  const std::string& 
  getMulticastInterface() const { return multicastInterface_; }

  void 
  setMulticastInterface(const std::string& multicastInterface) { multicastInterface_ = multicastInterface; wire_.reset(); }

  // MulticastTTL
  int32_t
  getMulticastTtl() const { return multicastTtl_; }

  void 
  setMulticastTtl(int32_t multicastTtl) { multicastTtl_ = multicastTtl; wire_.reset(); }

  // Freshness
  int 
  getFreshnessPeriod() const { return freshnessPeriod_; }

  void 
  setFreshnessPeriod(int freshnessPeriod) { freshnessPeriod_ = freshnessPeriod; wire_.reset(); }

  // Wire
  inline const Block&
  wireEncode() const;
  
  inline void 
  wireDecode(const Block &wire);
  
private:
  std::string action_;
  int64_t     faceId_;
  int32_t     ipProto_;
  std::string host_;
  std::string port_;
  std::string multicastInterface_;
  int32_t     multicastTtl_;
  Milliseconds freshnessPeriod_;
  
  mutable Block wire_;
};

inline const Block&
FaceInstance::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  // FaceInstance ::= FACE-INSTANCE-TYPE TLV-LENGTH
  //                  Action?
  //                  FaceID?
  //                  IPProto?
  //                  Host?
  //                  Port?
  //                  MulticastInterface?
  //                  MulticastTTL?
  //                  FreshnessPeriod?
  
  wire_ = Block(Tlv::FaceManagement::FaceInstance);

  // Action
  if (!action_.empty())
    {
      wire_.push_back
        (dataBlock(Tlv::FaceManagement::Action, action_.c_str(), action_.size()));
    }

  // FaceID
  if (faceId_ >= 0)
    {
      wire_.push_back
        (nonNegativeIntegerBlock(Tlv::FaceManagement::FaceID, faceId_));
    }

  // IPProto
  if (ipProto_ >= 0)
    {
      wire_.push_back
        (nonNegativeIntegerBlock(Tlv::FaceManagement::IPProto, ipProto_));
    }
  
  // Host
  if (!host_.empty())
    {
      wire_.push_back
        (dataBlock(Tlv::FaceManagement::Host, host_.c_str(), host_.size()));
    }

  // Port
  if (!port_.empty())
    {
      wire_.push_back
        (dataBlock(Tlv::FaceManagement::Port, port_.c_str(), port_.size()));
    }

  // MulticastInterface
  if (!multicastInterface_.empty())
    {
      wire_.push_back
        (dataBlock(Tlv::FaceManagement::MulticastInterface, multicastInterface_.c_str(), multicastInterface_.size()));
    }

  // MulticastTTL
  if (multicastTtl_ >= 0)
    {
      wire_.push_back
        (nonNegativeIntegerBlock(Tlv::FaceManagement::MulticastTTL, multicastTtl_));
    }

  // FreshnessPeriod
  if (freshnessPeriod_ >= 0)
    {
      wire_.push_back
        (nonNegativeIntegerBlock(Tlv::FreshnessPeriod, freshnessPeriod_));
    }
  
  wire_.encode();
  return wire_;    
}
  
inline void 
FaceInstance::wireDecode(const Block &wire)
{
  action_.clear();
  faceId_ = -1;
  ipProto_ = -1;
  host_.clear();
  port_.clear();
  multicastInterface_.clear();
  multicastTtl_ = -1;
  freshnessPeriod_ = -1;

  wire_ = wire;
  wire_.parse();

  // FaceInstance ::= FACE-INSTANCE-TYPE TLV-LENGTH
  //                  Action?
  //                  FaceID?
  //                  IPProto?
  //                  Host?
  //                  Port?
  //                  MulticastInterface?
  //                  MulticastTTL?
  //                  FreshnessPeriod?

  // Action
  Block::element_iterator val = wire_.find(Tlv::FaceManagement::Action);
  if (val != wire_.getAll().end())
    {
      action_ = std::string(reinterpret_cast<const char*>(val->value()), val->value_size());
    }

  // FaceID
  val = wire_.find(Tlv::FaceManagement::FaceID);
  if (val != wire_.getAll().end())
    {
      faceId_ = readNonNegativeInteger(*val);
    }

  // IPProto
  val = wire_.find(Tlv::FaceManagement::IPProto);
  if (val != wire_.getAll().end())
    {
      ipProto_ = readNonNegativeInteger(*val);
    }

  // Host
  val = wire_.find(Tlv::FaceManagement::Host);
  if (val != wire_.getAll().end())
    {
      host_ = std::string(reinterpret_cast<const char*>(val->value()), val->value_size());
    }

  // Port
  val = wire_.find(Tlv::FaceManagement::Port);
  if (val != wire_.getAll().end())
    {
      port_ = std::string(reinterpret_cast<const char*>(val->value()), val->value_size());
    }

  // MulticastInterface
  val = wire_.find(Tlv::FaceManagement::MulticastInterface);
  if (val != wire_.getAll().end())
    {
      multicastInterface_ = std::string(reinterpret_cast<const char*>(val->value()), val->value_size());
    }

  // MulticastTTL
  val = wire_.find(Tlv::FaceManagement::MulticastTTL);
  if (val != wire_.getAll().end())
    {
      multicastTtl_ = readNonNegativeInteger(*val);
    }

  // FreshnessPeriod
  val = wire_.find(Tlv::FreshnessPeriod);
  if (val != wire_.getAll().end())
    {
      freshnessPeriod_ = readNonNegativeInteger(*val);
    }
}

inline std::ostream&
operator << (std::ostream &os, const FaceInstance &entry)
{
  os << "FaceInstance(";
  
  // Action
  if (!entry.getAction().empty())
    {
      os << "Action:" << entry.getAction() << ", ";
    }

  // FaceID
  if (entry.getFaceId() >= 0)
    {
      os << "FaceID:" << entry.getFaceId() << ", ";
    }

  // IPProto
  if (entry.getIpProto() >= 0)
    {
      os << "IPProto:" << entry.getIpProto() << ", ";
    }

  // Host
  if (!entry.getHost().empty())
    {
      os << "Host:" << entry.getHost() << ", ";
    }

  // Port
  if (!entry.getPort().empty())
    {
      os << "Port:" << entry.getPort() << ", ";
    }

  // MulticastInterface
  if (!entry.getMulticastInterface().empty())
    {
      os << "MulticastInterface:" << entry.getMulticastInterface() << ", ";
    }

  // MulticastTTL
  if (entry.getMulticastTtl() >= 0)
    {
      os << "MulticastTTL:" << entry.getMulticastTtl() << ", ";
    }

  // FreshnessPeriod
  if (entry.getFreshnessPeriod() >= 0)
    {
      os << "FreshnessPeriod:" << entry.getFreshnessPeriod() << ", ";
    }

  os << ")";
  return os;
}

}

#endif // FACE_INSTANCE
