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

#include "util/io.hpp"

#include "boost-test.hpp"
#include "identity-management-fixture.hpp"

#include <boost/filesystem.hpp>

namespace ndn {
namespace tests {

class IoFixture
{
protected:
  IoFixture()
    : filepath(boost::filesystem::path(UNIT_TEST_CONFIG_PATH) /= "TestIo")
    , filename(filepath.string())
  {
    boost::filesystem::create_directories(filepath.parent_path());
  }

  ~IoFixture()
  {
    boost::system::error_code ec;
    boost::filesystem::remove(filepath, ec); // ignore error
  }

  /** \brief create a directory at filename, so that it's neither readable nor writable as a file
   */
  void
  mkdir() const
  {
    boost::filesystem::create_directory(filepath);
  }

  template<typename Container, typename CharT = typename Container::value_type>
  Container
  readFile() const
  {
    Container container;
    std::ifstream fs(filename, std::ios_base::binary);
    char ch;
    while (fs.get(ch)) {
      container.push_back(static_cast<CharT>(ch));
    }
    return container;
  }

  template<typename Container, typename CharT = typename Container::value_type>
  void
  writeFile(const Container& content) const
  {
    std::ofstream fs(filename, std::ios_base::binary);
    for (CharT ch : content) {
      fs.put(static_cast<char>(ch));
    }
    fs.close();
    BOOST_REQUIRE_MESSAGE(fs, "error writing file");
  }

protected:
  const boost::filesystem::path filepath;
  const std::string filename;
};

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestIo, IoFixture)

class EncodableType
{
public:
  Block
  wireEncode() const
  {
    if (shouldThrow) {
      BOOST_THROW_EXCEPTION(tlv::Error("encode error"));
    }

    // block will be 0xAA, 0x01, 0xDD
    return makeNonNegativeIntegerBlock(0xAA, 0xDD);
  }

public:
  bool shouldThrow = false;
};

template<bool SHOULD_THROW = false>
class DecodableTypeTpl
{
public:
  DecodableTypeTpl() = default;

  explicit
  DecodableTypeTpl(const Block& block)
  {
    this->wireDecode(block);
  }

  void
  wireDecode(const Block& block)
  {
    if (m_shouldThrow) {
      BOOST_THROW_EXCEPTION(tlv::Error("decode error"));
    }

    // block must be 0xBB, 0x01, 0xEE
    BOOST_CHECK_EQUAL(block.type(), 0xBB);
    BOOST_REQUIRE_EQUAL(block.value_size(), 1);
    BOOST_CHECK_EQUAL(block.value()[0], 0xEE);
  }

private:
  bool m_shouldThrow = SHOULD_THROW;
};

typedef DecodableTypeTpl<false> DecodableType;
typedef DecodableTypeTpl<true> DecodableTypeThrow;

BOOST_AUTO_TEST_CASE(LoadNoEncoding)
{
  this->writeFile<std::vector<uint8_t>>({0xBB, 0x01, 0xEE});
  shared_ptr<DecodableType> decoded = io::load<DecodableType>(filename, io::NO_ENCODING);
  BOOST_CHECK(decoded != nullptr);
}

BOOST_AUTO_TEST_CASE(LoadBase64)
{
  this->writeFile<std::string>("uwHu\n"); // printf '\xBB\x01\xEE' | base64
  shared_ptr<DecodableType> decoded = io::load<DecodableType>(filename, io::BASE64);
  BOOST_CHECK(decoded != nullptr);
}

BOOST_AUTO_TEST_CASE(LoadBase64Newline64)
{
  this->writeFile<std::string>(
    "CEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"
    "AAAAAAAAAAAA\n");
  // printf '\x08\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00
  //         \x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00
  //         \x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00
  //         \x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' | base64
  shared_ptr<name::Component> decoded = io::load<name::Component>(filename, io::BASE64);
  BOOST_CHECK(decoded != nullptr);
}

BOOST_AUTO_TEST_CASE(LoadBase64Newline32)
{
  this->writeFile<std::string>(
    "CEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"
    "AAAAAAAAAAAA\n");
  shared_ptr<name::Component> decoded = io::load<name::Component>(filename, io::BASE64);
  BOOST_CHECK(decoded != nullptr);
}

BOOST_AUTO_TEST_CASE(LoadBase64NewlineEnd)
{
  this->writeFile<std::string>(
    "CEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
  shared_ptr<name::Component> decoded = io::load<name::Component>(filename, io::BASE64);
  BOOST_CHECK(decoded != nullptr);
}

BOOST_AUTO_TEST_CASE(LoadBase64NoNewline)
{
  this->writeFile<std::string>(
    "CEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
  shared_ptr<name::Component> decoded = io::load<name::Component>(filename, io::BASE64);
  BOOST_CHECK(decoded != nullptr);
}

BOOST_AUTO_TEST_CASE(LoadHex)
{
  this->writeFile<std::string>("BB01EE");
  shared_ptr<DecodableType> decoded = io::load<DecodableType>(filename, io::HEX);
  BOOST_CHECK(decoded != nullptr);
}

BOOST_AUTO_TEST_CASE(LoadException)
{
  this->writeFile<std::vector<uint8_t>>({0xBB, 0x01, 0xEE});
  shared_ptr<DecodableTypeThrow> decoded;
  BOOST_CHECK_NO_THROW(decoded = io::load<DecodableTypeThrow>(filename, io::NO_ENCODING));
  BOOST_CHECK(decoded == nullptr);
}

BOOST_AUTO_TEST_CASE(LoadNotHex)
{
  this->writeFile<std::string>("not-hex");
  shared_ptr<DecodableType> decoded;
  BOOST_CHECK_NO_THROW(decoded = io::load<DecodableType>(filename, io::HEX));
  BOOST_CHECK(decoded == nullptr);
}

BOOST_AUTO_TEST_CASE(LoadFileNotReadable)
{
  shared_ptr<DecodableType> decoded;
  BOOST_CHECK_NO_THROW(decoded = io::load<DecodableType>(filename, io::NO_ENCODING));
  BOOST_CHECK(decoded == nullptr);
}

BOOST_AUTO_TEST_CASE(SaveNoEncoding)
{
  EncodableType encoded;
  BOOST_CHECK_NO_THROW(io::save(encoded, filename, io::NO_ENCODING));
  auto content = this->readFile<std::vector<uint8_t>>();
  uint8_t expected[] = {0xAA, 0x01, 0xDD};
  BOOST_CHECK_EQUAL_COLLECTIONS(content.begin(), content.end(),
                                expected, expected + sizeof(expected));
}

BOOST_AUTO_TEST_CASE(SaveBase64)
{
  EncodableType encoded;
  BOOST_CHECK_NO_THROW(io::save(encoded, filename, io::BASE64));
  auto content = this->readFile<std::string>();
  BOOST_CHECK_EQUAL(content, "qgHd\n"); // printf '\xAA\x01\xDD' | base64
}

BOOST_AUTO_TEST_CASE(SaveHex)
{
  EncodableType encoded;
  BOOST_CHECK_NO_THROW(io::save(encoded, filename, io::HEX));
  auto content = this->readFile<std::string>();
  BOOST_CHECK_EQUAL(content, "AA01DD");
}

BOOST_AUTO_TEST_CASE(SaveException)
{
  EncodableType encoded;
  encoded.shouldThrow = true;
  BOOST_CHECK_THROW(io::save(encoded, filename, io::NO_ENCODING), io::Error);
}

BOOST_AUTO_TEST_CASE(SaveFileNotWritable)
{
  this->mkdir();
  EncodableType encoded;
  encoded.shouldThrow = true;
  BOOST_CHECK_THROW(io::save(encoded, filename, io::NO_ENCODING), io::Error);
}

class IdCertFixture : public IoFixture
                    , public IdentityManagementFixture
{
};

BOOST_FIXTURE_TEST_CASE(IdCert, IdCertFixture)
{
  auto identity = addIdentity("/TestIo/IdCert", RsaKeyParams());
  const auto& cert = identity.getDefaultKey().getDefaultCertificate();
  io::save(cert, filename);

  auto readCert = io::load<security::v2::Certificate>(filename);

  BOOST_REQUIRE(readCert != nullptr);
  BOOST_CHECK_EQUAL(cert.getName(), readCert->getName());

  this->writeFile<std::string>("");
  readCert = io::load<security::v2::Certificate>(filename);
  BOOST_REQUIRE(readCert == nullptr);
}

BOOST_AUTO_TEST_SUITE_END() // TestIo
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace ndn
