#!/usr/bin/env python
# encoding: utf-8

import sys
sys.path.insert(0, sys.path[0] + '/waf_tools')

VERSION = '0.0.1'
APPNAME = 'particle_simu'

srcdir = '.'
blddir = 'build'

import os
import fnmatch
import glob
import inspect
import particle_simu

from waflib import Logs
from waflib.Errors import WafError
from waflib.Build import BuildContext
from waflib.Tools import waf_unit_test


def options(opt):
    opt.load('compiler_cxx')
    opt.load('compiler_c')
    opt.load('boost')
    opt.load('eigen')

    opt.add_option('--create', type='string',
                   help='create a new exp', dest='create_exp')
    opt.add_option('--exp', type='string',
                   help='exp(s) to build, separate by comma', dest='exp')
    opt.add_option('--examples', action='store_true', default=False,
                   help='build examples', dest='examples')
    opt.add_option('--no-native', action='store_true', default=False,
                   help='disable -march=native, which can cause some troubles [--no-native]', dest='no_native')

    try:
        os.mkdir(blddir)
    except:
        print("build dir not created (it probably already exists, this is fine)")
    opt.logger = Logs.make_logger(blddir + '/options.log', 'optlogger')

    for i in glob.glob('exp/*'):
        if os.path.isdir(i):
            opt.start_msg('command-line options for [%s]' % i)
            try:
                opt.recurse(i)
                opt.end_msg(' -> OK')
            except WafError:
                opt.end_msg(' -> no options found')


def configure(conf):
    conf.load('compiler_cxx')
    conf.load('compiler_c')
    conf.load('waf_unit_test')
    conf.load('boost')
    conf.load('eigen')

    conf.check_boost(
        lib='regex system filesystem unit_test_framework', min_version='1.46')
    conf.check_eigen(required=True)

    native_flags = "-march=native"

    if conf.env.CXX_NAME in ["icc", "icpc"]:
        common_flags = "-Wall -std=c++17"
        opt_flags = " -O3 -xHost -g"
        native_flags = "-mtune=native -unroll -fma"
    else:
        if conf.env.CXX_NAME in ["gcc", "g++"] and int(conf.env['CC_VERSION'][0]+conf.env['CC_VERSION'][1]) < 47:
            common_flags = "-Wall -std=c++0x"
        else:
            common_flags = "-Wall -std=c++17"
        if conf.env.CXX_NAME in ["clang", "llvm"]:
            common_flags += " -fdiagnostics-color"
        opt_flags = " -O3 -g"

    conf.env.INCLUDES_AEGEAN = os.path.dirname(os.path.abspath(
        inspect.getfile(inspect.currentframe()))) + "/src"
    conf.env.LIBRARIES = 'BOOST EIGEN'

    all_flags = common_flags + opt_flags
    conf.env['CXXFLAGS'] = conf.env['CXXFLAGS'] + all_flags.split(' ')
    conf.env.append_value("LINKFLAGS", ["-pthread"])
    Logs.pprint('NORMAL', 'CXXFLAGS: %s' %
                (conf.env['CXXFLAGS'] + conf.env['CXXFLAGS_OMP']))

    if conf.options.exp:
        for i in conf.options.exp.split(','):
            Logs.pprint('NORMAL', 'configuring for exp: %s' % i)
            conf.recurse('exp/' + i)


def build(bld):
    if bld.options.exp:
        for i in bld.options.exp.split(','):
            Logs.pprint('NORMAL', 'Building exp: %s' % i)
            bld.recurse('exp/' + i)


def shutdown(ctx):
    if ctx.options.create_exp:
        particle_simu.create_exp(ctx.options.create_exp, ctx.options)
