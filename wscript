# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

from waflib import Logs, Utils, Context
import os

VERSION = "0.1.0"
APPNAME = "ndn-cxx"
PACKAGE_BUGREPORT = "http://redmine.named-data.net/projects/ndn-cxx"
PACKAGE_URL = "http://named-data.net/doc/ndn-cxx/"

def options(opt):
    opt.load(['compiler_cxx', 'gnu_dirs', 'c_osx'])
    opt.load(['default-compiler-flags', 'coverage', 'osx-security', 'pch',
              'boost', 'openssl', 'cryptopp', 'sqlite3',
              'doxygen', 'sphinx_build'],
             tooldir=['.waf-tools'])

    opt = opt.add_option_group('Library Options')

    opt.add_option('--with-tests', action='store_true', default=False, dest='with_tests',
                   help='''build unit tests''')

    opt.add_option('--without-tools', action='store_false', default=True, dest='with_tools',
                   help='''Do not build tools''')

    opt.add_option('--with-examples', action='store_true', default=False, dest='with_examples',
                   help='''Build examples''')

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
    conf.env['WITH_EXAMPLES'] = conf.options.with_examples

    conf.find_program('sh', var='SH', mandatory=True)

    conf.check_cxx(lib='pthread', uselib_store='PTHREAD', define_name='HAVE_PTHREAD',
                   mandatory=False)
    conf.check_cxx(lib='rt', uselib_store='RT', define_name='HAVE_RT', mandatory=False)
    conf.check_cxx(cxxflags=['-fPIC'], uselib_store='PIC', mandatory=False)

    conf.check_osx_security(mandatory=False)

    conf.check_openssl(mandatory=True)
    conf.check_sqlite3(mandatory=True)
    conf.check_cryptopp(mandatory=True, use='PTHREAD')

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
        conf.define('HAVE_TESTS', 1)

    conf.check_boost(lib=USED_BOOST_LIBS, mandatory=True)
    if conf.env.BOOST_VERSION_NUMBER < 104800:
        Logs.error("Minimum required boost version is 1.48.0")
        Logs.error("Please upgrade your distribution or install custom boost libraries" +
                    " (http://redmine.named-data.net/projects/nfd/wiki/Boost_FAQ)")
        return

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

    conf.write_config_header('src/ndn-cxx-config.hpp', define_prefix='NDN_CXX_')

def build(bld):
    version(bld)

    bld(features="subst",
        name='version',
        source='src/version.hpp.in',
        target='src/version.hpp',
        install_path=None,
        VERSION_STRING=VERSION_BASE,
        VERSION_BUILD=VERSION,
        VERSION=int(VERSION_SPLIT[0]) * 1000000 +
                int(VERSION_SPLIT[1]) * 1000 +
                int(VERSION_SPLIT[2]),
        VERSION_MAJOR=VERSION_SPLIT[0],
        VERSION_MINOR=VERSION_SPLIT[1],
        VERSION_PATCH=VERSION_SPLIT[2],
        )

    libndn_cxx = bld(
        features=['pch', 'cxx', 'cxxstlib'], # 'cxxshlib',
        # vnum=VERSION,
        target="ndn-cxx",
        name="ndn-cxx",
        source=bld.path.ant_glob('src/**/*.cpp',
                                   excl=['src/**/*-osx.cpp', 'src/**/*-sqlite3.cpp']),
        headers='src/common-pch.hpp',
        use='version BOOST OPENSSL CRYPTOPP SQLITE3 RT PIC PTHREAD',
        includes=". src",
        export_includes="src",
        install_path='${LIBDIR}',
        )

    if bld.env['HAVE_OSX_SECURITY']:
        libndn_cxx.source += bld.path.ant_glob('src/**/*-osx.cpp')
        libndn_cxx.mac_app = True
        libndn_cxx.use += " OSX_COREFOUNDATION OSX_SECURITY"

    # In case we want to make it optional later
    libndn_cxx.source += bld.path.ant_glob('src/**/*-sqlite3.cpp')

    # Prepare flags that should go to pkgconfig file
    pkgconfig_libs = []
    pkgconfig_ldflags = []
    pkgconfig_linkflags = []
    pkgconfig_includes = []
    pkgconfig_cxxflags = []
    for lib in Utils.to_list(libndn_cxx.use):
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
         source="libndn-cxx.pc.in",
         target="libndn-cxx.pc",
         install_path="${LIBDIR}/pkgconfig",
         VERSION=VERSION_BASE,

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

    if bld.env['WITH_TOOLS']:
        bld.recurse("tools")

    if bld.env['WITH_EXAMPLES']:
        bld.recurse("examples")

    headers = bld.path.ant_glob(['src/**/*.hpp'])
    bld.install_files("%s/ndn-cxx" % bld.env['INCLUDEDIR'], headers,
                      relative_trick=True, cwd=bld.path.find_node('src'))

    bld.install_files("%s/ndn-cxx" % bld.env['INCLUDEDIR'],
                      bld.path.find_resource('src/ndn-cxx-config.hpp'))

    bld.install_files("%s/ndn-cxx" % bld.env['INCLUDEDIR'],
                      bld.path.find_resource('src/version.hpp'))

    bld.install_files("${SYSCONFDIR}/ndn", "client.conf.sample")

    if bld.env['SPHINX_BUILD']:
        bld(features="sphinx",
            builder="man",
            outdir="docs/manpages",
            config="docs/conf.py",
            source=bld.path.ant_glob('docs/manpages/**/*.rst'),
            install_path="${MANDIR}/",
            VERSION=VERSION)

def docs(bld):
    from waflib import Options
    Options.commands = ['doxygen', 'sphinx'] + Options.commands

def doxygen(bld):
    version(bld)

    if not bld.env.DOXYGEN:
        Logs.error("ERROR: cannot build documentation (`doxygen' is not found in $PATH)")
    else:
        bld(features="subst",
            name="doxygen-conf",
            source=["docs/doxygen.conf.in",
                    "docs/named_data_theme/named_data_footer-with-analytics.html.in"],
            target=["docs/doxygen.conf",
                    "docs/named_data_theme/named_data_footer-with-analytics.html"],
            VERSION=VERSION,
            HTML_FOOTER="../build/docs/named_data_theme/named_data_footer-with-analytics.html" \
                          if os.getenv('GOOGLE_ANALYTICS', None) \
                          else "../docs/named_data_theme/named_data_footer.html",
            GOOGLE_ANALYTICS=os.getenv('GOOGLE_ANALYTICS', ""),
            )

        bld(features="doxygen",
            doxyfile='docs/doxygen.conf',
            use="doxygen-conf")

def sphinx(bld):
    version(bld)

    if not bld.env.SPHINX_BUILD:
        bld.fatal("ERROR: cannot build documentation (`sphinx-build' is not found in $PATH)")
    else:
        bld(features="sphinx",
            outdir="docs",
            source=bld.path.ant_glob("docs/**/*.rst"),
            config="docs/conf.py",
            VERSION=VERSION)


def version(ctx):
    if getattr(Context.g_module, 'VERSION_BASE', None):
        return

    Context.g_module.VERSION_BASE = Context.g_module.VERSION
    Context.g_module.VERSION_SPLIT = [v for v in VERSION_BASE.split('.')]

    try:
        cmd = ['git', 'describe', '--match', 'ndn-cxx-*']
        p = Utils.subprocess.Popen(cmd, stdout=Utils.subprocess.PIPE,
                                   stderr=None, stdin=None)
        out = p.communicate()[0].strip()
        if p.returncode == 0 and out != "":
            Context.g_module.VERSION = out[8:]
    except:
        pass

def dist(ctx):
    version(ctx)

def distcheck(ctx):
    version(ctx)
