# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

from waflib import Utils

def options(opt):
    opt.add_option('--build', default=Utils.unversioned_sys_platform(),
                   help='Build platform that is doing the actual compilation (e.g., darwin)')
    opt.add_option('--host', default=Utils.unversioned_sys_platform(),
                   help='Host platform on which the compiled binary will run (e.g., android)')
    # opt.add_option('--target', default=Utils.unversioned_sys_platform(),
    #                help='Target platform on which the compiled binary's output will run')

def configure(conf):
    conf.env.BUILD = conf.options.build
    conf.env.HOST = conf.options.host
    # conf.env.TARGET = conf.options.target

    conf.start_msg('Build platform')
    conf.end_msg(conf.env.BUILD)

    conf.start_msg('Host platform')
    conf.end_msg(conf.env.HOST)

    # conf.start_msg('Target platform')
    # conf.end_msg(conf.env.TARGET)
