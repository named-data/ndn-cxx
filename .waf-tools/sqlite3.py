#! /usr/bin/env python
# encoding: utf-8

from waflib import Options, Logs
from waflib.Configure import conf

def options(opt):
    opt.add_option('--with-sqlite3', type='string', default=None,
                   dest='with_sqlite3', help='''Path to SQLite3, e.g., /usr/local''')

@conf
def check_sqlite3(self, *k, **kw):
    root = k and k[0] or kw.get('path', None) or Options.options.with_sqlite3
    mandatory = kw.get('mandatory', True)
    var = kw.get('uselib_store', 'SQLITE3')

    if root:
        self.check_cxx(lib='sqlite3',
                       msg='Checking for SQLite3 library',
                       define_name='HAVE_%s' % var,
                       uselib_store=var,
                       mandatory=mandatory,
                       includes="%s/include" % root,
                       libpath="%s/lib" % root)
    else:
        try:
            self.check_cfg(package='sqlite3',
                           args=['--cflags', '--libs'],
                           global_define=True,
                           define_name='HAVE_%s' % var,
                           uselib_store='SQLITE3',
                           mandatory=True)
        except:
            self.check_cxx(lib='sqlite3',
                           msg='Checking for SQLite3 library',
                           define_name='HAVE_%s' % var,
                           uselib_store=var,
                           mandatory=mandatory)
