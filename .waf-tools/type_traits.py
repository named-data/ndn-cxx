# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def checkForTypeProperty(conf, prop, tparams):
    if conf.check_cxx(msg=('Checking for std::%s' % prop),
                      fragment=('#include <type_traits>\nstatic_assert(std::%s<%s>::value, "");' %
                                (prop, tparams)),
                      features='cxx', mandatory=False):
        define = 'HAVE_' + prop.upper()
        conf.define(define, 1)
        conf.env[define] = True

def configure(conf):
    checkForTypeProperty(conf, 'is_default_constructible', 'int')
    checkForTypeProperty(conf, 'is_nothrow_move_constructible', 'int')
    checkForTypeProperty(conf, 'is_nothrow_move_assignable', 'int')
