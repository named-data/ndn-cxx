# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

from waflib.Configure import conf

FRIEND_TYPENAME = '''
class A;

template<typename T>
class B
{
  friend T;
};

B<A> g_b;
'''

FRIEND_WRAPPER = '''
class A;

template<typename T>
struct TypeWrapper
{
  typedef T Type;
};

template<typename T>
class B
{
  friend class TypeWrapper<T>::Type;
};

B<A> g_b;
'''

@conf
def check_friend_typename(self):
    if self.check_cxx(msg='Checking for friend typename-specifier',
                      fragment=FRIEND_TYPENAME,
                      features='cxx', mandatory=False):
        self.define('HAVE_CXX_FRIEND_TYPENAME', 1)
    elif self.check_cxx(msg='Checking for friend typename using wrapper',
                      fragment=FRIEND_WRAPPER,
                      features='cxx', mandatory=True):
        self.define('HAVE_CXX_FRIEND_TYPENAME_WRAPPER', 1)

def configure(conf):
    conf.check_friend_typename()
