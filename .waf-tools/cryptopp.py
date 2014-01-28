#! /usr/bin/env python
# encoding: utf-8

'''

When using this tool, the wscript will look like:

	def options(opt):
	        opt.tool_options('cryptopp', tooldir=["waf-tools"])

	def configure(conf):
		conf.load('compiler_cxx cryptopp')

	def build(bld):
		bld(source='main.cpp', target='app', use='CRYPTOPP')

Options are generated, in order to specify the location of cryptopp includes/libraries.


'''
import sys
import re
from waflib import Utils,Logs,Errors
from waflib.Configure import conf
CRYPTOPP_DIR=['/usr','/usr/local','/opt/local','/sw']
CRYPTOPP_VERSION_FILE='config.h'
CRYPTOPP_VERSION_CODE='''
#include <iostream>
#include <cryptopp/config.h>
int main() { std::cout << CRYPTOPP_VERSION; }
'''

def options(opt):
	opt.add_option('--cryptopp',type='string',default='',dest='cryptopp_dir',help='''path to where cryptopp is installed, e.g. /opt/local''')
@conf
def __cryptopp_get_version_file(self,dir):
	try:
		return self.root.find_dir(dir).find_node('%s/%s' % ('include/cryptopp', CRYPTOPP_VERSION_FILE))
	except:
		return None
@conf
def cryptopp_get_version(self,dir):
	val=self.check_cxx(fragment=CRYPTOPP_VERSION_CODE,includes=['%s/%s' % (dir, 'include')], execute=True, define_ret = True, mandatory=True)
	return val
@conf
def cryptopp_get_root(self,*k,**kw):
	root=k and k[0]or kw.get('path',None)
	# Logs.pprint ('RED', '   %s' %root)
	if root and self.__cryptopp_get_version_file(root):
		return root
	for dir in CRYPTOPP_DIR:
		if self.__cryptopp_get_version_file(dir):
			return dir
	if root:
		self.fatal('CryptoPP not found in %s'%root)
	else:
		self.fatal('CryptoPP not found, please provide a --cryptopp argument (see help)')
@conf
def check_cryptopp(self,*k,**kw):
	if not self.env['CXX']:
		self.fatal('load a c++ compiler first, conf.load("compiler_cxx")')

	var=kw.get('uselib_store','CRYPTOPP')
	self.start_msg('Checking Crypto++ lib')
	root = self.cryptopp_get_root(*k,**kw)
	self.env.CRYPTOPP_VERSION=self.cryptopp_get_version(root)

	self.env['INCLUDES_%s'%var]= '%s/%s' % (root, "include")
	self.env['LIB_%s'%var] = "cryptopp"
	self.env['LIBPATH_%s'%var] = '%s/%s' % (root, "lib")

	self.end_msg(self.env.CRYPTOPP_VERSION)
	if Logs.verbose:
		Logs.pprint('CYAN','	CRYPTOPP include : %s'%self.env['INCLUDES_%s'%var])
		Logs.pprint('CYAN','	CRYPTOPP lib     : %s'%self.env['LIB_%s'%var])
		Logs.pprint('CYAN','	CRYPTOPP libpath : %s'%self.env['LIBPATH_%s'%var])

