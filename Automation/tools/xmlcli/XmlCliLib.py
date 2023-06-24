#!/usr/bin/env python2.7
# Cscripts remove start
#-----------------------------------------------------------------------------------------------------------------------------------------
# CLI Python Library functions for CLI interface via S/W BIOS interface
# CLI interface Library is implemented using ITP-DAL, SVOS & SV HIF interface
# This CLI Lib can be modified to be used for windows/Linux enviornment.
#
# Author:   Amol A. Shinde (amol.shinde@intel.com)
# Created:  23rd July 2012
# Modified V0.1:   26th July 2012 by Amol
# Modified V0.3:   31st July 2012 by Amol
# Modified V0.4:   5th August 2012 by Amol
# Modified V0.5:   10th August 2012 by Amol
# Modified V0.6:   16th August 2012 by Amol
# Modified V0.7:   22nd August 2012 by Kevin Kirkus (added SVOS related API support)
# Modified V0.8:   27th August 2012 by Amol, Deepika, Pawan & Saurabh K. (changes to support SV HIF card interface & new XML parser )
# Modified V0.9:   20th October 2012 by Amol (changes to clear CLI buffer func, added Block by block BIOS flashing)
# Modified V0.95:  2nd November 2012 by Amol, Saurabh K. & Nick (changes to have generic XML knobs parsing irrespective of type of XML, Display CLI spec version in ProcessBiosKnobs func.)
# Modified V1.0:   13th March 2013 by Amol, Ken Banks (Temp folder related changes for ProgBios, also added CV functions)
# Modified V1.1:   17th October 2013 by Ken Banks (For itpii mode replaced ipt.halt() with robust itpii_halt() in haltcpu() )
# Modified V1.1:   10th May 2013 by Amol
#                  - Added support for returning back to the orignal state when XmlCli scripts are called.
#                  - Added support for warm and cold reset scripts.
# Modified V1.2:   26th July 2013 by Amol & Shirisha (for 64-bit SvHif DLL's)
#                  - upgraded the ExeSvCode() func to take ArgBuff File to pass arguments to the custom SV code.
#                  - Added support for loading 64 bit SvHif DLL's for 64 bit python.
#                  - Updated other areas of SvHif interface scripts.
#                  - Updated ReadMe.txt for the updated usage model with ITP, SVOS & SvHif interfaces.
#
# Modified V1.3:   23th Aug 2013 by Amol & Pawan (for adding Legacy ITP interface support)
#                  - upgraded all required LIB API functions for adding Legacy ITP support.
#                  - XmlCli Python scripts can now work on Simics as well.
#                  - Updated ReadMe.txt for the updated usage model with ITP II, SVOS, SvHif & Legacy ITP interfaces.
#
# Modified V1.4:   16th Dec 2013 by Amol
#                  - Added E82table print script and other minor changes.
# Modified V1.5:   12th Feb 2014 by Amol
#                  - Added support for Simics Python DAL interface.
#                  - Added support for downloading compressed XML for DAL interface, if BIOS suppots it.
#                  - Added few more cleanups and optimizations.
# Scope:  Intended for internal Validation use only
#-----------------------------------------------------------------------------------------------------------------------------------------
__author__ = 'ashinde'
# Cscripts remove end
import sys as _sys
import os as _os
import binascii as _binascii
import string as _string

try:
	import common.toolbox as _tools
except ImportError:
	import tools.toolbox as _tools

class _GlobalCliFiles():
	def __init__(self):
		self.refPath = _os.path.abspath(_os.path.dirname(__file__))

	def setRefPath(self,refPath):
		self.refPath = _os.path.abspath(self.path.dirname(__file__))

	def getRefPath(self):
		return self.refPath

	def getKnobsXmlFile(self):
		try:
			return _os.sep.join([_os.environ['XML_CLI_OUT'], "BiosKnobs.xml"])
		except:
			return _os.sep.join([self.refPath, "out", "BiosKnobs.xml"])

	def getKnobsIniFile(self):
		return _os.sep.join([self.refPath, "cfg", "BiosKnobs.ini"])

	def getTmpKnobsIniFile(self):
		try:
			return _os.sep.join([_os.environ['XML_CLI_OUT'], "TmpBiosKnobs.ini"])
		except:
			return _os.sep.join([self.refPath, "out", "TmpBiosKnobs.ini"])

	def getPlatformConfigXml(self):
		try:
			return _os.sep.join([_os.environ['XML_CLI_OUT'], "PlatformConfig.xml"])
		except:
			return _os.sep.join([self.refPath, "out", "PlatformConfig.xml"])

cliaccess = None
FlexConCfgFile = False
ForceReInitCliAccess = False
_defaultFiles = _GlobalCliFiles()
XmlCliPath = _defaultFiles.getRefPath()
KnobsIniFile = _defaultFiles.getKnobsIniFile()
KnobsXmlFile = _defaultFiles.getKnobsXmlFile()
PlatformConfigXml = _defaultFiles.getPlatformConfigXml()
TmpKnobsIniFile = _defaultFiles.getTmpKnobsIniFile()
OutBinFile = ""
XmlCliToolsDir = XmlCliPath+_os.sep+"tools"
TempFolder = _os.path.dirname(KnobsXmlFile)
gDramSharedMbAddr = 0
InterfaceType = "stub"

_log = _tools.getLogger("XmlCli")
_log.setFile(_os.path.dirname(KnobsXmlFile)+_os.sep+"XmlCli.log",dynamic=True)
_log.setFileFormat("simple")
_log.setFileLevel("info")
_log.setConsoleLevel("result")

CliRespFlags = 0
if (_sys.platform[0:3].lower() == 'win'):
	_isExeAvailable = True
else:
	_isExeAvailable = False

SHAREDMB_SIG1                   =   0xBA5EBA11
SHAREDMB_SIG2                   =   0xBA5EBA11
SHARED_MB_LEGMB_SIG_OFF         =   0x20
SHARED_MB_LEGMB_ADDR_OFF        =   0x24
LEGACYMB_SIG                    =   0x5A7ECAFE
XML_START                       =   "<SYSTEM>"
XML_END                         =   "</SYSTEM>"
SHAREDMB_SIG1_OFF               =   0x00
SHAREDMB_SIG2_OFF               =   0x08
CLI_SPEC_VERSION_MINOR_OFF      =   0x14
CLI_SPEC_VERSION_MAJOR_OFF      =   0x15
CLI_SPEC_VERSION_RELEASE_OFF    =   0x17
LEGACYMB_SIG_OFF                =   0x20
LEGACYMB_OFF                    =   0x24
LEGACYMB_XML_OFF                =   0x0C
STRING                          =   0x51
ASCII                           =   0xA5
HEX                             =   0x16
SETUP_KNOBS_ADDR_OFF            =   0x13C
SETUP_KNOBS_SIZE_OFF            =   0x140
CPUSV_MAILBOX_ADDR_OFF          =   0x14C
SHARED_MB_CLI_REQ_BUFF_SIG      =   0xCA11AB1E
SHARED_MB_CLI_RES_BUFF_SIG      =   0xCA11B0B0
SHARED_MB_CLI_REQ_BUFF_SIG_OFF  =   0x30
SHARED_MB_CLI_RES_BUFF_SIG_OFF  =   0x40
SHARED_MB_CLI_REQ_BUFF_ADDR_OFF =   0x34
SHARED_MB_CLI_RES_BUFF_ADDR_OFF =   0x44
CLI_REQ_READY_SIG               =   0xC001C001
CLI_RES_READY_SIG               =   0xCAFECAFE
CLI_REQ_RES_READY_SIG_OFF       =   0x00
CLI_REQ_RES_READY_CMD_OFF       =   0x04
CLI_REQ_RES_READY_FLAGS_OFF     =   0x06
CLI_REQ_RES_READY_STATUS_OFF    =   0x08
CLI_REQ_RES_READY_PARAMSZ_OFF   =   0x0C
CLI_REQ_RES_BUFF_HEADER_SIZE    =   0x10
WRITE_MSR_OPCODE                =   0x11
READ_MSR_OPCODE                 =   0x21
IO_READ_OPCODE                  =   0x31
IO_WRITE_OPCODE                 =   0x32
APPEND_BIOS_KNOBS_CMD_ID        =   0x48
RESTOREMODIFY_KNOBS_CMD_ID      =   0x49
READ_BIOS_KNOBS_CMD_ID          =   0x4A
LOAD_DEFAULT_KNOBS_CMD_ID       =   0x4B
PROG_BIOS_CMD_ID                =   0xB4
FETCH_BIOS_CMD_ID               =   0xB5
BIOS_VERSION_OPCODE             =   0xB1
READ_MSR_OPCODE                 =   0x21
EXE_SV_SPECIFIC_CODE_OPCODE     =   0x300
READ_BRT_OPCODE                 =   0x310
CREATE_FRESH_BRT_OPCODE         =   0x311
ADD_BRT_OPCODE                  =   0x312
DEL_BRT_OPCODE                  =   0x313
DIS_BRT_OPCODE                  =   0x314
CLI_KNOB_APPEND                 =   0x0
CLI_KNOB_RESTORE_MODIFY         =   0x1
CLI_KNOB_READ_ONLY              =   0x2
CLI_KNOB_LOAD_DEFAULTS          =   0x3

CliSpecRelVersion               =   0x00
CliSpecMajorVersion             =   0x00
CliSpecMinorVersion             =   0x00

CliCmdDict = {APPEND_BIOS_KNOBS_CMD_ID:    'APPEND_BIOS_KNOBS_CMD_ID', \
              RESTOREMODIFY_KNOBS_CMD_ID:  'RESTOREMODIFY_KNOBS_CMD_ID', \
              READ_BIOS_KNOBS_CMD_ID:      'READ_BIOS_KNOBS_CMD_ID', \
              LOAD_DEFAULT_KNOBS_CMD_ID:   'LOAD_DEFAULT_KNOBS_CMD_ID', \
              PROG_BIOS_CMD_ID:            'PROG_BIOS_CMD_ID', \
              FETCH_BIOS_CMD_ID:           'FETCH_BIOS_CMD_ID', \
              BIOS_VERSION_OPCODE:         'BIOS_VERSION_OPCODE', \
              READ_MSR_OPCODE:             'READ_MSR_OPCODE', \
              WRITE_MSR_OPCODE:            'WRITE_MSR_OPCODE', \
              IO_READ_OPCODE:              'IO_READ_OPCODE', \
              IO_WRITE_OPCODE:             'IO_WRITE_OPCODE', \
              EXE_SV_SPECIFIC_CODE_OPCODE: 'EXE_SV_SPECIFIC_CODE_OPCODE', \
              READ_BRT_OPCODE:             'READ_BRT_OPCODE', \
              CREATE_FRESH_BRT_OPCODE:     'CREATE_FRESH_BRT_OPCODE', \
              ADD_BRT_OPCODE:              'ADD_BRT_OPCODE', \
              DEL_BRT_OPCODE:              'DEL_BRT_OPCODE', \
              DIS_BRT_OPCODE:              'DIS_BRT_OPCODE'};

def _setCliAccess(req_access=None):
	global cliaccess, InterfaceType, _isExeAvailable
	if req_access != None:
		InterfaceType = req_access
	elif _os.environ.get('SVHIF') == "svhif":     # user need to set the key if he plans to use hif interface: using <_os.environ['SVHIF']="svhif">
		InterfaceType = _os.environ['SVHIF']
	elif _os.environ.get('SVLEGITP') == "svlegitp":     # user need to set the key if he plans to use Legacy ITP interface: using <_os.environ['SVLEGITP']="svlegitp">
		InterfaceType = _os.environ['SVLEGITP']
	elif _os.environ.get('SIMICS') == "simics":
		InterfaceType = _os.environ['SIMICS']
	elif _os.environ.get('LINUX') == "linux":
		InterfaceType = _os.environ['LINUX']
	elif _os.environ.get('WIN') == "win":
		InterfaceType = _os.environ['win']
	elif _os.environ.get('WINHWAPI') == "winhwapi":
		InterfaceType = _os.environ['winhwapi']
	elif _os.name == "edk2":
		InterfaceType = 'uefi'
	else:
		try:
			import common.baseaccess as baseaccess
			try:
				InterfaceType = baseaccess._access
			except:
				InterfaceType = baseaccess.getaccess()
		except:
			InterfaceType = "stub"
	if(InterfaceType == 'lauterbach'):
		InterfaceType = "ltb"
	try:
		if InterfaceType == "itpii":
			import access.itpiixmlcli as _gbtaccess
			cliaccess = _gbtaccess.itpiiAccess()
		elif InterfaceType == "dci":
			import access.dcixmlcli as _gbtaccess
			cliaccess = _gbtaccess.dciAccess()
		elif InterfaceType == "ltb":
			import access.ltbxmlcli as _gbtaccess
			cliaccess = _gbtaccess.ltbAccess()
		elif InterfaceType == "svhif":
			import access.svhifxmlcli as _gbtaccess
			cliaccess = _gbtaccess.svhifAccess()
		elif InterfaceType == "svlegitp":
			import access.itpxmlcli as _gbtaccess
			cliaccess = _gbtaccess.itpAccess()
		elif InterfaceType == "itpsimics":
			import access.itpsimicsxmlcli as _gbtaccess
			cliaccess = _gbtaccess.itpSimicsAccess()
		elif InterfaceType == "linux":
			import access.linuxxmlcli as _gbtaccess
			cliaccess = _gbtaccess.linuxAccess()
		elif InterfaceType == "win":
			import access.winxmlcli as _gbtaccess
			cliaccess = _gbtaccess.winAccess()
		elif InterfaceType == "winhwapi":
			import access.winhwapixmlcli as _gbtaccess
			cliaccess = _gbtaccess.winhwapiAccess()
			_isExeAvailable = False
		elif InterfaceType == "svos":
			import access.svosxmlcli as _gbtaccess
			cliaccess = _gbtaccess.svosAccess()
		elif InterfaceType == "simics":
			import access.simicsxmlcli as _gbtaccess
			cliaccess = _gbtaccess.simicsAccess()
		elif InterfaceType == "tssa":
			import access.inbandxmlcli as _gbtaccess
			cliaccess = _gbtaccess.inbandAccess()
		elif InterfaceType == "uefi":
			import access.uefixmlcli as _gbtaccess
			cliaccess = _gbtaccess.uefiAccess()
		else:
			raise TypeError, "Unknown Xml Cli InterfaceType requested (%s)"%InterfaceType
	except:
		InterfaceType = "stub"

	if InterfaceType == "stub":
		import access.stubxmlcli as _gbtaccess
		cliaccess = _gbtaccess.stubAccess()
	print "****  Using \"%s\" mode as Interface  ****" %InterfaceType
		
def _checkCliAccess():
	global cliaccess, ForceReInitCliAccess
	if ((cliaccess == None) or (ForceReInitCliAccess)):
		_setCliAccess()

def haltcpu(delay=0):
	global cliaccess
	_checkCliAccess()
	return cliaccess.haltcpu(delay)

def runcpu():
	global cliaccess
	_checkCliAccess()
	return cliaccess.runcpu()

def InitInterface():
	global cliaccess
	_checkCliAccess()
	return cliaccess.InitInterface()

def CloseInterface():
	global cliaccess
	_checkCliAccess()
	return cliaccess.CloseInterface()

# warmreset function
def warmreset():
	global cliaccess
	_checkCliAccess()
	return cliaccess.warmreset()

# coldreset function
def coldreset():
	global cliaccess
	_checkCliAccess()
	return cliaccess.coldreset()

# reads the data block of given size from target memory starting from given address
# the read data is in bit format, so we will convert it in string/ASCII so that it could be manupulated on byte granularity
def memBlock(address, size):
	global cliaccess
	_checkCliAccess()
	return cliaccess.memBlock(address, size)

# saves the memory block of given byte size to desired file
def memsave(filename, address, size):
	global cliaccess
	_checkCliAccess()
	return cliaccess.memsave(filename, address, size)

# mem read function, Max size supported is 8 bytes, cannot be used for block reads
def memread(address, size):
	global cliaccess
	_checkCliAccess()
	return cliaccess.memread(address, size)

# mem write function, Max size supported is 8 bytes, cannot be used for block writes
def memwrite(address, size, value):
	global cliaccess
	_checkCliAccess()
	return cliaccess.memwrite(address, size, value)

# loads the given file data to the desired memory address for size number of bytes
def load_data(filename, address):
	global cliaccess
	_checkCliAccess()
	return cliaccess.load_data(filename, address)

# Read IO function
def readIO(address, size):
	global cliaccess
	_checkCliAccess()
	return cliaccess.readIO(address, size)

# Write IO function
def writeIO(address, size, value):
	global cliaccess
	_checkCliAccess()
	return cliaccess.writeIO(address, size, value)

# Trigger S/W SMI of desired value
def triggerSMI(SmiVal):
	global cliaccess
	_checkCliAccess()
	return cliaccess.triggerSMI(SmiVal)

# Read SM Base Address
def ReadSmbase():
	global cliaccess
	_checkCliAccess()
	return cliaccess.ReadSmbase()

def ReadMSR(MSR_Val):
	global cliaccess
	_checkCliAccess()
	return cliaccess.ReadMSR(MSR_Val)

def RemoveFile (FileName):
	if (_os.path.isfile(FileName)):
		_os.remove(FileName)

def RenameFile (FileName, NewFileName):
	if (_os.path.isfile(NewFileName)):
		_os.remove(NewFileName)
	_os.rename(FileName, NewFileName)

# read Cmos
def readcmos(Reg):
	if (Reg < 0x80):
		writeIO(0x70, 1, Reg)
		Val = readIO(0x71, 1)

	if (Reg >= 0x80):
		writeIO(0x72, 1, Reg)
		Val = readIO(0x73, 1)
	return Val

# write Cmos
def writecmos(Reg, Val):
	if (Reg < 0x80):
		writeIO(0x70, 1, Reg)
		writeIO(0x71, 1, Val)

	if (Reg >= 0x80):
		writeIO(0x72, 1, Reg)
		writeIO(0x73, 1, Val)

# Clear all Cmos locations to 0 and set Cmos BAD flag
def clearcmos():
	_log.warning("Clearing CMOS")
	for i in range(0x0,0x80,1):
		writeIO(0x70, 1, i)
		writeIO(0x71, 1, 0)
		value = i|0x80
		if ((value == 0xF0) or (value == 0xF1)):        # skip clearing the cmos register's which hold Dram Shared MB address.
			continue
		writeIO(0x72, 1, value)
		writeIO(0x73, 1, 0)
	writeIO(0x70, 1, 0x0E)
	writeIO(0x71, 1, 0xC0)  # set Cmos BAD flag

	RTC_RegPciAddress = ((1 << 31) + (0 << 16) + (31 << 11) + (0 << 8) + 0xA4)
	writeIO(0xCF8, 4, RTC_RegPciAddress)
	RTC_value = readIO(0xCFC, 2)
	RTC_value = RTC_value | 0x4
	writeIO(0xCF8, 4, RTC_RegPciAddress)
	writeIO(0xCFC, 2, RTC_value)		# set cmos bad in PCH RTC register

# read all Cmos locations from 0 to 0xFF
def readallcmos():
	Value = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
	_log.result("Reading CMOS")
	_log.result("    |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|")
	_log.result("Addr|00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F|")
	_log.result("----|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|")
	for i in range(0x0,0x8,1):
		for j in range(0x0,0x10,1):
			writeIO(0x70, 1, ((i<<4) + j) )
			Value[j] = readIO(0x71, 1)
		_log.result(" %2X |%2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X|" %((i<<4), Value[0], Value[1], Value[2], Value[3], Value[4], Value[5], Value[6], Value[7], Value[8], Value[9], Value[10], Value[11], Value[12], Value[13], Value[14], Value[15]))
	_log.result(" ---|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|")
	for i in range(0x8,0x10,1):
		for j in range(0x0,0x10,1):
			writeIO(0x72, 1, ((i<<4) + j) )
			Value[j] = readIO(0x73, 1)
		_log.result(" %2X |%2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X|" %((i<<4), Value[0], Value[1], Value[2], Value[3], Value[4], Value[5], Value[6], Value[7], Value[8], Value[9], Value[10], Value[11], Value[12], Value[13], Value[14], Value[15]))
	_log.result(" ---|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|")

def GetCliSpecVersion(DramMbAddr):
	global CliSpecRelVersion, CliSpecMajorVersion, CliSpecMinorVersion, LEGACYMB_XML_OFF, CLI_REQ_READY_SIG, CLI_RES_READY_SIG
	CliSpecRelVersion    = memread((DramMbAddr+CLI_SPEC_VERSION_RELEASE_OFF), 1) & 0xF
	CliSpecMajorVersion  = memread((DramMbAddr+CLI_SPEC_VERSION_MAJOR_OFF), 2)
	if(CliSpecMajorVersion >= 7):
		LegMbOffset = memread((DramMbAddr+LEGACYMB_OFF), 4)
		if(LegMbOffset < 0xFFFF):
			LegMbOffset = DramMbAddr+LegMbOffset
		if(memread((LegMbOffset+0x4C), 4) == 0):
			LEGACYMB_XML_OFF  = 0x50
		else:
			LEGACYMB_XML_OFF  = 0x4C
		CLI_REQ_READY_SIG = 0xD055C001
		CLI_RES_READY_SIG = 0xD055CAFE
	CliSpecMinorVersion  = memread((DramMbAddr+CLI_SPEC_VERSION_MINOR_OFF), 1)
	_log.info("CLI Spec Version = %d.%d.%d" %(CliSpecRelVersion, CliSpecMajorVersion, CliSpecMinorVersion))

# Read DRAM Shared Mailbox Address from cmos locations 0xBB [23:16] & 0xBC [31:24]
def GetDramMbAddr():
	global gDramSharedMbAddr, InterfaceType

	InitInterface()
	writeIO(0x72, 1, 0xF0)                          # Read a byte from cmos offset 0xF0
	result0 = (readIO(0x73, 4) & 0xFF)              # Read a byte from cmos offset 0xBB [23:16]
	writeIO(0x72, 1, 0xF1)                          # Read a byte from cmos offset 0xF1
	result1 = (readIO(0x73, 4) & 0xFF)              # Read a byte from cmos offset 0xBC [31:24]
	DramSharedMbAddr = ((result1 << 24) | (result0 << 16))    # Get bits [31:24] of the Dram MB address
	SharedMbSig1 = memread((DramSharedMbAddr+SHAREDMB_SIG1_OFF), 4)
	SharedMbSig2 = memread((DramSharedMbAddr+SHAREDMB_SIG2_OFF), 4)
	if ( (SharedMbSig1 == SHAREDMB_SIG1) and (SharedMbSig2 == SHAREDMB_SIG2) ):
		ShareMbEntry1Sig = memread((DramSharedMbAddr+LEGACYMB_SIG_OFF), 4)
		if (ShareMbEntry1Sig == LEGACYMB_SIG):
			GetCliSpecVersion(DramSharedMbAddr)
			CloseInterface()
			return DramSharedMbAddr

	writeIO(0x72, 1, 0xBB)                          # Read a byte from cmos offset 0xF0
	result0 = (readIO(0x73, 4) & 0xFF)              # Read a byte from cmos offset 0xBB [23:16]
	writeIO(0x72, 1, 0xBC)                          # Read a byte from cmos offset 0xF1
	result1 = (readIO(0x73, 4) & 0xFF)              # Read a byte from cmos offset 0xBC [31:24]
	DramSharedMbAddr = ((result1 << 24) | (result0 << 16))    # Get bits [31:24] of the Dram MB address
	SharedMbSig1 = memread((DramSharedMbAddr+SHAREDMB_SIG1_OFF), 4)
	SharedMbSig2 =  memread((DramSharedMbAddr+SHAREDMB_SIG2_OFF), 4)
	if ( (SharedMbSig1 == SHAREDMB_SIG1) and (SharedMbSig2 == SHAREDMB_SIG2) ):
		ShareMbEntry1Sig = memread((DramSharedMbAddr+LEGACYMB_SIG_OFF), 4)
		if (ShareMbEntry1Sig == LEGACYMB_SIG):
			GetCliSpecVersion(DramSharedMbAddr)
			CloseInterface()
			return DramSharedMbAddr

	if((gDramSharedMbAddr != 0) and (InterfaceType == "svhif")):
		DramSharedMbAddr = int(gDramSharedMbAddr)
		SharedMbSig1 = memread((DramSharedMbAddr+SHAREDMB_SIG1_OFF), 4)
		SharedMbSig2 =  memread((DramSharedMbAddr+SHAREDMB_SIG2_OFF), 4)
		if ( (SharedMbSig1 == SHAREDMB_SIG1) and (SharedMbSig2 == SHAREDMB_SIG2) ):
			ShareMbEntry1Sig = memread((DramSharedMbAddr+LEGACYMB_SIG_OFF), 4)
			if (ShareMbEntry1Sig == LEGACYMB_SIG):
				GetCliSpecVersion(DramSharedMbAddr)
				CloseInterface()
				return DramSharedMbAddr
	DramSharedMbAddr = readDramMbAddrFromEFI()
	if(DramSharedMbAddr != 0):
		GetCliSpecVersion(DramSharedMbAddr)
		CloseInterface()
		return DramSharedMbAddr
	CloseInterface()
	return 0

def ConfXmlCli():
	InitInterface()
	DRAM_MbAddr = GetDramMbAddr() # Get DRam MAilbox Address from Cmos.
	_log.debug("DRAM_MbAddr = 0x%X" %(DRAM_MbAddr))
	Status = 0
	if (DRAM_MbAddr == 0x0):
		_log.error("Dram Shared Mailbox not Valid, XmlCli May not be Enabled, Trying to Enable now..")
		try:
			import tools.EnableXmlCli as exc
		except ImportError:
			print "Import error on tools.EnableXmlCli, please rename this file as .pyc"
			CloseInterface()
			return 0xF
		Status = exc.EnableXmlCli()
		if(Status == 0):
			Status = 2		# Xml Cli support was not Enabled, its now Enabled, Reboot Required
		else:
			_log.error("Xml Cli support is not Availaible in Your BIOS, Contact your BIOS Engineer..")
			Status = 1
	else:
		_log.result("XmlCli support is Enabled..")
		Status = 0
	CloseInterface()
	return Status

# Read buffer function: read the desired format of Data of specified size from the given offset of the buffer,
# input buffer is in big endain ASCII format
def ReadBuffer(buffer, offset, size, type):
	Val = buffer[offset:offset+size]
	if(type == ASCII):
		return Val
	if(type == HEX):
		return int(_binascii.hexlify(Val[::-1]), 16)
	return 0

def ReadList(buffer, offset, size):
	return int(_binascii.hexlify(_string.join(buffer[offset:offset+size][::-1], '')), 16)

def ReadBios(BiosBinListBuff, BinSize, Addr, Size):
	if(BiosBinListBuff == 0):	# Online mode
		return memread(Addr, Size)
	else:	# Offline mode
		return ReadList(BiosBinListBuff, (BinSize-(0x100000000-Addr)), Size)

def WaitForCliResponse(CLI_ResBuffAddr, Delay, PrintRes=1):
	global CliRespFlags
	CliRespFlags = 0
	ret=0
	ComandSideEffect = ["NoSideEffect", "WarmResetRequired", "PowerGoodResetRequired", "Reserved"]

	for retryCnt in range(0x0,12,1):		# set number of retries here, currently set to 3
		haltcpu(delay=Delay)
		ResHeaderbuff = memBlock(CLI_ResBuffAddr, CLI_REQ_RES_BUFF_HEADER_SIZE)
		ResReadySig = ReadBuffer(ResHeaderbuff, CLI_REQ_RES_READY_SIG_OFF, 4, HEX)
		if (ResReadySig == CLI_RES_READY_SIG):		   # Verify if BIOS is done with the request
			if(PrintRes == 1):
				ResCmdId = ReadBuffer(ResHeaderbuff, CLI_REQ_RES_READY_CMD_OFF, 2, HEX)
				ResFlags = ReadBuffer(ResHeaderbuff, CLI_REQ_RES_READY_FLAGS_OFF, 2, HEX)
				CliRespFlags = ResFlags
				ResStatus = ReadBuffer(ResHeaderbuff, CLI_REQ_RES_READY_STATUS_OFF, 4, HEX)
				ResParamSize = ReadBuffer(ResHeaderbuff, CLI_REQ_RES_READY_PARAMSZ_OFF, 4, HEX)
				_log.info("CLI Response Header:")
				_log.info("   CmdID = 0x%X (\"%s\") " %(ResCmdId, CliCmdDict[ResCmdId]))
				_log.info("   Status = 0x%X;  ParamSize = 0x%X;  Flags.WrongParam = %X;" %(ResStatus, ResParamSize, (ResFlags & 0x1)))
				_log.info("   Flags.CantExe = %X;  Flags.SideEffects = \"%s\"; " %(((ResFlags>>1) & 0x1), ComandSideEffect[int((ResFlags>>2) & 0xF)]))
				if( ((ResFlags & 0x3) == 0) and (ResStatus == 0) ):
					_log.info("CLI command executed successfully..")
				else:
					_log.error("CLI command executed, but with errors. See Logfile.")
					ret=1 #AR Nick: may want to remove this and/or revert back to original implementation
			return ret
		else:		# CLI Response is not Ready yet
			if (ReadSmbase() == 0x30000 ):
				_log.error("SM Base was relocated to the default value(0x30000), S/W SMI's wont work!")
				return 1
			_log.info("CLI Response not yet ready, retrying..")
		runcpu()
	_log.error("CLI Response not ready even after retries, exiting..")
	return 1

# Get XML Base Address & XML size details from the Shared Mailbox temp buffer
def readxmldetails(DramSharedMBbuf):
	SharedMbSig1 = ReadBuffer(DramSharedMBbuf, SHAREDMB_SIG1_OFF, 4, HEX)
	SharedMbSig2 = ReadBuffer(DramSharedMBbuf, SHAREDMB_SIG2_OFF, 4, HEX)
	GBT_XML_Addr = 0
	GBT_XML_Size = 0
	if (( SharedMbSig1 == SHAREDMB_SIG1) and ( SharedMbSig2 == SHAREDMB_SIG2)):
		ShareMbEntry1Sig = ReadBuffer(DramSharedMBbuf, LEGACYMB_SIG_OFF, 4, HEX)
		if (ShareMbEntry1Sig == LEGACYMB_SIG):
			LegMbOffset = ReadBuffer(DramSharedMBbuf, LEGACYMB_OFF, 4, HEX)
			if(LegMbOffset > 0xFFFF):
				GBT_XML_Addr = memread(LegMbOffset+LEGACYMB_XML_OFF, 4)+4
			else:
				GBT_XML_Addr = ReadBuffer(DramSharedMBbuf, LegMbOffset+LEGACYMB_XML_OFF, 4, HEX)+4
			GBT_XML_Size = memread(GBT_XML_Addr-4, 4)
	return GBT_XML_Addr, GBT_XML_Size

# Check if Target XML is Valid or not
def isxmlvalid(GBT_XML_Addr, GBT_XML_Size):
	try:
		tmpbuf = memBlock(GBT_XML_Addr,0x08)   # Read/save parameter buffer
		SystemStart = ReadBuffer(tmpbuf, 0, 0x08, ASCII)
		tmpbuf = memBlock(GBT_XML_Addr+GBT_XML_Size-0xB,0x09)   # Read/save parameter buffer
		SystemEnd = ReadBuffer(tmpbuf, 0, 0x09, ASCII)
		if(( SystemStart == XML_START) and ( SystemEnd == XML_END)):
			return True
		else:
			return False
	except:
		_log.error("Exception detected when determining if xml is valid.")
		return False

# Get CLI Request Buffer Address from the Shared Mailbox temp buffer
def readclireqbufAddr(DramSharedMBbuf):
	CLI_REQ_BUFF_Addr = 0
	if (ReadBuffer(DramSharedMBbuf, SHARED_MB_CLI_REQ_BUFF_SIG_OFF, 4, HEX) == SHARED_MB_CLI_REQ_BUFF_SIG):
		CLI_REQ_BUFF_Addr = ReadBuffer(DramSharedMBbuf, SHARED_MB_CLI_REQ_BUFF_ADDR_OFF, 4, HEX)
	return CLI_REQ_BUFF_Addr

# Get CLI Response Buffer Address from the Shared Mailbox temp buffer
def readcliresbufAddr(DramSharedMBbuf):
	CLI_RES_BUFF_Addr = 0
	if (ReadBuffer(DramSharedMBbuf, SHARED_MB_CLI_RES_BUFF_SIG_OFF, 4, HEX) == SHARED_MB_CLI_RES_BUFF_SIG):
		CLI_RES_BUFF_Addr = ReadBuffer(DramSharedMBbuf, SHARED_MB_CLI_RES_BUFF_ADDR_OFF, 4, HEX)
	return CLI_RES_BUFF_Addr

# Get Legacy DRAM Mailbox Address offset from the Shared Mailbox temp buffer
def readLegMailboxAddrOffset(DramSharedMBbuf):
	LegMailboxAddrOffset = 0
	if (ReadBuffer(DramSharedMBbuf, SHARED_MB_LEGMB_SIG_OFF, 4, HEX) == LEGACYMB_SIG):
		LegMailboxAddrOffset = ReadBuffer(DramSharedMBbuf, SHARED_MB_LEGMB_ADDR_OFF, 4, HEX)
	return LegMailboxAddrOffset

# Check & store the given XML File (only the Header + Knobs section)
def fetchHdrNknob(KnobFilename=None, PlatformXml=None):
	if KnobFilename == None:
		KnobFilename = KnobsXmlFile
	if PlatformXml == None:
		PlatformXml = PlatformConfigXml
	if ( SaveXml(PlatformXml) == 1 ):   # Check and Save the GBT XML knobs section.
		_log.error("Aborting due to Error!")
		return 1
	newFile=open(KnobFilename,'w')
	src=open(PlatformXml,'r')
	biosKnobStarted=biosKnobEnded=searchbiosKnob=systemDone=platformDone=biosDone=gbtDone=False
	for line in src :
		if ( line.find("<SYSTEM>") >= 0 ):
			newFile.write(line)
			systemDone=True
		elif ( line.find("<PLATFORM") >= 0 ):
			newFile.write(line)
			platformDone=True
		elif ( (line.find("<CPUSVBIOS") >= 0) or (line.find("<SVBIOS") >= 0) or (line.find("<BIOS") >= 0) ):
			newFile.write(line)
			biosDone=True
		elif (line.find("<GBT") >= 0):
			newFile.write(line)
			gbtDone=True
		elif ( systemDone and platformDone and biosDone and gbtDone ): #All the headers are written then start from Bios Knob
			break
	for line in src:
		if ( line.find("<biosknobs>") >= 0 ):
			newFile.write(line)
			break
	for line in src:
		newFile.write(line)
		if ( line.find("</biosknobs>") >= 0 ):
			break
	for line in src:
		if ( line.find("</SYSTEM>") >= 0 ):
			newFile.write(line)
			break
	src.close()
	newFile.close()

def PatchXmlData(XmlListBuff, XmlAddr, XmlSize):
	XmlPatchDataFound = 0
	NewXmlPatchDataFound = 0
	PacketAddr = ((XmlAddr+XmlSize+0xFFF) & 0xFFFFF000)
	for count in range (0, 2):
		PacketHdr  = int(memread(PacketAddr, 8))
		PacketSize = ((PacketHdr >> 40) & 0xFFFFFF)
		if ( ((PacketHdr & 0xFFFFFFFFFF) == 0x4c444B5824) and (PacketSize != 0) ):	# cmp with $XKDL
			XmlKnobsDeltaBuff = memBlock((PacketAddr+8), PacketSize)
			XmlPatchDataFound = 1
			break
		if ( ((PacketHdr & 0xFFFFFFFFFF) == 0x54444B5824) and (PacketSize != 0) ):	# cmp with $XKDT
			XmlKnobsDeltaBuff = memBlock((PacketAddr+8), PacketSize)
			NewXmlPatchDataFound = 1
			break
		PacketAddr = ((PacketAddr+8+PacketSize+0xFFF) & 0xFFFFF000)
	if( (XmlPatchDataFound == 1) or (NewXmlPatchDataFound == 1) ):
		offset = 0
		while(1):   # read and print the return knobs entry parameters from CLI's response buffer
			if (offset >= PacketSize):
				break
			KnobEntryOffset = ReadBuffer(XmlKnobsDeltaBuff, offset+0, 3, HEX)
			Data16          = ReadBuffer(XmlKnobsDeltaBuff, offset+3, 2, HEX)
			DataOfst        = KnobEntryOffset+(Data16 & 0xFFF)
			if(NewXmlPatchDataFound):
				DataSize    = ReadBuffer(XmlKnobsDeltaBuff, offset+5, 1, HEX)
				ValueToReplace  = ReadBuffer(XmlKnobsDeltaBuff, offset+6, DataSize, HEX)
			else:
				DataSize    = (Data16 >> 12) & 0xF
				ValueToReplace  = ReadBuffer(XmlKnobsDeltaBuff, offset+5, DataSize, HEX)
			StrValToReplace = hex(ValueToReplace)[2::].strip('L').zfill(DataSize*2).upper()
			XmlListBuff[DataOfst:DataOfst+(DataSize*2)] = list(StrValToReplace)
			if(NewXmlPatchDataFound):
				offset = offset + 6 + DataSize
			else:
				offset = offset + 5 + DataSize
		_log.info("Patch buffer data size = %d bytes" %(PacketSize))

InValidXmlChar=['\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x0B', '\x0C', '\x0E', '\x0F', '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', '\x7F', '\x80', '\x81', '\x82', '\x83', '\x84', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F', '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F']
def SanitizeXml(filename):
	TempXML=open(filename, "rb")
	XmlListBuff = list(TempXML.read())
	TempXML.close()
	Modified = False
	for Index in range (0, len(XmlListBuff)):
		CurrVal = XmlListBuff[Index]
		if (CurrVal == '\xB5'):
			XmlListBuff[Index] = "u"
			Modified = True
		elif (CurrVal == '\xA0'):
			XmlListBuff[Index] = "."
			Modified = True
		elif (CurrVal in InValidXmlChar):
			XmlListBuff[Index] = " "
			Modified = True

	if(Modified):
		_log.info("SanitizeXml(): Fixing XML syntax errors found with source XML file.")
		RemoveFile(filename)
		NewXmlFile = open(filename, "wb") # opening for writing
		NewXmlFile.write(_string.join(XmlListBuff, ''))
		NewXmlFile.close()

def IsXmlGenerated():
	Status = 0
	InitInterface()
	DRAM_MbAddr = GetDramMbAddr() # Get DRam MAilbox Address from Cmos.
	_log.debug("DRAM_MbAddr = 0x%X" %(DRAM_MbAddr))
	if (DRAM_MbAddr == 0x0):
		_log.error("Dram Shared Mailbox not Valid, hence exiting")
		CloseInterface()
		return 1
	DramSharedMBbuf = memBlock(DRAM_MbAddr,0x200) # Read/save parameter buffer
	(XmlAddr,XmlSize)  = readxmldetails(DramSharedMBbuf) # read GBTG XML address and Size
	if (XmlAddr == 0):
		_log.error("Platform Configuration XML not yet generated, hence exiting")
		CloseInterface()
		return 1
	if(isxmlvalid(XmlAddr,XmlSize)):
		_log.result("Xml Is Generated and it is Valid")
	else:
		_log.error("XML is not valid or not yet generated XmlAddr = 0x%X, XmlSize = 0x%X" %(XmlAddr,XmlSize))
		Status = 1
	CloseInterface()
	return Status

# save entire/complete Target XML to desired file.
def SaveXml(filename=None, ITPOptimz=0, MbAddr=0):
	if filename == None:
		filename = PlatformConfigXml
	Status = 0
	InitInterface()
	DRAM_MbAddr = 0
	if MbAddr == 0:
		DRAM_MbAddr = GetDramMbAddr() # Get DRam MAilbox Address from Cmos.
	else:
		DRAM_MbAddr = MbAddr
	_log.debug("DRAM_MbAddr = 0x%X" %(DRAM_MbAddr))
	if (DRAM_MbAddr == 0x0):
		_log.error("Dram Shared Mailbox not Valid, hence exiting")
		CloseInterface()
		return 1
	DramSharedMBbuf = memBlock(DRAM_MbAddr,0x200) # Read/save parameter buffer
	(XmlAddr,XmlSize)  = readxmldetails(DramSharedMBbuf) # read GBTG XML address and Size
	if (XmlAddr == 0):
		_log.error("Platform Configuration XML not yet generated, hence exiting")
		CloseInterface()
		return 1
	if(isxmlvalid(XmlAddr,XmlSize)):
		ComprXmlFound = False
		if(_isExeAvailable):
			PacketAddr = ((XmlAddr+XmlSize+0xFFF) & 0xFFFFF000)
			for count in range (0, 2):
				PacketHdr  = int(memread(PacketAddr, 8))
				PacketSize = ((PacketHdr >> 40) & 0xFFFFFF)
				if ( ((PacketHdr & 0xFFFFFFFFFF) == 0x414d5a4c24) and (PacketSize != 0) ):	# cmp with $LZMA
					_log.info("Found LZMA Compressed XML, Downloading it")
					memsave("%s%sGbtLzC.bin" %(TempFolder, _os.sep), (PacketAddr+8), int(PacketSize))
					if _os.name == "nt":
						_os.system('%s%sLzmaCompress.exe -d -q %s%sGbtLzC.bin -o %s%sGbtPc.xml' %(XmlCliToolsDir,_os.sep, TempFolder, _os.sep, TempFolder, _os.sep))
					else:
						_os.system('%s%sLzmaCompress -d -q %s%sGbtLzC.bin -o %s%sGbtPc.xml' %(XmlCliToolsDir,_os.sep, TempFolder, _os.sep, TempFolder, _os.sep))
					RemoveFile("%s%sGbtLzC.bin" %(TempFolder, _os.sep))
					try:
						if(_os.path.getsize('%s%sGbtPc.xml' %(TempFolder, _os.sep))):
							_log.info("LZMA Compressed XML Decompressed Successfully")
							ComprXmlFound = True
							break
					except:
						_log.info("Decompression Failed!, falling back to regular XML download.")
						ComprXmlFound = False
				if ( ((PacketHdr & 0xFFFFFFFFFF) == 0x434F4E5424) and (PacketSize != 0) ):	# cmp with $TNOC
					_log.info("Found Tiano Compressed XML, Downloading it")
					memsave("%s%sGbtTianoC.bin" %(TempFolder, _os.sep), (PacketAddr+8), int(PacketSize))
					if _os.name == "nt":
						_os.system('%s%sTianoCompress.exe -d -q %s%sGbtTianoC.bin -o %s%sGbtPc.xml' %(XmlCliToolsDir,_os.sep, TempFolder, _os.sep, TempFolder, _os.sep))
					else:
						_os.system('%s%sTianoCompress -d -q %s%sGbtTianoC.bin -o %s%sGbtPc.xml' %(XmlCliToolsDir,_os.sep, TempFolder, _os.sep, TempFolder, _os.sep))
					RemoveFile("%s%sGbtTianoC.bin" %(TempFolder, _os.sep))
					try:
						if(_os.path.getsize('%s%sGbtPc.xml' %(TempFolder, _os.sep))):
							_log.info("Tiano Compressed XML Decompressed Successfully")
							ComprXmlFound = True
							break
					except:
						_log.info("Decompression Failed!, falling back to regular XML download.")
						ComprXmlFound = False
				PacketAddr = ((PacketAddr+8+PacketSize+0xFFF) & 0xFFFFF000)
		if (ComprXmlFound):
			TempXML=open('%s%sGbtPc.xml' %(TempFolder, _os.sep), "rb")
			XmlListBuff = list(TempXML.read())
			TempXML.close()
			PatchXmlData(XmlListBuff, XmlAddr,XmlSize)
			RemoveFile("%s%sGbtPc.xml" %(TempFolder, _os.sep))
			NewXmlFile = open(filename, "wb") # opening for writing
			NewXmlFile.write(_string.join(XmlListBuff, ''))
			NewXmlFile.close()
		else:
			_log.info("Compressed XML is not supported, Downloading Regular XML")
			if((InterfaceType != "itpii") and (InterfaceType != "simics") and (InterfaceType != "ltb") and (InterfaceType != "svlegitp")):
				ITPOptimz = 0
			if ( (XmlCmp(filename, XmlAddr) == False) or (ITPOptimz == 0) ):
				_log.info( "Host XML didn't exist or is different from Target XML, downloading Target XML..")
				memsave(filename, XmlAddr, int(XmlSize))  # saves complete xml
			else:
				_log.info("Target XML is same as the one Pointed to, skipping XML download")
		_log.info( "Saved XML Data as %s" %filename)
	else:
		_log.error("XML is not valid or not yet generated XmlAddr = 0x%X, XmlSize = 0x%X" %(XmlAddr,XmlSize))
		Status = 1
	SanitizeXml(filename)
	CloseInterface()
	return Status

# Create or Compare and Save Target XML Header to file.
def XmlCmp(filename, XmlAddr):
	HdrCmpLen = 0x140
	targetbuff = list(memBlock(XmlAddr, HdrCmpLen))
	if( (_os.path.isfile(filename)) and (_os.path.getsize(filename) > 0x800) ):
		_log.info("File Exists:  comparing target & host XML header")
		HostXML=open(filename, "rb")
		hbuffer = list(HostXML.read(HdrCmpLen))
		HostXML.close()
		if(hbuffer[0:HdrCmpLen-1] == targetbuff[0:HdrCmpLen-1]):  # compare host & target XML header
			return True # indicates Target XML was unchanged
	return False  # indicates Target XML file was not yet created

# Extract knob name from given KnobEntry pointer.
def findKnobName(KnobEntryAdd):
	KnobEntryBuff = memBlock(KnobEntryAdd, 0x100) # copy first 256 chars in temp buffer
	Type = Name = ''
	for i in range(0x0,0x100,1): # assuming the name attribute will be found within first 256 chars
		Knobname = ReadBuffer(KnobEntryBuff, i, 11, HEX)  # read 11 chars from buffer
		if (Knobname == 0x223D657079547075746573):  # compare with setupType="
			for j in range(0x0,0x80,1):  # assuming max knob name size of 128 chars
				if (ReadBuffer(KnobEntryBuff, i+11+j, 01, HEX) == 0x22): # save till next "
					Type = ReadBuffer(KnobEntryBuff, i+11, j, ASCII) # return Knob name
					break
	for i in range(0x0,0x100,1): # assuming the name attribute will be found within first 256 chars
		Knobname = ReadBuffer(KnobEntryBuff, i, 0x06, HEX)  # read 6 chars from buffer
		if (Knobname == 0x223D656D616E):  # compare with name="
			for j in range(0x0,0x80,1):  # assuming max knob name size of 128 chars
				if (ReadBuffer(KnobEntryBuff, i+6+j, 01, HEX) == 0x22): # save till next "
					Name = ReadBuffer(KnobEntryBuff, i+6, j, ASCII) # return Knob name
					break
	return (Type,Name)

# Extract BIOS Version details from XML.
def getBiosDetails():
	Platformname=''
	BiosName=''
	BiosTimestamp=''
	InitInterface()
	DRAM_MbAddr = GetDramMbAddr()  # Get DRam MAilbox Address from Cmos.
	_log.debug("DRAM_MbAddr = 0x%X" %(DRAM_MbAddr))
	if (DRAM_MbAddr == 0x0):
		_log.error("Dram Shared Mailbox not Valid, hence exiting")
		CloseInterface()
		return (Platformname, BiosName, BiosTimestamp) #empty strings
	DramSharedMBbuf = memBlock(DRAM_MbAddr,0x200) # Read/save parameter buffer
	(XmlAddr,XmlSize)  = readxmldetails(DramSharedMBbuf)
	if (XmlAddr == 0):
		_log.error("Platform Configuration XML not ready, hence exiting")
		runcpu()
		CloseInterface()
		return (Platformname, BiosName, BiosTimestamp) #empty Strings
	if(isxmlvalid(XmlAddr,XmlSize)):
		XmlEntryBuff = memBlock(XmlAddr, 0x200) # copy first 512 chars in temp buffer
		for i in range(0x0,0x200,1): # assuming the name attribute will be found within first 512 chars
			Platformnametmp = ReadBuffer(XmlEntryBuff, i, 15, ASCII)  # read 16 chars from buffer
			BiosDetailstmp = ReadBuffer(XmlEntryBuff, i, 10, ASCII)  # read 16 chars from buffer
			if ( (Platformnametmp == "<PLATFORM NAME=") and (Platformname == '') ):  # compare with name="
				for j in range(0x0,0x80,1):  # assuming max Platform name size of 128 chars
					if (ReadBuffer(XmlEntryBuff, i+16+j, 01, HEX) == 0x22): # save till next "
						Platformname = ReadBuffer(XmlEntryBuff, i+16, j, ASCII) # return Knob name
						_log.result("Platform Name = %s" %Platformname)
						break
			if ( ( (BiosDetailstmp == "<CPUSVBIOS") or (BiosDetailstmp[0:7] == "<SVBIOS") or (BiosDetailstmp[0:5] == "<BIOS") ) and (BiosName == '') and (BiosTimestamp == '') ):  # compare with name="
				if(BiosDetailstmp == "<CPUSVBIOS"):
					AtriLen = 11
				elif(BiosDetailstmp[0:7] == "<SVBIOS"):
					AtriLen = 8
				elif(BiosDetailstmp[0:5] == "<BIOS"):
					AtriLen = 6
				for j in range(0x0,0x100,1):  # assuming max BIOS name size of 256 chars
					BiosNametmp = ReadBuffer(XmlEntryBuff, i+AtriLen+j, 8, ASCII)  # read 16 chars from buffer
					BiosTimestamptmp = ReadBuffer(XmlEntryBuff, i+AtriLen+j, 7, ASCII)  # read 16 chars from buffer
					if(( BiosNametmp == "VERSION=") and (BiosName == '') ):
						for k in range(0x0,0x80,1):  # assuming max BIOS name size of 128 chars
							if (ReadBuffer(XmlEntryBuff, i+AtriLen+j+9+k, 01, HEX) == 0x22): # save till next "
								BiosName = ReadBuffer(XmlEntryBuff, i+AtriLen+j+9, k, ASCII) # return Knob name
								_log.result("Bios Version = %s" %BiosName)
								break
					if( (BiosTimestamptmp == "TSTAMP=") and (BiosTimestamp == '') ):
						for k in range(0x0,0x80,1):  # assuming max BIOS name size of 128 chars
							if (ReadBuffer(XmlEntryBuff, i+AtriLen+j+8+k, 01, HEX) == 0x22): # save till next "
								BiosTimestamp = ReadBuffer(XmlEntryBuff, i+AtriLen+j+8, k, ASCII) # return Knob name
								_log.result("Bios Timestamp = %s" %BiosTimestamp)
								break

	CloseInterface()
	return (Platformname, BiosName, BiosTimestamp)

def ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr):
	memwrite( CLI_ReqBuffAddr, 8, 0 )
	memwrite( CLI_ReqBuffAddr + 8, 8, 0 )
	memwrite( CLI_ResBuffAddr, 8, 0 )
	memwrite( CLI_ResBuffAddr + 8, 8, 0 )

#  Search fo rthe EFI Compatible tables in 0xE000/F000 segments
#TODO: What are you printing here?
def getEfiCompatibleTableBase():
	EfiComTblSig = 0x24454649
	for Index in range (0, 0x1000, 0x10):
		Sig1 = memread(0xE0000+Index, 4)
		Sig2 = memread(0xF0000+Index, 4)
		if (Sig1 == EfiComTblSig):
			BaseAddress = 0xE0000+Index;
			#print "Found EfiCompatibleTable Signature at 0x%X" %(BaseAddress)
			return BaseAddress
		if (Sig2 == EfiComTblSig):
			BaseAddress = 0xF0000+Index;
			#print "Found EfiCompatibleTable Signature at 0x%X" %(BaseAddress)
			return BaseAddress
	_log.result(hex(Index))
	return 0

#  Search for the GBT EXT table in BIOS 0xF000 segment
#TODO: What are you printing here?
def getGbtExtBase():
	GbtExtTblSig = 0x54424724
	for Index in range (0, 0x8000, 0x100):
		Sig1 = memread(0xF0000+Index, 4)
		Sig2 = memread(0xF8000+Index, 4)
		if (Sig1 == GbtExtTblSig):
			BaseAddress = 0xF0000+Index;
			_log.info("Found GBT EXT table Signature at 0x%X" %(BaseAddress))
			return BaseAddress
		if (Sig2 == GbtExtTblSig):
			BaseAddress = 0xF8000+Index;
			_log.info("Found GBT EXT table Signature at 0x%X" %(BaseAddress))
			return BaseAddress
	_log.result(hex(Index))
	return 0

#  gDramSharedMailBoxGuid = { 0x9d99a394, 0x1878, 0x4d2c, { 0x98, 0xe9, 0xc1, 0x6b, 0x8e, 0xc4, 0x79, 0x33 }}
def readDramMbAddrFromEFI():
	DramSharedMailBoxGuidLow = 0x4D2C18789D99A394
	DramSharedMailBoxGuidHigh = 0x3379C48E6BC1E998

	EfiCompatibleTableBase = getEfiCompatibleTableBase()
	if(EfiCompatibleTableBase == 0):
		return 0
	gST = memread(EfiCompatibleTableBase+0x14, 4)
	#print " EFI SYSTEM TABLE Address = 0x%X " %gST
	Signature = _binascii.unhexlify((hex(memread(gST, 8))[2:]).strip('L'))[::-1]
	#print "Signature at EFI ST = \"%s\"  Revision = %d.%d" %(Signature, memread(gST+8, 2), memread(gST+0xA, 2))
	count = 0
	FirmwarePtr = memread(gST+0x18, 8)
	FirmwareRevision = memread(gST+0x20, 4)
	BiosStr = ""
	while (1):
		Value = int(memread(FirmwarePtr+count, 2))
		if (Value == 0):
			break
		BiosStr = BiosStr + _binascii.unhexlify(hex(Value)[2:])
		count = count + 2
	_log.info("Firmware : %s" %(BiosStr))
	#print "Firmware Revision: 0x%X" %FirmwareRevision
	EfiConfigTblEntries = memread(gST+0x68, 8)
	EfiConfigTbl = memread(gST+0x70, 8)
	#print "EfiConfigTblEntries = %d  EfiConfigTbl Addr = 0x%X" %(EfiConfigTblEntries, EfiConfigTbl)
	_log.info("Searching Dram Shared Mailbox address from EfiConfigTable..")
	Offset = 0
	DramMailboxAddr = 0
	for Index in range (0, EfiConfigTblEntries):
		GuidLow = memread(EfiConfigTbl+Offset, 8)
		GuidHigh = memread(EfiConfigTbl+8+Offset, 8)
		if ( (GuidLow == DramSharedMailBoxGuidLow) and (GuidHigh == DramSharedMailBoxGuidHigh) ):
			DramMailboxAddr = int(memread(EfiConfigTbl+16+Offset, 8))
			#print "Found Dram Shared MailBox Address = 0x%X" %(DramMailboxAddr)
			break
		Offset = Offset + 0x18
	return DramMailboxAddr

#EFI ST offset = 0x14
#ACPI table offset = 0x1C
#E820 Table offset = 0x22
#E820 table Length = 0x26
def PrintE820Table ():
	Offset = 0
	Index = 0
	E820TableList = {}
	EfiCompatibleTableBase = getEfiCompatibleTableBase()
	E820Ptr = memread(EfiCompatibleTableBase+0x22, 4)
	Size = memread(EfiCompatibleTableBase+0x26, 4)
	_log.result( ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,")
	_log.result( "E820[no]: Start Block Address ---- End Block Address , Type = Mem Type")
	_log.result( "``````````````````````````````````````````````````````````````````````")
	while (1):
		BaseAddr = memread(E820Ptr+Offset, 8)
		Length = memread(E820Ptr+Offset+8, 8)
		Type =  memread(E820Ptr+Offset+16, 4)
		_log.result("E820[%2d]:  0x%16lX ---- 0x%-16lX, Type = 0X%x " %(Index, BaseAddr, (BaseAddr+Length), Type))
		E820TableList[Index] = [BaseAddr, Length, Type]
		Index = Index + 1
		Offset = Offset + 20
		if (Offset >= Size):
			break
	_log.result(",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,")
	return E820TableList
