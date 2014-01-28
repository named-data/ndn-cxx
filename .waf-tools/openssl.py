#! /usr/bin/env python
# encoding: utf-8

'''

When using this tool, the wscript will look like:

	def options(opt):
	        opt.tool_options('openssl')

	def configure(conf):
		conf.load('compiler_c openssl')

                conf.check_openssl()

	def build(bld):
		bld(source='main.cpp', target='app', use='OPENSSL')

'''

from waflib import Options
from waflib.Configure import conf

@conf
def check_openssl(self,*k,**kw):
        root = k and k[0] or kw.get('path',None) or Options.options.with_openssl
        mandatory = kw.get('mandatory', True)
        var = kw.get('var', 'OPENSSL')

        CODE = """
#include <openssl/crypto.h>
#include <stdio.h>

int main(int argc, char **argv) {
	(void)argc;
        printf ("%s", argv[0]);

	return 0;
}
"""
        if root:
                libcrypto = self.check_cc (lib=['ssl', 'crypto'],
                                           header_name='openssl/crypto.h',
                                           define_name='HAVE_%s' % var,
                                           uselib_store=var,
                                           mandatory = mandatory,
                                           cflags="-I%s/include" % root,
                                           linkflags="-L%s/lib" % root,
                                           execute = True, fragment = CODE, define_ret = True)
        else:
                libcrypto = self.check_cc (lib=['ssl', 'crypto'],
                                           header_name='openssl/crypto.h',
                                           define_name='HAVE_%s' % var,
                                           uselib_store=var,
                                           mandatory = mandatory,
                                           execute = True, fragment = CODE, define_ret = True)

def options(opt):
        opt.add_option('--with-openssl',type='string',default='',dest='with_openssl',help='''Path to OpenSSL''')
