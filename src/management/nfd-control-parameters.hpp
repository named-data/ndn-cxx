/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_MANAGEMENT_NFD_CONTROL_PARAMETERS_HPP
#define NDN_MANAGEMENT_NFD_CONTROL_PARAMETERS_HPP

#include "../encoding/nfd-constants.hpp"
#include "../name.hpp"
#include "../util/time.hpp"
#include "../mgmt/control-parameters.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 */
enum ControlParameterField {
  CONTROL_PARAMETER_NAME,
  CONTROL_PARAMETER_FACE_ID,
  CONTROL_PARAMETER_URI,
  CONTROL_PARAMETER_LOCAL_CONTROL_FEATURE,
  CONTROL_PARAMETER_ORIGIN,
  CONTROL_PARAMETER_COST,
  CONTROL_PARAMETER_FLAGS,
  CONTROL_PARAMETER_STRATEGY,
  CONTROL_PARAMETER_EXPIRATION_PERIOD,
  CONTROL_PARAMETER_UBOUND
};

const std::string CONTROL_PARAMETER_FIELD[CONTROL_PARAMETER_UBOUND] = {
  "Name",
  "FaceId",
  "Uri",
  "LocalControlFeature",
  "Origin",
  "Cost",
  "Flags",
  "Strategy",
  "ExpirationPeriod",
};

/**
 * \ingroup management
 */
enum LocalControlFeature {
  LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID = 1,
  LOCAL_CONTROL_FEATURE_NEXT_HOP_FACE_ID = 2
};

/**
 * @ingroup management
 * @brief represents parameters in a ControlCommand request or response
 * @sa http://redmine.named-data.net/projects/nfd/wiki/ControlCommand#ControlParameters
 * @detail This type is copyable because it's an abstraction of a TLV type.
 */
class ControlParameters : public ndn::mgmt::ControlParameters
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  ControlParameters();

  explicit
  ControlParameters(const Block& block);

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  virtual Block
  wireEncode() const NDN_CXX_DECL_FINAL;

  virtual void
  wireDecode(const Block& wire) NDN_CXX_DECL_FINAL;

public: // getters & setters

  bool
  hasName() const
  {
    return m_hasFields[CONTROL_PARAMETER_NAME];
  }

  const Name&
  getName() const
  {
    BOOST_ASSERT(this->hasName());
    return m_name;
  }

  ControlParameters&
  setName(const Name& name)
  {
    m_wire.reset();
    m_name = name;
    m_hasFields[CONTROL_PARAMETER_NAME] = true;
    return *this;
  }

  ControlParameters&
  unsetName()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_NAME] = false;
    return *this;
  }

  bool
  hasFaceId() const
  {
    return m_hasFields[CONTROL_PARAMETER_FACE_ID];
  }

  uint64_t
  getFaceId() const
  {
    BOOST_ASSERT(this->hasFaceId());
    return m_faceId;
  }

  ControlParameters&
  setFaceId(uint64_t faceId)
  {
    m_wire.reset();
    m_faceId = faceId;
    m_hasFields[CONTROL_PARAMETER_FACE_ID] = true;
    return *this;
  }

  ControlParameters&
  unsetFaceId()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_FACE_ID] = false;
    return *this;
  }

  bool
  hasUri() const
  {
    return m_hasFields[CONTROL_PARAMETER_URI];
  }

  const std::string&
  getUri() const
  {
    BOOST_ASSERT(this->hasUri());
    return m_uri;
  }

  ControlParameters&
  setUri(const std::string& uri)
  {
    m_wire.reset();
    m_uri = uri;
    m_hasFields[CONTROL_PARAMETER_URI] = true;
    return *this;
  }

  ControlParameters&
  unsetUri()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_URI] = false;
    return *this;
  }

  bool
  hasLocalControlFeature() const
  {
    return m_hasFields[CONTROL_PARAMETER_LOCAL_CONTROL_FEATURE];
  }

  LocalControlFeature
  getLocalControlFeature() const
  {
    BOOST_ASSERT(this->hasLocalControlFeature());
    return m_localControlFeature;
  }

  ControlParameters&
  setLocalControlFeature(LocalControlFeature localControlFeature)
  {
    m_wire.reset();
    m_localControlFeature = localControlFeature;
    m_hasFields[CONTROL_PARAMETER_LOCAL_CONTROL_FEATURE] = true;
    return *this;
  }

  ControlParameters&
  unsetLocalControlFeature()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_LOCAL_CONTROL_FEATURE] = false;
    return *this;
  }

  bool
  hasOrigin() const
  {
    return m_hasFields[CONTROL_PARAMETER_ORIGIN];
  }

  uint64_t
  getOrigin() const
  {
    BOOST_ASSERT(this->hasOrigin());
    return m_origin;
  }

  ControlParameters&
  setOrigin(uint64_t origin)
  {
    m_wire.reset();
    m_origin = origin;
    m_hasFields[CONTROL_PARAMETER_ORIGIN] = true;
    return *this;
  }

  ControlParameters&
  unsetOrigin()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_ORIGIN] = false;
    return *this;
  }

  bool
  hasCost() const
  {
    return m_hasFields[CONTROL_PARAMETER_COST];
  }

  uint64_t
  getCost() const
  {
    BOOST_ASSERT(this->hasCost());
    return m_cost;
  }

  ControlParameters&
  setCost(uint64_t cost)
  {
    m_wire.reset();
    m_cost = cost;
    m_hasFields[CONTROL_PARAMETER_COST] = true;
    return *this;
  }

  ControlParameters&
  unsetCost()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_COST] = false;
    return *this;
  }

  bool
  hasFlags() const
  {
    return m_hasFields[CONTROL_PARAMETER_FLAGS];
  }

  uint64_t
  getFlags() const
  {
    BOOST_ASSERT(this->hasFlags());
    return m_flags;
  }

  ControlParameters&
  setFlags(uint64_t flags)
  {
    m_wire.reset();
    m_flags = flags;
    m_hasFields[CONTROL_PARAMETER_FLAGS] = true;
    return *this;
  }

  ControlParameters&
  unsetFlags()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_FLAGS] = false;
    return *this;
  }

  bool
  hasStrategy() const
  {
    return m_hasFields[CONTROL_PARAMETER_STRATEGY];
  }

  const Name&
  getStrategy() const
  {
    BOOST_ASSERT(this->hasStrategy());
    return m_strategy;
  }

  ControlParameters&
  setStrategy(const Name& strategy)
  {
    m_wire.reset();
    m_strategy = strategy;
    m_hasFields[CONTROL_PARAMETER_STRATEGY] = true;
    return *this;
  }

  ControlParameters&
  unsetStrategy()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_STRATEGY] = false;
    return *this;
  }

  bool
  hasExpirationPeriod() const
  {
    return m_hasFields[CONTROL_PARAMETER_EXPIRATION_PERIOD];
  }

  const time::milliseconds&
  getExpirationPeriod() const
  {
    BOOST_ASSERT(this->hasExpirationPeriod());
    return m_expirationPeriod;
  }

  ControlParameters&
  setExpirationPeriod(const time::milliseconds& expirationPeriod)
  {
    m_wire.reset();
    m_expirationPeriod = expirationPeriod;
    m_hasFields[CONTROL_PARAMETER_EXPIRATION_PERIOD] = true;
    return *this;
  }

  ControlParameters&
  unsetExpirationPeriod()
  {
    m_wire.reset();
    m_hasFields[CONTROL_PARAMETER_EXPIRATION_PERIOD] = false;
    return *this;
  }

  const std::vector<bool>&
  getPresentFields() const
  {
    return m_hasFields;
  }

private: // fields
  std::vector<bool>   m_hasFields;

  Name                m_name;
  uint64_t            m_faceId;
  std::string         m_uri;
  LocalControlFeature m_localControlFeature;
  uint64_t            m_origin;
  uint64_t            m_cost;
  uint64_t            m_flags;
  Name                m_strategy;
  time::milliseconds  m_expirationPeriod;

private:
  mutable Block m_wire;
};

std::ostream&
operator<<(std::ostream& os, const ControlParameters& parameters);

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROL_PARAMETERS_HPP
