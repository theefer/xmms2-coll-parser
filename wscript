# encoding: utf-8
#
# WAF build script for xmmsclient-coll-parser

import os, sys

import Utils, Options

APPNAME = 'xm-coll-parser'
VERSION = '0.1'

srcdir = '.'
blddir = '_build'
abs_srcdir = os.path.abspath(srcdir)
abs_blddir = os.path.join(abs_srcdir, blddir)
abs_parser = os.path.join(abs_srcdir, 'src', 'parser-c')

# Initialization
def init():
  pass

def build(bld):
  lib = bld(
      features = 'cshlib cc',
      includes = 'src',
      target = APPNAME,
      uselib = "xmmsclient")
  if bld.env['NO_BISON']:
    lib.find_sources_in_dirs(['src', 'src/parser-c'], [], ['.c'])
    lib.includes += ' src/parser-c'
  else:
    lib.find_sources_in_dirs('src', [], ['.l', '.y', '.c'])

  if bld.env['BUILD_TEST']:
    app = bld(
        features = 'cprogram cc',
        includes = 'src/',
        target = 'coll-test',
        source = 'src/test/test.c',
        uselib = "glib",
        uselib_local = APPNAME)

  if Options.options.save_parser and not bld.env['NO_BISON']:
    # XXX This is not beautiful, but at least, it works.
    bld.add_group()
    bld(rule = "cp ${SRC} %s && touch ${TGT}" % abs_parser,
        source = "src/parser.tab.c src/parser.tab.h src/scanner.lex.c",
        target = "src/parser.saved")


def configure(conf):
  conf.check_tool('misc gnu_dirs gcc')

  if Options.options.no_bison:
    conf.env['NO_BISON'] = True
  else:
    conf.check_tool('flex bison')
    if not conf.env['BISON'] or not conf.env['FLEX']:
      conf.env['NO_BISON'] = True
    else:
      conf.env['NO_BISON'] = False

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
  opt.add_option('--save-parser', action="store_true",
      dest='save_parser', default=False,
      help="Save generated Bison/Flex C-code.")
  opt.add_option('--without-bison-flex', action="store_true", dest='no_bison', default=False,
      help="Use pre-generated C-code instead of Bison/Flex sources. [default: autodetect]")


def shutdown():
  pass

