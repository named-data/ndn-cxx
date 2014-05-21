/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#ifndef NDN_MANAGEMENT_NFD_CONTROL_COMMAND_HPP
#define NDN_MANAGEMENT_NFD_CONTROL_COMMAND_HPP

#include "nfd-control-parameters.hpp"
#include "../util/command-interest-generator.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief base class of NFD ControlCommand
 * \sa http://redmine.named-data.net/projects/nfd/wiki/ControlCommand
 */
class ControlCommand : noncopyable
{
public:
  /** \brief a callback on signing command interest
   */
  typedef function<void(Interest&)> Sign;

  /** \brief represents an error in ControlParameters
   */
  class ArgumentError : public std::invalid_argument
  {
  public:
    explicit
    ArgumentError(const std::string& what)
      : std::invalid_argument(what)
    {
    }
  };

  /** \return Name prefix of this ControlCommand
   */
  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  /** \brief make a Command Interest from parameters
   */
  Interest
  makeCommandInterest(const ControlParameters& parameters,
                      const Sign& sign) const
  {
    this->validateRequest(parameters);

    Name name = m_prefix;
    name.append(parameters.wireEncode());
    Interest commandInterest(name);
    sign(commandInterest);
    return commandInterest;
  }

  /** \brief validate request parameters
   *  \throw ArgumentError
   */
  virtual void
  validateRequest(const ControlParameters& parameters) const
  {
    m_requestValidator.validate(parameters);
  }

  /** \brief apply default values to missing fields in request
   */
  virtual void
  applyDefaultsToRequest(ControlParameters& parameters) const
  {
  }

  /** \brief validate response parameters
   *  \throw ArgumentError
   */
  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    m_responseValidator.validate(parameters);
  }

  /** \brief apply default values to missing fields in response
   */
  virtual void
  applyDefaultsToResponse(ControlParameters& parameters) const
  {
  }

protected:
  ControlCommand(const std::string& module, const std::string& verb)
    : m_prefix("ndn:/localhost/nfd")
  {
    m_prefix.append(module).append(verb);
  }

  class FieldValidator
  {
  public:
    FieldValidator()
      : m_required(CONTROL_PARAMETER_UBOUND)
      , m_optional(CONTROL_PARAMETER_UBOUND)
    {
    }

    /** \brief declare a required field
     */
    FieldValidator&
    required(ControlParameterField field)
    {
      m_required[field] = true;
      return *this;
    }

    /** \brief declare an optional field
     */
    FieldValidator&
    optional(ControlParameterField field)
    {
      m_optional[field] = true;
      return *this;
    }

    /** \brief verify that all required fields are present,
     *         and all present fields are either required or optional
     *  \throw ArgumentError
     */
    void
    validate(const ControlParameters& parameters) const
    {
      const std::vector<bool>& presentFields = parameters.getPresentFields();

      for (size_t i = 0; i < CONTROL_PARAMETER_UBOUND; ++i) {
        bool isPresent = presentFields[i];
        if (m_required[i]) {
          if (!isPresent) {
            throw ArgumentError(CONTROL_PARAMETER_FIELD[i] + " is required but missing");
          }
        }
        else if (isPresent && !m_optional[i]) {
          throw ArgumentError(CONTROL_PARAMETER_FIELD[i] + " is forbidden but present");
        }
      }
    }

  private:
    std::vector<bool> m_required;
    std::vector<bool> m_optional;
  };

protected:
  /** \brief FieldValidator for request ControlParameters
   *
   *  Constructor of subclass should populate this validator.
   */
  FieldValidator m_requestValidator;
  /** \brief FieldValidator for response ControlParameters
   *
   *  Constructor of subclass should populate this validator.
   */
  FieldValidator m_responseValidator;

private:
  Name m_prefix;
};


/**
 * \ingroup management
 * \brief represents a faces/create command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Create-a-face
 */
class FaceCreateCommand : public ControlCommand
{
public:
  FaceCreateCommand()
    : ControlCommand("faces", "create")
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_URI);
    m_responseValidator
      .required(CONTROL_PARAMETER_URI)
      .required(CONTROL_PARAMETER_FACE_ID);
  }

  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    this->ControlCommand::validateResponse(parameters);

    if (parameters.getFaceId() == 0) {
      throw ArgumentError("FaceId must not be zero");
    }
  }
};


/**
 * \ingroup management
 * \brief represents a faces/destroy command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Destroy-a-face
 */
class FaceDestroyCommand : public ControlCommand
{
public:
  FaceDestroyCommand()
    : ControlCommand("faces", "destroy")
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_FACE_ID);
    m_responseValidator = m_requestValidator;
  }

  virtual void
  validateRequest(const ControlParameters& parameters) const
  {
    this->ControlCommand::validateRequest(parameters);

    if (parameters.getFaceId() == 0) {
      throw ArgumentError("FaceId must not be zero");
    }
  }

  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    this->validateRequest(parameters);
  }
};

/**
 * \ingroup management
 * \brief Base class for faces/[*]-local-control commands
 */
class FaceLocalControlCommand : public ControlCommand
{
public:
  virtual void
  validateRequest(const ControlParameters& parameters) const
  {
    this->ControlCommand::validateRequest(parameters);

    switch (parameters.getLocalControlFeature()) {
      case LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID:
      case LOCAL_CONTROL_FEATURE_NEXT_HOP_FACE_ID:
        break;
      default:
        throw ArgumentError("LocalControlFeature is invalid");
    }
  }

  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    this->validateRequest(parameters);
  }

protected:
  explicit
  FaceLocalControlCommand(const std::string& verb)
    : ControlCommand("faces", verb)
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_LOCAL_CONTROL_FEATURE);
    m_responseValidator = m_requestValidator;
  }
};


/**
 * \ingroup management
 * \brief represents a faces/enable-local-control command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Enable-a-LocalControlHeader-feature
 */
class FaceEnableLocalControlCommand : public FaceLocalControlCommand
{
public:
  FaceEnableLocalControlCommand()
    : FaceLocalControlCommand("enable-local-control")
  {
  }
};


/**
 * \ingroup management
 * \brief represents a faces/disable-local-control command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Disable-a-LocalControlHeader-feature
 */
class FaceDisableLocalControlCommand : public FaceLocalControlCommand
{
public:
  FaceDisableLocalControlCommand()
    : FaceLocalControlCommand("disable-local-control")
  {
  }
};


/**
 * \ingroup management
 * \brief represents a fib/add-nexthop command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FibMgmt#Add-a-nexthop
 */
class FibAddNextHopCommand : public ControlCommand
{
public:
  FibAddNextHopCommand()
    : ControlCommand("fib", "add-nexthop")
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_NAME)
      .optional(CONTROL_PARAMETER_FACE_ID)
      .optional(CONTROL_PARAMETER_COST);
    m_responseValidator
      .required(CONTROL_PARAMETER_NAME)
      .required(CONTROL_PARAMETER_FACE_ID)
      .required(CONTROL_PARAMETER_COST);
  }

  virtual void
  applyDefaultsToRequest(ControlParameters& parameters) const
  {
    if (!parameters.hasFaceId()) {
      parameters.setFaceId(0);
    }
    if (!parameters.hasCost()) {
      parameters.setCost(0);
    }
  }

  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    this->ControlCommand::validateResponse(parameters);

    if (parameters.getFaceId() == 0) {
      throw ArgumentError("FaceId must not be zero");
    }
  }
};


/**
 * \ingroup management
 * \brief represents a fib/remove-nexthop command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FibMgmt#Remove-a-nexthop
 */
class FibRemoveNextHopCommand : public ControlCommand
{
public:
  FibRemoveNextHopCommand()
    : ControlCommand("fib", "remove-nexthop")
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_NAME)
      .optional(CONTROL_PARAMETER_FACE_ID);
    m_responseValidator
      .required(CONTROL_PARAMETER_NAME)
      .required(CONTROL_PARAMETER_FACE_ID);
  }

  virtual void
  applyDefaultsToRequest(ControlParameters& parameters) const
  {
    if (!parameters.hasFaceId()) {
      parameters.setFaceId(0);
    }
  }

  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    this->ControlCommand::validateResponse(parameters);

    if (parameters.getFaceId() == 0) {
      throw ArgumentError("FaceId must not be zero");
    }
  }
};


/**
 * \ingroup management
 * \brief represents a strategy-choice/set command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Set-the-strategy-for-a-namespace
 */
class StrategyChoiceSetCommand : public ControlCommand
{
public:
  StrategyChoiceSetCommand()
    : ControlCommand("strategy-choice", "set")
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_NAME)
      .required(CONTROL_PARAMETER_STRATEGY);
    m_responseValidator = m_requestValidator;
  }
};


/**
 * \ingroup management
 * \brief represents a strategy-choice/set command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Unset-the-strategy-for-a-namespace
 */
class StrategyChoiceUnsetCommand : public ControlCommand
{
public:
  StrategyChoiceUnsetCommand()
    : ControlCommand("strategy-choice", "unset")
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_NAME);
    m_responseValidator = m_requestValidator;
  }

  virtual void
  validateRequest(const ControlParameters& parameters) const
  {
    this->ControlCommand::validateRequest(parameters);

    if (parameters.getName().size() == 0) {
      throw ArgumentError("Name must not be ndn:/");
    }
  }

  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    this->validateRequest(parameters);
  }
};


/**
 * \ingroup management
 */
enum {
  // route origin
  ROUTE_ORIGIN_APP    = 0,
  ROUTE_ORIGIN_NLSR   = 128,
  ROUTE_ORIGIN_STATIC = 255,

  // route inheritance flags
  ROUTE_FLAG_CHILD_INHERIT = 1,
  ROUTE_FLAG_CAPTURE       = 2
};


/**
 * \ingroup management
 * \brief represents a rib/register command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/RibMgmt#Register-a-route
 */
class RibRegisterCommand : public ControlCommand
{
public:
  RibRegisterCommand()
    : ControlCommand("rib", "register")
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_NAME)
      .optional(CONTROL_PARAMETER_FACE_ID)
      .optional(CONTROL_PARAMETER_ORIGIN)
      .optional(CONTROL_PARAMETER_COST)
      .optional(CONTROL_PARAMETER_FLAGS)
      .optional(CONTROL_PARAMETER_EXPIRATION_PERIOD);
    m_responseValidator
      .required(CONTROL_PARAMETER_NAME)
      .required(CONTROL_PARAMETER_FACE_ID)
      .required(CONTROL_PARAMETER_ORIGIN)
      .required(CONTROL_PARAMETER_COST)
      .required(CONTROL_PARAMETER_FLAGS)
      .required(CONTROL_PARAMETER_EXPIRATION_PERIOD);
  }

  virtual void
  applyDefaultsToRequest(ControlParameters& parameters) const
  {
    if (!parameters.hasFaceId()) {
      parameters.setFaceId(0);
    }
    if (!parameters.hasOrigin()) {
      parameters.setOrigin(ROUTE_ORIGIN_APP);
    }
    if (!parameters.hasCost()) {
      parameters.setCost(0);
    }
    if (!parameters.hasFlags()) {
      parameters.setFlags(ROUTE_FLAG_CHILD_INHERIT);
    }
    if (!parameters.hasExpirationPeriod()) {
      if (parameters.getFaceId() == 0) {
        parameters.setExpirationPeriod(time::milliseconds::max());
      }
      else {
        parameters.setExpirationPeriod(time::hours(1));
      }
    }
  }

  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    this->ControlCommand::validateResponse(parameters);

    if (parameters.getFaceId() == 0) {
      throw ArgumentError("FaceId must not be zero");
    }
  }
};


/**
 * \ingroup management
 * \brief represents a rib/unregister command
 * \sa http://redmine.named-data.net/projects/nfd/wiki/RibMgmt#Unregister-a-route
 */
class RibUnregisterCommand : public ControlCommand
{
public:
  RibUnregisterCommand()
    : ControlCommand("rib", "unregister")
  {
    m_requestValidator
      .required(CONTROL_PARAMETER_NAME)
      .optional(CONTROL_PARAMETER_FACE_ID)
      .optional(CONTROL_PARAMETER_ORIGIN);
    m_responseValidator
      .required(CONTROL_PARAMETER_NAME)
      .required(CONTROL_PARAMETER_FACE_ID)
      .required(CONTROL_PARAMETER_ORIGIN);
  }

  virtual void
  applyDefaultsToRequest(ControlParameters& parameters) const
  {
    if (!parameters.hasFaceId()) {
      parameters.setFaceId(0);
    }
    if (!parameters.hasOrigin()) {
      parameters.setOrigin(ROUTE_ORIGIN_APP);
    }
  }

  virtual void
  validateResponse(const ControlParameters& parameters) const
  {
    this->ControlCommand::validateResponse(parameters);

    if (parameters.getFaceId() == 0) {
      throw ArgumentError("FaceId must not be zero");
    }
  }
};


} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROL_COMMAND_HPP
