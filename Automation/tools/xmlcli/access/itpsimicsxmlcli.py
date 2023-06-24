#!/usr/bin/env python2.7
# Cscripts remove start
__author__ = 'ashinde'
# Cscripts remove end
from . import cliaccessutil as _cliutil
import time as _time
import binascii as _binascii
import types as _types
import os as _os

########### Code to run @ import time ##############
import itpii
from itpii.datatypes import *

try:
	import common.toolbox as _tools
except ImportError:
	import tools.toolbox as _tools

_log = _tools.getLogger("itpsimics")
_log.setFile("itpsimics.log",dynamic=True)
_log.setFileFormat("simple")
_log.setFileLevel("info")
_log.setConsoleLevel("result")

class itpSimicsAccess(_cliutil.cliaccess):
	def __init__(self):
		super(itpSimicsAccess,self).__init__("itpsimics")
		self.__dict__["level_Dal"] = 0
		self.__dict__["IsRunning"] = 0x00       # initialize IsRunnning [bitmap] global variable to False for all bits
		self.__dict__["itp"] = itpii.baseaccess()
		self.__dict__["thread"] = None
		self.itp.vp = 0
		self.itp.base = 16 if not hasattr(self.itp, 'base') else self.itp.base
		threadlist=self.getAliveThreads()
		self.setThread(threadlist[0])

	def haltcpu(self, delay=0):
		_time.sleep(delay)
		if(not self.itp.threads):
			msg="haltcpu: Really? There are no existing CPU threads?  Really?  Thanks, DAL."
			_log.debug(msg)
			raise Exception(msg)
			return 1
		if(self.thread.cv.isrunning):
			_log.debug("haltcpu: CPU is running, issuing halt()")
			try:
				self.itp.halt()
			except:
				_log.result("haltcpu: issue in halting threads")
			_log.debug("haltcpu: CPU 0 is now halted")
		if(self.thread.cv.isrunning == False):
			pass

	def runcpu(self):
		if(self.thread.cv.isrunning):
			_log.info("runcpu: CPU is already running")
		else:
			_log.debug("runcpu: CPU is halted, issuing go()")
			self.itp.go()
			_log.debug("runcpu: CPU is running now")
		return 0

	def InitInterface(self):
		threadlist=self.getAliveThreads()
		self.setThread(threadlist[0])
		self.IsRunning = (self.IsRunning & (~(0x1 << self.level_Dal) & 0xFF)) + ((self.thread.cv.isrunning & 0x1) << self.level_Dal )
		self.haltcpu()
		self.level_Dal = self.level_Dal + 1
		return 0

	def CloseInterface(self):
		if ( (self.IsRunning >> (self.level_Dal-1)) & 0x1 ):
			self.runcpu()
		else:
			self.haltcpu()
		self.level_Dal = self.level_Dal - 1
		return 0

	def warmreset(self):
		self.itp.resettarget()

	def coldreset(self):
		self.itp.pulsepwrgood()

	def memBlock(self, address, size):
		if(size <= 0x400):
			result = self.thread.memblock(hex(address).rstrip("L")+'p',size,0)
		else:
			Remainder = size % 0x400
			OneKBlks = size / 0x400
			result = self.thread.memblock(hex(address).rstrip("L")+'p',0x400,0)
			for blkcnt in range(0x1,(OneKBlks),1):
				result = result + self.thread.memblock(hex(address+(blkcnt*0x400)).rstrip("L")+'p',0x400,0)
			if (Remainder > 0):
				result = result + self.thread.memblock(hex(address+size-Remainder).rstrip("L")+'p',Remainder,0)
		return _binascii.unhexlify((hex(result)[2:]).zfill(size*2))[::-1]

	def memsave(self, filename, address, size):
		#Due to a bug in IPC (Latterbach) relative pathnames do not resolve correctly. To adjust for this, all files must be absolute
		if not _os.path.isabs(filename):
			filename = _os.path.abspath(filename)
		self.thread.memsave(filename, hex(address).rstrip("L")+'p', size, 1)

	def memread(self, address, size):
		return self.thread.mem(hex(address).rstrip("L")+'p', size)

	def memwrite(self, address, size, value):
		self.thread.mem(hex(address).rstrip("L")+'p', size, value)

	def load_data(self, filename, address):
		#Due to a bug in IPC (Latterbach) relative pathnames do not resolve correctly. To adjust for this, all files must be absolute
		if not _os.path.isabs(filename):
			filename = _os.path.abspath(filename)
		self.thread.memload(filename, hex(address).rstrip("L")+'p')

	def readIO(self, address, size):
		if(size == 1): return self.thread.port(address)
		if(size == 2): return self.thread.wport(address)
		if(size == 4): return self.thread.dport(address)

	def writeIO(self, address, size, value):
		if(size == 1): self.thread.port(address, value)
		if(size == 2): self.thread.wport(address, value)
		if(size == 4): self.thread.dport(address, value)

	def triggerSMI(self, SmiVal):
		self.haltcpu()
		self.writeIO(0xB2, 1, SmiVal)
		self.runcpu()

	def ReadMSR(self, Ap, MSR_Addr):
		return self.itp.threads[Ap].msr(MSR_Addr)

	def WriteMSR(self, Ap, MSR_Addr, MSR_Val):
		return self.itp.threads[Ap].msr(MSR_Addr, MSR_Val)

	def ReadSmbase(self):
		self.haltcpu()
		return self.thread.msr(0x171)

	def getAliveThreads(self):
		return filter(isThreadAlive,self.itp.threads)

	def setThread(self,thread):
		self.thread = thread

def isThreadAlive(thread):
	return (thread.cv.isenabled)
