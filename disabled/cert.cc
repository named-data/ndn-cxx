/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */
#include "cert.h"

#include <tinyxml.h>
#include <boost/lexical_cast.hpp>

#include "logging.h"

INIT_LOGGER ("ndn.Cert");

using namespace std;

namespace ndn {

Cert::Cert()
    : m_pkey(0)
    , m_meta("", "",  0, 0)
{
}

Cert::Cert(const PcoPtr &keyObject, const PcoPtr &metaObject = PcoPtr())
    : m_pkey(0)
    , m_meta("", "", 0, 0)
{
  m_name = keyObject->name();
  m_rawKeyBytes = keyObject->content();
  m_keyHash = *(Hash::FromBytes(m_rawKeyBytes));
  m_pkey = ccn_d2i_pubkey(head(m_rawKeyBytes), m_rawKeyBytes.size());
  updateMeta(metaObject);
}

Cert::~Cert()
{
  if (m_pkey != 0)
  {
    ccn_pubkey_free(m_pkey);
    m_pkey = 0;
  }
}

void
Cert::updateMeta(const PcoPtr &metaObject)
{
  if (metaObject)
  {
    Bytes xml = metaObject->content();
    // just make sure it's null terminated as it's required by TiXmlDocument::parse
    xml.push_back('\0');
    TiXmlDocument doc;
    doc.Parse((const char *)(head(xml)));
    if (!doc.Error())
    {
      TiXmlElement *root = doc.RootElement();
      for (TiXmlElement *child = root->FirstChildElement(); child; child = child->NextSiblingElement())
      {
        string elemName = child->Value();
        string text = child->GetText();
        if (elemName == "Name")
        {
          m_meta.realworldID = text;
          _LOG_TRACE("Name = " << text);
        }
        else if (elemName == "Affiliation")
        {
          m_meta.affiliation = text;
          _LOG_TRACE("Affiliation = " << text);
        }
        else if (elemName == "Valid_to")
        {
          m_meta.validTo = boost::lexical_cast<time_t>(text);
          _LOG_TRACE("Valid_to = " << text);
        }
        else if (elemName == "Valid_from")
        {
          // this is not included in the key meta yet
          // but it should eventually be there
        }
        else
        {
          // ignore known stuff
        }
      }
    }
    else
    {
      _LOG_ERROR("Cannot parse meta info:" << std::string((const char *)head(xml), xml.size()));
    }
  }
}

Cert::VALIDITY
Cert::validity()
{
  if (m_meta.validFrom == 0 && m_meta.validTo == 0)
  {
    return OTHER;
  }

  time_t now = time(NULL);
  if (now < m_meta.validFrom)
  {
    return NOT_YET_VALID;
  }

  if (now >= m_meta.validTo)
  {
    return EXPIRED;
  }

  return WITHIN_VALID_TIME_SPAN;
}

} // ndn
