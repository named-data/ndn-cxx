/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "key-locator.h"
#include "ndn-cpp/error.h"

namespace ndn {

KeyLocator::KeyLocator ()
  : m_type (NOTSET)
  , m_data (0)
{
}

KeyLocator::KeyLocator (const KeyLocator &keyLocator)
  : m_type (keyLocator.getType ())
  , m_data (0)
{
  switch (m_type)
    {
    case NOTSET:
      break;
    case KEY:
      m_data = new Blob (keyLocator.getKey ());
      break;
    case CERTIFICATE:
      m_data = new Blob (keyLocator.getCertificate ());
      break;
    case KEYNAME:
      m_data = new Name (keyLocator.getKeyName ());
      break;
    }
}

KeyLocator::~KeyLocator ()
{
  deleteData ();
}

KeyLocator &
KeyLocator::operator = (const KeyLocator &keyLocator)
{
  if (this == &keyLocator)
    return *this;

  deleteData ();
  m_type = keyLocator.getType ();
  
  switch (m_type)
    {
    case NOTSET:
      break;
    case KEY:
      m_data = new Blob (keyLocator.getKey ());
      break;
    case CERTIFICATE:
      m_data = new Blob (keyLocator.getCertificate ());
      break;
    case KEYNAME:
      m_data = new Name (keyLocator.getKeyName ());
      break;
    }

  return *this;
}

void
KeyLocator::deleteData ()
{
  switch (m_type)
    {
    case NOTSET: // nothing to clean up
      break;
    case KEY:
      delete reinterpret_cast<Blob*> (m_data);
      break;
    case CERTIFICATE:
      delete reinterpret_cast<Blob*> (m_data);
      break;
    case KEYNAME:
      delete reinterpret_cast<Name*> (m_data);
      break;
    }
}

void
KeyLocator::setType (KeyLocator::Type type)
{
  if (m_type == type)
    return;

  deleteData ();
  m_type = type;
  
  switch (m_type)
    {
    case NOTSET:
      m_data = 0;
    case KEY:
      m_data = new Blob;
      break;
    case CERTIFICATE:
      m_data = new Blob;
      break;
    case KEYNAME:
      m_data = new Name;
      break;
    }
}

const Blob &
KeyLocator::getKey () const
{
  if (m_type != KEY)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("getKey called, but KeyLocator is not of type KeyLocator::KEY"));
  return *reinterpret_cast<const Blob*> (m_data);
}

Blob &
KeyLocator::getKey ()
{
  if (m_type != KEY)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("getKey called, but KeyLocator is not of type KeyLocator::KEY"));
  return *reinterpret_cast<Blob*> (m_data);
}

void
KeyLocator::setKey (const Blob &key)
{
  if (m_type != KEY)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("setKey called, but KeyLocator is not of type KeyLocator::KEY"));
  *reinterpret_cast<Blob*> (m_data) = key;
}

const Blob &
KeyLocator::getCertificate () const
{
  if (m_type != CERTIFICATE)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("getCertificate called, but KeyLocator is not of type KeyLocator::CERTIFICATE"));
  return *reinterpret_cast<const Blob*> (m_data);
}

Blob &
KeyLocator::getCertificate ()
{
  if (m_type != CERTIFICATE)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("getCertificate called, but KeyLocator is not of type KeyLocator::CERTIFICATE"));
  return *reinterpret_cast<Blob*> (m_data);
}

void
KeyLocator::setCertificate (const Blob &certificate)
{
  if (m_type != CERTIFICATE)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("setCertificate called, but KeyLocator is not of type KeyLocator::CERTIFICATE"));
  *reinterpret_cast<Blob*> (m_data) = certificate;
}

const Name &
KeyLocator::getKeyName () const
{
  if (m_type != KEYNAME)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("getKeyName called, but KeyLocator is not of type KeyLocator::KEYNAME"));
  return *reinterpret_cast<const Name*> (m_data);
}

Name &
KeyLocator::getKeyName ()
{
  if (m_type != KEYNAME)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("getKeyName called, but KeyLocator is not of type KeyLocator::KEYNAME"));
  return *reinterpret_cast<Name*> (m_data);
}


void
KeyLocator::setKeyName (const Name &name)
{
  if (m_type != KEYNAME)
    BOOST_THROW_EXCEPTION (error::KeyLocator ()
                           << error::msg ("setKeyName called, but KeyLocator is not of type KeyLocator::KEYNAME"));
  *reinterpret_cast<Name*> (m_data) = name;
}

}
