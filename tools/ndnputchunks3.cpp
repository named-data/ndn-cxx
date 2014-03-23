/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
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
 * Author: Wentao Shang <wentao@cs.ucla.edu>
 */

#include "face.hpp"
#include "security/key-chain.hpp"

#define MAX_SEG_SIZE 4096

class Producer
{
public:
  Producer(const char* name)
    : m_name(name)
    , m_verbose(false)
  {
    int segnum = 0;
    char* buf = new char[MAX_SEG_SIZE];
    do
      {
        std::cin.read(buf, MAX_SEG_SIZE);
        int got = std::cin.gcount ();

        if (got > 0)
          {
            ndn::shared_ptr<ndn::Data> data =
              ndn::make_shared<ndn::Data>(ndn::Name(m_name).appendSegment(segnum));

            data->setFreshnessPeriod(ndn::time::milliseconds(10000)); // 10 sec
            data->setContent(reinterpret_cast<const uint8_t*>(buf), got);

            m_keychain.sign(*data);
            m_store.push_back(data);
            segnum++;
          }
      }
    while (static_cast<bool>(std::cin));

    if (m_verbose)
      std::cerr << "Created " << segnum << " chunks for prefix [" << m_name << "]" << std::endl;
  }

  void
  onInterest(const ndn::Name& name, const ndn::Interest& interest)
  {
    if (m_verbose)
      std::cerr << "<< I: " << interest << std::endl;

    size_t segnum = static_cast<size_t>(interest.getName().rbegin()->toSegment());

    if (segnum < m_store.size())
      {
        m_face.put(*m_store[segnum]);
      }
  }

  void
  onRegisterFailed(const ndn::Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix in local hub's daemon (" << reason << ")" << std::endl;
    m_face.shutdown();
  }

  void
  run()
  {
    if (m_store.empty())
      {
        std::cerr << "Nothing to serve. Exiting." << std::endl;
        return;
      }

    m_face.setInterestFilter(m_name,
                             ndn::bind(&Producer::onInterest, this, _1, _2),
                             ndn::bind(&Producer::onRegisterFailed, this, _1, _2));
    m_face.processEvents();
  }

private:
  ndn::Name m_name;
  ndn::Face m_face;
  ndn::KeyChain m_keychain;

  std::vector< ndn::shared_ptr<ndn::Data> > m_store;

  bool m_verbose;
};

int
main(int argc, char *argv[])
{
  if (argc < 2)
    {
      std::cerr << "Usage: ./ndnputchunks [data_prefix]\n";
      return -1;
    }

  try
    {
      ndn::time::steady_clock::TimePoint startTime = ndn::time::steady_clock::now();

      std::cerr << "Preparing the input..." << std::endl;
      Producer producer(argv[1]);
      std::cerr << "Ready... (took " << (ndn::time::steady_clock::now() - startTime) << std::endl;

      while (true)
        {
          try
            {
              producer.run(); // this will exit when daemon dies... so try to connect again if possible
            }
          catch (std::exception& e)
            {
              std::cerr << "ERROR: " << e.what() << std::endl;
              // and keep going
              sleep(1);
            }
        }
    }
  catch (std::exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
    }
  return 0;
}
