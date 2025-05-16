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
#include "ndn-cxx/prefix-announcement.hpp"

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
 * \brief Implements decoding, encoding, and validation of control command parameters carried
 *        in the ApplicationParameters of the request (Interest packet).
 * \note This format is applicable to control command requests only.
 */
template<typename PT>
class ApplicationParametersCommandFormat
{
public:
  using ParametersType = PT;

  /**
   * \brief Does nothing.
   */
  void
  validate(const ParametersType&) const
  {
  }

  /**
   * \brief Extract the parameters from the request \p interest.
   */
  static shared_ptr<ParametersType>
  decode(const Interest& interest, size_t prefixLen = 0)
  {
    auto params = make_shared<ParametersType>();
    params->wireDecode(interest.getApplicationParameters());
    return params;
  }

  /**
   * \brief Serialize the parameters into the request \p interest.
   */
  static void
  encode(Interest& interest, const ParametersType& params)
  {
    interest.setApplicationParameters(params.wireEncode());
  }
};


/**
 * \ingroup management
 * \brief Base class for all NFD control commands.
 * \tparam RequestFormatType  A class type that will handle the encoding and validation of the request
 *                            parameters. The type can be ApplicationParametersCommandFormat or
 *                            ControlParametersCommandFormat.
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
    return PartialName().append(Derived::module).append(Derived::verb);
  }

  /**
   * \brief Construct request Interest.
   * \throw ArgumentError if parameters are invalid
   */
  static Interest
  createRequest(Name commandPrefix, const RequestParameters& params)
  {
    validateRequest(params);

    Interest request(commandPrefix.append(Derived::module).append(Derived::verb));
    Derived::s_requestFormat.encode(request, params);
    return request;
  }

  /**
   * \brief Extract parameters from request Interest.
   */
  static shared_ptr<mgmt::ControlParametersBase>
  parseRequest(const Interest& interest, size_t prefixLen)
  {
    // +2 to account for module and verb components
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

#define NDN_CXX_CONTROL_COMMAND(module_, verb_) \
  public: \
  static inline const ::ndn::name::Component module{std::string_view(module_)}; \
  static inline const ::ndn::name::Component verb{std::string_view(verb_)}; \
  private: \
  static const RequestFormat s_requestFormat; \
  static const ResponseFormat s_responseFormat; \
  friend Base


/**
 * \ingroup management
 * \brief Represents a `faces/create` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Create-a-face
 */
class FaceCreateCommand : public ControlCommand<FaceCreateCommand>
{
  NDN_CXX_CONTROL_COMMAND("faces", "create");

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
  NDN_CXX_CONTROL_COMMAND("faces", "update");

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
  NDN_CXX_CONTROL_COMMAND("faces", "destroy");

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
  NDN_CXX_CONTROL_COMMAND("fib", "add-nexthop");

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
  NDN_CXX_CONTROL_COMMAND("fib", "remove-nexthop");

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
  NDN_CXX_CONTROL_COMMAND("cs", "config");
};


/**
 * \ingroup management
 * \brief Represents a `cs/erase` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#Erase-entries
 */
class CsEraseCommand : public ControlCommand<CsEraseCommand>
{
  NDN_CXX_CONTROL_COMMAND("cs", "erase");

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
  NDN_CXX_CONTROL_COMMAND("strategy-choice", "set");
};


/**
 * \ingroup management
 * \brief Represents a `strategy-choice/unset` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Unset-the-strategy-for-a-namespace
 */
class StrategyChoiceUnsetCommand : public ControlCommand<StrategyChoiceUnsetCommand>
{
  NDN_CXX_CONTROL_COMMAND("strategy-choice", "unset");

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
  NDN_CXX_CONTROL_COMMAND("rib", "register");
  friend class RibAnnounceCommand;

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
  NDN_CXX_CONTROL_COMMAND("rib", "unregister");

  static void
  applyDefaultsToRequestImpl(ControlParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};


/**
 * \ingroup management
 * \brief Request parameters for `rib/announce` command.
 */
class RibAnnounceParameters final : public mgmt::ControlParametersBase
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  const PrefixAnnouncement&
  getPrefixAnnouncement() const
  {
    return m_prefixAnn;
  }

  RibAnnounceParameters&
  setPrefixAnnouncement(const PrefixAnnouncement& pa)
  {
    m_prefixAnn = pa;
    return *this;
  }

  void
  wireDecode(const Block& wire) final;

  Block
  wireEncode() const final;

private:
  PrefixAnnouncement m_prefixAnn;
};


/**
 * \ingroup management
 * \brief Represents a `rib/announce` command.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#Register-a-route-with-Prefix-Announcement-object
 */
class RibAnnounceCommand : public ControlCommand<RibAnnounceCommand,
                                                 ApplicationParametersCommandFormat<RibAnnounceParameters>>
{
  NDN_CXX_CONTROL_COMMAND("rib", "announce");

  static void
  validateRequestImpl(const RibAnnounceParameters& parameters);

  static void
  validateResponseImpl(const ControlParameters& parameters);
};

} // namespace ndn::nfd

#endif // NDN_CXX_MGMT_NFD_CONTROL_COMMAND_HPP
