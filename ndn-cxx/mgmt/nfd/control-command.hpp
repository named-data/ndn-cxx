/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2025 Regents of the University of California.
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

#ifndef NDN_CXX_MGMT_NFD_CONTROL_COMMAND_HPP
#define NDN_CXX_MGMT_NFD_CONTROL_COMMAND_HPP

#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/mgmt/nfd/control-parameters.hpp"

#include <bitset>

namespace ndn::nfd {

/**
 * \ingroup management
 * \brief Represents an error in the parameters of the control command request or response.
 */
class ArgumentError : public std::invalid_argument
{
public:
  using std::invalid_argument::invalid_argument;
};


/**
 * \ingroup management
 * \brief Implements decoding, encoding, and validation of ControlParameters in control commands.
 *
 * According to this format, the request parameters are encoded as a single GenericNameComponent
 * in the Interest name, immediately after the command module and command verb components.
 *
 * \sa https://redmine.named-data.net/projects/nfd/wiki/ControlCommand
 */
class ControlParametersCommandFormat
{
public:
  using ParametersType = ControlParameters;

  /**
   * \brief Declare a required field.
   */
  ControlParametersCommandFormat&
  required(ControlParameterField field)
  {
    m_required.set(field);
    return *this;
  }

  /**
   * \brief Declare an optional field.
   */
  ControlParametersCommandFormat&
  optional(ControlParameterField field)
  {
    m_optional.set(field);
    return *this;
  }

  /**
   * \brief Verify that all required fields are present, and all present fields
   *        are either required or optional.
   * \throw ArgumentError Parameters validation failed.
   */
  void
  validate(const ControlParameters& params) const;

  /**
   * \brief Extract the parameters from the request \p interest.
   */
  static shared_ptr<ControlParameters>
  decode(const Interest& interest, size_t prefixLen);

  /**
   * \brief Serialize the parameters into the request \p interest.
   * \pre \p params are valid.
   */
  static void
  encode(Interest& interest, const ControlParameters& params);

private:
  std::bitset<CONTROL_PARAMETER_UBOUND> m_required;
  std::bitset<CONTROL_PARAMETER_UBOUND> m_optional;
};


/**
 * \ingroup management
 * \brief Base class for all NFD control commands.
 * \tparam RequestFormatType  A class type that will handle the encoding and validation of the request
 *                            parameters. Only ControlParametersCommandFormat is supported for now.
 * \tparam ResponseFormatType A class type that will handle the encoding and validation of the response
 *                            parameters. Only ControlParametersCommandFormat is supported for now.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/ControlCommand
 */
template<typename Derived,
         typename RequestFormatType = ControlParametersCommandFormat,
         typename ResponseFormatType = ControlParametersCommandFormat>
class ControlCommand : noncopyable
{
protected:
  using Base = ControlCommand<Derived, RequestFormatType, ResponseFormatType>;

public:
  using RequestFormat = RequestFormatType;
  using ResponseFormat = ResponseFormatType;
  using RequestParameters = typename RequestFormat::ParametersType;
  using ResponseParameters = typename ResponseFormat::ParametersType;

  ControlCommand() = delete;

  /**
   * \brief Return the command name (module + verb).
   */
  static PartialName
  getName()
  {
    return PartialName().append(Derived::s_module).append(Derived::s_verb);
  }

  /**
   * \brief Construct request Interest.
   * \throw ArgumentError if parameters are invalid
   */
  static Interest
  createRequest(Name commandPrefix, const RequestParameters& params)
  {
    validateRequest(params);

    Interest request(commandPrefix.append(Derived::s_module).append(Derived::s_verb));
    Derived::s_requestFormat.encode(request, params);
    return request;
  }

  /**
   * \brief Extract parameters from request Interest.
   */
  static shared_ptr<mgmt::ControlParameters>
  parseRequest(const Interest& interest, size_t prefixLen)
  {
    // /<prefix>/<module>/<verb>
    return Derived::s_requestFormat.decode(interest, prefixLen + 2);
  }

  /**
   * \brief Validate request parameters.
   * \throw ArgumentError if parameters are invalid
   */
  static void
  validateRequest(const RequestParameters& params)
  {
    Derived::s_requestFormat.validate(params);
    Derived::validateRequestImpl(params);
  }

  /**
   * \brief Apply default values to missing fields in request.
   */
  static void
  applyDefaultsToRequest(RequestParameters& params)
  {
    Derived::applyDefaultsToRequestImpl(params);
  }

  /**
   * \brief Validate response parameters.
   * \throw ArgumentError if parameters are invalid
   */
  static void
  validateResponse(const ResponseParameters& params)
  {
    Derived::s_responseFormat.validate(params);
    Derived::validateResponseImpl(params);
  }

  /**
   * \brief Apply default values to missing fields in response.
   */
  static void
  applyDefaultsToResponse(ResponseParameters& params)
  {
    Derived::applyDefaultsToResponseImpl(params);
  }

private:
  static void
  validateRequestImpl(const RequestParameters&)
  {
  }

  static void
  applyDefaultsToRequestImpl(RequestParameters&)
  {
  }

  static void
  validateResponseImpl(const ResponseParameters&)
  {
  }

  static void
  applyDefaultsToResponseImpl(ResponseParameters&)
  {
  }
};

#define NDN_CXX_CONTROL_COMMAND(cmd, module, verb) \
  private: \
  friend Base; \
  static inline const ::ndn::name::Component s_module{module}; \
  static inline const ::ndn::name::Component s_verb{verb}; \
  static const RequestFormat s_requestFormat; \
  static const ResponseFormat s_responseFormat


/**
 * \ingroup management
 * \brief Represents a `faces/create` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Create-a-face
 */
class FaceCreateCommand : public ControlCommand<FaceCreateCommand>
{
  NDN_CXX_CONTROL_COMMAND(FaceCreateCommand, "faces", "create");

  static void
  applyDefaultsToRequestImpl(ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Represents a `faces/update` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Update-the-static-properties-of-a-face
 */
class FaceUpdateCommand : public ControlCommand<FaceUpdateCommand>
{
  NDN_CXX_CONTROL_COMMAND(FaceUpdateCommand, "faces", "update");

  static void
  applyDefaultsToRequestImpl(ControlParameters& parameters);

  /**
   * \note This can only validate ControlParameters in a success response.
   *       Failure responses should be validated with validateRequest.
   */
  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Represents a `faces/destroy` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Destroy-a-face
 */
class FaceDestroyCommand : public ControlCommand<FaceDestroyCommand>
{
  NDN_CXX_CONTROL_COMMAND(FaceDestroyCommand, "faces", "destroy");

  static void
  validateRequestImpl(const ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Represents a `fib/add-nexthop` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FibMgmt#Add-a-nexthop
 */
class FibAddNextHopCommand : public ControlCommand<FibAddNextHopCommand>
{
  NDN_CXX_CONTROL_COMMAND(FibAddNextHopCommand, "fib", "add-nexthop");

  static void
  applyDefaultsToRequestImpl(ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Represents a `fib/remove-nexthop` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FibMgmt#Remove-a-nexthop
 */
class FibRemoveNextHopCommand : public ControlCommand<FibRemoveNextHopCommand>
{
  NDN_CXX_CONTROL_COMMAND(FibRemoveNextHopCommand, "fib", "remove-nexthop");

  static void
  applyDefaultsToRequestImpl(ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Represents a `cs/config` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#Update-configuration
 */
class CsConfigCommand : public ControlCommand<CsConfigCommand>
{
  NDN_CXX_CONTROL_COMMAND(CsConfigCommand, "cs", "config");
};


/**
 * \ingroup management
 * \brief Represents a `cs/erase` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#Erase-entries
 */
class CsEraseCommand : public ControlCommand<CsEraseCommand>
{
  NDN_CXX_CONTROL_COMMAND(CsEraseCommand, "cs", "erase");

  static void
  validateRequestImpl(const ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Represents a `strategy-choice/set` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Set-the-strategy-for-a-namespace
 */
class StrategyChoiceSetCommand : public ControlCommand<StrategyChoiceSetCommand>
{
  NDN_CXX_CONTROL_COMMAND(StrategyChoiceSetCommand, "strategy-choice", "set");
};


/**
 * \ingroup management
 * \brief Represents a `strategy-choice/unset` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Unset-the-strategy-for-a-namespace
 */
class StrategyChoiceUnsetCommand : public ControlCommand<StrategyChoiceUnsetCommand>
{
  NDN_CXX_CONTROL_COMMAND(StrategyChoiceUnsetCommand, "strategy-choice", "unset");

  static void
  validateRequestImpl(const ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Represents a `rib/register` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#Register-a-route
 */
class RibRegisterCommand : public ControlCommand<RibRegisterCommand>
{
  NDN_CXX_CONTROL_COMMAND(RibRegisterCommand, "rib", "register");

  static void
  applyDefaultsToRequestImpl(ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Represents a `rib/unregister` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#Unregister-a-route
 */
class RibUnregisterCommand : public ControlCommand<RibUnregisterCommand>
{
  NDN_CXX_CONTROL_COMMAND(RibUnregisterCommand, "rib", "unregister");

  static void
  applyDefaultsToRequestImpl(ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};

} // namespace ndn::nfd

#endif // NDN_CXX_MGMT_NFD_CONTROL_COMMAND_HPP
