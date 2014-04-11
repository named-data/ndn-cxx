#! /usr/bin/env python
# encoding: utf-8

from waflib import Logs, Utils, Task, TaskGen
from waflib.Tools import c_preproc

def options(opt):
    opt.add_option('--with-pch', action='store_true', default=False, dest='with_pch',
                   help='''Try to use precompiled header to speed up compilation '''
                        '''(only gcc and clang)''')

def configure(conf):
    conf.env['WITH_PCH'] = conf.options.with_pch


@TaskGen.feature('cxx')
@TaskGen.before('process_source')
def process_pch(self):
    if getattr(self, 'pch', ''):
        # for now support only gcc-compatible things
        if self.env['COMPILER_CXX'] == 'g++':
            nodes = self.to_nodes(self.pch, path=self.path)
            for x in nodes:
                z = self.create_task('gchx', x, x.change_ext('.hpp.gch'))
                z.orig_self = self

class gchx(Task.Task):
    run_str = '${CXX} -x c++-header ${CXXFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ' + \
                '${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ' + \
                '${CXX_SRC_F}${SRC} ${CXX_TGT_F}${TGT}'
    scan    = c_preproc.scan
    ext_out = ['.hpp']
    color   = 'BLUE'

    def post_run(self):
        super(gchx, self).post_run()
        self.orig_self.env['CXXFLAGS'] = ['-include', self.inputs[0].relpath()] + \
                                         self.env['CXXFLAGS']
