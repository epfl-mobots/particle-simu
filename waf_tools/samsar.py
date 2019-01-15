#! /usr/bin/env python
# encoding: utf-8
# Vaios Papaspyros - 2018

"""
Quick n dirty samsar detection
"""

import os
import glob
import types
import subprocess
from waflib.Configure import conf


def options(opt):
    opt.add_option('--samsar', type='string',
                   help='path to samsar', dest='samsar')


@conf
def check_samsar(conf, *k, **kw):
    includes_check = ['/usr/include/samsar',
                      '/usr/local/include/samsar', '/usr/include', '/usr/local/include']
    envincs = os.getenv('SAMSAR_INCLUDE_DIR')
    if envincs:
        includes_check += [envincs]
    required = kw.get('required', False)

    if conf.options.eigen:
        includes_check = [conf.options.samsar]

    conf.start_msg('Checking for SAMSAR includes')
    try:
        res = conf.find_file('simulation/simulation.hpp', includes_check)
        incl = res[:-len('simulation/simulation.hpp')-1]
        conf.env.INCLUDES_SAMSAR = [incl]
        conf.end_msg(incl)
    except:
        if required:
            conf.fatal('Not found in %s' % str(includes_check))
        conf.end_msg('Not found in %s' % str(includes_check), 'RED')

    libs_check = ['/usr/lib/samsar', '/usr/local/lib/samsar', '/usr/lib',
                  '/usr/local/lib'] + [envincs[:-len(envincs.split('/')[-1])-1]]
    libs_check = [l + '/build/samsar' for l in libs_check]
    conf.start_msg('Checking for SAMSAR libs')
    try:
        res = conf.find_file('libsamsar.a', libs_check)
        lib = res[:-len('libsamsar.a')-1]
        conf.env.LIBPATH_SAMSAR = [lib]
        conf.env.STLIB_SAMSAR = ['samsar']
        conf.env.LIB_SAMSAR = ['samsar']
        conf.end_msg(lib)
    except:
        if required:
            conf.fatal('Not found in %s' % str(libs_check))
        conf.end_msg('Not found in %s' % str(libs_check), 'RED')
    return 1
