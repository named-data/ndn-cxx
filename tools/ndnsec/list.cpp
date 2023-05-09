/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "accumulator.hpp"
#include "ndnsec.hpp"
#include "util.hpp"

#include "ndn-cxx/util/indented-stream.hpp"

namespace ndn::ndnsec {
namespace {

// https://en.wikipedia.org/wiki/Box_Drawing
// https://git.altlinux.org/people/legion/packages/kbd.git?p=kbd.git;a=blob;f=data/consolefonts/README.eurlatgr
static const char GLYPH_HORIZONTAL[]         = "\u2500 ";      // "─ "
static const char GLYPH_VERTICAL[]           = "\u2502 ";      // "│ "
static const char GLYPH_VERTICAL_AND_RIGHT[] = "\u251c\u2500"; // "├─"
static const char GLYPH_UP_AND_RIGHT[]       = "\u2514\u2500"; // "└─"
static const char GLYPH_SPACE[]              = "  ";
static const char GLYPH_STAR[]               = "* ";

enum class Verbosity {
  IDENTITY = 0,
  KEY = 1,
  CERT_NAME = 2,
  CERT_FULL = 3,
};

class TreePrinter
{
public:
  explicit
  TreePrinter(std::ostream& output)
    : m_out(output)
  {
  }

  template<typename C, typename F>
  void
  forEachChild(const C& container, F fn) const
  {
    m_branches.push_back(false);

    auto end = container.end();
    for (auto it = container.begin(); it != end; ++it) {
      bool isLast = std::next(it) == end;
      if (isLast) {
        m_branches.back() = true;
      }
      printBranches(m_out);
      fn(*it);
    }

    m_branches.pop_back();
  }

  std::string
  getIndent() const
  {
    std::ostringstream oss;
    printBranches(oss, true);
    return oss.str();
  }

private:
  void
  printBranches(std::ostream& os, bool ignoreLast = false) const
  {
    for (size_t i = 0; i < m_branches.size(); ++i) {
      if (i == m_branches.size() - 1 && !ignoreLast) {
        os << (m_branches[i] ? GLYPH_UP_AND_RIGHT : GLYPH_VERTICAL_AND_RIGHT);
      }
      else {
        os << (m_branches[i] ? GLYPH_SPACE : GLYPH_VERTICAL) << GLYPH_SPACE;
      }
    }
  }

protected:
  std::ostream& m_out;

private:
  mutable std::vector<bool> m_branches;
};

class Printer : private TreePrinter
{
public:
  explicit
  Printer(std::ostream& output, Verbosity verbosity)
    : TreePrinter(output)
    , m_verbosity(verbosity)
  {
  }

  void
  printPib(const security::Pib& pib) const
  {
    m_out << pib.getPibLocator() << std::endl;

    security::Identity defaultIdentity;
    try {
      defaultIdentity = pib.getDefaultIdentity();
    }
    catch (const security::Pib::Error&) {
      // no default identity
    }

    forEachChild(pib.getIdentities(), [&] (const auto& identity) {
      printIdentity(identity, identity == defaultIdentity);
    });
  }

  void
  printIdentity(const security::Identity& identity, bool isDefault) const
  {
    printDefault(isDefault);
    m_out << identity.getName() << std::endl;

    if (m_verbosity >= Verbosity::KEY) {
      security::Key defaultKey;
      try {
        defaultKey = identity.getDefaultKey();
      }
      catch (const security::Pib::Error&) {
        // no default key
      }

      forEachChild(identity.getKeys(), [&] (const auto& key) {
        printKey(key, key == defaultKey);
      });
    }
  }

  void
  printKey(const security::Key& key, bool isDefault) const
  {
    printDefault(isDefault);
    m_out << key.getName() << std::endl;

    if (m_verbosity >= Verbosity::CERT_NAME) {
      security::Certificate defaultCert;
      try {
        defaultCert = key.getDefaultCertificate();
      }
      catch (const security::Pib::Error&) {
        // no default certificate
      }

      forEachChild(key.getCertificates(), [&] (const auto& cert) {
        printCertificate(cert, cert == defaultCert);
      });
    }
  }

  void
  printCertificate(const security::Certificate& cert, bool isDefault) const
  {
    printDefault(isDefault);
    m_out << cert.getName() << std::endl;

    if (m_verbosity >= Verbosity::CERT_FULL) {
      util::IndentedStream os(m_out, getIndent() + GLYPH_SPACE);
      os << cert;
    }
  }

private:
  void
  printDefault(bool isDefault) const
  {
    m_out << (isDefault ? GLYPH_STAR : GLYPH_HORIZONTAL);
  }

private:
  Verbosity m_verbosity;
};

} // namespace

int
ndnsec_list(int argc, char** argv)
{
  namespace po = boost::program_options;

  bool wantKey = false;
  bool wantCert = false;
  auto verboseLevel = to_underlying(Verbosity::IDENTITY);

  po::options_description description(
    "Usage: ndnsec list [-h] [-k] [-c] [-v]\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("key,k",     po::bool_switch(&wantKey), "list all keys associated with each identity")
    ("cert,c",    po::bool_switch(&wantCert), "list all certificates associated with each key")
    ("verbose,v", accumulator<std::underlying_type_t<Verbosity>>(&verboseLevel),
                  "verbose mode, can be repeated for increased verbosity: -v is equivalent to -k, "
                  "-vv is equivalent to -c, -vvv shows detailed information for each certificate")
    ;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n\n"
              << description << std::endl;
    return 2;
  }

  if (vm.count("help") > 0) {
    std::cout << description << std::endl;
    return 0;
  }

  KeyChain keyChain;

  auto verbosity = std::max(static_cast<Verbosity>(verboseLevel),
                            wantCert ? Verbosity::CERT_NAME :
                                       wantKey ? Verbosity::KEY :
                                                 Verbosity::IDENTITY);
  Printer printer(std::cout, verbosity);
  printer.printPib(keyChain.getPib());

  return 0;
}

} // namespace ndn::ndnsec
