# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

VERSION = '0.4.0'
APPNAME = "ndn-cpp-dev"
PACKAGE_BUGREPORT = "http://redmine.named-data.net/projects/ndn-cpp-dev"
PACKAGE_URL = "https://github.com/named-data/ndn-cpp-dev"

from waflib import Logs, Utils

def options(opt):
    opt.load(['compiler_cxx', 'gnu_dirs', 'c_osx'])
    opt.load(['default-compiler-flags', 'coverage', 'osx-security', 'pch',
              'boost', 'openssl', 'cryptopp', 'sqlite3',
              'doxygen', 'sphinx_build'],
             tooldir=['.waf-tools'])

    opt = opt.add_option_group('Library Options')

    opt.add_option('--with-tests', action='store_true', default=False, dest='with_tests',
                   help='''build unit tests''')
    opt.add_option('--with-log4cxx', action='store_true', default=False, dest='log4cxx',
                   help='''Compile with log4cxx logging support''')

    opt.add_option('--with-c++11', action='store_true', default=False, dest='use_cxx11',
                   help='''Use C++11 features, even if available in the compiler''')
    opt.add_option('--without-tools', action='store_false', default=True, dest='with_tools',
                   help='''Do not build tools''')

    opt.add_option('--without-sqlite-locking', action='store_false', default=True,
                   dest='with_sqlite_locking',
                   help='''Disable filesystem locking in sqlite3 database '''
                        '''(use unix-dot locking mechanism instead). '''
                        '''This option may be necessary if home directory is hosted on NFS.''')
    opt.add_option('--without-osx-keychain', action='store_false', default=True,
                   dest='with_osx_keychain',
                   help='''On Darwin, do not use OSX keychain as a default TPM''')

def configure(conf):
    conf.load(['compiler_cxx', 'gnu_dirs', 'c_osx',
               'default-compiler-flags', 'osx-security', 'pch',
               'boost', 'openssl', 'cryptopp', 'sqlite3',
               'doxygen', 'sphinx_build'])

    conf.env['WITH_TESTS'] = conf.options.with_tests
    conf.env['WITH_TOOLS'] = conf.options.with_tools

    conf.check_osx_security(mandatory=False)

    conf.check_openssl(mandatory=True)
    conf.check_sqlite3(mandatory=True)
    conf.check_cryptopp(mandatory=True)

    if conf.options.log4cxx:
        conf.check_cfg(package='liblog4cxx', args=['--cflags', '--libs'], uselib_store='LOG4CXX',
                       mandatory=True)
        conf.define("HAVE_LOG4CXX", 1)

    if conf.options.use_cxx11:
        conf.check(msg='Checking for type std::shared_ptr',
                   type_name="std::shared_ptr<int>", header_name="memory",
                   define_name='HAVE_STD_SHARED_PTR', mandatory=True)
        conf.check(msg='Checking for type std::function',
                   type_name="std::function<void()>", header_name="functional",
                   define_name='HAVE_STD_FUNCTION', mandatory=True)
        conf.define('HAVE_CXX11', 1)

    USED_BOOST_LIBS = ['system', 'filesystem', 'date_time', 'iostreams',
                       'regex', 'program_options', 'chrono']
    if conf.env['WITH_TESTS']:
        USED_BOOST_LIBS += ['unit_test_framework']

    conf.check_boost(lib=USED_BOOST_LIBS, mandatory=True)
    if conf.env.BOOST_VERSION_NUMBER < 104800:
        Logs.error("Minimum required boost version is 1.48.0")
        Logs.error("Please upgrade your distribution or install custom boost libraries" +
                    " (http://redmine.named-data.net/projects/nfd/wiki/Boost_FAQ)")
        return

    conf.check_cxx(lib='pthread', uselib_store='PTHREAD', define_name='HAVE_PTHREAD',
                   mandatory=False)
    conf.check_cxx(lib='rt', uselib_store='RT', define_name='HAVE_RT', mandatory=False)
    conf.check_cxx(cxxflags=['-fPIC'], uselib_store='cxxstlib', mandatory=False)

    if not conf.options.with_sqlite_locking:
        conf.define('DISABLE_SQLITE3_FS_LOCKING', 1)

    if conf.env['HAVE_OSX_SECURITY']:
        conf.env['WITH_OSX_KEYCHAIN'] = conf.options.with_osx_keychain
        if conf.options.with_osx_keychain:
            conf.define('WITH_OSX_KEYCHAIN', 1)
    else:
        conf.env['WITH_OSX_KEYCHAIN'] = False

    # Loading "late" to prevent tests to be compiled with profiling flags
    conf.load('coverage')

    conf.define('SYSCONFDIR', conf.env['SYSCONFDIR'])

    conf.write_config_header('src/ndn-cpp-config.hpp', define_prefix='NDN_CPP_')

def build(bld):
    libndn_cpp = bld(
        features=['cxx', 'cxxstlib'], # 'cxxshlib',
        # vnum="0.3.0",
        target="ndn-cpp-dev",
        name="ndn-cpp-dev",
        source=bld.path.ant_glob('src/**/*.cpp',
                                   excl=['src/**/*-osx.cpp', 'src/**/*-sqlite3.cpp']),
        use='BOOST OPENSSL LOG4CXX CRYPTOPP SQLITE3 RT PIC PTHREAD',
        includes=". src",
        export_includes="src",
        install_path='${LIBDIR}',
        )

    if bld.env['WITH_PCH']:
        libndn_cpp.pch="src/common.hpp"

    if bld.env['HAVE_OSX_SECURITY']:
        libndn_cpp.source += bld.path.ant_glob('src/**/*-osx.cpp')
        libndn_cpp.mac_app = True
        libndn_cpp.use += " OSX_COREFOUNDATION OSX_SECURITY"

    # In case we want to make it optional later
    libndn_cpp.source += bld.path.ant_glob('src/**/*-sqlite3.cpp')

    # Prepare flags that should go to pkgconfig file
    pkgconfig_libs = []
    pkgconfig_ldflags = []
    pkgconfig_linkflags = []
    pkgconfig_includes = []
    pkgconfig_cxxflags = []
    for lib in Utils.to_list(libndn_cpp.use):
        if bld.env['LIB_%s' % lib]:
            pkgconfig_libs += Utils.to_list(bld.env['LIB_%s' % lib])
        if bld.env['LIBPATH_%s' % lib]:
            pkgconfig_ldflags += Utils.to_list(bld.env['LIBPATH_%s' % lib])
        if bld.env['INCLUDES_%s' % lib]:
            pkgconfig_includes += Utils.to_list(bld.env['INCLUDES_%s' % lib])
        if bld.env['LINKFLAGS_%s' % lib]:
            pkgconfig_linkflags += Utils.to_list(bld.env['LINKFLAGS_%s' % lib])
        if bld.env['CXXFLAGS_%s' % lib]:
            pkgconfig_cxxflags += Utils.to_list(bld.env['CXXFLAGS_%s' % lib])

    EXTRA_FRAMEWORKS = "";
    if bld.env['HAVE_OSX_SECURITY']:
        EXTRA_FRAMEWORKS = "-framework CoreFoundation -framework Security"

    def uniq(alist):
        set = {}
        return [set.setdefault(e,e) for e in alist if e not in set]

    pkconfig = bld(features="subst",
         source="libndn-cpp-dev.pc.in",
         target="libndn-cpp-dev.pc",
         install_path="${LIBDIR}/pkgconfig",
         VERSION=VERSION,

         # This probably not the right thing to do, but to simplify life of apps
         # that use the library
         EXTRA_LIBS=" ".join([('-l%s' % i) for i in uniq(pkgconfig_libs)]),
         EXTRA_LDFLAGS=" ".join([('-L%s' % i) for i in uniq(pkgconfig_ldflags)]),
         EXTRA_LINKFLAGS=" ".join(uniq(pkgconfig_linkflags)),
         EXTRA_INCLUDES=" ".join([('-I%s' % i) for i in uniq(pkgconfig_includes)]),
         EXTRA_CXXFLAGS=" ".join(uniq(pkgconfig_cxxflags)),
         EXTRA_FRAMEWORKS=EXTRA_FRAMEWORKS,
        )

    # Unit tests
    if bld.env['WITH_TESTS']:
        bld.recurse('tests')
        bld.recurse('tests-integrated')

    if bld.env['WITH_TOOLS']:
        bld.recurse("tools examples")
    else:
        bld.recurse("examples")

    headers = bld.path.ant_glob(['src/**/*.hpp'])
    bld.install_files("%s/ndn-cpp-dev" % bld.env['INCLUDEDIR'], headers,
                      relative_trick=True, cwd=bld.path.find_node('src'))

    bld.install_files("%s/ndn-cpp-dev" % bld.env['INCLUDEDIR'],
                      bld.path.find_resource('src/ndn-cpp-config.hpp'))

    bld.install_files("${SYSCONFDIR}/ndn", "client.conf.sample")

def doxygen(bld):
    if not bld.env.DOXYGEN:
        bld.fatal("ERROR: cannot build documentation (`doxygen' is not found in $PATH)")
    bld(features="doxygen",
        doxyfile='docs/doxygen.conf')

def sphinx(bld):
    if not bld.env.SPHINX_BUILD:
        bld.fatal("ERROR: cannot build documentation (`sphinx-build' is not found in $PATH)")
    bld(features="sphinx",
        outdir="docs/html",
        source=bld.path.ant_glob("docs/**/*.rst"),
        config="docs/source/conf.py")
