# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

top = '..'

def build(bld):
    # List all .cpp files (whole example should be in one .cpp)
    for i in bld.path.ant_glob(['*.cpp']):
        name = str(i)[:-len(".cpp")]
        bld(features=['cxx', 'cxxprogram'],
            target=name,
            source=[i] + bld.path.ant_glob(['%s/**/*.cpp' % name]),
            use='ndn-cxx',
            install_path=None
            )

    # List all directories files (example can has multiple .cpp in the directory)
    for name in bld.path.ant_glob(['*'], dir=True, src=False):
        bld(features=['cxx', 'cxxprogram'],
            target="%s/%s" % (name, name),
            source=bld.path.ant_glob(['%s/**/*.cpp' % name]),
            use='ndn-cxx',
            install_path=None,
            includes='%s' % name,
            )
