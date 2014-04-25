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

namespace ndn {

class Consumer
{
public:
  Consumer(const std::string& dataName,
           size_t pipeSize, size_t nTotalSegments,
           int scope = -1, bool mustBeFresh = true)
    : m_dataName(dataName)
    , m_pipeSize(pipeSize)
    , m_nTotalSegments(nTotalSegments)
    , m_nextSegment(0)
    , m_totalSize(0)
    , m_isOutputEnabled(false)
    , m_scope(scope)
    , m_mustBeFresh(mustBeFresh)
  {
  }

  inline void
  enableOutput()
  {
    m_isOutputEnabled = true;
  }

  void
  run();

private:
  void
  onData(const Interest& interest, Data& data);

  void
  onTimeout(const Interest& interest);

  Face m_face;
  Name m_dataName;
  size_t m_pipeSize;
  size_t m_nTotalSegments;
  size_t m_nextSegment;
  size_t m_totalSize;
  bool m_isOutputEnabled;  // set to false by default

  int m_scope;
  bool m_mustBeFresh;
};

void
Consumer::run()
{
  try
    {
      for (size_t i = 0; i < m_pipeSize; i++)
        {
          Interest interest(Name(m_dataName).appendSegment(m_nextSegment++));
          interest.setInterestLifetime(time::milliseconds(4000));
          if (m_scope >= 0)
            interest.setScope(m_scope);
          interest.setMustBeFresh(m_mustBeFresh);

          m_face.expressInterest(interest,
                                 bind(&Consumer::onData, this, _1, _2),
                                 bind(&Consumer::onTimeout, this, _1));
        }

      // processEvents will block until the requested data received or timeout occurs
      m_face.processEvents();
    }
  catch (std::exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void
Consumer::onData(const Interest& interest, Data& data)
{
  const Block& content = data.getContent();
  const Name& name = data.getName();

  if (m_isOutputEnabled)
    {
      std::cout.write(reinterpret_cast<const char*>(content.value()), content.value_size());
    }

  m_totalSize += content.value_size();

  if (name[-1].toSegment() + 1 == m_nTotalSegments)
    {
      std::cerr << "Last segment received." << std::endl;
      std::cerr << "Total # bytes of content received: " << m_totalSize << std::endl;
    }
  else
    {
      // Send interest for next segment
      Interest interest(Name(m_dataName).appendSegment(m_nextSegment++));
      if (m_scope >= 0)
        interest.setScope(m_scope);
      interest.setInterestLifetime(time::milliseconds(4000));
      interest.setMustBeFresh(m_mustBeFresh);

      m_face.expressInterest(interest,
                             bind(&Consumer::onData, this, _1, _2),
                             bind(&Consumer::onTimeout, this, _1));
    }
}


void
Consumer::onTimeout(const Interest& interest)
{
  //XXX: currently no retrans
  std::cerr << "TIMEOUT: last interest sent for segment #" << (m_nextSegment - 1) << std::endl;
}


int
usage(const std::string &filename)
{
  std::cerr << "Usage: \n    "
            << filename << " [-p pipeSize] [-c nTotalSegmentsment] [-o] /ndn/name\n";
  return 1;
}


int
main(int argc, char** argv)
{
  std::string name;
  int pipeSize = 1;
  int nTotalSegments = std::numeric_limits<int>::max();
  bool output = false;

  int opt;
  while ((opt = getopt(argc, argv, "op:c:")) != -1)
    {
      switch (opt)
        {
        case 'p':
          pipeSize = atoi(optarg);
          if (pipeSize <= 0)
            pipeSize = 1;
          std::cerr << "main(): set pipe size = " << pipeSize << std::endl;
          break;
        case 'c':
          nTotalSegments = atoi(optarg);
          if (nTotalSegments <= 0)
            nTotalSegments = 1;
          std::cerr << "main(): set total seg = " << nTotalSegments << std::endl;
          break;
        case 'o':
          output = true;
          break;
        default:
          return usage(argv[0]);
        }
    }

  if (optind < argc)
    {
      name = argv[optind];
    }

  if (name.empty())
    {
      return usage(argv[0]);
    }

  Consumer consumer(name, pipeSize, nTotalSegments);

  if (output)
    consumer.enableOutput();

  consumer.run();

  return 0;
}

} // namespace ndn

int
main(int argc, char** argv)
{
  return ndn::main(argc, argv);
}
