#! /usr/bin/env python
# encoding: utf-8

from waflib import Logs, Utils
from waflib.Configure import conf

OSX_SECURITY_CODE='''
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#include <Security/SecRandom.h>
#include <CoreServices/CoreServices.h>
#include <Security/SecDigestTransform.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    return 0;
}
'''

@conf
def check_osx_security(conf, *k, **kw):
    if Utils.unversioned_sys_platform() == "darwin":
        try:
            conf.check_cxx(framework_name='CoreFoundation', uselib_store='OSX_COREFOUNDATION',
                           mandatory=True)
            conf.check_cxx(framework_name='CoreServices', uselib_store='OSX_CORESERVICES',
                           mandatory=True)
            conf.check_cxx(framework_name='Security', uselib_store='OSX_SECURITY',
                           define_name='HAVE_SECURITY', use="OSX_COREFOUNDATION",
                           fragment=OSX_SECURITY_CODE, mandatory=True)

            conf.define('HAVE_OSX_SECURITY', 1)
            conf.env['HAVE_OSX_SECURITY'] = True
        except:
            Logs.warn("Compiling on OSX, but CoreFoundation, CoreServices, or Security framework is not functional.")
            Logs.warn("The frameworks are known to work only with Apple-specific compilers: llvm-gcc-4.2 or clang")
