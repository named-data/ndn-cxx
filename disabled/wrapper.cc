/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "wrapper.h"

extern "C" {
#include <ccn/fetch.h>
}
#include <poll.h>
#include <boost/throw_exception.hpp>
#include <boost/random.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>

// #include "ndn.cxx/verifier.h"
#include "executor/executor.h"

#include "logging.h"
#include "ndn.cxx/wire/ccnb.h"


INIT_LOGGER ("ndn.Wrapper");

typedef boost::error_info<struct tag_errmsg, std::string> errmsg_info_str;
typedef boost::error_info<struct tag_errmsg, int> errmsg_info_int;

using namespace std;
using namespace boost;

namespace ndn {

// hack to enable fake signatures
// min length for signature field is 16, as defined in ccn_buf_decoder.c:728
const int DEFAULT_SIGNATURE_SIZE = 16;

// Although ccn_buf_decoder.c:745 defines minimum length 16, something else is checking and only 32-byte fake value is accepted by ccnd
const int PUBLISHER_KEY_SIZE = 32;

static int
ccn_encode_garbage_Signature(struct ccn_charbuf *buf)
{
    int res = 0;

    res |= ccn_charbuf_append_tt(buf, CCN_DTAG_Signature, CCN_DTAG);

    // Let's cheat more.  Default signing algorithm in ccnd is SHA256, so we just need add 32 bytes of garbage
    static char garbage [DEFAULT_SIGNATURE_SIZE];

    // digest and witness fields are optional, so use default ones

    res |= ccn_charbuf_append_tt(buf, CCN_DTAG_SignatureBits, CCN_DTAG);
    res |= ccn_charbuf_append_tt(buf, DEFAULT_SIGNATURE_SIZE, CCN_BLOB);
    res |= ccn_charbuf_append(buf, garbage, DEFAULT_SIGNATURE_SIZE);
    res |= ccn_charbuf_append_closer(buf);

    res |= ccn_charbuf_append_closer(buf);

    return(res == 0 ? 0 : -1);
}

static int
ccn_pack_unsigned_ContentObject(struct ccn_charbuf *buf,
                                const struct ccn_charbuf *Name,
                                const struct ccn_charbuf *SignedInfo,
                                const void *data,
                                size_t size)
{
    int res = 0;
    struct ccn_charbuf *content_header;
    size_t closer_start;

    content_header = ccn_charbuf_create();
    res |= ccn_charbuf_append_tt(content_header, CCN_DTAG_Content, CCN_DTAG);
    if (size != 0)
        res |= ccn_charbuf_append_tt(content_header, size, CCN_BLOB);
    closer_start = content_header->length;
    res |= ccn_charbuf_append_closer(content_header);
    if (res < 0)
        return(-1);

    res |= ccn_charbuf_append_tt(buf, CCN_DTAG_ContentObject, CCN_DTAG);

    res |= ccn_encode_garbage_Signature(buf);

    res |= ccn_charbuf_append_charbuf(buf, Name);
    res |= ccn_charbuf_append_charbuf(buf, SignedInfo);
    res |= ccnb_append_tagged_blob(buf, CCN_DTAG_Content, data, size);
    res |= ccn_charbuf_append_closer(buf);

    ccn_charbuf_destroy(&content_header);
    return(res == 0 ? 0 : -1);
}

Wrapper::Wrapper()
  : m_handle (0)
  , m_running (true)
  , m_connected (false)
  , m_executor (new Executor(1))
  // , m_verifier(new Verifier(this))
{
  start ();
}

void
Wrapper::connectCcnd()
{
  if (m_handle != 0) {
    ccn_disconnect (m_handle);
    //ccn_destroy (&m_handle);
  }
  else
    {
      m_handle = ccn_create ();
    }

  UniqueRecLock lock(m_mutex);
  if (ccn_connect(m_handle, NULL) < 0)
  {
    BOOST_THROW_EXCEPTION (Error::ndnOperation() << errmsg_info_str("connection to ccnd failed"));
  }
  m_connected = true;

  if (!m_registeredInterests.empty())
  {
   for (map<Name, InterestCallback>::const_iterator it = m_registeredInterests.begin(); it != m_registeredInterests.end(); ++it)
    {
      clearInterestFilter(it->first, false);
      setInterestFilter(it->first, it->second, false);
    }
  }
}

Wrapper::~Wrapper()
{
  shutdown ();
  // if (m_verifier != 0)
  // {
  //   delete m_verifier;
  //   m_verifier = 0;
  // }
}

void
Wrapper::start () // called automatically in constructor
{
  connectCcnd();
  m_thread = thread (&Wrapper::ccnLoop, this);
  m_executor->start();
}

void
Wrapper::shutdown () // called in destructor, but can called manually
{
  m_executor->shutdown();

  {
    UniqueRecLock lock(m_mutex);
    m_running = false;
  }

  _LOG_DEBUG ("+++++++++SHUTDOWN+++++++");
  if (m_connected)
    {
      m_thread.join ();

      ccn_disconnect (m_handle);
      //ccn_destroy (&m_handle);
      m_connected = false;
    }
}

void
Wrapper::ccnLoop ()
{
  static boost::mt19937 randomGenerator (static_cast<unsigned int> (std::time (0)));
  static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rangeUniformRandom (randomGenerator, uniform_int<> (0,1000));

  while (m_running)
    {
      try
        {
          int res = 0;
          {
            UniqueRecLock lock(m_mutex);
            res = ccn_run (m_handle, 0);
          }

          if (!m_running) break;

          if (res < 0) {
            _LOG_ERROR ("ccn_run returned negative status: " << res);

            BOOST_THROW_EXCEPTION (Error::ndnOperation()
                                   << errmsg_info_str("ccn_run returned error"));
          }


          pollfd pfds[1];
          {
            UniqueRecLock lock(m_mutex);

            pfds[0].fd = ccn_get_connection_fd (m_handle);
            pfds[0].events = POLLIN;
            if (ccn_output_is_pending (m_handle))
              pfds[0].events |= POLLOUT;
          }

          int ret = poll (pfds, 1, 1);
          if (ret < 0)
            {
              BOOST_THROW_EXCEPTION (Error::ndnOperation() << errmsg_info_str("ccnd socket failed (probably ccnd got stopped)"));
            }
        }
        catch (Error::ndnOperation &e)
        {
          m_connected = false;
          // probably ccnd has been stopped
          // try reconnect with sleep
          int interval = 1;
          int maxInterval = 32;
          while (m_running)
          {
            try
            {
              this_thread::sleep (boost::get_system_time () +  time::Seconds (interval) + time::Milliseconds (rangeUniformRandom ()));

              connectCcnd ();
              _LOG_DEBUG("reconnect to ccnd succeeded");
              break;
            }
            catch (Error::ndnOperation &e)
            {
              this_thread::sleep (boost::get_system_time () +  time::Seconds (interval) + time::Milliseconds (rangeUniformRandom ()));

              // do exponential backup for reconnect interval
              if (interval < maxInterval)
              {
                interval *= 2;
              }
            }
          }
        }
        catch (const std::exception &exc)
          {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
          }
        catch (...)
          {
            cout << "UNKNOWN EXCEPTION !!!" << endl;
          }
     }
}

Bytes
Wrapper::createContentObject(const Name  &name, const void *buf, size_t len, int freshness, const Name &keyNameParam)
{
  {
    UniqueRecLock lock(m_mutex);
    if (!m_running || !m_connected)
      {
        _LOG_TRACE ("<< not running or connected");
        return Bytes ();
      }
  }

  ccn_charbuf *content = ccn_charbuf_create();

  struct ccn_signing_params sp = CCN_SIGNING_PARAMS_INIT;
  sp.freshness = freshness;

  Name keyName;

  if (keyNameParam.size() == 0)
  {
    // use default key name
    CharbufPtr defaultKeyNamePtr = boost::make_shared<Charbuf>();
    ccn_get_public_key_and_name(m_handle, &sp, NULL, NULL, defaultKeyNamePtr->getBuf());
    keyName = Name(*defaultKeyNamePtr);

    _LOG_DEBUG ("DEFAULT KEY NAME: " << keyName);
  }
  else
  {
    keyName = keyNameParam;
  }

  if (sp.template_ccnb == NULL)
  {
    sp.template_ccnb = ccn_charbuf_create();
    ccn_charbuf_append_tt(sp.template_ccnb, CCN_DTAG_SignedInfo, CCN_DTAG);
  }
  // no idea what the following 3 lines do, but it was there
  else if (sp.template_ccnb->length > 0) {
      sp.template_ccnb->length--;
  }
  ccn_charbuf_append_tt(sp.template_ccnb, CCN_DTAG_KeyLocator, CCN_DTAG);
  ccn_charbuf_append_tt(sp.template_ccnb, CCN_DTAG_KeyName, CCN_DTAG);

  charbuf_stream keyStream;
  wire::Ccnb::appendName (keyStream, keyName);
  
  ccn_charbuf_append(sp.template_ccnb, keyStream.buf ().getBuf ()->buf, keyStream.buf ().getBuf ()->length);
  ccn_charbuf_append_closer(sp.template_ccnb); // </KeyName>
  ccn_charbuf_append_closer(sp.template_ccnb); // </KeyLocator>
  sp.sp_flags |= CCN_SP_TEMPL_KEY_LOCATOR;
  ccn_charbuf_append_closer(sp.template_ccnb); // </SignedInfo>

  charbuf_stream nameStream;
  wire::Ccnb::appendName (nameStream, name);
  
  if (ccn_sign_content(m_handle, content, nameStream.buf ().getBuf (), &sp, buf, len) != 0)
  {
    BOOST_THROW_EXCEPTION(Error::ndnOperation() << errmsg_info_str("sign content failed"));
  }

  Bytes bytes;
  readRaw(bytes, content->buf, content->length);

  ccn_charbuf_destroy (&content);
  if (sp.template_ccnb != NULL)
  {
    ccn_charbuf_destroy (&sp.template_ccnb);
  }

  return bytes;
}

int
Wrapper::putToCcnd (const Bytes &contentObject)
{
  _LOG_TRACE (">> putToCcnd");
  UniqueRecLock lock(m_mutex);
  if (!m_running || !m_connected)
    {
      _LOG_TRACE ("<< not running or connected");
      return -1;
    }


  if (ccn_put(m_handle, head(contentObject), contentObject.size()) < 0)
  {
    _LOG_ERROR ("ccn_put failed");
    // BOOST_THROW_EXCEPTION(Error::ndnOperation() << errmsg_info_str("ccnput failed"));
  }
  else
    {
      _LOG_DEBUG ("<< putToCcnd");
    }

  return 0;
}

int
Wrapper::publishData (const Name &name, const unsigned char *buf, size_t len, int freshness, const Name &keyName)
{
  _LOG_TRACE ("publishData: " << name);
  Bytes co = createContentObject(name, buf, len, freshness, keyName);
  return putToCcnd(co);
}

int
Wrapper::publishUnsignedData(const Name &name, const unsigned char *buf, size_t len, int freshness)
{
  _LOG_TRACE ("publishUnsignedData: " << name);
  {
    UniqueRecLock lock(m_mutex);
    if (!m_running || !m_connected)
      {
        _LOG_TRACE ("<< not running or connected");
        return -1;
      }
  }

  ccn_charbuf *content = ccn_charbuf_create();
  ccn_charbuf *signed_info = ccn_charbuf_create();

  static char fakeKey[PUBLISHER_KEY_SIZE];

  int res = ccn_signed_info_create(signed_info,
                                   fakeKey, PUBLISHER_KEY_SIZE,
                                   NULL,
                                   CCN_CONTENT_DATA,
                                   freshness,
                                   NULL,
                                   NULL  // ccnd is happy with absent key locator and key itself... ha ha
                                   );

  charbuf_stream nameStream;
  wire::Ccnb::appendName (nameStream, name);

  ccn_pack_unsigned_ContentObject(content, nameStream.buf ().getBuf (), signed_info, buf, len);

  Bytes bytes;
  readRaw(bytes, content->buf, content->length);

  ccn_charbuf_destroy (&content);
  ccn_charbuf_destroy (&signed_info);

  return putToCcnd (bytes);
}


static void
deleterInInterestTuple (tuple<Wrapper::InterestCallback *, ExecutorPtr> *tuple)
{
  delete tuple->get<0> ();
  delete tuple;
}

static ccn_upcall_res
incomingInterest(ccn_closure *selfp,
                 ccn_upcall_kind kind,
                 ccn_upcall_info *info)
{
  Wrapper::InterestCallback *f;
  ExecutorPtr executor;
  tuple<Wrapper::InterestCallback *, ExecutorPtr> *realData = reinterpret_cast< tuple<Wrapper::InterestCallback *, ExecutorPtr>* > (selfp->data);
  tie (f, executor) = *realData;

  switch (kind)
    {
    case CCN_UPCALL_FINAL: // effective in unit tests
      // delete closure;
      executor->execute (bind (deleterInInterestTuple, realData));

      delete selfp;
      _LOG_TRACE ("<< incomingInterest with CCN_UPCALL_FINAL");
      return CCN_UPCALL_RESULT_OK;

    case CCN_UPCALL_INTEREST:
      _LOG_TRACE (">> incomingInterest upcall: " << Name(info->interest_ccnb, info->interest_comps));
      break;

    default:
      _LOG_TRACE ("<< incomingInterest with CCN_UPCALL_RESULT_OK: " << Name(info->interest_ccnb, info->interest_comps));
      return CCN_UPCALL_RESULT_OK;
    }

  InterestPtr interest = make_shared<Interest> (info->pi);
  interest->setName (Name (info->interest_ccnb, info->interest_comps));

  executor->execute (bind (*f, interest));
  // this will be run in executor
  // (*f) (interest);
  // closure->runInterestCallback(interest);

  return CCN_UPCALL_RESULT_OK;
}

static void
deleterInDataTuple (tuple<Closure *, ExecutorPtr, InterestPtr> *tuple)
{
  delete tuple->get<0> ();
  delete tuple;
}

static ccn_upcall_res
incomingData(ccn_closure *selfp,
             ccn_upcall_kind kind,
             ccn_upcall_info *info)
{
  // Closure *cp = static_cast<Closure *> (selfp->data);
  Closure *cp;
  ExecutorPtr executor;
  InterestPtr interest;
  tuple<Closure *, ExecutorPtr, InterestPtr> *realData = reinterpret_cast< tuple<Closure*, ExecutorPtr, InterestPtr>* > (selfp->data);
  tie (cp, executor, interest) = *realData;

  switch (kind)
    {
    case CCN_UPCALL_FINAL:  // effecitve in unit tests
      executor->execute (bind (deleterInDataTuple, realData));

      cp = NULL;
      delete selfp;
      _LOG_TRACE ("<< incomingData with CCN_UPCALL_FINAL");
      return CCN_UPCALL_RESULT_OK;

    case CCN_UPCALL_CONTENT:
      _LOG_TRACE (">> incomingData content upcall: " << Name (info->content_ccnb, info->content_comps));
      break;

    // this is the case where the intentionally unsigned packets coming (in Encapsulation case)
    case CCN_UPCALL_CONTENT_BAD:
      _LOG_TRACE (">> incomingData content bad upcall: " << Name (info->content_ccnb, info->content_comps));
      break;

    // always ask ccnd to try to fetch the key
    case CCN_UPCALL_CONTENT_UNVERIFIED:
      _LOG_TRACE (">> incomingData content unverified upcall: " << Name (info->content_ccnb, info->content_comps));
      break;

    case CCN_UPCALL_INTEREST_TIMED_OUT: {
      if (cp != NULL)
      {
        Name interestName (info->interest_ccnb, info->interest_comps);
        _LOG_TRACE ("<< incomingData timeout: " << Name (info->interest_ccnb, info->interest_comps));
        executor->execute (bind (&Closure::runTimeoutCallback, cp, interestName, *cp, interest));
      }
      else
        {
          _LOG_TRACE ("<< incomingData timeout, but callback is not set...: " << Name (info->interest_ccnb, info->interest_comps));
        }
      return CCN_UPCALL_RESULT_OK;
    }

    default:
      _LOG_TRACE(">> unknown upcall type");
      return CCN_UPCALL_RESULT_OK;
    }

  PcoPtr pco = make_shared<ParsedContentObject> (info->content_ccnb, info->pco->offset[CCN_PCO_E]);

  // this will be run in executor
  executor->execute (bind (&Closure::runDataCallback, cp, pco->name (), pco));
  _LOG_TRACE (">> incomingData");

  return CCN_UPCALL_RESULT_OK;
}

int Wrapper::sendInterest (const Interest &interest, const Closure &closure)
{
  _LOG_TRACE (">> sendInterest: " << interest.getName ());
  {
    UniqueRecLock lock(m_mutex);
    if (!m_running || !m_connected)
      {
        _LOG_ERROR ("<< sendInterest: not running or connected");
        return -1;
      }
  }

  ccn_closure *dataClosure = new ccn_closure;

  // Closure *myClosure = new ExecutorClosure(closure, m_executor);
  Closure *myClosure = closure.dup ();
  dataClosure->data = new tuple<Closure*, ExecutorPtr, InterestPtr> (myClosure, m_executor, make_shared<Interest> (interest));

  dataClosure->p = &incomingData;

  UniqueRecLock lock(m_mutex);

  charbuf_stream nameStream;
  wire::Ccnb::appendName (nameStream, interest.getName ());

  charbuf_stream interestStream;
  wire::Ccnb::appendInterest (interestStream, interest);

  if (ccn_express_interest (m_handle, nameStream.buf ().getBuf (),
                            dataClosure,
                            interestStream.buf ().getBuf ()
                            ) < 0)
  {
    _LOG_ERROR ("<< sendInterest: ccn_express_interest FAILED!!!");
  }

  return 0;
}

int Wrapper::setInterestFilter (const Name &prefix, const InterestCallback &interestCallback, bool record/* = true*/)
{
  _LOG_TRACE (">> setInterestFilter");
  UniqueRecLock lock(m_mutex);
  if (!m_running || !m_connected)
  {
    return -1;
  }

  ccn_closure *interestClosure = new ccn_closure;

  // interestClosure->data = new ExecutorInterestClosure(interestCallback, m_executor);

  interestClosure->data = new tuple<Wrapper::InterestCallback *, ExecutorPtr> (new InterestCallback (interestCallback), m_executor); // should be removed when closure is removed
  interestClosure->p = &incomingInterest;

  charbuf_stream prefixStream;
  wire::Ccnb::appendName (prefixStream, prefix);

  int ret = ccn_set_interest_filter (m_handle, prefixStream.buf ().getBuf (), interestClosure);
  if (ret < 0)
  {
    _LOG_ERROR ("<< setInterestFilter: ccn_set_interest_filter FAILED");
  }

  if (record)
    {
      m_registeredInterests.insert(pair<Name, InterestCallback>(prefix, interestCallback));
    }

  _LOG_TRACE ("<< setInterestFilter");

  return ret;
}

void
Wrapper::clearInterestFilter (const Name &prefix, bool record/* = true*/)
{
  _LOG_TRACE (">> clearInterestFilter");
  UniqueRecLock lock(m_mutex);
  if (!m_running || !m_connected)
    return;

  charbuf_stream prefixStream;
  wire::Ccnb::appendName (prefixStream, prefix);

  int ret = ccn_set_interest_filter (m_handle, prefixStream.buf ().getBuf (), 0);
  if (ret < 0)
  {
  }

  if (record)
    {
      m_registeredInterests.erase(prefix);
    }

  _LOG_TRACE ("<< clearInterestFilter");
}

Name
Wrapper::getLocalPrefix ()
{
  struct ccn * tmp_handle = ccn_create ();
  int res = ccn_connect (tmp_handle, NULL);
  if (res < 0)
    {
      return Name();
    }

  string retval = "";

  struct ccn_charbuf *templ = ccn_charbuf_create();
  ccn_charbuf_append_tt(templ, CCN_DTAG_Interest, CCN_DTAG);
  ccn_charbuf_append_tt(templ, CCN_DTAG_Name, CCN_DTAG);
  ccn_charbuf_append_closer(templ); /* </Name> */
  // XXX - use pubid if possible
  ccn_charbuf_append_tt(templ, CCN_DTAG_MaxSuffixComponents, CCN_DTAG);
  ccnb_append_number(templ, 1);
  ccn_charbuf_append_closer(templ); /* </MaxSuffixComponents> */
  ccnb_tagged_putf(templ, CCN_DTAG_Scope, "%d", 2);
  ccn_charbuf_append_closer(templ); /* </Interest> */

  struct ccn_charbuf *name = ccn_charbuf_create ();
  res = ccn_name_from_uri (name, "/local/ndn/prefix");
  if (res < 0) {
  }
  else
    {
      struct ccn_fetch *fetch = ccn_fetch_new (tmp_handle);

      struct ccn_fetch_stream *stream = ccn_fetch_open (fetch, name, "/local/ndn/prefix",
                                                        NULL, 4, CCN_V_HIGHEST, 0);
      if (stream == NULL) {
      }
      else
        {
          ostringstream os;

          int counter = 0;
          char buf[256];
          while (true) {
            res = ccn_fetch_read (stream, buf, sizeof(buf));

            if (res == 0) {
              break;
            }

            if (res > 0) {
              os << string(buf, res);
            } else if (res == CCN_FETCH_READ_NONE) {
              if (counter < 2)
                {
                  ccn_run(tmp_handle, 1000);
                  counter ++;
                }
              else
                {
                  break;
                }
            } else if (res == CCN_FETCH_READ_END) {
              break;
            } else if (res == CCN_FETCH_READ_TIMEOUT) {
              break;
            } else {
              break;
            }
          }
          retval = os.str ();
          stream = ccn_fetch_close(stream);
        }
      fetch = ccn_fetch_destroy(fetch);
    }

  ccn_charbuf_destroy (&name);

  ccn_disconnect (tmp_handle);
  ccn_destroy (&tmp_handle);

  boost::algorithm::trim(retval);
  return Name(retval);
}

bool
Wrapper::verify(PcoPtr &pco, double maxWait)
{
  return true; // totally fake
  // return m_verifier->verify(pco, maxWait);
}

/// @cond include_hidden
// This is needed just for get function implementation
struct GetState
{
  GetState (double maxWait)
  {
    double intPart, fraction;
    fraction = modf (std::abs(maxWait), &intPart);

    m_maxWait = time::Now ()
      + time::Seconds (intPart)
      + time::Microseconds (fraction * 1000000);
  }

  PcoPtr
  WaitForResult ()
  {
    //_LOG_TRACE("GetState::WaitForResult start");
    boost::unique_lock<boost::mutex> lock (m_mutex);
    m_cond.timed_wait (lock, m_maxWait);
    //_LOG_TRACE("GetState::WaitForResult finish");

    return m_retval;
  }

  void
  DataCallback (Name name, PcoPtr pco)
  {
    //_LOG_TRACE("GetState::DataCallback, Name [" << name << "]");
    boost::unique_lock<boost::mutex> lock (m_mutex);
    m_retval = pco;
    m_cond.notify_one ();
  }

  void
  TimeoutCallback (Name name)
  {
    boost::unique_lock<boost::mutex> lock (m_mutex);
    m_cond.notify_one ();
  }

private:
  Time m_maxWait;

  boost::mutex m_mutex;
  boost::condition_variable    m_cond;

  PcoPtr  m_retval;
};
/// @endcond

PcoPtr
Wrapper::get(const Interest &interest, double maxWait/* = 4.0*/)
{
  _LOG_TRACE (">> get: " << interest.getName ());
  {
    UniqueRecLock lock(m_mutex);
    if (!m_running || !m_connected)
      {
        _LOG_ERROR ("<< get: not running or connected");
        return PcoPtr ();
      }
  }

  GetState state (maxWait);
  this->sendInterest (interest, Closure (boost::bind (&GetState::DataCallback, &state, _1, _2),
                                         boost::bind (&GetState::TimeoutCallback, &state, _1)));
  return state.WaitForResult ();
}

}
