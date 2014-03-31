/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_IO_HPP
#define NDN_UTIL_IO_HPP

#include "../common.hpp"

#include "../encoding/block.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include "../security/cryptopp.hpp"


namespace ndn {
namespace io {

struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

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
      
      switch(encoding)
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
  catch(CryptoPP::Exception& e)
    {
      return shared_ptr<T>();
    }
  catch(Block::Error& e)
    {
      return shared_ptr<T>();
    }
  catch(TypeError& e)
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
      
      switch(encoding)
	{
	case NO_ENCODING:
	  {
	    StringSource ss(block.wire(), block.size(), true, new FileSink(os));
	    break;
	  }
	case BASE_64:
	  {
	    StringSource ss(block.wire(), block.size(), true, new Base64Encoder(new FileSink(os), true, 64));
	    break;
	  }
	case HEX:
	  {
	    StringSource ss(block.wire(), block.size(), true, new HexEncoder(new FileSink(os)));
	    break;
	  }
	default:
	  return; 
	}
      return;
    }
  catch(CryptoPP::Exception& e)
    {
      throw Error(e.what());
    }
  catch(Block::Error& e)
    {
      throw Error(e.what());
    }
  catch(TypeError& e)
    {
      throw Error(e.what());
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

