/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology,
 *                         The University of Memphis.
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

#include "face-uri.hpp"

#include "address-converter.hpp"
#include "dns.hpp"
#include "util/string-helper.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/regex.hpp>
#include <set>
#include <sstream>

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<FaceUri>));

FaceUri::FaceUri()
  : m_isV6(false)
{
}

FaceUri::FaceUri(const std::string& uri)
{
  if (!parse(uri)) {
    BOOST_THROW_EXCEPTION(Error("Malformed URI: " + uri));
  }
}

FaceUri::FaceUri(const char* uri)
  : FaceUri(std::string(uri))
{
}

bool
FaceUri::parse(const std::string& uri)
{
  m_scheme.clear();
  m_host.clear();
  m_port.clear();
  m_path.clear();
  m_isV6 = false;

  static const boost::regex protocolExp("(\\w+\\d?(\\+\\w+)?)://([^/]*)(\\/[^?]*)?");
  boost::smatch protocolMatch;
  if (!boost::regex_match(uri, protocolMatch, protocolExp)) {
    return false;
  }
  m_scheme = protocolMatch[1];
  const std::string& authority = protocolMatch[3];
  m_path = protocolMatch[4];

  // pattern for IPv6 link local address enclosed in [ ], with optional port number
  static const boost::regex v6LinkLocalExp("^\\[([a-fA-F0-9:]+)%([^\\s/:]+)\\](?:\\:(\\d+))?$");
  // pattern for IPv6 address enclosed in [ ], with optional port number
  static const boost::regex v6Exp("^\\[([a-fA-F0-9:]+)\\](?:\\:(\\d+))?$");
  // pattern for Ethernet address in standard hex-digits-and-colons notation
  static const boost::regex etherExp("^\\[((?:[a-fA-F0-9]{1,2}\\:){5}(?:[a-fA-F0-9]{1,2}))\\]$");
  // pattern for IPv4-mapped IPv6 address, with optional port number
  static const boost::regex v4MappedV6Exp("^\\[::ffff:(\\d+(?:\\.\\d+){3})\\](?:\\:(\\d+))?$");
  // pattern for IPv4/hostname/fd/ifname, with optional port number
  static const boost::regex v4HostExp("^([^:]+)(?:\\:(\\d+))?$");

  if (authority.empty()) {
    // UNIX, internal
  }
  else {
    boost::smatch match;
    if (boost::regex_match(authority, match, v6LinkLocalExp)) {
      m_isV6 = true;
      m_host = match[1] + "%" + match[2];
      m_port = match[3];
      return true;
    }

    m_isV6 = boost::regex_match(authority, match, v6Exp);
    if (m_isV6 ||
        boost::regex_match(authority, match, etherExp) ||
        boost::regex_match(authority, match, v4MappedV6Exp) ||
        boost::regex_match(authority, match, v4HostExp)) {
      m_host = match[1];
      m_port = match[2];
    }
    else {
      return false;
    }
  }

  return true;
}

FaceUri::FaceUri(const boost::asio::ip::udp::endpoint& endpoint)
{
  m_isV6 = endpoint.address().is_v6();
  m_scheme = m_isV6 ? "udp6" : "udp4";
  m_host = endpoint.address().to_string();
  m_port = to_string(endpoint.port());
}

FaceUri::FaceUri(const boost::asio::ip::tcp::endpoint& endpoint)
{
  m_isV6 = endpoint.address().is_v6();
  m_scheme = m_isV6 ? "tcp6" : "tcp4";
  m_host = endpoint.address().to_string();
  m_port = to_string(endpoint.port());
}

FaceUri::FaceUri(const boost::asio::ip::tcp::endpoint& endpoint, const std::string& scheme)
{
  m_isV6 = endpoint.address().is_v6();
  m_scheme = scheme;
  m_host = endpoint.address().to_string();
  m_port = to_string(endpoint.port());
}

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
FaceUri::FaceUri(const boost::asio::local::stream_protocol::endpoint& endpoint)
  : m_scheme("unix")
  , m_path(endpoint.path())
  , m_isV6(false)
{
}
#endif // BOOST_ASIO_HAS_LOCAL_SOCKETS

FaceUri
FaceUri::fromFd(int fd)
{
  FaceUri uri;
  uri.m_scheme = "fd";
  uri.m_host = to_string(fd);
  return uri;
}

FaceUri::FaceUri(const ethernet::Address& address)
  : m_scheme("ether")
  , m_host(address.toString())
  , m_isV6(true)
{
}

FaceUri
FaceUri::fromDev(const std::string& ifname)
{
  FaceUri uri;
  uri.m_scheme = "dev";
  uri.m_host = ifname;
  return uri;
}

FaceUri
FaceUri::fromUdpDev(const boost::asio::ip::udp::endpoint& endpoint, const std::string& ifname)
{
  FaceUri uri;
  uri.m_scheme = endpoint.address().is_v6() ? "udp6+dev" : "udp4+dev";
  uri.m_host = ifname;
  uri.m_port = to_string(endpoint.port());
  return uri;
}

bool
FaceUri::operator==(const FaceUri& rhs) const
{
  return m_isV6 == rhs.m_isV6 &&
         m_scheme == rhs.m_scheme &&
         m_host == rhs.m_host &&
         m_port == rhs.m_port &&
         m_path == rhs.m_path;
}

bool
FaceUri::operator!=(const FaceUri& rhs) const
{
  return !(*this == rhs);
}

std::string
FaceUri::toString() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

std::ostream&
operator<<(std::ostream& os, const FaceUri& uri)
{
  os << uri.m_scheme << "://";
  if (uri.m_isV6) {
    os << "[" << uri.m_host << "]";
  }
  else {
    os << uri.m_host;
  }
  if (!uri.m_port.empty()) {
    os << ":" << uri.m_port;
  }
  os << uri.m_path;
  return os;
}


/** \brief a CanonizeProvider provides FaceUri canonization functionality for a group of schemes
 */
class CanonizeProvider : noncopyable
{
public:
  virtual
  ~CanonizeProvider() = default;

  virtual std::set<std::string>
  getSchemes() const = 0;

  virtual bool
  isCanonical(const FaceUri& faceUri) const = 0;

  virtual void
  canonize(const FaceUri& faceUri,
           const FaceUri::CanonizeSuccessCallback& onSuccess,
           const FaceUri::CanonizeFailureCallback& onFailure,
           boost::asio::io_service& io, time::nanoseconds timeout) const = 0;
};

template<typename Protocol>
class IpHostCanonizeProvider : public CanonizeProvider
{
public:
  std::set<std::string>
  getSchemes() const override
  {
    return {m_baseScheme, m_v4Scheme, m_v6Scheme};
  }

  bool
  isCanonical(const FaceUri& faceUri) const override
  {
    if (faceUri.getPort().empty()) {
      return false;
    }
    if (!faceUri.getPath().empty()) {
      return false;
    }

    boost::system::error_code ec;
    auto addr = ip::addressFromString(unescapeHost(faceUri.getHost()), ec);
    if (ec) {
      return false;
    }

    bool hasCorrectScheme = (faceUri.getScheme() == m_v4Scheme && addr.is_v4()) ||
                            (faceUri.getScheme() == m_v6Scheme && addr.is_v6());
    if (!hasCorrectScheme) {
      return false;
    }

    auto checkAddressWithUri = [] (const boost::asio::ip::address& addr,
                                   const FaceUri& faceUri) -> bool {
      if (addr.is_v4() || !addr.to_v6().is_link_local()) {
        return addr.to_string() == faceUri.getHost();
      }

      std::vector<std::string> addrFields, faceUriFields;
      std::string addrString = addr.to_string();
      std::string faceUriString = faceUri.getHost();

      boost::algorithm::split(addrFields, addrString, boost::is_any_of("%"));
      boost::algorithm::split(faceUriFields, faceUriString, boost::is_any_of("%"));
      if (addrFields.size() != 2 || faceUriFields.size() != 2) {
        return false;
      }

      if (faceUriFields[1].size() > 2 && faceUriFields[1].compare(0, 2, "25") == 0) {
        // %25... is accepted, but not a canonical form
        return false;
      }

      return addrFields[0] == faceUriFields[0] &&
             addrFields[1] == faceUriFields[1];
    };

    return checkAddressWithUri(addr, faceUri) && checkAddress(addr).first;
  }

  void
  canonize(const FaceUri& faceUri,
           const FaceUri::CanonizeSuccessCallback& onSuccess,
           const FaceUri::CanonizeFailureCallback& onFailure,
           boost::asio::io_service& io, time::nanoseconds timeout) const override
  {
    if (this->isCanonical(faceUri)) {
      onSuccess(faceUri);
      return;
    }

    // make a copy because caller may modify faceUri
    auto uri = make_shared<FaceUri>(faceUri);
    boost::system::error_code ec;
    auto ipAddress = ip::addressFromString(unescapeHost(faceUri.getHost()), ec);
    if (!ec) {
      // No need to resolve IP address if host is already an IP
      if ((faceUri.getScheme() == m_v4Scheme && !ipAddress.is_v4()) ||
          (faceUri.getScheme() == m_v6Scheme && !ipAddress.is_v6())) {
        return onFailure("IPv4/v6 mismatch");
      }

      onDnsSuccess(uri, onSuccess, onFailure, ipAddress);
    }
    else {
      dns::AddressSelector addressSelector;
      if (faceUri.getScheme() == m_v4Scheme) {
        addressSelector = dns::Ipv4Only();
      }
      else if (faceUri.getScheme() == m_v6Scheme) {
        addressSelector = dns::Ipv6Only();
      }
      else {
        BOOST_ASSERT(faceUri.getScheme() == m_baseScheme);
        addressSelector = dns::AnyAddress();
      }

      dns::asyncResolve(unescapeHost(faceUri.getHost()),
        bind(&IpHostCanonizeProvider<Protocol>::onDnsSuccess, this, uri, onSuccess, onFailure, _1),
        bind(&IpHostCanonizeProvider<Protocol>::onDnsFailure, this, uri, onFailure, _1),
        io, addressSelector, timeout);
    }
  }

protected:
  explicit
  IpHostCanonizeProvider(const std::string& baseScheme,
                         uint16_t defaultUnicastPort = 6363,
                         uint16_t defaultMulticastPort = 56363)
    : m_baseScheme(baseScheme)
    , m_v4Scheme(baseScheme + '4')
    , m_v6Scheme(baseScheme + '6')
    , m_defaultUnicastPort(defaultUnicastPort)
    , m_defaultMulticastPort(defaultMulticastPort)
  {
  }

private:
  void
  onDnsSuccess(const shared_ptr<FaceUri>& faceUri,
               const FaceUri::CanonizeSuccessCallback& onSuccess,
               const FaceUri::CanonizeFailureCallback& onFailure,
               const dns::IpAddress& ipAddress) const
  {
    bool isOk = false;
    std::string reason;
    std::tie(isOk, reason) = this->checkAddress(ipAddress);
    if (!isOk) {
      return onFailure(reason);
    }

    uint16_t port = 0;
    if (faceUri->getPort().empty()) {
      port = ipAddress.is_multicast() ? m_defaultMulticastPort : m_defaultUnicastPort;
    }
    else {
      try {
        port = boost::lexical_cast<uint16_t>(faceUri->getPort());
      }
      catch (const boost::bad_lexical_cast&) {
        return onFailure("invalid port number '" + faceUri->getPort() + "'");
      }
    }

    FaceUri canonicalUri(typename Protocol::endpoint(ipAddress, port));
    BOOST_ASSERT(canonicalUri.isCanonical());
    onSuccess(canonicalUri);
  }

  void
  onDnsFailure(const shared_ptr<FaceUri>& faceUri,
               const FaceUri::CanonizeFailureCallback& onFailure,
               const std::string& reason) const
  {
    onFailure(reason);
  }

  /** \brief when overriden in a subclass, check the IP address is allowable
   *  \return (true,ignored) if the address is allowable;
   *          (false,reason) if the address is not allowable.
   */
  virtual std::pair<bool, std::string>
  checkAddress(const dns::IpAddress& ipAddress) const
  {
    return {true, ""};
  }

  static std::string
  unescapeHost(std::string host)
  {
    auto escapePos = host.find("%25");
    if (escapePos != std::string::npos && escapePos < host.size() - 3) {
      host = unescape(host);
    }
    return host;
  }

private:
  std::string m_baseScheme;
  std::string m_v4Scheme;
  std::string m_v6Scheme;
  uint16_t m_defaultUnicastPort;
  uint16_t m_defaultMulticastPort;
};

class UdpCanonizeProvider : public IpHostCanonizeProvider<boost::asio::ip::udp>
{
public:
  UdpCanonizeProvider()
    : IpHostCanonizeProvider("udp")
  {
  }
};

class TcpCanonizeProvider : public IpHostCanonizeProvider<boost::asio::ip::tcp>
{
public:
  TcpCanonizeProvider()
    : IpHostCanonizeProvider("tcp")
  {
  }

protected:
  std::pair<bool, std::string>
  checkAddress(const dns::IpAddress& ipAddress) const override
  {
    if (ipAddress.is_multicast()) {
      return {false, "cannot use multicast address"};
    }
    return {true, ""};
  }
};

class EtherCanonizeProvider : public CanonizeProvider
{
public:
  std::set<std::string>
  getSchemes() const override
  {
    return {"ether"};
  }

  bool
  isCanonical(const FaceUri& faceUri) const override
  {
    if (!faceUri.getPort().empty()) {
      return false;
    }
    if (!faceUri.getPath().empty()) {
      return false;
    }

    auto addr = ethernet::Address::fromString(faceUri.getHost());
    return addr.toString() == faceUri.getHost();
  }

  void
  canonize(const FaceUri& faceUri,
           const FaceUri::CanonizeSuccessCallback& onSuccess,
           const FaceUri::CanonizeFailureCallback& onFailure,
           boost::asio::io_service& io, time::nanoseconds timeout) const override
  {
    auto addr = ethernet::Address::fromString(faceUri.getHost());
    if (addr.isNull()) {
      return onFailure("invalid ethernet address '" + faceUri.getHost() + "'");
    }

    FaceUri canonicalUri(addr);
    BOOST_ASSERT(canonicalUri.isCanonical());
    onSuccess(canonicalUri);
  }
};

class DevCanonizeProvider : public CanonizeProvider
{
public:
  std::set<std::string>
  getSchemes() const override
  {
    return {"dev"};
  }

  bool
  isCanonical(const FaceUri& faceUri) const override
  {
    return !faceUri.getHost().empty() && faceUri.getPort().empty() && faceUri.getPath().empty();
  }

  void
  canonize(const FaceUri& faceUri,
           const FaceUri::CanonizeSuccessCallback& onSuccess,
           const FaceUri::CanonizeFailureCallback& onFailure,
           boost::asio::io_service& io, time::nanoseconds timeout) const override
  {
    if (faceUri.getHost().empty()) {
      onFailure("network interface name is missing");
      return;
    }
    if (!faceUri.getPort().empty()) {
      onFailure("port number is not allowed");
      return;
    }
    if (!faceUri.getPath().empty() && faceUri.getPath() != "/") { // permit trailing slash only
      onFailure("path is not allowed");
      return;
    }

    FaceUri canonicalUri = FaceUri::fromDev(faceUri.getHost());
    BOOST_ASSERT(canonicalUri.isCanonical());
    onSuccess(canonicalUri);
  }
};

class UdpDevCanonizeProvider : public CanonizeProvider
{
public:
  std::set<std::string>
  getSchemes() const override
  {
    return {"udp4+dev", "udp6+dev"};
  }

  bool
  isCanonical(const FaceUri& faceUri) const override
  {
    if (faceUri.getPort().empty()) {
      return false;
    }
    if (!faceUri.getPath().empty()) {
      return false;
    }
    return true;
  }

  void
  canonize(const FaceUri& faceUri,
           const FaceUri::CanonizeSuccessCallback& onSuccess,
           const FaceUri::CanonizeFailureCallback& onFailure,
           boost::asio::io_service& io, time::nanoseconds timeout) const override
  {
    if (this->isCanonical(faceUri)) {
      onSuccess(faceUri);
    }
    else {
      onFailure("cannot canonize " + faceUri.toString());
    }
  }
};

using CanonizeProviders = boost::mpl::vector<UdpCanonizeProvider*,
                                             TcpCanonizeProvider*,
                                             EtherCanonizeProvider*,
                                             DevCanonizeProvider*,
                                             UdpDevCanonizeProvider*>;
using CanonizeProviderTable = std::map<std::string, shared_ptr<CanonizeProvider>>;

class CanonizeProviderTableInitializer
{
public:
  explicit
  CanonizeProviderTableInitializer(CanonizeProviderTable& providerTable)
    : m_providerTable(providerTable)
  {
  }

  template<typename CP>
  void
  operator()(CP*)
  {
    shared_ptr<CanonizeProvider> cp = make_shared<CP>();
    auto schemes = cp->getSchemes();
    BOOST_ASSERT(!schemes.empty());

    for (const auto& scheme : schemes) {
      BOOST_ASSERT(m_providerTable.count(scheme) == 0);
      m_providerTable[scheme] = cp;
    }
  }

private:
  CanonizeProviderTable& m_providerTable;
};

static const CanonizeProvider*
getCanonizeProvider(const std::string& scheme)
{
  static CanonizeProviderTable providerTable;
  if (providerTable.empty()) {
    boost::mpl::for_each<CanonizeProviders>(CanonizeProviderTableInitializer(providerTable));
    BOOST_ASSERT(!providerTable.empty());
  }

  auto it = providerTable.find(scheme);
  return it == providerTable.end() ? nullptr : it->second.get();
}


bool
FaceUri::canCanonize(const std::string& scheme)
{
  return getCanonizeProvider(scheme) != nullptr;
}

bool
FaceUri::isCanonical() const
{
  const CanonizeProvider* cp = getCanonizeProvider(this->getScheme());
  if (cp == nullptr) {
    return false;
  }

  return cp->isCanonical(*this);
}

void
FaceUri::canonize(const CanonizeSuccessCallback& onSuccess,
                  const CanonizeFailureCallback& onFailure,
                  boost::asio::io_service& io, time::nanoseconds timeout) const
{
  const CanonizeProvider* cp = getCanonizeProvider(this->getScheme());
  if (cp == nullptr) {
    if (onFailure) {
      onFailure("scheme not supported");
    }
    return;
  }

  static CanonizeSuccessCallback successNop = bind([]{});
  static CanonizeFailureCallback failureNop = bind([]{});
  cp->canonize(*this,
               onSuccess ? onSuccess : successNop,
               onFailure ? onFailure : failureNop,
               io, timeout);
}

} // namespace ndn
