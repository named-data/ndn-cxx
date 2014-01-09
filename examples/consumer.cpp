/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/face.hpp>
#include <stdexcept>

#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

void
onData(ndn::Face &face,
       const ndn::ptr_lib::shared_ptr<const ndn::Interest> &interest, const ndn::ptr_lib::shared_ptr<ndn::Data> &data)
{
  std::cout << "I: " << interest->toUri() << std::endl;
  std::cout << "D: " << data->getName().toUri() << std::endl;
}

void
onTimeout(ndn::Face &face,
          const ndn::ptr_lib::shared_ptr<const ndn::Interest> &interest)
{
  std::cout << "Timeout" << std::endl;
}

void
BlockPrinter(const ndn::Block &block, const std::string &indent="")
{
  std::cout << indent << block.type() << " (" << block.value_size() << ") [[";
  std::cout.write(reinterpret_cast<const char *>(block.value()), block.value_size());
  std::cout<< "]]" << std::endl;

  for(ndn::Block::element_const_iterator i = block.getAll().begin();
      i != block.getAll().end();
      ++i)
    {
      BlockPrinter(*i, indent+"  ");
    }
}

int main()
{
  try {
    ndn::Interest i(ndn::Name("/localhost/testApp/randomData"));
    i.setScope(1);
    i.setInterestLifetime(1000);
    i.setMustBeFresh(true);

    ndn::Face face;
    face.expressInterest(i,
                          ndn::func_lib::bind(onData, boost::ref(face), _1, _2),
                          ndn::func_lib::bind(onTimeout, boost::ref(face), _1));

    // processEvents will block until the requested data received or timeout occurs
    face.processEvents();
  }
  catch(std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
