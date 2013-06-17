#! /usr/bin/env python
# encoding: utf-8

'''

When using this tool, the wscript will look like:

	def options(opt):
	        opt.tool_options('tinyxml', tooldir=["waf-tools"])

	def configure(conf):
		conf.load('compiler_cxx tiny')

	def build(bld):
		bld(source='main.cpp', target='app', use='TINYXML')

Options are generated, in order to specify the location of tinyxml includes/libraries.


'''
import sys
import re
from waflib import Utils,Logs,Errors
from waflib.Configure import conf
TINYXML_DIR=['/usr','/usr/local','/opt/local','/sw']
TINYXML_VERSION_FILE='tinyxml.h'
TINYXML_VERSION_CODE='''
#include <iostream>
#include <tinyxml.h>
int main() { std::cout << TIXML_MAJOR_VERSION << "." << TIXML_MINOR_VERSION << "." << TIXML_PATCH_VERSION; }
'''

def options(opt):
	opt.add_option('--tinyxml',type='string',default='',dest='tinyxml_dir',help='''path to where TinyXML is installed, e.g. /usr/local''')
@conf
def __tinyxml_get_version_file(self,dir):
	try:
		return self.root.find_dir(dir).find_node('%s/%s' % ('include', TINYXML_VERSION_FILE))
	except:
		return None
@conf
def tinyxml_get_version(self,dir):
	val=self.check_cxx(fragment=TINYXML_VERSION_CODE,includes=['%s/%s' % (dir, 'include')], execute=True, define_ret = True, mandatory=True)
	return val
@conf
def tinyxml_get_root(self,*k,**kw):
	root=k and k[0]or kw.get('path',None)
	# Logs.pprint ('RED', '   %s' %root)
	if root and self.__tinyxml_get_version_file(root):
		return root
	for dir in TINYXML_DIR:
		if self.__tinyxml_get_version_file(dir):
			return dir
	if root:
		self.fatal('TinyXML not found in %s'%root)
	else:
		self.fatal('TinyXML not found, please provide a --tinyxml argument (see help)')
@conf
def check_tinyxml(self,*k,**kw):
	if not self.env['CXX']:
		self.fatal('load a c++ compiler first, conf.load("compiler_cxx")')

	var=kw.get('uselib_store','TINYXML')
	self.start_msg('Checking TinyXML')
	root = self.tinyxml_get_root(*k,**kw);
	self.env.TINYXML_VERSION=self.tinyxml_get_version(root)

	self.env['INCLUDES_%s'%var]= '%s/%s' % (root, "include");
	self.env['LIB_%s'%var] = "tinyxml"
	self.env['LIBPATH_%s'%var] = '%s/%s' % (root, "lib")

	self.end_msg(self.env.TINYXML_VERSION)
	if Logs.verbose:
		Logs.pprint('CYAN','	TinyXML include : %s'%self.env['INCLUDES_%s'%var])
		Logs.pprint('CYAN','	TinyXML lib     : %s'%self.env['LIB_%s'%var])
		Logs.pprint('CYAN','	TinyXML libpath : %s'%self.env['LIBPATH_%s'%var])
