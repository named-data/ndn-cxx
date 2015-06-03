/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
           bool mustBeFresh = true)
    : m_dataName(dataName)
    , m_pipeSize(pipeSize)
    , m_nTotalSegments(nTotalSegments)
    , m_nextSegment(0)
    , m_totalSize(0)
    , m_isOutputEnabled(false)
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
  onData(Data& data);

  void
  onTimeout(const Interest& interest);

  Face m_face;
  Name m_dataName;
  size_t m_pipeSize;
  size_t m_nTotalSegments;
  size_t m_nextSegment;
  size_t m_totalSize;
  bool m_isOutputEnabled;  // set to false by default

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
          interest.setMustBeFresh(m_mustBeFresh);

          m_face.expressInterest(interest,
                                 bind(&Consumer::onData, this, _2),
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
Consumer::onData(Data& data)
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
      interest.setInterestLifetime(time::milliseconds(4000));
      interest.setMustBeFresh(m_mustBeFresh);

      m_face.expressInterest(interest,
                             bind(&Consumer::onData, this, _2),
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
