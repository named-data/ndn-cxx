#! /usr/bin/env python
# encoding: utf-8
# Yingdi Yu (UCLA) 2016

'''

When using this tool, the wscript will look like:

    def options(opt):
        opt.load('openssl')

    def configure(conf):
        conf.load('compiler_cxx openssl')
        conf.check_openssl()

    def build(bld):
        bld(source='main.cpp', target='app', use='OPENSSL')

'''

import re
from waflib import Utils, Logs
from waflib.Configure import conf

OPENSSL_VERSION_FILE = 'opensslv.h'
OPENSSL_DIR_OSX = ['/usr/local', '/opt/local', '/usr/local/opt/openssl']
OPENSSL_DIR = ['/usr', '/usr/local', '/opt/local', '/sw']

def options(opt):
    opt.add_option('--with-openssl', type='string', default=None, dest='openssl_dir',
                   help='''Path to where OpenSSL is installed, e.g., /usr/local''')


@conf
def __openssl_get_version_file(self, dir):
    try:
        return self.root.find_dir(dir).find_node('%s/%s' % ('include/openssl',
                                                            OPENSSL_VERSION_FILE))
    except:
        return None

@conf
def __openssl_find_root_and_version_file(self, *k, **kw):
    root = k and k[0] or kw.get('path', self.options.openssl_dir)

    file = self.__openssl_get_version_file(root)
    if root and file:
        return (root, file)

    openssl_dir = [];
    if Utils.unversioned_sys_platform() == "darwin":
        openssl_dir = OPENSSL_DIR_OSX
    else:
        openssl_dir = OPENSSL_DIR

    if not root:
        for dir in openssl_dir:
            file = self.__openssl_get_version_file(dir)
            if file:
                return (dir, file)

    if root:
        self.fatal('OpenSSL not found in %s' % root)
    else:
        self.fatal('OpenSSL not found, please provide a --with-openssl=PATH argument (see help)')

@conf
def check_openssl(self, *k, **kw):
    atleast_version = kw.get('atleast_version', 0)
    var = kw.get('uselib_store', 'OPENSSL')

    self.start_msg('Checking for OpenSSL lib')
    (root, file) = self.__openssl_find_root_and_version_file(*k, **kw)

    try:
        txt = file.read()
        re_version = re.compile('^#\\s*define\\s+OPENSSL_VERSION_NUMBER\\s+(.*)L', re.M)
        version_number = re_version.search(txt)

        re_version_text = re.compile('^#\\s*define\\s+OPENSSL_VERSION_TEXT\\s+(.*)', re.M)
        version_text = re_version_text.search(txt)

        if version_number and version_text:
            version = version_number.group(1)
            self.end_msg(version_text.group(1))
        else:
            self.fatal('OpenSSL version file is present, but is not recognizable')
    except:
        self.fatal('OpenSSL version file is not found or is not usable')

    if int(version, 16) < atleast_version:
        self.fatal("The version of OpenSSL is too old\n" +
                   "Please upgrade your distribution or install newer version of OpenSSL library")

    kwargs = {}
    if root:
        kwargs['includes'] = "%s/include" % root
        kwargs['libpath'] = "%s/lib" % root

    libcrypto = self.check_cxx(lib=['ssl', 'crypto'],
                               msg='Checking if OpenSSL library works',
                               define_name='HAVE_%s' % var,
                               uselib_store=var,
                               mandatory=True,
                               **kwargs)
