#! /usr/bin/env python
# encoding: utf-8
# Vaios Papaspyros - 2018

"""
Quick n dirty samsar detection
"""

import os, glob, types
import subprocess
from waflib.Configure import conf


def options(opt):
    opt.add_option('--samsar', type='string', help='path to samsar', dest='samsar')

@conf
def check_samsar(conf, *k, **kw):
    conf.start_msg('Checking for SAMSAR')
    includes_check = ['/usr/include/samsar', '/usr/local/include/samsar', '/usr/include', '/usr/local/include']
    envincs = [os.getenv('SAMSAR_INCLUDE_DIR')]
    print(envincs)
    if envincs:
        includes_check += envincs
    required = kw.get('required', False)

    if conf.options.eigen:
        includes_check = [conf.options.samsar]

    try:
        res = conf.find_file('simulation/simulation_base.hpp', includes_check)
        incl = res
        conf.env.INCLUDES_SAMSAR = [incl]
        conf.end_msg(incl)
    except:
        if required:
            conf.fatal('Not found in %s' % str(includes_check))
        conf.end_msg('Not found in %s' % str(includes_check), 'RED')
    return 1
