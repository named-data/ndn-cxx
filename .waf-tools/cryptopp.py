#! /usr/bin/env python
# encoding: utf-8

'''

When using this tool, the wscript will look like:

    def options(opt):
        opt.tool_options('cryptopp', tooldir=["waf-tools"])

    def configure(conf):
        conf.load('compiler_cxx cryptopp')

    def build(bld):
        bld(source='main.cpp', target='app', use='CRYPTOPP')

Options are generated, in order to specify the location of cryptopp includes/libraries.


'''
import sys
import re
from waflib import Utils,Logs,Errors
from waflib.Configure import conf
CRYPTOPP_DIR = ['/usr', '/usr/local', '/opt/local', '/sw']
CRYPTOPP_VERSION_FILE = 'config.h'

def options(opt):
    opt.add_option('--cryptopp', type='string', default='', dest='cryptopp_dir',
                   help='''Path to where CryptoPP is installed, e.g. /opt/local''')

@conf
def __cryptopp_get_version_file(self, dir):
    try:
        return self.root.find_dir(dir).find_node('%s/%s' % ('include/cryptopp',
                                                            CRYPTOPP_VERSION_FILE))
    except:
        return None

@conf
def __cryptopp_find_root_and_version_file(self, *k, **kw):
    root = k and k[0]or kw.get('path', None)

    file = self.__cryptopp_get_version_file(root)
    if root and file:
        return (root, file)
    for dir in CRYPTOPP_DIR:
        file = self.__cryptopp_get_version_file(dir)
        if file:
            return (dir, file)

    if root:
        self.fatal('CryptoPP not found in %s' % root)
    else:
        self.fatal('CryptoPP not found, please provide a --cryptopp argument (see help)')

@conf
def check_cryptopp(self, *k, **kw):
    if not self.env['CXX']:
        self.fatal('Load a c++ compiler first, e.g., conf.load("compiler_cxx")')

    var = kw.get('uselib_store','CRYPTOPP')
    self.start_msg('Checking Crypto++ lib')
    (root, file) = self.__cryptopp_find_root_and_version_file(*k, **kw)

    try:
        txt = file.read()
        re_version = re.compile('^#define\\s+CRYPTOPP_VERSION\\s+(.*)', re.M)
        match = re_version.search(txt)

        if match:
            self.env.CRYPTOPP_VERSION = match.group(1)
            self.end_msg(self.env.CRYPTOPP_VERSION)
        else:
            self.fatal('CryptoPP files are present, but are not recognizable')
    except:
        self.fatal('CryptoPP not found or is not usable')

    val = self.check_cxx(msg='Checking if CryptoPP library works',
                         header_name='cryptopp/config.h',
                         lib='cryptopp',
                         cxxflags="-I%s/include" % root,
                         linkflags="-L%s/lib" % root,
                         mandatory=True,
                         uselib_store=var)
