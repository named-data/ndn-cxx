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

class Consumer
{
public:
  Consumer(const std::string& data_name,
           size_t pipe_size, size_t total_seg,
           int scope = -1, bool mustBeFresh = true)
    : m_data_name (data_name)
    , m_pipe_size (pipe_size)
    , m_total_seg (total_seg)
    , m_next_seg (0)
    , m_total_size (0)
    , m_output (false)
    , m_scope(scope)
    , m_mustBeFresh(mustBeFresh)
  {
  }

  inline void
  enable_output()
  {
    m_output = true;
  }

  void
  run();

private:
  void
  on_data(const ndn::Interest& interest, ndn::Data& data);

  void
  on_timeout(const ndn::Interest& interest);

  ndn::Face m_face;
  ndn::Name m_data_name;
  size_t m_pipe_size;
  size_t m_total_seg;
  size_t m_next_seg;
  size_t m_total_size;
  bool m_output;  // set to false by default

  int m_scope;
  bool m_mustBeFresh;
};

void
Consumer::run()
{
  try
    {
      for (int i = 0; i < m_pipe_size; i++)
        {
          ndn::Interest interest(ndn::Name(m_data_name).appendSegment(m_next_seg++));
          interest.setInterestLifetime(ndn::time::milliseconds(4000));
          if (m_scope >= 0)
            interest.setScope(m_scope);
          interest.setMustBeFresh(m_mustBeFresh);

          m_face.expressInterest (interest,
                                  ndn::bind(&Consumer::on_data, this, _1, _2),
                                  ndn::bind(&Consumer::on_timeout, this, _1));
        }

      // processEvents will block until the requested data received or timeout occurs
      m_face.processEvents();
    }
  catch (std::exception& e)
    {
      std::cerr << "ERROR: " << e.what () << std::endl;
    }
}

void
Consumer::on_data(const ndn::Interest& interest, ndn::Data& data)
{
  const ndn::Block& content = data.getContent();
  const ndn::Name& name = data.getName();

  if (m_output)
    {
      std::cout.write(reinterpret_cast<const char*>(content.value()), content.value_size());
    }

  m_total_size += content.value_size ();

  if (name[-1].toSegment() + 1 == m_total_seg)
    {
      std::cerr << "Last segment received." << std::endl;
      std::cerr << "Total # bytes of content received: " << m_total_size << std::endl;
    }
  else
    {
      // Send interest for next segment
      ndn::Interest interest(ndn::Name(m_data_name).appendSegment(m_next_seg++));
      if (m_scope >= 0)
        interest.setScope(m_scope);
      interest.setInterestLifetime(ndn::time::milliseconds(4000));
      interest.setMustBeFresh(m_mustBeFresh);

      m_face.expressInterest (interest,
                              ndn::bind(&Consumer::on_data, this, _1, _2),
                              ndn::bind(&Consumer::on_timeout, this, _1));
    }
}


void
Consumer::on_timeout(const ndn::Interest& interest)
{
  //XXX: currently no retrans
  std::cerr << "TIMEOUT: last interest sent for segment #" << (m_next_seg - 1) << std::endl;
}


int
usage(const std::string &filename)
{
  std::cerr << "Usage: \n    " << filename << " [-p pipe_size] [-c total_segment] [-o] /ndn/name\n";
  return 1;
}


int
main(int argc, char **argv)
{
  std::string name;
  int pipe_size = 1;
  int total_seg = std::numeric_limits<int>::max();
  bool output = false;

  int opt;
  while ((opt = getopt(argc, argv, "op:c:")) != -1)
    {
      switch (opt)
        {
        case 'p':
          pipe_size = atoi (optarg);
          if (pipe_size <= 0)
            pipe_size = 1;
          std::cerr << "main (): set pipe size = " << pipe_size << std::endl;
          break;
        case 'c':
          total_seg = atoi (optarg);
          if (total_seg <= 0)
            total_seg = 1;
          std::cerr << "main (): set total seg = " << total_seg << std::endl;
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

  Consumer consumer (name, pipe_size, total_seg);

  if (output)
    consumer.enable_output ();

  consumer.run ();

  return 0;
}
