#! /usr/bin/env python
# encoding: utf-8

from waflib import Logs, Utils, TaskGen
from waflib.Configure import conf

OSX_SECURITY_CODE = '''
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#include <Security/SecRandom.h>
#include <CoreServices/CoreServices.h>
#include <Security/SecDigestTransform.h>
int main() {}
'''

OSX_SYSTEMCONFIGURATION_CODE = '''
#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>
int main() {}
'''

@conf
def check_osx_frameworks(conf, *k, **kw):
    if Utils.unversioned_sys_platform() == "darwin":
        try:
            conf.check_cxx(framework_name='CoreFoundation', uselib_store='OSX_COREFOUNDATION',
                           mandatory=True)
            conf.check_cxx(framework_name='CoreServices', uselib_store='OSX_CORESERVICES',
                           mandatory=True)
            conf.check_cxx(framework_name='Security', uselib_store='OSX_SECURITY',
                           use='OSX_COREFOUNDATION', fragment=OSX_SECURITY_CODE,
                           mandatory=True)
            conf.check_cxx(framework_name='SystemConfiguration', uselib_store='OSX_SYSTEMCONFIGURATION',
                           use='OSX_COREFOUNDATION', fragment=OSX_SYSTEMCONFIGURATION_CODE,
                           mandatory=True)

            conf.check_cxx(framework_name='Foundation', uselib_store='OSX_FOUNDATION',
                           mandatory=True, compile_filename='test.mm')
            conf.check_cxx(framework_name='CoreWLAN', uselib_store='OSX_COREWLAN',
                           use="OSX_FOUNDATION", mandatory=True, compile_filename='test.mm')

            conf.define('HAVE_OSX_FRAMEWORKS', 1)
            conf.env['HAVE_OSX_FRAMEWORKS'] = True
        except:
            Logs.warn("Compiling on macOS, but required framework(s) is(are) not functional.")
            Logs.warn("Note that the frameworks are known to work only with the Apple clang compiler.")

@TaskGen.extension('.mm')
def m_hook(self, node):
    """Alias .mm files to be compiled the same as .cpp files, gcc/clang will do the right thing."""
    return self.create_compiled_task('cxx', node)
