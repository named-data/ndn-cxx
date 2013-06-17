# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
VERSION='0.6.0'

from waflib import Build, Logs, Utils, Task, TaskGen, Configure

def options(opt):
    opt.add_option('--debug',action='store_true',default=False,dest='debug',help='''debugging mode''')
    opt.add_option('--test', action='store_true',default=False,dest='_test',help='''build unit tests''')
    opt.add_option('--log4cxx', action='store_true',default=False,dest='log4cxx',help='''Compile with log4cxx logging support''')

    opt.load('compiler_c compiler_cxx boost ccnx doxygen gnu_dirs c_osx')
    opt.load('tinyxml', tooldir=['waf-tools'])

def configure(conf):
    conf.load("compiler_c compiler_cxx boost ccnx gnu_dirs tinyxml doxygen c_osx")

    if conf.options.debug:
        conf.define ('_DEBUG', 1)
        conf.add_supported_cxxflags (cxxflags = ['-O0',
                                                 '-Wall',
                                                 '-Wno-unused-variable',
                                                 '-g3',
                                                 '-Wno-unused-private-field', # only clang supports
                                                 '-fcolor-diagnostics',       # only clang supports
                                                 '-Qunused-arguments'         # only clang supports
                                                 ])
    else:
        conf.add_supported_cxxflags (cxxflags = ['-O3', '-g'])

    # if Utils.unversioned_sys_platform () == "darwin":
    #     conf.check_cxx(framework_name='Foundation', uselib_store='OSX_FOUNDATION', mandatory=True, compile_filename='test.mm')
    #     # conf.check_cxx(framework_name='AppKit',     uselib_store='OSX_APPKIT',     mandatory=True, compile_filename='test.mm')
    #     conf.check_cxx(framework_name='Security',   uselib_store='OSX_SECURITY',   define_name='HAVE_SECURITY',
    #                    use="OSX_FOUNDATION", mandatory=True, compile_filename='test.mm')

    conf.define ("NDN_CXX_VERSION", VERSION)

    conf.check_cfg(package='libevent', args=['--cflags', '--libs'], uselib_store='LIBEVENT', mandatory=True)
    conf.check_cfg(package='libevent_pthreads', args=['--cflags', '--libs'], uselib_store='LIBEVENT_PTHREADS', mandatory=True)

    if not conf.check_cfg(package='openssl', args=['--cflags', '--libs'], uselib_store='SSL', mandatory=False):
        libcrypto = conf.check_cc(lib='crypto',
                                  header_name='openssl/crypto.h',
                                  define_name='HAVE_SSL',
                                  uselib_store='SSL')
    else:
        conf.define ("HAVE_SSL", 1)
    if not conf.get_define ("HAVE_SSL"):
        conf.fatal ("Cannot find SSL libraries")

    conf.check_cfg(package="libcrypto",  args=['--cflags', '--libs'], uselib_store='CRYPTO', mandatory=True)

    if conf.options.log4cxx:
        conf.check_cfg(package='liblog4cxx', args=['--cflags', '--libs'], uselib_store='LOG4CXX', mandatory=True)
        conf.define ("HAVE_LOG4CXX", 1)

    conf.check_tinyxml(path=conf.options.tinyxml_dir)
    conf.check_doxygen(mandatory=False)

    conf.check_boost(lib='system test iostreams filesystem thread date_time')

    boost_version = conf.env.BOOST_VERSION.split('_')
    if int(boost_version[0]) < 1 or int(boost_version[1]) < 46:
        Logs.error ("Minumum required boost version is 1.46")
        return

    conf.check_ccnx (path=conf.options.ccnx_dir)

    if conf.options._test:
        conf.define ('_TESTS', 1)
        conf.env.TEST = 1

    conf.write_config_header('config.h')

def build (bld):
    executor = bld.objects (
        target = "executor",
        features = ["cxx"],
        cxxflags = "-fPIC",
        source = bld.path.ant_glob(['executor/**/*.cc']),
        use = 'BOOST BOOST_THREAD LIBEVENT LIBEVENT_PTHREADS LOG4CXX',
        includes = ".",
        )

    scheduler = bld.objects (
        target = "scheduler",
        features = ["cxx"],
        cxxflags = "-fPIC",
        source = bld.path.ant_glob(['scheduler/**/*.cc']),
        use = 'BOOST BOOST_THREAD LIBEVENT LIBEVENT_PTHREADS LOG4CXX executor',
        includes = ".",
        )

    libndn_cxx = bld (
        target="ndn.cxx",
        features=['cxx', 'cxxshlib'],
        source = bld.path.ant_glob(['ndn.cxx/**/*.cpp', 'ndn.cxx/**/*.cc',
                                    'logging.cc',
                                    'libndn.cxx.pc.in']),
        use = 'CRYPTO TINYXML BOOST BOOST_THREAD SSL CCNX LOG4CXX scheduler executor',
        includes = ".",
        )

    # if Utils.unversioned_sys_platform () == "darwin":
    #     libndn_cxx.mac_app = True
    #     libndn_cxx.source += bld.path.ant_glob (['platforms/osx/**/*.mm'])
    #     libndn_cxx.use += " OSX_FOUNDATION OSX_SECURITY"

    # Unit tests
    if bld.env['TEST']:
      unittests = bld.program (
          target="unit-tests",
          features = "cxx cxxprogram",
          defines = "WAF",
          source = bld.path.ant_glob(['test/*.cc']),
          use = 'BOOST_TEST BOOST_FILESYSTEM BOOST_DATE_TIME LOG4CXX ndn.cxx',
          includes = ".",
          install_prefix = None,
          )

    headers = bld.path.ant_glob(['ndn.cxx.h', 'ndn.cxx/**/*.h'])
    bld.install_files("%s" % bld.env['INCLUDEDIR'], headers, relative_trick=True)

@Configure.conf
def add_supported_cxxflags(self, cxxflags):
    """
    Check which cxxflags are supported by compiler and add them to env.CXXFLAGS variable
    """
    self.start_msg('Checking allowed flags for c++ compiler')

    supportedFlags = []
    for flag in cxxflags:
        if self.check_cxx (cxxflags=[flag], mandatory=False):
            supportedFlags += [flag]

    self.end_msg (' '.join (supportedFlags))
    self.env.CXXFLAGS += supportedFlags


# doxygen docs
from waflib.Build import BuildContext
class doxy (BuildContext):
    cmd = "doxygen"
    fun = "doxygen"

def doxygen (bld):
    if not bld.env.DOXYGEN:
        bld.fatal ("ERROR: cannot build documentation (`doxygen' is not found in $PATH)")
    bld (features="doxygen",
         doxyfile='doc/doxygen.conf')

# doxygen docs
from waflib.Build import BuildContext
class sphinx (BuildContext):
    cmd = "sphinx"
    fun = "sphinx"

def sphinx (bld):
    bld.load('sphinx_build', tooldir=['waf-tools'])

    bld (features="sphinx",
         outdir = "doc/html",
         source = "doc/source/conf.py")


@TaskGen.extension('.mm')
def mm_hook(self, node):
    """Alias .mm files to be compiled the same as .cc files, gcc will do the right thing."""
    return self.create_compiled_task('cxx', node)
