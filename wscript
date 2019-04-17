# #!/usr/bin/env python
# # encoding: utf-8
# import sys
# import os
# import fnmatch
# import glob
# sys.path.insert(0, sys.path[0]+'/waf_tools')

# VERSION = '0.0.1'
# APPNAME = 'fish_in_ring'

# srcdir = '.'
# incdir = 'src/'
# blddir = 'build'

# from waflib.Build import BuildContext
# from waflib import Logs
# import eigen
# import boost


# def options(opt):
#     opt.load('compiler_cxx waf_unit_test')
#     opt.load('eigen')
#     opt.load('boost')


# def configure(conf):
#     conf.load('compiler_cxx waf_unit_test')
#     conf.load('eigen')
#     conf.load('boost')

#     conf.check_eigen()
#     conf.check_boost(lib='regex system filesystem', min_version='1.46')
#     conf.env.append_value('INCLUDES', [incdir])

#     # we want C++11
#     if conf.env.CXX_NAME in ["icc", "icpc"]:
#         common_flags = "-Wall -std=c++11"
#         opt_flags = " -O3 -xHost -mtune=native -unroll -g"
#     elif conf.env.CXX_NAME in ["clang"]:
#         common_flags = "-Wall -std=c++11"
#         opt_flags = " -O3 -march=native -g"
#     else:
#         if int(conf.env['CC_VERSION'][0]+conf.env['CC_VERSION'][1]) < 47:
#             common_flags = "-Wall -std=c++0x"
#         else:
#             common_flags = "-Wall -std=c++11"
#         opt_flags = " -O3 -march=native -g"
#     common_flags += " -Wextra"

#     all_flags = common_flags + opt_flags
#     conf.env['CXXFLAGS'] = conf.env['CXXFLAGS'] + all_flags.split(' ')
#     print(conf.env['CXXFLAGS'])

#     conf.env.LIB_PARTICLE_SIMU = ['simu']
#     conf.env.LIBPATH_PARTICLE_SIMU = [os.path.join(os.getcwd(), 'build/')]
#     conf.env.INCLUDES_PARTICLE_SIMU = [os.path.join(os.getcwd(), 'src/')]


# def build(bld):
#     srcs = []
#     incs = ['src/']

#     nodes = bld.path.ant_glob('src/**/*.cpp', src=True, dir=False)
#     for n in nodes:
#         srcs += [n.bldpath()]

#     bld.stlib(features='cxx cxxstlib',
#               source=srcs,
#               cxxflags=['-O3', '-fPIC', '-rdynamic'],
#               uselib='BOOST BOOST_SYSTEM BOOST_FILESYSTEM BOOST_REGEX EIGEN',
#               target='simu')

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
                   help='create a new animal', dest='create_exp')
    opt.add_option('--animals', type='string',
                   help='animal(s) to build, separate by comma', dest='animal')
    opt.add_option('--examples', action='store_true', default=False,
                   help='build examples', dest='examples')
    opt.add_option('--cpp14', action='store_true', default=False,
                   help='force c++-14 compilation [--cpp14]', dest='cpp14')
    opt.add_option('--no-native', action='store_true', default=False,
                   help='disable -march=native, which can cause some troubles [--no-native]', dest='no_native')

    try:
        os.mkdir(blddir)
    except:
        print("build dir not created (it probably already exists, this is fine)")
    opt.logger = Logs.make_logger(blddir + '/options.log', 'optlogger')

    for i in glob.glob('animals/*'):
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

    is_cpp14 = conf.options.cpp14
    if is_cpp14:
        is_cpp14 = conf.check_cxx(
            cxxflags="-std=c++14", mandatory=False, msg='Checking for C++14')
        if not is_cpp14:
            conf.msg('C++14 is requested, but your compiler does not support it!',
                     'Disabling it!', color='RED')
    if conf.env.CXX_NAME in ["icc", "icpc"]:
        common_flags = "-Wall -std=c++11"
        opt_flags = " -O3 -xHost -g"
        native_flags = "-mtune=native -unroll -fma"
    else:
        if conf.env.CXX_NAME in ["gcc", "g++"] and int(conf.env['CC_VERSION'][0]+conf.env['CC_VERSION'][1]) < 47:
            common_flags = "-Wall -std=c++0x"
        else:
            common_flags = "-Wall -std=c++11"
        if conf.env.CXX_NAME in ["clang", "llvm"]:
            common_flags += " -fdiagnostics-color"
        opt_flags = " -O3 -g"

    if is_cpp14:
        common_flags = common_flags + " -std=c++14"

    conf.env.INCLUDES_AEGEAN = os.path.dirname(os.path.abspath(
        inspect.getfile(inspect.currentframe()))) + "/src"
    conf.env.LIBRARIES = 'BOOST EIGEN'

    all_flags = common_flags + opt_flags
    conf.env['CXXFLAGS'] = conf.env['CXXFLAGS'] + all_flags.split(' ')
    conf.env.append_value("LINKFLAGS", ["-pthread"])
    Logs.pprint('NORMAL', 'CXXFLAGS: %s' %
                (conf.env['CXXFLAGS'] + conf.env['CXXFLAGS_OMP']))

    if conf.options.animal:
        for i in conf.options.animal.split(','):
            Logs.pprint('NORMAL', 'configuring for animal: %s' % i)
            conf.recurse('animal/' + i)


def build(bld):
    if bld.options.animal:
        for i in bld.options.animal.split(','):
            Logs.pprint('NORMAL', 'Building animal: %s' % i)
            bld.recurse('animals/' + i)


def shutdown(ctx):
    if ctx.options.create_exp:
        particle_simu.create_exp(ctx.options.create_exp, ctx.options)

