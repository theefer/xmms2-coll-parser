# encoding: utf-8
#
# WAF build script for xmmsclient-coll-parser

import os, sys

import Utils

APPNAME = 'xm-coll-parser'
VERSION = '0.1'

srcdir = '.'
blddir = '_build'

# Initialization
def init():
  pass

def build(bld):
  lib = bld.new_task_gen(
      features = 'cshlib cc',
      includes = 'src/',
      target = APPNAME,
      uselib = "xmmsclient")
  lib.find_sources_in_dirs('src/', ['test.c'], ['.l', '.y', '.c'])

  #app = bld.new_task_gen(
  #    features = 'cprogram cc',
  #    includes = 'src/',
  #    target = 'coll-test',
  #    source = 'coll-test.c',
  #    uselib_local = APPNAME)

def configure(conf):
  conf.check_tool('misc gnu_dirs gcc')
  conf.check_tool('flex bison')
  conf.check_cfg(
      package='xmms2-client',
      atleast_version="0.6",
      uselib_store="xmmsclient",
      args='--cflags --libs',
      mantadoty=True)

def set_options(opt):
  opt.tool_options('gnu_dirs')
  opt.tool_options('gcc')

def shutdown():
  pass

