# encoding: utf-8
#
# WAF build script for xmmsclient-coll-parser

import os, sys

import Utils, Options

APPNAME = 'xm-coll-parser'
VERSION = '0.1'

srcdir = '.'
blddir = '_build'

# Initialization
def init():
  pass

def build(bld):
  lib = bld(
      features = 'cshlib cc',
      includes = 'src/',
      target = APPNAME,
      uselib = "xmmsclient")
  lib.find_sources_in_dirs('src/', ['test'], ['.l', '.y', '.c'])

  if bld.env['BUILD_TEST']:
    app = bld(
        features = 'cprogram cc',
        includes = 'src/',
        target = 'coll-test',
        source = 'src/test/test.c',
        uselib = "glib",
        uselib_local = APPNAME)

def configure(conf):
  conf.check_tool('misc gnu_dirs gcc')
  conf.check_tool('flex bison')
  conf.check_cfg(
      package='xmms2-client',
      atleast_version="0.6",
      uselib_store="xmmsclient",
      args='--cflags --libs',
      mantadoty=True)
  if conf.check_cfg(
      package='glib-2.0',
      uselib_store="glib",
      args='--cflags --libs'):
    conf.env['BUILD_TEST'] = True
  else:
    conf.env['BUILD_TEST'] = False
  flags_list = Utils.to_list(conf.env['CCFLAGS'])
  if Options.options.debug:
    conf.env['CCFLAGS'] = '-O0 -g3 -Wall -pedantic --std=gnu99'.split()
  else:
    conf.env['CCFLAGS'] = '-O2 -Wall'.split()
  # Command-line cflags, such as -O* takes priority when they appear last.
  for c in flags_list:
    conf.env.append_unique('CCFLAGS', c)

def set_options(opt):
  opt.tool_options('gnu_dirs')
  opt.tool_options('gcc')
  opt.add_option('--debug', action="store_true", dest='debug', default=False,
                 help="Build with debug CFLAGS.")

def shutdown():
  pass

