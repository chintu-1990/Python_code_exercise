#!/usr/bin/env python2.7
__author__ = 'ashinde'
from . import cliaccessutil as _cliutil
import binascii as _binascii
import types as _types
import ctypes as _ctypes
from ctypes import *

########### Code to run @ import time ##############
import sys as _sys
import os as _os
basepath = _os.getcwd()
if (_sys.maxsize == 0x7fffffffffffffff):
	HwApiDllFile = r"c:\Program Files\Intel Corporation\Intel(R)CCBSDK\Dll\Winx64\Release\HWAPIDLL.dll"
else:
	HwApiDllFile = r"c:\Program Files\Intel Corporation\Intel(R)CCBSDK\Dll\Winx86\Release\HWAPIDLL.dll"
HwApiDll = _ctypes.CDLL(HwApiDllFile)

class winhwapiAccess(_cliutil.cliaccess):
	def __init__(self):
		super(winhwapiAccess,self).__init__("winhwapi")
		self.__dict__["_HwApiStatus"] = 0
		self.__dict__["_HwApiInitLvl"] = 0

	def haltcpu(self, delay=0):
		return 0

	def runcpu(self):
		return 0

	def InitInterface(self):
		self._HwApiInitLvl = self._HwApiInitLvl + 1
		if (self._HwApiStatus == 0):
			RetStatus = HwApiDll.HWAPIInitialize()
			self._HwApiStatus = 1
			if (RetStatus != 1):
				return 1
		return 0

	def CloseInterface(self):
		if (self._HwApiStatus):
			if (self._HwApiInitLvl):
				self._HwApiInitLvl = self._HwApiInitLvl - 1
			if (self._HwApiInitLvl == 0):
				RetStatus = HwApiDll.HWAPITerminate()
				self._HwApiStatus = 0
				if (RetStatus != 1):
					return 1
		return 0

	def warmreset(self):
		Tmpbuf = ['\x01', '\x00', '\x00', '\x00', '\x06']
		Status = HwApiDll.WritePort(0xCF9, Tmpbuf)
		if (Status != 1):	# Retry
			HwApiDll.WritePort(0xCF9, Tmpbuf)

	def coldreset(self):
		Tmpbuf = ['\x01', '\x00', '\x00', '\x00', '\x0E']
		Status = HwApiDll.WritePort(0xCF9, Tmpbuf)
		if (Status != 1):	# Retry
			HwApiDll.WritePort(0xCF9, Tmpbuf)

	def memBlock(self, address, size):
		Tmpbuf = create_string_buffer('\000' * (4+size-1))	# initialize the Read Buffer
		Tmpbuf[0:4] = [chr(size & 0xFF), chr((size >> 8) & 0xFF), chr((size >> 16) & 0xFF), chr((size >> 24) & 0xFF)]    # initialize the read size
		Status = HwApiDll.PhysicalMemRead(0, address, Tmpbuf)
		if (Status != 1):	# Retry
			HwApiDll.PhysicalMemRead(0, address, Tmpbuf)
		return Tmpbuf[4:(4+size)]

	def memsave(self, filename, address, size):
		tmpBuf=self.memBlock(address,size)
		out_file = open(filename, "wb") # opening for writing
		out_file.write(tmpBuf)
		out_file.close()

	def memread(self, address, size):
		Tmpbuf = create_string_buffer('\000' * (4+size-1))	# initialize the Read Buffer
		Tmpbuf[0:4] = [chr(size & 0xFF), chr((size >> 8) & 0xFF), chr((size >> 16) & 0xFF), chr((size >> 24) & 0xFF)]    # initialize the read size
		Status = HwApiDll.PhysicalMemRead(0, address, Tmpbuf)
		if (Status != 1):	# Retry
			HwApiDll.PhysicalMemRead(0, address, Tmpbuf)
		return int(_binascii.hexlify(Tmpbuf.raw[4:(4+size)][::-1]), 16)

	def memwrite(self, address, size, value):
		Tmpbuf = create_string_buffer('\000' * (4+size-1))	# initialize the Write Buffer
		Tmpbuf[0:4] = [chr(size & 0xFF), chr((size >> 8) & 0xFF), chr((size >> 16) & 0xFF), chr((size >> 24) & 0xFF)]    # initialize the read size
		Tmpbuf[4:(4+size)] = [chr(value & 0xFF), chr((value >> 8) & 0xFF), chr((value >> 16) & 0xFF), chr((value >> 24) & 0xFF), chr((value >> 32) & 0xFF), chr((value >> 40) & 0xFF), chr((value >> 48) & 0xFF), chr((value >> 56) & 0xFF)] [0:size]   # initialize the write Value
		Status = HwApiDll.PhysicalMemWrite(0, address, Tmpbuf)
		if (Status != 1):	# Retry
			HwApiDll.PhysicalMemWrite(0, address, Tmpbuf)

	def load_data(self, filename, address):
		in_file = open(filename, "rb") # opening for [r]eading as [b]inary
		data = in_file.read()
		in_file.close()
		Size = len(data)
		Tmpbuf = [chr(Size & 0xFF), chr((Size >> 8) & 0xFF), chr((Size >> 16) & 0xFF), chr((Size >> 24) & 0xFF)]    # initialize the read size
		Loadbuf = str(bytearray(Tmpbuf)) + data
		Status = HwApiDll.PhysicalMemWrite(0, address, Loadbuf)
		if (Status != 1):	# Retry
			HwApiDll.PhysicalMemWrite(0, address, Loadbuf)

	def readIO(self, address, size):
		Tmpbuf = create_string_buffer('\000' * (4+size-1))	# initialize the Read Buffer
		Tmpbuf[0:4] = [chr(size & 0xFF), chr((size >> 8) & 0xFF), chr((size >> 16) & 0xFF), chr((size >> 24) & 0xFF)]    # initialize the read size
		Status = HwApiDll.ReadPort(address, Tmpbuf)
		if (Status != 1):	# Retry
			HwApiDll.ReadPort(address, Tmpbuf)
		return int(_binascii.hexlify(Tmpbuf.raw[4:(4+size)][::-1]), 16)

	def writeIO(self, address, size, value):
		Tmpbuf = create_string_buffer('\000' * (4+size-1))	# initialize the Write Buffer
		Tmpbuf[0:4] = [chr(size & 0xFF), chr((size >> 8) & 0xFF), chr((size >> 16) & 0xFF), chr((size >> 24) & 0xFF)]    # initialize the read size
		Tmpbuf[4:(4+size)] = [chr(value & 0xFF), chr((value >> 8) & 0xFF), chr((value >> 16) & 0xFF), chr((value >> 24) & 0xFF)] [0:size]   # initialize the write Value
		Status = HwApiDll.WritePort(address, Tmpbuf)
		if (Status != 1):	# Retry
			HwApiDll.WritePort(address, Tmpbuf)

	def triggerSMI(self, SmiVal):
		self.writeIO(0xB2, 1, SmiVal)

	def ReadMSR(self, Ap, MSR_Addr):
		return 0

	def WriteMSR(self, Ap, MSR_Addr, MSR_Val):
		return 0

	def ReadSmbase(self):
		return 0
