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

#ifndef NDN_UTIL_IO_HPP
#define NDN_UTIL_IO_HPP

#include "../common.hpp"

#include "../encoding/block.hpp"
#include "../encoding/buffer-stream.hpp"

#include <iostream>
#include <fstream>
#include "../security/cryptopp.hpp"


namespace ndn {
namespace io {

class Error : public std::runtime_error
{
public:
  explicit
  Error(const std::string& what)
    : std::runtime_error(what)
  {
  }
};

enum IoEncoding {
  NO_ENCODING,
  BASE_64,
  HEX
};

template<typename T>
shared_ptr<T>
load(std::istream& is, IoEncoding encoding = BASE_64)
{
  typedef typename T::Error TypeError;
  try
    {
      using namespace CryptoPP;

      shared_ptr<T> object = make_shared<T>();

      OBufferStream os;

      switch (encoding)
        {
        case NO_ENCODING:
          {
            FileSource ss(is, true, new FileSink(os));
            break;
          }
        case BASE_64:
          {
            FileSource ss(is, true, new Base64Decoder(new FileSink(os)));
            break;
          }
        case HEX:
          {
            FileSource ss(is, true, new HexDecoder(new FileSink(os)));
            break;
          }
        default:
          return shared_ptr<T>();
        }

      object->wireDecode(Block(os.buf()));
      return object;
    }
  catch (TypeError& e)
    {
      return shared_ptr<T>();
    }
  catch (CryptoPP::Exception& e)
    {
      return shared_ptr<T>();
    }
  catch (tlv::Error& e)
    {
      return shared_ptr<T>();
    }
}

template<typename T>
shared_ptr<T>
load(const std::string& file, IoEncoding encoding = BASE_64)
{
  std::ifstream is(file.c_str());
  return load<T>(is, encoding);
}

template<typename T>
void
save(const T& object, std::ostream& os, IoEncoding encoding = BASE_64)
{
  typedef typename T::Error TypeError;
  try
    {
      using namespace CryptoPP;

      Block block = object.wireEncode();

      switch (encoding)
        {
        case NO_ENCODING:
          {
            StringSource ss(block.wire(), block.size(), true,
                            new FileSink(os));
            break;
          }
        case BASE_64:
          {
            StringSource ss(block.wire(), block.size(), true,
                            new Base64Encoder(new FileSink(os), true, 64));
            break;
          }
        case HEX:
          {
            StringSource ss(block.wire(), block.size(), true,
                            new HexEncoder(new FileSink(os)));
            break;
          }
        default:
          return;
        }
      return;
    }
  catch (TypeError& e)
    {
      BOOST_THROW_EXCEPTION(Error(e.what()));
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_THROW_EXCEPTION(Error(e.what()));
    }
  catch (tlv::Error& e)
    {
      BOOST_THROW_EXCEPTION(Error(e.what()));
    }
}

template<typename T>
void
save(const T& object, const std::string& file, IoEncoding encoding = BASE_64)
{
  std::ofstream os(file.c_str());
  save(object, os, encoding);
}

} // namespace io
} // namespace ndn

#endif // NDN_UTIL_IO_HPP
