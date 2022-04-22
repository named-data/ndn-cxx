/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/transform/block-cipher.hpp"

#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"

#include "tests/boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestBlockCipher)

//added_GM, by liupenghui
#if 1
BOOST_AUTO_TEST_CASE(sm4Cbc)
{
  const uint8_t key[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };
  const uint8_t iv[] = {
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
  };
  const uint8_t plainText[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };
  //
  // You can use the following shell one-liner to calculate the ciphertext:
  //   echo ${plaintext} | xxd -p -r | openssl enc -aes-128-cbc -K ${key} -iv ${iv} | xxd -i
  //
  const uint8_t cipherText[] = {
	0xd4, 0x81, 0xa5, 0x10, 0x1d, 0x74, 0x1f, 0xfb,
	0x87, 0x6e, 0x63, 0x65, 0xec, 0xc0, 0xcd, 0x22,
	0x54, 0xbc, 0xcc, 0x26, 0x81, 0xaa, 0x88, 0x85,
	0x6d, 0x0e, 0xa2, 0xc2, 0x16, 0x3e, 0x82, 0x2c,
	0x4a, 0xea, 0x93, 0x73, 0x46, 0xd4, 0x08, 0x7f,
	0x29, 0x6b, 0x89, 0x23, 0xb4, 0xd5, 0x02, 0x28
  };

  // encrypt
  OBufferStream os;
  bufferSource(plainText)
	>> blockCipher(BlockCipherAlgorithm::SM4_CBC, CipherOperator::ENCRYPT, key, iv)
	>> streamSink(os);

  auto buf = os.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(cipherText, cipherText + sizeof(cipherText),
								buf->begin(), buf->end());

  // decrypt
  OBufferStream os2;
  bufferSource(cipherText)
	>> blockCipher(BlockCipherAlgorithm::SM4_CBC, CipherOperator::DECRYPT, key, iv)
	>> streamSink(os2);

  auto buf2 = os2.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(plainText, plainText + sizeof(plainText),
								buf2->begin(), buf2->end());

  // invalid key length
  const uint8_t badKey[] = {0x00, 0x01, 0x02, 0x03};
  BOOST_CHECK_THROW(BlockCipher(BlockCipherAlgorithm::SM4_CBC, CipherOperator::ENCRYPT, badKey, iv),
					Error);

  // wrong iv length
  const uint8_t badIv[] = {0x00, 0x01, 0x02, 0x03};
  BOOST_CHECK_THROW(BlockCipher(BlockCipherAlgorithm::SM4_CBC, CipherOperator::ENCRYPT, key, badIv),
					Error);
}

BOOST_AUTO_TEST_CASE(sm4Ecb)
{
	const uint8_t key[16] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	const uint8_t iv[16] = {
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	const uint8_t input[16] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	/*
	 * This test vector comes from Example 1 of GB/T 32907-2016,
	 * and described in Internet Draft draft-ribose-cfrg-sm4-02.
	 */
	const uint8_t expected[16] = {
	  0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
	  0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46
	};
	
	// encrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os;
	bufferSource(make_span(input, 16))
	  >> blockCipher(BlockCipherAlgorithm::SM4_EBC, CipherOperator::ENCRYPT, key, iv)
	  >> streamSink(os);
	
	auto buf = os.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
								  buf->begin(), buf->begin()+16);
	// decrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os2;
	bufferSource(make_span(expected, 16))
	  >> blockCipher(BlockCipherAlgorithm::SM4_EBC, CipherOperator::DECRYPT, key, iv)
	  >> streamSink(os2);
	
	auto buf2 = os2.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(input, input + sizeof(input),
								  buf2->begin(), buf2->begin()+16);
		
}


BOOST_AUTO_TEST_CASE(AesEcb)
{
	const uint8_t key[16] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	const uint8_t iv[16] = {
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	const uint8_t input[16] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	/*
	 * This test vector comes from Example 1 of GB/T 32907-2016,
	 * and described in Internet Draft draft-ribose-cfrg-sm4-02.
	 */
	const uint8_t expected[16] = {
	  0xa6, 0x74, 0xf5, 0xa3, 0x89, 0x25, 0x35, 0x65,
	  0x26, 0x0d, 0x08, 0xdc, 0xbe, 0xd5, 0xc9, 0x71
	};
	
	// encrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os;
	bufferSource(make_span(input, 16))
	  >> blockCipher(BlockCipherAlgorithm::AES_EBC, CipherOperator::ENCRYPT, key, iv)
	  >> streamSink(os);
	
	auto buf = os.buf();
	
	BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
								  buf->begin(), buf->begin()+16);
	
	// decrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os2;
	bufferSource(make_span(expected, 16))
	  >> blockCipher(BlockCipherAlgorithm::AES_EBC, CipherOperator::DECRYPT, key, iv)
	  >> streamSink(os2);
	
	auto buf2 = os2.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(input, input + sizeof(input),
								  buf2->begin(), buf2->begin()+16);
		
}


BOOST_AUTO_TEST_CASE(sm4Ofb)
{
	const uint8_t key[16] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	const uint8_t iv[16] = {
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	const uint8_t input[32] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	/*
	 * This test vector comes from Example 1 of GB/T 32907-2016,
	 * and described in Internet Draft draft-ribose-cfrg-sm4-02.
	 */
	const uint8_t expected[32] = {
	  0x27, 0x54, 0xb1, 0x0c, 0x80, 0x6a, 0xef, 0x23,
	  0x69, 0x89, 0x89, 0x88, 0x2d, 0x80, 0x90, 0x3a,
	  0x2d, 0x33, 0x7e, 0x89, 0xa0, 0x19, 0xa4, 0xd3,
	  0x21, 0x66, 0x7e, 0xd5, 0xbb, 0xac, 0x8d, 0x7c
	};
	
	// encrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os;
	bufferSource(make_span(input, 32))
	  >> blockCipher(BlockCipherAlgorithm::SM4_OFB, CipherOperator::ENCRYPT, key, iv)
	  >> streamSink(os);
	
		
	auto buf = os.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
								  buf->begin(), buf->begin()+32);
	// decrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os2;
	bufferSource(make_span(expected, 32))
	  >> blockCipher(BlockCipherAlgorithm::SM4_OFB, CipherOperator::DECRYPT, key, iv)
	  >> streamSink(os2);
	
	auto buf2 = os2.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(input, input + sizeof(input),
								  buf2->begin(), buf2->begin()+32);
		
}

BOOST_AUTO_TEST_CASE(sm4Cfb)
{
	const uint8_t key[16] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	const uint8_t iv[16] = {
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	const uint8_t input[32] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	/*
	 * This test vector comes from Example 1 of GB/T 32907-2016,
	 * and described in Internet Draft draft-ribose-cfrg-sm4-02.
	 */
	const uint8_t expected[32] = {
	  0x27, 0x54, 0xb1, 0x0c, 0x80, 0x6a, 0xef, 0x23,
	  0x69, 0x89, 0x89, 0x88, 0x2d, 0x80, 0x90, 0x3a,
	  0xf7, 0x31, 0x1a, 0x15, 0xfc, 0x65, 0x98, 0xc3,
	  0xc4, 0xf7, 0x06, 0x6d, 0x45, 0x8a, 0xb8, 0x2b
	};
	
	// encrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os;
	bufferSource(make_span(input, 32))
	  >> blockCipher(BlockCipherAlgorithm::SM4_CFB, CipherOperator::ENCRYPT, key, iv)
	  >> streamSink(os);

	
	auto buf = os.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
								  buf->begin(), buf->begin()+32);
	// decrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os2;
	bufferSource(make_span(expected, 32))
	  >> blockCipher(BlockCipherAlgorithm::SM4_CFB, CipherOperator::DECRYPT, key, iv)
	  >> streamSink(os2);
	
	auto buf2 = os2.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(input, input + sizeof(input),
								  buf2->begin(), buf2->begin()+32);
		
}

BOOST_AUTO_TEST_CASE(aesOfb)
{
	const uint8_t key[16] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	const uint8_t iv[16] = {
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	const uint8_t input[32] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	/*
	 * This test vector comes from Example 1 of GB/T 32907-2016,
	 * and described in Internet Draft draft-ribose-cfrg-sm4-02.
	 */
	const uint8_t expected[32] = {
		0xd4, 0xeb, 0x60, 0xc5, 0x96, 0xaf, 0xa9, 0xd4,
		0xbd, 0x3e, 0x65, 0xaa, 0x0e, 0xf3, 0x50, 0xe7,
		0xea, 0xb1, 0x41, 0x98, 0x88, 0x67, 0xdb, 0x10,
		0x37, 0xcc, 0x00, 0xa3, 0xfa, 0x61, 0x8f, 0x25
	};
	
	// encrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os;
	bufferSource(make_span(input, 32))
	  >> blockCipher(BlockCipherAlgorithm::AES_OFB, CipherOperator::ENCRYPT, key, iv)
	  >> streamSink(os);
	
		
	auto buf = os.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
								  buf->begin(), buf->begin()+32);
	// decrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os2;
	bufferSource(make_span(expected, 32))
	  >> blockCipher(BlockCipherAlgorithm::AES_OFB, CipherOperator::DECRYPT, key, iv)
	  >> streamSink(os2);
	
	auto buf2 = os2.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(input, input + sizeof(input),
								  buf2->begin(), buf2->begin()+32);
		
}

BOOST_AUTO_TEST_CASE(aesCfb)
{
	const uint8_t key[16] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	const uint8_t iv[16] = {
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	const uint8_t input[32] = {
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
	  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	
	/*
	 * This test vector comes from Example 1 of GB/T 32907-2016,
	 * and described in Internet Draft draft-ribose-cfrg-sm4-02.
	 */
	const uint8_t expected[32] = {
	  0xd4, 0xeb, 0x60, 0xc5, 0x96, 0xaf, 0xa9, 0xd4,
	  0xbd, 0x3e, 0x65, 0xaa, 0x0e, 0xf3, 0x50, 0xe7,
	  0xa9, 0x12, 0x67, 0x39, 0x62, 0xbd, 0xb7, 0x9d,
	  0x28, 0xbc, 0x20, 0xc0, 0x5e, 0xaf, 0xd9, 0x97
	};
	
	// encrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os;
	bufferSource(make_span(input, 32))
	  >> blockCipher(BlockCipherAlgorithm::AES_CFB, CipherOperator::ENCRYPT, key, iv)
	  >> streamSink(os);

	
	auto buf = os.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
								  buf->begin(), buf->begin()+32);
	// decrypt
	// the user must ensure the length of input data is a mutilple of 16.
	OBufferStream os2;
	bufferSource(make_span(expected, 32))
	  >> blockCipher(BlockCipherAlgorithm::AES_CFB, CipherOperator::DECRYPT, key, iv)
	  >> streamSink(os2);
	
	auto buf2 = os2.buf();
	BOOST_CHECK_EQUAL_COLLECTIONS(input, input + sizeof(input),
								  buf2->begin(), buf2->begin()+32);
		
}

#endif

BOOST_AUTO_TEST_CASE(AesCbc)
{
  const uint8_t key[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };
  const uint8_t iv[] = {
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
  };
  const uint8_t plainText[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };
  //
  // You can use the following shell one-liner to calculate the ciphertext:
  //   echo ${plaintext} | xxd -p -r | openssl enc -aes-128-cbc -K ${key} -iv ${iv} | xxd -i
  //
  const uint8_t cipherText[] = {
    0x07, 0x4d, 0x32, 0x68, 0xc3, 0x40, 0x64, 0x43,
    0x1e, 0x66, 0x4c, 0x25, 0x66, 0x42, 0x0f, 0x59,
    0x0a, 0x51, 0x19, 0x07, 0x67, 0x5c, 0x0e, 0xfa,
    0xa6, 0x8c, 0xbb, 0xaf, 0xfd, 0xea, 0x47, 0xd4,
    0xc7, 0x2c, 0x12, 0x34, 0x79, 0xde, 0xec, 0xc8,
    0x75, 0x33, 0x8f, 0x6b, 0xd6, 0x55, 0xf3, 0xfa
  };

  // encrypt
  OBufferStream os;
  bufferSource(plainText)
    >> blockCipher(BlockCipherAlgorithm::AES_CBC, CipherOperator::ENCRYPT, key, iv)
    >> streamSink(os);

  auto buf = os.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(cipherText, cipherText + sizeof(cipherText),
                                buf->begin(), buf->end());

  // decrypt
  OBufferStream os2;
  bufferSource(cipherText)
    >> blockCipher(BlockCipherAlgorithm::AES_CBC, CipherOperator::DECRYPT, key, iv)
    >> streamSink(os2);

  auto buf2 = os2.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(plainText, plainText + sizeof(plainText),
                                buf2->begin(), buf2->end());

  // invalid key length
  const uint8_t badKey[] = {0x00, 0x01, 0x02, 0x03};
  BOOST_CHECK_THROW(BlockCipher(BlockCipherAlgorithm::AES_CBC, CipherOperator::ENCRYPT, badKey, iv),
                    Error);

  // wrong iv length
  const uint8_t badIv[] = {0x00, 0x01, 0x02, 0x03};
  BOOST_CHECK_THROW(BlockCipher(BlockCipherAlgorithm::AES_CBC, CipherOperator::ENCRYPT, key, badIv),
                    Error);
}

BOOST_AUTO_TEST_CASE(InvalidAlgorithm)
{
  BOOST_CHECK_THROW(BlockCipher(BlockCipherAlgorithm::NONE, CipherOperator::DECRYPT, {}, {}), Error);
  BOOST_CHECK_THROW(BlockCipher(BlockCipherAlgorithm::NONE, CipherOperator::ENCRYPT, {}, {}), Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestBlockCipher
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
