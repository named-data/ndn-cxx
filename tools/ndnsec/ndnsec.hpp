/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_TOOLS_NDNSEC_NDNSEC_HPP
#define NDN_TOOLS_NDNSEC_NDNSEC_HPP

namespace ndn {
namespace ndnsec {

// TODO convert to tool registry (may be)

int
ndnsec_list(int argc, char** argv);

int
ndnsec_get_default(int argc, char** argv);

int
ndnsec_set_default(int argc, char** argv);

int
ndnsec_key_gen(int argc, char** argv);

int
ndnsec_sign_req(int argc, char** argv);

int
ndnsec_cert_gen(int argc, char** argv);

int
ndnsec_cert_dump(int argc, char** argv);

int
ndnsec_cert_install(int argc, char** argv);

int
ndnsec_delete(int argc, char** argv);

int
ndnsec_export(int argc, char** argv);

int
ndnsec_import(int argc, char** argv);

int
ndnsec_unlock_tpm(int argc, char** argv);

} // namespace ndnsec
} // namespace ndn

#endif // NDN_TOOLS_NDNSEC_NDNSEC_HPP
