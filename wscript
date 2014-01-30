# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
VERSION='0.3~dev0'
NAME="ndn-cpp-dev"

from waflib import Build, Logs, Utils, Task, TaskGen, Configure

def options(opt):
    opt.load('compiler_c compiler_cxx gnu_dirs c_osx')
    opt.load('boost doxygen openssl cryptopp', tooldir=['.waf-tools'])

    opt = opt.add_option_group('NDN-CPP Options')

    opt.add_option('--debug',action='store_true',default=False,dest='debug',help='''debugging mode''')
    
    opt.add_option('--with-tests', action='store_true',default=False,dest='with_tests',
                   help='''build unit tests''')
    opt.add_option('--with-log4cxx', action='store_true',default=False,dest='log4cxx',
                   help='''Compile with log4cxx logging support''')

    opt.add_option('--with-c++11', action='store_true', default=False, dest='use_cxx11',
                   help='''Use C++11 features, even if available in the compiler''')
    opt.add_option('--without-system-boost', action='store_false', default=True, dest='use_system_boost',
                   help='''Use system's boost libraries''')


def configure(conf):
    conf.load("compiler_c compiler_cxx boost gnu_dirs c_osx openssl cryptopp")
    try:
        conf.load("doxygen")
    except:
        pass

    if conf.options.with_tests:
        conf.env['WITH_TESTS'] = True

    conf.check_openssl()

    if conf.options.debug:
        conf.define ('_DEBUG', 1)
        flags = ['-O0',
                 '-Wall',
                 # '-Werror',
                 '-Wno-unused-variable',
                 '-g3',
                 '-Wno-unused-private-field', # only clang supports
                 '-fcolor-diagnostics',       # only clang supports
                 '-Qunused-arguments',        # only clang supports
                 '-Wno-tautological-compare', # suppress warnings from CryptoPP
                 '-Wno-unused-function',      # another annoying warning from CryptoPP

                 '-Wno-deprecated-declarations',
                 ]

        conf.add_supported_cxxflags (cxxflags = flags)
    else:
        flags = ['-O3', '-g', '-Wno-tautological-compare', '-Wno-unused-function', '-Wno-deprecated-declarations']
        conf.add_supported_cxxflags (cxxflags = flags)

    if Utils.unversioned_sys_platform () == "darwin":
        conf.check_cxx(framework_name='CoreFoundation', uselib_store='OSX_COREFOUNDATION', mandatory=True)
        conf.check_cxx(framework_name='CoreServices', uselib_store='OSX_CORESERVICES', mandatory=True)
        conf.check_cxx(framework_name='Security',   uselib_store='OSX_SECURITY',   define_name='HAVE_SECURITY',
                       use="OSX_COREFOUNDATION", mandatory=True)
        conf.define('HAVE_OSX_SECURITY', 1)

    conf.define ("PACKAGE_BUGREPORT", "ndn-lib@lists.cs.ucla.edu")
    conf.define ("PACKAGE_NAME", NAME)
    conf.define ("PACKAGE_VERSION", VERSION)
    conf.define ("PACKAGE_URL", "https://github.com/named-data/ndn-cpp")

    conf.check_cfg(package='sqlite3', args=['--cflags', '--libs'], uselib_store='SQLITE3', mandatory=True)

    if conf.options.log4cxx:
        conf.check_cfg(package='liblog4cxx', args=['--cflags', '--libs'], uselib_store='LOG4CXX', mandatory=True)
        conf.define ("HAVE_LOG4CXX", 1)

    conf.check_cryptopp(path=conf.options.cryptopp_dir, mandatory=True)

    if conf.options.use_cxx11:
        conf.add_supported_cxxflags(cxxflags = ['-std=c++11', '-std=c++0x'])

        conf.check(msg='Checking for type std::shared_ptr',
                   type_name="std::shared_ptr<int>", header_name="memory", define_name='HAVE_STD_SHARED_PTR')
        conf.check(msg='Checking for type std::function',
                   type_name="std::function<void()>", header_name="functional", define_name='HAVE_STD_FUNCTION')
        conf.define('HAVE_CXX11', 1)
    else:
        if conf.options.use_system_boost:
            USED_BOOST_LIBS = 'system filesystem date_time iostreams regex'
            if conf.env['WITH_TESTS']:
                USED_BOOST_LIBS += " unit_test_framework"

            conf.check_boost(lib=USED_BOOST_LIBS)

            boost_version = conf.env.BOOST_VERSION.split('_')
            if int(boost_version[0]) > 1 or (int(boost_version[0]) == 1 and int(boost_version[1]) >= 46):
                conf.env['USE_SYSTEM_BOOST'] = True
                conf.define('USE_SYSTEM_BOOST', 1)

    conf.check_cxx(lib='pthread', uselib_store='PTHREAD', define_name='HAVE_PTHREAD', mandatory=False)
    conf.check_cxx(lib='rt', uselib_store='RT', define_name='HAVE_RT', mandatory=False)

    conf.write_config_header('src/ndn-cpp-config.h', define_prefix='NDN_CPP_')

def build (bld):
    libndn_cpp = bld (
        features=['cxx', 'cxxstlib'], # 'cxxshlib',
        # vnum = "0.3.0",
        target="ndn-cpp-dev",
        name = "ndn-cpp-dev",
        source = bld.path.ant_glob('src/**/*.cpp',
                                   excl = ['src/**/*-osx.cpp', 'src/**/*-sqlite3.cpp']),
        use = 'BOOST OPENSSL LOG4CXX CRYPTOPP SQLITE3 RT PTHREAD',
        includes = "src",
        export_includes = "src",
        install_path = '${LIBDIR}',
        )

    if Utils.unversioned_sys_platform () == "darwin":
        libndn_cpp.source += bld.path.ant_glob('src/**/*-osx.cpp')
        libndn_cpp.mac_app = True
        libndn_cpp.use += " OSX_COREFOUNDATION OSX_SECURITY"

    # In case we want to make it optional later
    libndn_cpp.source += bld.path.ant_glob('src/**/*-sqlite3.cpp')


    pkgconfig_libs = []
    pkgconfig_ldflags = []
    pkgconfig_includes = []
    for lib in Utils.to_list(libndn_cpp.use):
        if bld.env['LIB_%s' % lib]:
            pkgconfig_libs += Utils.to_list(bld.env['LIB_%s' % lib])
        if bld.env['LIBPATH_%s' % lib]:
            pkgconfig_ldflags += Utils.to_list(bld.env['LIBPATH_%s' % lib])
        if bld.env['INCLUDES_%s' % lib]:
            pkgconfig_includes += Utils.to_list(bld.env['INCLUDES_%s' % lib])

    def uniq(alist):
        set = {}
        return [set.setdefault(e,e) for e in alist if e not in set]
                        
    bld (features = "subst",
         source = "libndn-cpp-dev.pc.in",
         target = "libndn-cpp-dev.pc",
         install_path = "${LIBDIR}/pkgconfig",
         VERSION = VERSION,

         # This probably not the right thing to do, but to simplify life of apps
         # that use the library
         EXTRA_LIBS = " ".join([('-l%s' % i) for i in uniq(pkgconfig_libs)]),
         EXTRA_LDFLAGS = " ".join([('-L%s' % i) for i in uniq(pkgconfig_ldflags)]),
         EXTRA_INCLUDES = " ".join([('-I%s' % i) for i in uniq(pkgconfig_includes)]),
        )
            
    # Unit tests
    if bld.env['WITH_TESTS']:
        bld.recurse('tests')

    bld.recurse("tools examples")
      
    headers = bld.path.ant_glob(['src/**/*.hpp'])
    bld.install_files("%s/ndn-cpp-dev" % bld.env['INCLUDEDIR'], headers, relative_trick=True, cwd=bld.path.find_node('src'))

    bld.install_files("%s/ndn-cpp-dev" % bld.env['INCLUDEDIR'], bld.path.find_resource('src/ndn-cpp-config.h'))

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
         doxyfile='Doxyfile')

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
