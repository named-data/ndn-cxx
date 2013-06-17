/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

/**
 * @file error.h
 * @brief This file defines basic elements for the library reporting
 *
 * The library throws a number of exceptions.
 * In general, the following example shows how to print out diagnostic information
 * when one of the exceptions is thrown
 * @code
 *     try
 *       {
 *         ... operations with ndn::Name
 *       }
 *     catch (boost::exception &e)
 *       {
 *         std::cerr << boost::diagnostic_information (e) << std::endl;
 *       }
 * @endcode
 */

#ifndef NDN_ERROR_H
#define NDN_ERROR_H

#include <boost/exception/all.hpp>

namespace ndn
{
namespace error
{

struct Error           : public virtual boost::exception, public virtual std::exception {}; ///< @brief Some error with error reporting engine
struct Uri             : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with URI processing
struct StringTransform : public virtual boost::exception, public virtual std::exception {};
struct Name            : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with Name
namespace name {
struct Component       : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with name::Component
}
struct Exclude         : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with Exclude
struct KeyLocator      : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with KeyLocator
namespace wire {
struct Ccnb            : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with wire::Ccnb encoding
}
struct Keychain        : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with security::Keychain

// Diagnostic information fields

/**
 * @brief Free-formatted text message explaining the error
 *
 * @code
 * ...
 * catch (boost::exception &e)
 * {
 *     if (const std::string *error = boost::get_error_info<error::msg> (e))
 *          ...
 * }
 * @endcode
 *
 * @see get_msg
 */
typedef boost::error_info<struct tag_msg, std::string> msg;

/**
 * @brief Helper method to get error message from the exception
 *
 * Method assumes that message is present, if not, an exception will be thrown
 */
inline const std::string &
get_msg (boost::exception &e)
{
  const std::string *error = boost::get_error_info<msg> (e);
  if (error == 0)
    BOOST_THROW_EXCEPTION (Error ());
  return *error;
}

/**
 * @brief Report of the position of the error (error-specific meaning)
 *
 * @code
 * ...
 * catch (boost::exception &e)
 * {
 *     if (const int *error = boost::get_error_info<error::pos> (e))
 *          ...
 * }
 * @endcode
 *
 * @see get_pos
 */
typedef boost::error_info<struct tag_pos, int> pos;

/**
 * @brief Helper method to get position of the error from the exception
 *
 * Method assumes that position is present, if not, an exception will be thrown
 */
inline int
get_pos (boost::exception &e)
{
  const int *position = boost::get_error_info<pos> (e);
  if (position == 0)
    BOOST_THROW_EXCEPTION (Error ());
  return *position;
}

} // error
} // ndn

#endif // NDN_ERROR_H
