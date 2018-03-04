/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#ifndef NDN_MGMT_NFD_CONTROL_COMMAND_HPP
#define NDN_MGMT_NFD_CONTROL_COMMAND_HPP

#include "control-parameters.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief base class of NFD ControlCommand
 * \sa https://redmine.named-data.net/projects/nfd/wiki/ControlCommand
 */
class ControlCommand : noncopyable
{
public:
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

  virtual
  ~ControlCommand();

  /** \brief validate request parameters
   *  \throw ArgumentError if parameters are invalid
   */
  virtual void
  validateRequest(const ControlParameters& parameters) const;

  /** \brief apply default values to missing fields in request
   */
  virtual void
  applyDefaultsToRequest(ControlParameters& parameters) const;

  /** \brief validate response parameters
   *  \throw ArgumentError if parameters are invalid
   */
  virtual void
  validateResponse(const ControlParameters& parameters) const;

  /** \brief apply default values to missing fields in response
   */
  virtual void
  applyDefaultsToResponse(ControlParameters& parameters) const;

  /** \brief construct the Name for a request Interest
   *  \throw ArgumentError if parameters are invalid
   */
  Name
  getRequestName(const Name& commandPrefix, const ControlParameters& parameters) const;

protected:
  ControlCommand(const std::string& module, const std::string& verb);

  class FieldValidator
  {
  public:
    FieldValidator();

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
    validate(const ControlParameters& parameters) const;

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
  name::Component m_module;
  name::Component m_verb;
};


/**
 * \ingroup management
 * \brief represents a faces/create command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Create-a-face
 */
class FaceCreateCommand : public ControlCommand
{
public:
  FaceCreateCommand();

  void
  applyDefaultsToRequest(ControlParameters& parameters) const override;

  void
  validateResponse(const ControlParameters& parameters) const override;
};


/**
 * \ingroup management
 * \brief represents a faces/update command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Update-the-static-properties-of-a-face
 */
class FaceUpdateCommand : public ControlCommand
{
public:
  FaceUpdateCommand();

  void
  applyDefaultsToRequest(ControlParameters& parameters) const override;

  /**
   * \note This can only validate ControlParameters in a success response.
   *       Failure responses should be validated with validateRequest.
   */
  void
  validateResponse(const ControlParameters& parameters) const override;
};


/**
 * \ingroup management
 * \brief represents a faces/destroy command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Destroy-a-face
 */
class FaceDestroyCommand : public ControlCommand
{
public:
  FaceDestroyCommand();

  void
  validateRequest(const ControlParameters& parameters) const override;

  void
  validateResponse(const ControlParameters& parameters) const override;
};


/**
 * \ingroup management
 * \brief represents a fib/add-nexthop command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FibMgmt#Add-a-nexthop
 */
class FibAddNextHopCommand : public ControlCommand
{
public:
  FibAddNextHopCommand();

  void
  applyDefaultsToRequest(ControlParameters& parameters) const override;

  void
  validateResponse(const ControlParameters& parameters) const override;
};


/**
 * \ingroup management
 * \brief represents a fib/remove-nexthop command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FibMgmt#Remove-a-nexthop
 */
class FibRemoveNextHopCommand : public ControlCommand
{
public:
  FibRemoveNextHopCommand();

  void
  applyDefaultsToRequest(ControlParameters& parameters) const override;

  void
  validateResponse(const ControlParameters& parameters) const override;
};


/**
 * \ingroup management
 * \brief represents a cs/config command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#Update-configuration
 */
class CsConfigCommand : public ControlCommand
{
public:
  CsConfigCommand();
};


/**
 * \ingroup management
 * \brief represents a cs/erase command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#Erase-entries
 */
class CsEraseCommand : public ControlCommand
{
public:
  CsEraseCommand();

  void
  validateRequest(const ControlParameters& parameters) const override;

  void
  validateResponse(const ControlParameters& parameters) const override;
};


/**
 * \ingroup management
 * \brief represents a strategy-choice/set command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Set-the-strategy-for-a-namespace
 */
class StrategyChoiceSetCommand : public ControlCommand
{
public:
  StrategyChoiceSetCommand();
};


/**
 * \ingroup management
 * \brief represents a strategy-choice/set command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Unset-the-strategy-for-a-namespace
 */
class StrategyChoiceUnsetCommand : public ControlCommand
{
public:
  StrategyChoiceUnsetCommand();

  void
  validateRequest(const ControlParameters& parameters) const override;

  void
  validateResponse(const ControlParameters& parameters) const override;
};


/**
 * \ingroup management
 * \brief represents a rib/register command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#Register-a-route
 */
class RibRegisterCommand : public ControlCommand
{
public:
  RibRegisterCommand();

  void
  applyDefaultsToRequest(ControlParameters& parameters) const override;

  void
  validateResponse(const ControlParameters& parameters) const override;
};


/**
 * \ingroup management
 * \brief represents a rib/unregister command
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#Unregister-a-route
 */
class RibUnregisterCommand : public ControlCommand
{
public:
  RibUnregisterCommand();

  void
  applyDefaultsToRequest(ControlParameters& parameters) const override;

  void
  validateResponse(const ControlParameters& parameters) const override;
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_CONTROL_COMMAND_HPP
