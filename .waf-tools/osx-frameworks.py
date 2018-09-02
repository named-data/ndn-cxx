#! /usr/bin/env python
# encoding: utf-8

from waflib import Logs, Utils, TaskGen
from waflib.Configure import conf

@conf
def check_osx_framework_cxx(conf, fw_name, **kw):
    conf.check_cxx(framework_name=fw_name,
                   uselib_store='OSX_' + fw_name.upper(),
                   fragment='#include <{0}/{0}.h>\nint main() {{}}'.format(fw_name),
                   **kw)

@conf
def check_osx_framework_mm(conf, fw_name, **kw):
    conf.check_cxx(framework_name=fw_name,
                   uselib_store='OSX_' + fw_name.upper(),
                   fragment='#import <{0}/{0}.h>\nint main() {{}}'.format(fw_name),
                   compile_filename='test.mm',
                   **kw)

@conf
def check_osx_frameworks(conf, *k, **kw):
    if Utils.unversioned_sys_platform() == 'darwin':
        try:
            conf.check_osx_framework_cxx('CoreFoundation', mandatory=True)
            conf.check_osx_framework_cxx('Security', use='OSX_COREFOUNDATION', mandatory=True)
            conf.check_osx_framework_cxx('SystemConfiguration', use='OSX_COREFOUNDATION', mandatory=True)

            conf.check_osx_framework_mm('Foundation', mandatory=True)
            conf.check_osx_framework_mm('CoreWLAN', use='OSX_FOUNDATION', mandatory=True)

            conf.define('HAVE_OSX_FRAMEWORKS', 1)
            conf.env['HAVE_OSX_FRAMEWORKS'] = True
        except:
            Logs.warn('Building on macOS, but one or more required frameworks are not functional.')
            Logs.warn('Note that the frameworks are known to work only with the Apple clang compiler.')

@TaskGen.extension('.mm')
def m_hook(self, node):
    '''Alias .mm files to be compiled the same as .cpp files, clang will do the right thing.'''
    return self.create_compiled_task('cxx', node)
