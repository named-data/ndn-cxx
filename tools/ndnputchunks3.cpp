/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Wentao Shang <http://irl.cs.ucla.edu/~wentao/>
 */

#include "face.hpp"
#include "security/key-chain.hpp"

namespace ndn {

const size_t MAX_SEG_SIZE = 4096;

class Producer
{
public:
  Producer(const char* name)
    : m_name(name)
    , m_isVerbose(false)
  {
    int segnum = 0;
    char* buf = new char[MAX_SEG_SIZE];
    do
      {
        std::cin.read(buf, MAX_SEG_SIZE);
        int got = std::cin.gcount();

        if (got > 0)
          {
            shared_ptr<Data> data =
              make_shared<Data>(Name(m_name).appendSegment(segnum));

            data->setFreshnessPeriod(time::milliseconds(10000)); // 10 sec
            data->setContent(reinterpret_cast<const uint8_t*>(buf), got);

            m_keychain.sign(*data);
            m_store.push_back(data);
            segnum++;
          }
      }
    while (static_cast<bool>(std::cin));

    if (m_isVerbose)
      std::cerr << "Created " << segnum << " chunks for prefix [" << m_name << "]" << std::endl;
  }

  void
  onInterest(const Name& name, const Interest& interest)
  {
    if (m_isVerbose)
      std::cerr << "<< I: " << interest << std::endl;

    size_t segnum = static_cast<size_t>(interest.getName().rbegin()->toSegment());

    if (segnum < m_store.size())
      {
        m_face.put(*m_store[segnum]);
      }
  }

  void
  onRegisterFailed(const Name& prefix, const std::string& reason)
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
                             bind(&Producer::onInterest, this, _1, _2),
                             RegisterPrefixSuccessCallback(),
                             bind(&Producer::onRegisterFailed, this, _1, _2));
    m_face.processEvents();
  }

private:
  Name m_name;
  Face m_face;
  KeyChain m_keychain;

  std::vector< shared_ptr<Data> > m_store;

  bool m_isVerbose;
};

int
main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cerr << "Usage: ./ndnputchunks [data_prefix]\n";
      return -1;
    }

  try
    {
      time::steady_clock::TimePoint startTime = time::steady_clock::now();

      std::cerr << "Preparing the input..." << std::endl;
      Producer producer(argv[1]);
      std::cerr << "Ready... (took " << (time::steady_clock::now() - startTime) << std::endl;

      while (true)
        {
          try
            {
              // this will exit when daemon dies... so try to connect again if possible
              producer.run();
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

} // namespace ndn

int
main(int argc, char** argv)
{
  return ndn::main(argc, argv);
}
