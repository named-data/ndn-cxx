/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi0@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "security/validator-null.hpp"
#include "security/key-chain.hpp"
#include "util/time.hpp"


using namespace std;

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestValidator)

void
onValidated(const shared_ptr<const Data>& data)
{ BOOST_CHECK(true); }

void
onValidationFailed(const shared_ptr<const Data>& data)
{ BOOST_CHECK(false); }

BOOST_AUTO_TEST_CASE (Null)
{
  KeyChainImpl<SecPublicInfoSqlite3, SecTpmFile> keyChain;
  Name identity(string("/TestValidator/Null/") + boost::lexical_cast<std::string>(time::now()));
  keyChain.createIdentity(identity);

  Name dataName = identity;
  dataName.append("1");
  shared_ptr<Data> data = make_shared<Data>(dataName);

  keyChain.signByIdentity(*data, identity);
  
  ValidatorNull validator;
  
  validator.validate(data,
		     bind(&onValidated, _1),
		     bind(&onValidationFailed, _1));

  keyChain.deleteIdentity(identity);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
