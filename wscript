#!/usr/bin/env python
# encoding: utf-8
import sys
import os
import fnmatch
import glob
sys.path.insert(0, sys.path[0]+'/waf_tools')

VERSION = '0.0.1'
APPNAME = 'fish_in_ring'

srcdir = '.'
incdir = 'src/'
blddir = 'build'

from waflib.Build import BuildContext
from waflib import Logs
import eigen
import boost


def options(opt):
    opt.load('compiler_cxx waf_unit_test')
#    opt.load('eigen')
    opt.load('boost')

def configure(conf):
    conf.load('compiler_cxx waf_unit_test')
#    conf.load('eigen')
    conf.load('boost')

#    conf.check_eigen()
    conf.check_boost(lib='regex system filesystem', min_version='1.46')
    conf.env.append_value('INCLUDES', [incdir])

    # we want C++11
    if conf.env.CXX_NAME in ["icc", "icpc"]:
        common_flags = "-Wall -std=c++11"
        opt_flags = " -O3 -xHost -mtune=native -unroll -g"
    elif conf.env.CXX_NAME in ["clang"]:
        common_flags = "-Wall -std=c++11"
        opt_flags = " -O3 -march=native -g"
    else:
        if int(conf.env['CC_VERSION'][0]+conf.env['CC_VERSION'][1]) < 47:
            common_flags = "-Wall -std=c++0x"
        else:
            common_flags = "-Wall -std=c++11"
        opt_flags = " -O3 -march=native -g"
    common_flags += " -Wextra"

    all_flags = common_flags + opt_flags
    conf.env['CXXFLAGS'] = conf.env['CXXFLAGS'] + all_flags.split(' ')
    print(conf.env['CXXFLAGS'])

    conf.env.LIB_SAMSAR = ['samsar']
    conf.env.LIBPATH_SAMSAR = [os.path.join(os.getcwd(), 'build/samsar')]
    conf.env.INCLUDES_SAMSAR = [os.path.join(os.getcwd(), 'src/')]

def build(bld):
    srcs = []
    incs = ['src/']

    nodes = bld.path.ant_glob('src/**/*.cpp', src=True, dir=False)
    for n in nodes:
        srcs += [n.bldpath()]

    bld.stlib(features='cxx cxxstlib',
        source=srcs,
        cxxflags=['-O3', '-fPIC', '-rdynamic'],
        uselib='BOOST BOOST_SYSTEM BOOST_FILESYSTEM BOOST_REGEX EIGEN',
        target='samsar/samsar')

    bld.program(features = 'cxx',
        source=srcs+['examples/fish_ring_example.cpp'],
        includes=incs,
        uselib='BOOST BOOST_SYSTEM BOOST_FILESYSTEM BOOST_REGEX EIGEN',
        target='fish_ring_example')

    bld.program(features = 'cxx',
        source=srcs+['examples/mixed_society.cpp'],
        includes=incs,
        uselib='BOOST BOOST_SYSTEM BOOST_FILESYSTEM BOOST_REGEX EIGEN',
        target='mixed_society')
