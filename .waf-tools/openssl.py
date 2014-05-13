#! /usr/bin/env python
# encoding: utf-8

'''

When using this tool, the wscript will look like:

    def options(opt):
        opt.tool_options('openssl')

    def configure(conf):
        conf.load('compiler_cxx openssl')
        conf.check_openssl()

    def build(bld):
        bld(source='main.cpp', target='app', use='OPENSSL')

'''

from waflib import Options
from waflib.Configure import conf

@conf
def check_openssl(self,*k,**kw):
    root = k and k[0] or kw.get('path', None) or Options.options.with_openssl
    mandatory = kw.get('mandatory', True)
    var = kw.get('uselib_store', 'OPENSSL')

    if root:
        libcrypto = self.check_cxx(lib=['ssl', 'crypto'],
                       msg='Checking for OpenSSL library',
                       define_name='HAVE_%s' % var,
                       uselib_store=var,
                       mandatory=mandatory,
                       includes="%s/include" % root,
                       libpath="%s/lib" % root)
    else:
        libcrypto = self.check_cxx(lib=['ssl', 'crypto'],
                       msg='Checking for OpenSSL library',
                       define_name='HAVE_%s' % var,
                       uselib_store=var,
                       mandatory=mandatory)

def options(opt):
    opt.add_option('--with-openssl', type='string', default=None,
                   dest='with_openssl', help='''Path to OpenSSL''')
