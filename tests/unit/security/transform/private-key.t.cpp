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

#include "ndn-cxx/security/transform/private-key.hpp"

#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/impl/openssl.hpp"
#include "ndn-cxx/security/key-params.hpp"
#include "ndn-cxx/security/transform/base64-decode.hpp"
#include "ndn-cxx/security/transform/bool-sink.hpp"
#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/public-key.hpp"
#include "ndn-cxx/security/transform/signer-filter.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/transform/verifier-filter.hpp"
#include "ndn-cxx/util/string-helper.hpp"

#include "tests/boost-test.hpp"

#include <boost/mpl/vector.hpp>

#include <sstream>

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestPrivateKey)

BOOST_AUTO_TEST_CASE(Empty)
{
  // test invoking member functions on an empty (default-constructed) PrivateKey
  PrivateKey sKey;
  BOOST_CHECK_EQUAL(sKey.getKeyType(), KeyType::NONE);
  BOOST_CHECK_EQUAL(sKey.getKeySize(), 0);
  BOOST_CHECK_THROW(sKey.getKeyDigest(DigestAlgorithm::SHA256), PrivateKey::Error);
  BOOST_CHECK_THROW(sKey.derivePublicKey(), PrivateKey::Error);
  BOOST_CHECK_THROW(sKey.decrypt({}), PrivateKey::Error);
  std::ostringstream os;
  BOOST_CHECK_THROW(sKey.savePkcs1(os), PrivateKey::Error);
  std::string passwd("password");
  BOOST_CHECK_THROW(sKey.savePkcs8(os, passwd.data(), passwd.size()), PrivateKey::Error);
}

#if OPENSSL_VERSION_NUMBER < 0x30000000L // FIXME #5154
BOOST_AUTO_TEST_CASE(KeyDigest)
{
  const Buffer buf(16);
  PrivateKey sKey;
  sKey.loadRaw(KeyType::HMAC, buf);
  auto digest = sKey.getKeyDigest(DigestAlgorithm::SHA256);

  const uint8_t expected[] = {
    0x37, 0x47, 0x08, 0xff, 0xf7, 0x71, 0x9d, 0xd5, 0x97, 0x9e, 0xc8, 0x75, 0xd5, 0x6c, 0xd2, 0x28,
    0x6f, 0x6d, 0x3c, 0xf7, 0xec, 0x31, 0x7a, 0x3b, 0x25, 0x63, 0x2a, 0xab, 0x28, 0xec, 0x37, 0xbb,
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(digest->begin(), digest->end(),
                                expected, expected + sizeof(expected));
}
#endif

BOOST_AUTO_TEST_CASE(LoadRaw)
{
  const Buffer buf(32);
  PrivateKey sKey;
  sKey.loadRaw(KeyType::HMAC, buf);
#if OPENSSL_VERSION_NUMBER < 0x30000000L // FIXME #5154
  BOOST_CHECK_EQUAL(sKey.getKeyType(), KeyType::HMAC);
  BOOST_CHECK_EQUAL(sKey.getKeySize(), 256);
#endif

  PrivateKey sKey2;
  BOOST_CHECK_THROW(sKey2.loadRaw(KeyType::NONE, buf), std::invalid_argument);
  BOOST_CHECK_THROW(sKey2.loadRaw(KeyType::RSA, buf), std::invalid_argument);
  BOOST_CHECK_THROW(sKey2.loadRaw(KeyType::EC, buf), std::invalid_argument);
  BOOST_CHECK_THROW(sKey2.loadRaw(KeyType::AES, buf), std::invalid_argument);
}

struct RsaKey_DesEncrypted
{
  const size_t keySize = 2048;
  const std::string privateKeyPkcs1 =
R"PEM(MIIEpAIBAAKCAQEAw0WM1/WhAxyLtEqsiAJgWDZWuzkYpeYVdeeZcqRZzzfRgBQT
sNozS5t4HnwTZhwwXbH7k3QN0kRTV826Xobws3iigohnM9yTK+KKiayPhIAm/+5H
GT6SgFJhYhqo1/upWdueojil6RP4/AgavHhopxlAVbk6G9VdVnlQcQ5Zv0OcGi73
c+EnYD/YgURYGSngUi/Ynsh779p2U69/te9gZwIL5PuE9BiO6I39cL9z7EK1SfZh
OWvDe/qH7YhD/BHwcWit8FjRww1glwRVTJsA9rH58ynaAix0tcR/nBMRLUX+e3rU
RHg6UbSjJbdb9qmKM1fTGHKUzL/5pMG6uBU0ywIDAQABAoIBADQkckOIl4IZMUTn
W8LFv6xOdkJwMKC8G6bsPRFbyY+HvC2TLt7epSvfS+f4AcYWaOPcDu2E49vt2sNr
cASly8hgwiRRAB3dHH9vcsboiTo8bi2RFvMqvjv9w3tK2yMxVDtmZamzrrnaV3YV
Q+5nyKo2F/PMDjQ4eUAKDOzjhBuKHsZBTFnA1MFNI+UKj5X4Yp64DFmKlxTX/U2b
wzVywo5hzx2Uhw51jmoLls4YUvMJXD0wW5ZtYRuPogXvXb/of9ef/20/wU11WFKg
Xb4gfR8zUXaXS1sXcnVm3+24vIs9dApUwykuoyjOqxWqcHRec2QT2FxVGkFEraze
CPa4rMECgYEA5Y8CywomIcTgerFGFCeMHJr8nQGqY2V/owFb3k9maczPnC9p4a9R
c5szLxA9FMYFxurQZMBWSEG2JS1HR2mnjigx8UKjYML/A+rvvjZOMe4M6Sy2ggh4
SkLZKpWTzjTe07ByM/j5v/SjNZhWAG7sw4/LmPGRQkwJv+KZhGojuOkCgYEA2cOF
T6cJRv6kvzTz9S0COZOVm+euJh/BXp7oAsAmbNfOpckPMzqHXy8/wpdKl6AAcB57
OuztlNfV1D7qvbz7JuRlYwQ0cEfBgbZPcz1p18HHDXhwn57ZPb8G33Yh9Omg0HNA
Imb4LsVuSqxA6NwSj7cpRekgTedrhLFPJ+Ydb5MCgYEAsM3Q7OjILcIg0t6uht9e
vrlwTsz1mtCV2co2I6crzdj9HeI2vqf1KAElDt6G7PUHhglcr/yjd8uEqmWRPKNX
ddnnfVZB10jYeP/93pac6z/Zmc3iU4yKeUe7U10ZFf0KkiiYDQd59CpLef/2XScS
HB0oRofnxRQjfjLc4muNT+ECgYEAlcDk06MOOTly+F8lCc1bA1dgAmgwFd2usDBd
Y07a3e0HGnGLN3Kfl7C5i0tZq64HvxLnMd2vgLVxQlXGPpdQrC1TH+XLXg+qnlZO
ivSH7i0/gx75bHvj75eH1XK65V8pDVDEoSPottllAIs21CxLw3N1ObOZWJm2EfmR
cuHICmsCgYAtFJ1idqMoHxES3mlRpf2JxyQudP3SCm2WpGmqVzhRYInqeatY5sUd
lPLHm/p77RT7EyxQHTlwn8FJPuM/4ZH1rQd/vB+Y8qAtYJCexDMsbvLW+Js+VOvk
jweEC0nrcL31j9mF0vz5E6tfRu4hhJ6L4yfWs0gSejskeVB/w8QY4g==
)PEM";
  const std::string privateKeyPkcs8 =
R"PEM(MIIFCzA9BgkqhkiG9w0BBQ0wMDAbBgkqhkiG9w0BBQwwDgQIOKYJXvB6p8kCAggA
MBEGBSsOAwIHBAiQgMK8kQXTyASCBMjeNiKYYw5/yHgs9BfSGrpqvV0LkkgMQNUW
R4ZY8fuNjZynd+PxDuw2pyrv1Yv3jc+tupwUehZEzYOnGd53wQAuLO+Z0TBgRFN7
Lhk+AxlT7hu0xaB3ZpJ/uvWpgEJHsq/aB/GYgyzXcQo2AiqzERVpMCWJVmE1L977
CHwJmLm5mxclVLYp1UK5lkIBFu/M4nPavmNmYNUU1LOrXRo56TlJ2kUp8gQyQI1P
VPxi4chmlsr/OnQ2d1eZN+euFm0CS+yP+LFgI9ZqdyH1w+J43SXdHDzauVcZp7oa
Kw24OrhHfolLAnQIECXEJYeT7tZmhC4O9V6B18PFVyxWnEU4eFNpFE8kYSmm8Um2
buvDKI71q43hm23moYT9uIM1f4M8UkoOliJGrlf4xgEcmDuokEX01PdOq1gc4nvG
0DCwDI9cOsyn8cxhk9UVtFgzuG/seuznxIv1F5H0hzYOyloStXxRisJES0kgByBt
FFTfyoFKRrmCjRIygwVKUSkSDR0DlQS5ZLvQyIswnSQFwxAHqfvoSL4dB9UAIAQ+
ALVF1maaHgptbL6Ifqf0GFCv0hdNCVNDNCdy8R+S6nEYE+YdYSIdT1L88KD5PjU3
YY/CMnxhTncMaT4acPO1UUYuSGRZ/JL6E0ihoqIU+bqUgLSHNzhPySPfN9uqN61Y
HFBtxeEPWKU0f/JPkRBMmZdMI1/OVmA3QHSRBydI+CQN8no2gZRFoVbHTkG8IMpE
1fiDJpwFkpzIv/JPiTSE7DeBH5NJk1bgu7TcuZfa4unyAqss0UuLnXzS06TppkUj
QGft0g8VPW56eli6B4xrSzzuvAdbrxsVfxdmtHPyYxLb3/UG1g4x/H/yULhx7x9P
iI6cw6JUE+8bwJV2ZIlHXXHO+wUp/gCFJ6MHo9wkR1QvnHP2ClJAzBm9OvYnUx2Y
SX0HxEowW8BkhxOF184LEmxeua0yyZUqCdrYmErp7x9EY/LhD1zBwH8OGRa0qzmR
VKxAPKihkb9OgxcUKbvKePx3k2cQ7fbCUspGPm4Kn1zwMgRAZ4fz/o8Lnwc8MSY3
lPWnmLTFu420SRH2g9N0o/r195hiZ5cc+KfF4pwZWKbEbKFk/UfXA9vmOi7BBtDJ
RWshOINhzMU6Ij3KuaEpHni1HoHjw0SQ97ow2x/aB8k2QC28tbsa49lD2KKJku6b
2Or89adwFKqMgS2IXfXMXs/iG5EFLYN6r8e40Dn5f1vJfRLJl03XByIfT2n92pw3
fP7muOIKLUsEKjOrmn94NwMlfeW13oQHEH2KjPOWFS/tyJHDdVU+of4COH5yg59a
TZqFkOTGeliE1O+6sfF9fRuVxFUF3D8Hpr0JIjdc6+3RgIlGsXc8BwiSjDSI2XW+
vo75/2zPU9t8OeXEIJk2CQGyqLwUJ6dyi/yDRrvZAgjrUvbpcxydnBAHrLbLUGXJ
aEHH2tjEtnTqVyTchr1yHoupcFOCkA0dAA66XqwcssQxJiMGrWTpCbgd9mrTXQaZ
U7afFN1jpO78tgBQUUpImXdHLLsqdN5tefqjileZGZ9x3/C6TNAfDwYJdsicNNn5
y+JVsbltfLWlJxb9teb3dtQiFlJ7ofprLJnJVqI/Js8lozY+KaxV2vtbZkcD4dM=
)PEM";
  const std::string publicKey =
R"PEM(MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAw0WM1/WhAxyLtEqsiAJg
WDZWuzkYpeYVdeeZcqRZzzfRgBQTsNozS5t4HnwTZhwwXbH7k3QN0kRTV826Xobw
s3iigohnM9yTK+KKiayPhIAm/+5HGT6SgFJhYhqo1/upWdueojil6RP4/AgavHho
pxlAVbk6G9VdVnlQcQ5Zv0OcGi73c+EnYD/YgURYGSngUi/Ynsh779p2U69/te9g
ZwIL5PuE9BiO6I39cL9z7EK1SfZhOWvDe/qH7YhD/BHwcWit8FjRww1glwRVTJsA
9rH58ynaAix0tcR/nBMRLUX+e3rURHg6UbSjJbdb9qmKM1fTGHKUzL/5pMG6uBU0
ywIDAQAB
)PEM";
};

struct RsaKey_Aes256Encrypted
{
  // Generated with OpenSSL 3.0.0
  // openssl genpkey -quiet -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -outform PEM -out key.pem
  // openssl pkcs8 -in key.pem -nocrypt -traditional -outform PEM -out pvt1.pem
  // openssl pkcs8 -in key.pem -topk8 -v2 aes256 -passout pass:password -outform PEM -out pvt8.pem
  // openssl pkey -in key.pem -pubout -outform PEM -out pub.pem

  const size_t keySize = 2048;
  const std::string privateKeyPkcs1 =
R"PEM(MIIEowIBAAKCAQEAzEKtiRrzvhwGDMpsUlKMd7Veo9+lvLpttdX/ETo2exU36HqV
6zSM7LnpfqId7zug+l+VGwPd+oJOzROSCAoUcP3C3m9CkfqT97H9g2zdfwXqJnMQ
tnpAGngXOTF6RSBDVqo+jfeRw3c+MCXDZ0cHd49uv+CnWks6Sj+gowppdsQfvQRr
2incnwqht3JuaiTytxNrk6DjpoNTHyZJ53SY6DibGSk6aDFWMJcfvTZ6EuUbcepP
7HRwABn4Ctj1FTx3hDPe4sIrBFrbfP7yDQ9NxYh1baxXcCjWfCS43qUjrtcQFVkK
vE3uucmw1rf0C0Wy7dDvF2n1v+WLAEPSgJnumwIDAQABAoIBAEdNgWGKkIqNIsmF
QhHssg85t2tSL3t1wsWGic8cOJd3vTgAzuO3yPf8IBeuBPAVqyirhBPVokAIC/UH
v2LiDeexlbxrL1xhEhUVw48Eyj9Es8uvQCbK/ySeRlEXRfzqecc/j62kPfRzZDiP
fipHv8ILRlhh1lmtSBBSLMOtZ0pnJUmrNIOaFlJa56t0G3a+4C1u2swOtoCOWtyR
o7NAHiCcZNbnFQQSLyfDj3Sqnsaxke41A+gtrSeeUHkqnP0rxdmqChdyU36Yszi2
nB+yUXJT7Iw5GD+/vVUoby1/FVCRbh8zNfXvozC3llUSmsn3CBzVLoI+uwD5/zSz
+zMZ1x0CgYEA1o/cuEnALy+FXYbyrdV2R+ecy6t2/3WFDmK7+7klFFHcPBbmh/nL
OIjFPHVYUcyv2qW6JnCYqCutPOJ97JGvyScriNdg047UPGHaFaPZY/4rTXLm6tzN
I6E5eTRLkaFDVb96zYevxLjf88Lceocp2g8vR88kHqKe//0K8sBw+n0CgYEA87WA
CJOzmBdeWN7GbfA9rqxQq8qsFMaG+mJ5sE1MQHW5j/Hzsj6xi5qS9Uy8D9qlQiSV
nA/frqNBjaXiagVIqvfKvkkOFqRFIkVfGIJcYJk3tzi3tgdcHJ0B0LNCg/xHtjuZ
9KiuBNlDfkhsV6d7g/YQ0Sf/ghWmSGyOtVEtQPcCgYEAs5wYK1jpfVZtcN5/lc8k
RYr4MXJmmfB5opI6RL028eyYzOBquJb9bGTpnvOoLEmJSCIFUxpcYCK30UjUGs3V
9jBI/DM3hcGBns5W7liLqW3iN+IgtaiCPPpAj1qci9sP797rYNPd6nLMXlTXleZB
vZ2KebVHyjFdonLj0FQR/00CgYBSbXLuc7ZsnIrGmCKZEIZsS8/FKvlk1XjVuvTZ
kmtV6ftnGjiIcvft9cv6t4dr/VGju2f2rs/C62jClfasUTkwyjqCfYcMVWcknj35
ti20Zl4X1FEeegLHkrsIcXjv1yYSFrqNq3egIDPZxHkQdI8sJM+vTk33G4dwO3dR
EDG0JQKBgBeZhFw4R4MgCYUjNecw+k6AcU5J1JZ+KCJpSBv6J9ilQhkM0PeGLEab
TsurvpoIdAlqojgMnNf98gBahz+zjyLJEB+SPU6AGsahkQZ8wRY1a7ua/SjHPbCk
I3CkY6wqJK8vcjQkUQhE+5y1ZBIhQpH22eHyM4WMLaTFsL9yMvzY
)PEM";
  const std::string privateKeyPkcs8 =
R"PEM(MIIFLTBXBgkqhkiG9w0BBQ0wSjApBgkqhkiG9w0BBQwwHAQIc3ZMcKTZDhcCAggA
MAwGCCqGSIb3DQIJBQAwHQYJYIZIAWUDBAEqBBA1/62RtDr/mX2ZuI3Nq9tTBIIE
0ITRsiXopz4ox+k3ltt64vwQDLrKhm3uoz9lb885mD/wDkRU+02a3Kq+LjhExawM
Po8VVIbMjyXa3c2eYlmPOUXuXyMz8zi1NyDD1ypZTcVQxIT1ZZK+qsTlIy9Q2MIt
PU/Xg/nPCCJn7FBjkjqpGa942SVnvAbBLk342y8XozHmdbKYDVvNRjkYd6zUpR/n
5EG1AHBHNFySbtJSg5/6HU5sX6pM7LNAiTT3i0r8LcCWijHofhmUbCfrg+DdgW5+
ttmsTo/HZCjSSsHtd6xqxdOJyfvFIV1BvVKFP5X6OK8cnHtk/0SuNt2OROjvbOtU
JqSGkzV6MukagrMVtTu9BuXGNz8ee1zVglArEwHw13MihI9a/lGD/AujwjU9CdOd
jH6l2Ibp1MrSn52injoY9i6VmFSTktOrTrIzkGVevHcHkyZBqN3mwQRL3M16t0Bp
RJl01z5b7UYoSDO/w4BOtf6NK/zWix1+GHDnz9fplsaebiDF3YFOq6+p+1/14zPg
cfMkqgco6AGijPimhCQhMVnuWoBKcwIHCs6+sp02lI5HsVl3Lke4586w3qqOU9QY
L8iJFlLax/Sm9MVM/usWUaolXY6BElqkqhd3Z7KJoCazWos2oxVHkutKArNYf9XI
DV309jwBSnBDzmgDpXk7VZWFswxJIc8I1z5759wh370NJ06HeGCLvgBuLp6Dkq2c
hNqR32HWN7wjVqX2hB7X7RPjwX4a/z2oih/fQFc5WuIHfKWTviWcVq8jhIxocLqz
dSGWVvj6j0LoDh7s26iCXe6hb5N5khWHI6Dh7ZFC27aSejljyNqB+KEblIHvTPjS
WgGF14QTwDL6L4tpwtxoPG+QFnoHhM2ORkvwFkmQRmfSKeBfTapiRDLPQvlfIzTI
6Ie3l9gA4H14HqffawdwfP1fS/lOlLFH7SyO59tSjtVC3ErLMhHD+1R+Q3X1w/lF
jUZ2vwvE+86ikhxZvQCVOiCGXqry9qZfm67cuXD/tolAv5pV2qFDxHAEe5QqUYSv
bJTgo6zin1XByu3jlQPblcEeJ33iRcQDqh96GgMnJstsGuUjBREVjbNZ30YZYeVg
+L/h9otyO92Nq0knADHeixy9JZ2ATRMoTAT4XydSN9lHOh+K5pC3kf9pke2NCJyr
r/w9VQnQiIkxZ+THFshHHJxZZLlLwYWWr894rKftyZB+nDidNZfXr6K8VWoCAwh4
XwV6109KeHx86Kim9KQPn5RYVFdoxt2dO5O9vcXWtcNciIEi4MPyUM+oHwWrKTLX
EQsVu5gaz8vFt3lg0+Ctf8vAIA1/FxXEZ/BTAiV96lvRcUbXU2AdL+rNU2rtpqxH
h9L3r6IEN9WVIkNH5Q4CqSS9z+BxIPj76dA1MXK1nb8OJ9Y+BMPhktMz9M+KcVpD
1rXebv+/9Tw0lKLcNL50AglJOP+IRL0hHh62xtXs+FP5KocBs1kVAJORrS6wiy22
DGwcVmgTvWsPHzIf3IEa7DEjhClnU6alxKxCEmzPtm29upYXL9o3y7CMwidvCYpD
zFU2pIKA85YpjUYL25MiCyJPODc3VoW9l+rBqPu6bVWZXsfgsD4lcdFaJbPqcVby
PEe2dEUjhwpYgjbjOO7geMe+VeShipZJ63yFsNaPWzcz
)PEM";
  const std::string publicKey =
R"PEM(MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAzEKtiRrzvhwGDMpsUlKM
d7Veo9+lvLpttdX/ETo2exU36HqV6zSM7LnpfqId7zug+l+VGwPd+oJOzROSCAoU
cP3C3m9CkfqT97H9g2zdfwXqJnMQtnpAGngXOTF6RSBDVqo+jfeRw3c+MCXDZ0cH
d49uv+CnWks6Sj+gowppdsQfvQRr2incnwqht3JuaiTytxNrk6DjpoNTHyZJ53SY
6DibGSk6aDFWMJcfvTZ6EuUbcepP7HRwABn4Ctj1FTx3hDPe4sIrBFrbfP7yDQ9N
xYh1baxXcCjWfCS43qUjrtcQFVkKvE3uucmw1rf0C0Wy7dDvF2n1v+WLAEPSgJnu
mwIDAQAB
)PEM";
};

struct EcKeySpecifiedCurve_DesEncrypted
{
  // EC keys are generated in "namedCurve" format only. However, old keys in "specifiedCurve"
  // format are still accepted. See https://redmine.named-data.net/issues/5037

  const size_t keySize = 256;
  const std::string privateKeyPkcs1 =
R"PEM(MIIBaAIBAQQgRxwcbzK9RV6AHYFsDcykI86o3M/a1KlJn0z8PcLMBZOggfowgfcC
AQEwLAYHKoZIzj0BAQIhAP////8AAAABAAAAAAAAAAAAAAAA////////////////
MFsEIP////8AAAABAAAAAAAAAAAAAAAA///////////////8BCBaxjXYqjqT57Pr
vVV2mIa8ZR0GsMxTsPY7zjw+J9JgSwMVAMSdNgiG5wSTamZ44ROdJreBn36QBEEE
axfR8uEsQkf4vOblY6RA8ncDfYEt6zOg9KE5RdiYwpZP40Li/hp/m47n60p8D54W
K84zV2sxXs7LtkBoN79R9QIhAP////8AAAAA//////////+85vqtpxeehPO5ysL8
YyVRAgEBoUQDQgAEaG4WJuDAt0QkEM4t29KDUdzkQlMPGrqWzkWhgt9OGnwc6O7A
ZLPSrDyhwyrKS7XLRXml5DisQ93RvByll32y8A==
)PEM";
  const std::string privateKeyPkcs8 =
R"PEM(MIIBwzA9BgkqhkiG9w0BBQ0wMDAbBgkqhkiG9w0BBQwwDgQIVHkBzLGtDvICAggA
MBEGBSsOAwIHBAhk6g9eI3toNwSCAYDd+LWPDBTrKV7vUyxTvDbpUd0eXfh73DKA
MHkdHuVmhpmpBbsF9XvaFuL8J/1xi1Yl2XGw8j3WyrprD2YEhl/+zKjNbdTDJmNO
SlomuwWb5AVCJ9reT94zIXKCnexUcyBFS7ep+P4dwuef0VjzprjfmnAZHrP+u594
ELHpKwi0ZpQLtcJjjud13bn43vbXb+aU7jmPV5lU2XP8TxaQJiYIibNEh1Y3TZGr
akJormYvhaYbiZkKLHQ9AvQMEjhoIW5WCB3q+tKZUKTzcQpjNnf9FOTeKN3jk3Kd
2OmibPZcbMJdgCD/nRVn1cBo7Hjn3IMjgtszQHtEUphOQiAkOJUnKmy9MTYqtcNN
6cuFItbu4QvbVwailgdUjOYwIJCmIxExlPV0ohS24pFGsO03Yn7W8rBB9VWENYmG
HkZIbGsHv7O9Wy7fv+FJgZkjeti0807IsNXSJl8LUK0ZIhAR7OU8uONWMsbHdQnk
q1HB1ZKa52ugACl7g/DF9b7CoSAjFeE=
)PEM";
  const std::string publicKey =
R"PEM(MIIBSzCCAQMGByqGSM49AgEwgfcCAQEwLAYHKoZIzj0BAQIhAP////8AAAABAAAA
AAAAAAAAAAAA////////////////MFsEIP////8AAAABAAAAAAAAAAAAAAAA////
///////////8BCBaxjXYqjqT57PrvVV2mIa8ZR0GsMxTsPY7zjw+J9JgSwMVAMSd
NgiG5wSTamZ44ROdJreBn36QBEEEaxfR8uEsQkf4vOblY6RA8ncDfYEt6zOg9KE5
RdiYwpZP40Li/hp/m47n60p8D54WK84zV2sxXs7LtkBoN79R9QIhAP////8AAAAA
//////////+85vqtpxeehPO5ysL8YyVRAgEBA0IABGhuFibgwLdEJBDOLdvSg1Hc
5EJTDxq6ls5FoYLfThp8HOjuwGSz0qw8ocMqyku1y0V5peQ4rEPd0bwcpZd9svA=
)PEM";
};

struct EcKeyNamedCurve_DesEncrypted
{
  // Generated with older (1.0.x?) OpenSSL
  // openssl ecparam -name secp384r1 -genkey -out pvt1.pem
  // openssl pkcs8 -in pvt1.pem -topk8 -passout pass:password -outform PEM -out pvt8.pem
  // openssl ec -in pvt1.pem -pubout -outform PEM -out pub.pem

  const size_t keySize = 384;
  const std::string privateKeyPkcs1 =
R"PEM(MIGkAgEBBDCUsb7NymksCkQAjdLMjUilWhOEyeYmGi79sX1RbsmfnoF/8SesKBhO
or+TZ8g8/8igBwYFK4EEACKhZANiAARWGplLOGdQiXRFQcd0VLPeTt0zXEj5zvSv
aHx9MrzBy57wgz10wTAiR561wuLtFAYxmqL9Ikrzx/BaEg0+v2zQ05NCzMNN8v2c
7/FzOhD7fmZrlJsT6Q2aHGExW0Rj3GE=
)PEM";
  const std::string privateKeyPkcs8 =
R"PEM(MIHgMBsGCSqGSIb3DQEFAzAOBAjniUjwJfWsMQICCAAEgcCakGTKa49csaPpmtzi
5sTJw+AH8ajUqcDbtp2pJP/Ni6M1p9fai9hOKPElf9uJuYh/S80FAU6WQmZBAxL4
bF598ncLPogpGvz21wLuSc1xnbD829zAsMmh0XZvMZpBWX2g0NnZJx7GOraskTSh
7qGu70B+uKzw+JxIzgBEeMcoBUg8mTEht5zghfLGYkQp6BpTPdpU64udpPAKzFNs
5X+BzBnT5Yy49/Lp4uYIji8qwJFF3VqTn8RFKunFYDDejRU=
)PEM";
  const std::string publicKey =
R"PEM(MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAEVhqZSzhnUIl0RUHHdFSz3k7dM1xI+c70
r2h8fTK8wcue8IM9dMEwIkeetcLi7RQGMZqi/SJK88fwWhINPr9s0NOTQszDTfL9
nO/xczoQ+35ma5SbE+kNmhxhMVtEY9xh
)PEM";
};

struct EcKeyNamedCurve_Aes256Encrypted
{
  // Generated with OpenSSL 3.0.0
  // openssl genpkey -quiet -algorithm EC -pkeyopt ec_paramgen_curve:P-384
  //                 -pkeyopt ec_param_enc:named_curve -outform PEM -out key.pem
  // openssl pkcs8 -in key.pem -nocrypt -traditional -outform PEM -out pvt1.pem
  // openssl pkcs8 -in key.pem -topk8 -v2 aes256 -passout pass:password -outform PEM -out pvt8.pem
  // openssl pkey -in key.pem -pubout -outform PEM -out pub.pem

  const size_t keySize = 384;
  const std::string privateKeyPkcs1 =
R"PEM(MIGkAgEBBDAa5I6hfwcDZ8Hzn6/ZU3jV/Fp6jZP+tgUHSdMcfRS3iLOY296KdcXd
xISVHU3g+XygBwYFK4EEACKhZANiAAQh3xGTxgMFXwoRNBCrnzawZnoQUojZ1Qe5
3VrR+9ECawVf2sPklX+Lw1pE1TG5q+YepM8imRclRbXLBSGPF6f+qJCiFWR71XS2
CX2kW/AZOGluxYrVMe22ns5TY2o50G8=
)PEM";
  const std::string privateKeyPkcs8 =
R"PEM(MIIBHDBXBgkqhkiG9w0BBQ0wSjApBgkqhkiG9w0BBQwwHAQIy4A7Il3JRNQCAggA
MAwGCCqGSIb3DQIJBQAwHQYJYIZIAWUDBAEqBBBPYOn/2qSDBlKhZ6+ckre8BIHA
jXwlDh+OX+1VayCWohXUooxZPfgKmIVkxhNrQzn0DV/FW9WbiXC72vss75/o5O2H
o4EuIOxnbxIM5HhsrdDY5g8Abkk5gKYt+eVJQn1EPAR+PU9T8OUKyQr9rO0Pvsu8
9h5V9+WFHNnNBTN3pqIV5cU25oVX7tqeZKs+5I4QIaq4hzPj+k15dLFaaq5XejZH
KjMKNdKbfz4TEs7u1W9YyrD3Z8/RgzLwVRjJqqOKeoJF4b1lOqOd5rWb4Oa06alr
)PEM";
  const std::string publicKey =
R"PEM(MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAEId8Rk8YDBV8KETQQq582sGZ6EFKI2dUH
ud1a0fvRAmsFX9rD5JV/i8NaRNUxuavmHqTPIpkXJUW1ywUhjxen/qiQohVke9V0
tgl9pFvwGThpbsWK1THttp7OU2NqOdBv
)PEM";
};

struct EcKeyNamedCurve_Des3Encrypted
{
  // Generated with OpenSSL 3.0.0
  // openssl genpkey -quiet -algorithm EC -pkeyopt ec_paramgen_curve:P-256
  //                 -pkeyopt ec_param_enc:named_curve -outform PEM -out key.pem
  // openssl pkcs8 -in key.pem -nocrypt -traditional -outform PEM -out pvt1.pem
  // openssl pkcs8 -in key.pem -topk8 -v2 des3 -passout pass:password -outform PEM -out pvt8.pem
  // openssl pkey -in key.pem -pubout -outform PEM -out pub.pem

  const size_t keySize = 256;
  const std::string privateKeyPkcs1 =
R"PEM(MHcCAQEEIEbb/pnOVzkIZP4jrpZQCxl2OTJVQD7rHBJxxv8Aa215oAoGCCqGSM49
AwEHoUQDQgAEvSFhYwmFe17QAj0xhumERNJFf5MTkAegdE+db4ojchAbcDLRcnzY
+TUx5pTBqdBg+STMK7h36ZUn+KcMeizMRA==
)PEM";
  const std::string privateKeyPkcs8 =
R"PEM(MIHjME4GCSqGSIb3DQEFDTBBMCkGCSqGSIb3DQEFDDAcBAj2rY09BeCZGgICCAAw
DAYIKoZIhvcNAgkFADAUBggqhkiG9w0DBwQI6CIHMAz7K7AEgZBzyr8QcH1OWEqc
ByPy4Vlze9D0izD7/NIxUeauHILqXMjU6Z057oYZ14Nm+DpbyJWu16gjZ2N+M2GH
4bH8gS6JUGhqtSbQ1Oo5lmjPXphefZp0tarv19Hp8S1VKmcVoj65kxfn5x0GKepg
fhXlMB2uQHvNoujU9PyeTZhwjAPQ6vHgFonbPPaL/f256NqMmnA=
)PEM";
  const std::string publicKey =
R"PEM(MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEvSFhYwmFe17QAj0xhumERNJFf5MT
kAegdE+db4ojchAbcDLRcnzY+TUx5pTBqdBg+STMK7h36ZUn+KcMeizMRA==
)PEM";
};

using KeyTestDataSets = boost::mpl::vector<
#if OPENSSL_VERSION_NUMBER < 0x30000000L
  // DES-encrypted keys
  // .privateKeyPkcs8 uses either the PBES1 or PBES2 encryption scheme with DES-CBC-Pad (see RFC 8018)
  // This is no longer supported out-of-the-box by OpenSSL 3.0 and later
  RsaKey_DesEncrypted,
  EcKeySpecifiedCurve_DesEncrypted,
  EcKeyNamedCurve_DesEncrypted,
#endif
  // AES-encrypted keys
  // .privateKeyPkcs8 uses the PBES2 encryption scheme with AES-CBC-Pad (see RFC 8018)
  // This works with all supported versions of OpenSSL
  RsaKey_Aes256Encrypted,
  EcKeyNamedCurve_Aes256Encrypted,
  // 3DES-encrypted keys
  // .privateKeyPkcs8 uses the PBES2 encryption scheme with DES-EDE3-CBC-Pad (see RFC 8018)
  // This works with all supported versions of OpenSSL
  EcKeyNamedCurve_Des3Encrypted
>;

static void
checkPkcs8Encoding(const ConstBufferPtr& encoding,
                   const std::string& password,
                   const ConstBufferPtr& pkcs1)
{
  PrivateKey sKey;
  sKey.loadPkcs8(*encoding, password.data(), password.size());
  OBufferStream os;
  sKey.savePkcs1(os);
  BOOST_CHECK_EQUAL_COLLECTIONS(pkcs1->begin(), pkcs1->end(),
                                os.buf()->begin(), os.buf()->end());
}

static void
checkPkcs8Base64Encoding(const ConstBufferPtr& encoding,
                         const std::string& password,
                         const ConstBufferPtr& pkcs1)
{
  OBufferStream os;
  bufferSource(*encoding) >> base64Decode() >> streamSink(os);
  checkPkcs8Encoding(os.buf(), password, pkcs1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SaveLoad, T, KeyTestDataSets)
{
  T dataSet;

  auto sKeyPkcs1Base64 = make_span(reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.data()),
                                   dataSet.privateKeyPkcs1.size());
  OBufferStream os;
  bufferSource(sKeyPkcs1Base64) >> base64Decode() >> streamSink(os);
  auto sKeyPkcs1 = os.buf();

  // load key in base64-encoded pkcs1 format
  PrivateKey sKey;
  BOOST_CHECK_NO_THROW(sKey.loadPkcs1Base64(sKeyPkcs1Base64));
  BOOST_CHECK_EQUAL(sKey.getKeySize(), dataSet.keySize);

  std::stringstream ss2(dataSet.privateKeyPkcs1);
  PrivateKey sKey2;
  BOOST_CHECK_NO_THROW(sKey2.loadPkcs1Base64(ss2));

  // load key in pkcs1 format
  PrivateKey sKey3;
  BOOST_CHECK_NO_THROW(sKey3.loadPkcs1(*sKeyPkcs1));
  BOOST_CHECK_EQUAL(sKey3.getKeySize(), dataSet.keySize);

  std::stringstream ss4;
  ss4.write(reinterpret_cast<const char*>(sKeyPkcs1->data()), sKeyPkcs1->size());
  PrivateKey sKey4;
  BOOST_CHECK_NO_THROW(sKey4.loadPkcs1(ss4));

  // save key in base64-encoded pkcs1 format
  OBufferStream os2;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs1Base64(os2));
  BOOST_CHECK_EQUAL_COLLECTIONS(sKeyPkcs1Base64.begin(), sKeyPkcs1Base64.end(),
                                os2.buf()->begin(), os2.buf()->end());

  // save key in pkcs1 format
  OBufferStream os3;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs1(os3));
  BOOST_CHECK_EQUAL_COLLECTIONS(sKeyPkcs1->begin(), sKeyPkcs1->end(),
                                os3.buf()->begin(), os3.buf()->end());

  auto sKeyPkcs8Base64 = make_span(reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs8.data()),
                                   dataSet.privateKeyPkcs8.size());
  OBufferStream os4;
  bufferSource(sKeyPkcs8Base64) >> base64Decode() >> streamSink(os4);
  auto sKeyPkcs8 = os4.buf();

  std::string password("password");
  std::string wrongpw("wrongpw");
  auto pwCallback = [&password] (char* buf, size_t size, int) -> int {
    BOOST_REQUIRE_LE(password.size(), size);
    std::copy(password.begin(), password.end(), buf);
    return static_cast<int>(password.size());
  };

  // load key in base64-encoded pkcs8 format
  PrivateKey sKey5;
  BOOST_CHECK_NO_THROW(sKey5.loadPkcs8Base64(sKeyPkcs8Base64, password.data(), password.size()));
  BOOST_CHECK_EQUAL(sKey5.getKeySize(), dataSet.keySize);

  PrivateKey sKey6;
  BOOST_CHECK_NO_THROW(sKey6.loadPkcs8Base64(sKeyPkcs8Base64, pwCallback));

  std::stringstream ss7(dataSet.privateKeyPkcs8);
  PrivateKey sKey7;
  BOOST_CHECK_NO_THROW(sKey7.loadPkcs8Base64(ss7, password.data(), password.size()));

  std::stringstream ss8(dataSet.privateKeyPkcs8);
  PrivateKey sKey8;
  BOOST_CHECK_NO_THROW(sKey8.loadPkcs8Base64(ss8, pwCallback));

  // load key in pkcs8 format
  PrivateKey sKey9;
  BOOST_CHECK_NO_THROW(sKey9.loadPkcs8(*sKeyPkcs8, password.data(), password.size()));
  BOOST_CHECK_EQUAL(sKey9.getKeySize(), dataSet.keySize);

  PrivateKey sKey10;
  BOOST_CHECK_NO_THROW(sKey10.loadPkcs8(*sKeyPkcs8, pwCallback));

  std::stringstream ss11;
  ss11.write(reinterpret_cast<const char*>(sKeyPkcs8->data()), sKeyPkcs8->size());
  PrivateKey sKey11;
  BOOST_CHECK_NO_THROW(sKey11.loadPkcs8(ss11, password.data(), password.size()));

  std::stringstream ss12;
  ss12.write(reinterpret_cast<const char*>(sKeyPkcs8->data()), sKeyPkcs8->size());
  PrivateKey sKey12;
  BOOST_CHECK_NO_THROW(sKey12.loadPkcs8(ss12, pwCallback));

  // load key using wrong password, Error is expected
  PrivateKey sKey13;
  BOOST_CHECK_THROW(sKey13.loadPkcs8Base64(sKeyPkcs8Base64, wrongpw.data(), wrongpw.size()),
                    PrivateKey::Error);
  BOOST_CHECK_EQUAL(sKey13.getKeyType(), KeyType::NONE);
  BOOST_CHECK_EQUAL(sKey13.getKeySize(), 0);

  // save key in base64-encoded pkcs8 format
  OBufferStream os14;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs8Base64(os14, password.data(), password.size()));
  checkPkcs8Base64Encoding(os14.buf(), password, sKeyPkcs1);

  OBufferStream os15;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs8Base64(os15, pwCallback));
  checkPkcs8Base64Encoding(os15.buf(), password, sKeyPkcs1);

  // save key in pkcs8 format
  OBufferStream os16;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs8(os16, password.data(), password.size()));
  checkPkcs8Encoding(os16.buf(), password, sKeyPkcs1);

  OBufferStream os17;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs8(os17, pwCallback));
  checkPkcs8Encoding(os17.buf(), password, sKeyPkcs1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DerivePublicKey, T, KeyTestDataSets)
{
  T dataSet;

  auto sKeyPkcs1Base64 = make_span(reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.data()),
                                   dataSet.privateKeyPkcs1.size());
  PrivateKey sKey;
  sKey.loadPkcs1Base64(sKeyPkcs1Base64);

  // derive public key and compare
  ConstBufferPtr pKeyBits = sKey.derivePublicKey();
  OBufferStream os;
  bufferSource(dataSet.publicKey) >> base64Decode() >> streamSink(os);
  BOOST_CHECK_EQUAL_COLLECTIONS(pKeyBits->begin(), pKeyBits->end(),
                                os.buf()->begin(), os.buf()->end());
}

BOOST_AUTO_TEST_CASE(RsaDecryption)
{
  RsaKey_Aes256Encrypted dataSet;

  PrivateKey sKey;
  sKey.loadPkcs1Base64({reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.data()),
                        dataSet.privateKeyPkcs1.size()});
  BOOST_CHECK_EQUAL(sKey.getKeyType(), KeyType::RSA);

  const uint8_t plaintext[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

  // Ciphertext generated with OpenSSL 3.0.0
  // printf ... | openssl pkeyutl -encrypt -inkey pub.pem -pubin -pkeyopt rsa_padding_mode:oaep | base64
  const std::string ciphertext =
R"BASE64(BgVqf6yKhMytCeL6+/oEoyx9rahh2+a1iU0D17RgfX6Q/3eNEcC2xY5QomjI7k/no8yiTZbXT9R4
REE6ic0DjFSbFwJjC/P0/oTSQyUNn4xNO+aK0MGnyeN34A8c3bZfLuo2DU496SQF0B9I5SS3gEA9
rqdIAdbWoOCKi1EX5SXtAOPTsj5PHHH+UM52bxOrUPvGSl9tRaz5S6hVf2haSoio4nnG3/bA6pdb
hHjyHHA/oi1PlFRIe0EO2/riAdqdIW3eOqZGP5t5QAMmiP178A9YQdbSavTgBSuE20T01U82Ln+L
pNk+PE/mmM20iNBo8C9cAJVUju6T4DCNZk7H5Q==
)BASE64";
  OBufferStream os;
  bufferSource(ciphertext) >> base64Decode() >> streamSink(os);

  auto decrypted = sKey.decrypt(*os.buf());
  BOOST_CHECK_EQUAL_COLLECTIONS(plaintext, plaintext + sizeof(plaintext),
                                decrypted->begin(), decrypted->end());
}

BOOST_AUTO_TEST_CASE(RsaEncryptDecrypt)
{
  RsaKey_Aes256Encrypted dataSet;

  PublicKey pKey;
  pKey.loadPkcs8Base64({reinterpret_cast<const uint8_t*>(dataSet.publicKey.data()),
                        dataSet.publicKey.size()});
  BOOST_CHECK_EQUAL(pKey.getKeyType(), KeyType::RSA);

  PrivateKey sKey;
  sKey.loadPkcs1Base64({reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.data()),
                        dataSet.privateKeyPkcs1.size()});
  BOOST_CHECK_EQUAL(sKey.getKeyType(), KeyType::RSA);

  const uint8_t plaintext[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

  auto ciphertext = pKey.encrypt(plaintext);
  auto decrypted = sKey.decrypt(*ciphertext);
  BOOST_CHECK_EQUAL_COLLECTIONS(plaintext, plaintext + sizeof(plaintext),
                                decrypted->begin(), decrypted->end());
}

BOOST_AUTO_TEST_CASE(UnsupportedDecryption)
{
  OBufferStream os;
  bufferSource("Y2lhbyFob2xhIWhlbGxvIQ==") >> base64Decode() >> streamSink(os);

  auto ecKey = generatePrivateKey(EcKeyParams());
  BOOST_CHECK_THROW(ecKey->decrypt(*os.buf()), PrivateKey::Error);

  auto hmacKey = generatePrivateKey(HmacKeyParams());
  BOOST_CHECK_THROW(hmacKey->decrypt(*os.buf()), PrivateKey::Error);
}

class RsaKeyGenParams
{
public:
  using Params = RsaKeyParams;
  using hasPublicKey = std::true_type;
  using canSavePkcs1 = std::true_type;

  static void
  checkPublicKey(const Buffer&)
  {
  }
};

class EcKeyGenParams
{
public:
  using Params = EcKeyParams;
  using hasPublicKey = std::true_type;
  using canSavePkcs1 = std::true_type;

  static void
  checkPublicKey(const Buffer& bits)
  {
    // EC key generation should use named curve format. See https://redmine.named-data.net/issues/5037
    // OBJECT IDENTIFIER 1.2.840.10045.3.1.7 prime256v1 (ANSI X9.62 named elliptic curve)
    const uint8_t oid[] = {0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07};
    BOOST_CHECK_MESSAGE(std::search(bits.begin(), bits.end(), oid, oid + sizeof(oid)) != bits.end(),
                        "OID not found in " << toHex(bits));
  }
};

class HmacKeyGenParams
{
public:
  using Params = HmacKeyParams;
  using hasPublicKey = std::false_type;
  using canSavePkcs1 = std::false_type;

  static void
  checkPublicKey(const Buffer&)
  {
  }
};

using KeyGenParams = boost::mpl::vector<
#if OPENSSL_VERSION_NUMBER < 0x30000000L // FIXME #5154
  HmacKeyGenParams,
#endif
  RsaKeyGenParams,
  EcKeyGenParams
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(GenerateKey, T, KeyGenParams)
{
  typename T::Params params;
  auto sKey = generatePrivateKey(params);
  BOOST_CHECK_EQUAL(sKey->getKeyType(), params.getKeyType());
  BOOST_CHECK_EQUAL(sKey->getKeySize(), params.getKeySize());

  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  OBufferStream os;
  BOOST_REQUIRE_NO_THROW(bufferSource(data) >>
                         signerFilter(DigestAlgorithm::SHA256, *sKey) >>
                         streamSink(os));
  auto sig = os.buf();

  bool result = false;
  if (typename T::hasPublicKey()) {
    auto pKeyBits = sKey->derivePublicKey();
    BOOST_REQUIRE(pKeyBits != nullptr);
    T::checkPublicKey(*pKeyBits);
    PublicKey pKey;
    pKey.loadPkcs8(*pKeyBits);
    BOOST_CHECK_NO_THROW(bufferSource(data) >>
                         verifierFilter(DigestAlgorithm::SHA256, pKey, *sig) >>
                         boolSink(result));
  }
  else {
    BOOST_CHECK_THROW(sKey->derivePublicKey(), PrivateKey::Error);
    BOOST_CHECK_NO_THROW(bufferSource(data) >>
                         verifierFilter(DigestAlgorithm::SHA256, *sKey, *sig) >>
                         boolSink(result));
  }
  BOOST_CHECK(result);

  if (typename T::canSavePkcs1()) {
    auto sKey2 = generatePrivateKey(params);

    OBufferStream os1;
    sKey->savePkcs1(os1);
    OBufferStream os2;
    sKey2->savePkcs1(os2);

    BOOST_CHECK(*os1.buf() != *os2.buf());
  }
  else {
    OBufferStream os1;
    BOOST_CHECK_THROW(sKey->savePkcs1(os1), PrivateKey::Error);
  }
}

BOOST_AUTO_TEST_CASE(GenerateKeyUnsupportedType)
{
  BOOST_CHECK_THROW(generatePrivateKey(AesKeyParams()), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // TestPrivateKey
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
