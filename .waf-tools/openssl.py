#! /usr/bin/env python
# encoding: utf-8

"""
When using this tool, the wscript should look like:

    def options(opt):
        opt.load('openssl')

    def configure(conf):
        conf.load('compiler_cxx openssl')
        conf.check_openssl()

    def build(bld):
        bld(source='main.cpp', target='app', use='OPENSSL')
"""

import re
from waflib import Utils
from waflib.Configure import conf

OPENSSL_DIR_OSX = ['/usr/local', '/opt/local', '/usr/local/opt/openssl']
OPENSSL_DIR = ['/usr', '/usr/local', '/opt/local', '/sw']

def options(opt):
    opt.add_option('--with-openssl', type='string', default=None, dest='openssl_dir',
                   help='directory where OpenSSL is installed, e.g., /usr/local')

@conf
def __openssl_get_version_file(self, dir):
    try:
        return self.root.find_dir(dir).find_node('include/openssl/opensslv.h')
    except:
        return None

@conf
def __openssl_find_root_and_version_file(self, root):
    if root:
        file = self.__openssl_get_version_file(root)
        if not file:
            self.fatal(f'OpenSSL not found in {root}')
        return (root, file)

    openssl_dirs = OPENSSL_DIR
    if Utils.unversioned_sys_platform() == 'darwin':
        openssl_dirs = OPENSSL_DIR_OSX

    for dir in openssl_dirs:
        file = self.__openssl_get_version_file(dir)
        if file:
            return (dir, file)

    self.fatal('OpenSSL not found, please provide a --with-openssl=PATH argument (see --help)')

@conf
def __openssl_check_version(self, version_file, atleast_version):
    min_version = tuple(int(i) for i in atleast_version.split('.'))
    txt = version_file.read()

    # OpenSSL 3.0.0 and later
    ver_tuple = (re.search(r'^#\s*define\s+OPENSSL_VERSION_MAJOR\s+(\d+)', txt, re.MULTILINE),
                 re.search(r'^#\s*define\s+OPENSSL_VERSION_MINOR\s+(\d+)', txt, re.MULTILINE),
                 re.search(r'^#\s*define\s+OPENSSL_VERSION_PATCH\s+(\d+)', txt, re.MULTILINE))
    ver_string = re.search(r'^#\s*define\s+OPENSSL_FULL_VERSION_STR\s+"(.+)"', txt, re.MULTILINE)
    if all(ver_tuple):
        version = tuple(int(i[1]) for i in ver_tuple)
        ver_string = ver_string[1] if ver_string else '.'.join(version)
        return (version >= min_version, ver_string)

    # OpenSSL 1.1.1 and earlier
    ver_number = re.search(r'^#\s*define\s+OPENSSL_VERSION_NUMBER\s+(.+)L', txt, re.MULTILINE)
    ver_string = re.search(r'^#\s*define\s+OPENSSL_VERSION_TEXT\s+"(.+)"', txt, re.MULTILINE)
    if ver_number and ver_string:
        version = int(ver_number[1], 16)
        min_version = (min_version[0] << 28) | (min_version[1] << 20) | (min_version[2] << 12) | 0xf
        return (version >= min_version, ver_string[1])

    self.fatal(f'Cannot extract version information from {version_file}')

@conf
def check_openssl(self, *k, **kw):
    self.start_msg('Checking for OpenSSL version')

    path = k and k[0] or kw.get('path', self.options.openssl_dir)
    root, version_file = self.__openssl_find_root_and_version_file(path)
    atleast_version = kw.get('atleast_version', 0)
    ok, version_str = self.__openssl_check_version(version_file, atleast_version)

    self.end_msg(version_str)
    if not ok:
        self.fatal(f'The version of OpenSSL is too old; {atleast_version} or later is required.\n'
                   'Please upgrade your distribution or manually install a newer version of OpenSSL.')

    if 'msg' not in kw:
        kw['msg'] = 'Checking if OpenSSL library works'
    if 'lib' not in kw:
        kw['lib'] = ['ssl', 'crypto']
    if 'uselib_store' not in kw:
        kw['uselib_store'] = 'OPENSSL'
    if 'define_name' not in kw:
        kw['define_name'] = f"HAVE_{kw['uselib_store']}"
    kw['includes'] = f'{root}/include'
    kw['libpath'] = f'{root}/lib'

    self.check_cxx(**kw)
