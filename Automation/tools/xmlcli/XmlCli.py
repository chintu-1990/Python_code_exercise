#!/usr/bin/env python2.6
# Cscripts remove start
#-------------------------------------------------------------------------------------------------------------
# CLI Python wrapper for Programing setup knobs via S/W BIOS interface, also several other capabilities.
# CLI interface buffer is located in DRAM, the address of this buffer is part of shared Mailbox data.
# DRAM Shared Mailbox Address is publised in Cmos location
# This CLI wrapper parses the user input file and the Knobs XML to generate a binary buffer which is
# is then passed to CLI for programming & reading back the desired knobs list.
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
# Modified V1.1:   10th May 2013 by Amol
#                  - Fixed the gangesProgKnobs() to work correctly in CpuSv enviornment.
#                  - Added support in cliProgBIOS() to flash descriptor region to enable all BIOS regions for complete BIOS flashing
#                  - Added cliProgBIOSSector() function to program BIOS sectors independently, this func can also be used to flash ME region.
#                  - Added RandResetTest() for CpuSv enviornment.
#                  - Added ExeSvCode() for running SV specific code, BIOS CLI needs to support this API (SV only usage)
# Modified V1.2:   26th July 2013 by Amol & Shirisha (for 64-bit SvHif DLL's)
#                  - upgraded the ExeSvCode() func to take ArgBuff File to pass arguments to the custom SV code.
#                  - Added support for loading 64 bit SvHif DLL's for 64 bit python.
#                  - Updated other areas of SvHif interface scripts.
#                  - Updated ReadMe.txt for the updated usage model with ITP, SVOS & SvHif interfaces.
#
# Modified V1.5:   4th December 2014 by Amol
#                  - Adding new feature to the Offline capability which will now parse more details from the given Bios Binary.
#                  - The details (Bios Regions, PCH straps, Ucode Entries, FIT entries, ME & ACM versions) will be published in the generated XML.
#
# Modified V1.6:   15th December 2014 by Amol
#                  - Added new Feature (Fit table verify & Update) to ProcessUcode() func.
#                  - The Fit Table verification will verify the FIT table for Ucode entries.
#                  - Will indicate the verification result for "read" operation.
#                  - Will indicate & Fix/update the FIT table for "update" & "delete" operations.
#                  - if one has to fix the FIT table without affecting anything else, then need to pass "fixfit" as 1st arg to ProcessUcode() func.
#
# Scope: Intended for internal Validation use only
#-------------------------------------------------------------------------------------------------------------
__author__ = 'ashinde'
# Cscripts remove end
import os, time, binascii, string, filecmp, struct
import XmlCliLib as clb
import XmlIniParser as prs
import UefiFwParser as fwp

clb._log.setConsoleLevel("info")

# TEMP enviroment fixx as SVOS env don't have TEMP defined
if os.environ.get('TEMP') is None:
	os.environ['TEMP']=''

def cliProcessKnobs(xmlfilename, inifilename, CmdSubType, ignoreXmlgeneration=False, PrintResParams=True, ResBufFilename=0, KnobsVerify=False, KnobsDict={}):
	clb.InitInterface()
	DRAM_MbAddr = clb.GetDramMbAddr() # Get DRam MAilbox Address.
	DramSharedMBbuf = clb.memBlock(DRAM_MbAddr,0x200) # Read/save parameter buffer
	if(ignoreXmlgeneration):
		clb._log.info("Skipping XML Download, Using the given XML File")
	else:
		if (CmdSubType != clb.CLI_KNOB_LOAD_DEFAULTS):
			if ( clb.SaveXml(xmlfilename, 1, MbAddr=DRAM_MbAddr) == 1 ):   # Check and Save the GBT XML knobs section.
				clb._log.error("Aborting due to Error!")
				clb.CloseInterface()
				return 1
	CLI_ReqBuffAddr      = clb.readclireqbufAddr(DramSharedMBbuf)  # Get CLI Request Buffer Adderss
	CLI_ResBuffAddr      = clb.readcliresbufAddr(DramSharedMBbuf)  # Get CLI Response Buffer Address
	clb._log.info("CLI Request Buffer Addr = 0x%X   CLI Response Buffer Addr = 0x%X" %(CLI_ReqBuffAddr, CLI_ResBuffAddr))
	if ( (CLI_ReqBuffAddr == 0) or (CLI_ResBuffAddr == 0) ):
		clb._log.error("CLI buffers are not valid or not supported, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	# Clear CLI Command & Response buffer headers
	clb.ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr)
	basepath = os.path.dirname(clb.PlatformConfigXml)

	if (CmdSubType != clb.CLI_KNOB_LOAD_DEFAULTS):
		binfile=os.sep.join([basepath, "biosKnobsdata.bin"])
		if(clb.FlexConCfgFile):
			prs.GenBiosKnobsIni(xmlfilename, inifilename, clb.TmpKnobsIniFile)
			inifilename = clb.TmpKnobsIniFile
		tmpBuff = prs.parseCliinixml(xmlfilename, inifilename, binfile)
		if(len(tmpBuff) == 0):
			clb._log.error("Aborting due to Error!")
			clb.CloseInterface()
			return 1
		elif (clb.ReadBuffer(tmpBuff, 0, 4, clb.HEX) == 0):
			clb._log.result("Request buffer is Empty, No Action required, Aborting...")
			clb.CloseInterface()
			return 0
		clb._log.info("Req Buffer Bin file used is %s" %binfile)
		clb.load_data(binfile, CLI_ReqBuffAddr+clb.CLI_REQ_RES_READY_PARAMSZ_OFF)

	if (CmdSubType == clb.CLI_KNOB_APPEND):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 4, clb.APPEND_BIOS_KNOBS_CMD_ID)
	elif (CmdSubType == clb.CLI_KNOB_RESTORE_MODIFY):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 4, clb.RESTOREMODIFY_KNOBS_CMD_ID)
	elif (CmdSubType == clb.CLI_KNOB_READ_ONLY):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 4, clb.READ_BIOS_KNOBS_CMD_ID)
	elif (CmdSubType == clb.CLI_KNOB_LOAD_DEFAULTS):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 4, clb.LOAD_DEFAULT_KNOBS_CMD_ID)

	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_SIG_OFF, 4, clb.CLI_REQ_READY_SIG )
	clb._log.info("CLI Mailbox programmed, now issuing S/W SMI to program knobs..")

	clb.triggerSMI(0xF6)	# trigger S/W SMI for CLI
	if (clb.WaitForCliResponse(CLI_ResBuffAddr, 3) == 1):
		clb._log.error("CLI Response not ready, Aborting....")
		clb.CloseInterface()
		return 1

	ResStatus = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_STATUS_OFF, 4))
	ResFlags = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_FLAGS_OFF, 2))
	if( ((ResFlags & 0x3) != 0) or (ResStatus != 0) ):
		clb._log.error("BIOS knobs CLI Command returned with some errors, Aborting!")
		clb.CloseInterface()
		return 1

	ResParamSize = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4))
	if (ResParamSize == 0):
		clb._log.result("BIOS knobs CLI Command ended successfully, CLI Response buffer's Parameter size is 0, hence returning..")
		clb.CloseInterface()
		return 0

	ResParambuff = clb.memBlock((CLI_ResBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE), ResParamSize)
	if (ResBufFilename == 0):
		ResBufFilename = os.sep.join([basepath, "RespBuffdata.bin"])
	out_file = open(ResBufFilename, "wb") # opening for writing
	out_file.write(ResParambuff)
	out_file.close()

	clb._log.result("BIOS knobs CLI Command ended successfully",)

	offsetIn = 4
	Index = 0
	KnobsDict.clear()
	if( CmdSubType != clb.CLI_KNOB_LOAD_DEFAULTS ):
		InputFile = open(binfile, 'rb')
		InputFilePart = InputFile.read()
		InputFile.close()
		NumberOfEntries = clb.ReadBuffer(InputFilePart, 0, 4, clb.HEX)
	else:
		NumberOfEntries = 1
	while(NumberOfEntries != 0):
		offsetOut = 0
		if( CmdSubType != clb.CLI_KNOB_LOAD_DEFAULTS ):
			InVarId      = clb.ReadBuffer(InputFilePart, offsetIn+0, 1, clb.HEX)
			InKnobOffset = clb.ReadBuffer(InputFilePart, offsetIn+1, 2, clb.HEX)
			InKnobSize   = clb.ReadBuffer(InputFilePart, offsetIn+3, 1, clb.HEX)
			InputValue   = clb.ReadBuffer(InputFilePart, offsetIn+4, InKnobSize, clb.HEX)
		else:
			InVarId = InKnobOffset = InKnobSize = InputValue = 0
		while(1):
			if (offsetOut >= ResParamSize):
				break
			OutVarId      = clb.ReadBuffer(ResParambuff, offsetOut+6, 1, clb.HEX)
			OutKnobOffset = clb.ReadBuffer(ResParambuff, offsetOut+7, 2, clb.HEX)
			OutKnobSize   = clb.ReadBuffer(ResParambuff, offsetOut+9, 1, clb.HEX)
			if( ((OutVarId == InVarId) and (OutKnobOffset == InKnobOffset) and (OutKnobSize == InKnobSize)) or  (CmdSubType == clb.CLI_KNOB_LOAD_DEFAULTS) ):
				DefVal           = clb.ReadBuffer(ResParambuff, offsetOut+10, OutKnobSize, clb.HEX)
				OutValue         = clb.ReadBuffer(ResParambuff, offsetOut+10+OutKnobSize, OutKnobSize, clb.HEX)
				KnobEntryAdd     = clb.ReadBuffer(ResParambuff, offsetOut+0, 4, clb.HEX)
				Type, KnobName   = clb.findKnobName(KnobEntryAdd)
				KnobsDict[Index] = {'Type': Type, 'KnobName': KnobName, 'VarId': OutVarId, 'Offset': OutKnobOffset, 'Size': OutKnobSize, 'InValue': InputValue, 'DefValue': DefVal, 'OutValue': OutValue, 'UqiVal': "", 'Prompt': ""}
				Index = Index + 1
				if( CmdSubType != clb.CLI_KNOB_LOAD_DEFAULTS ):
					break
			offsetOut = offsetOut+10+(OutKnobSize*2)
		offsetIn = offsetIn+4+InKnobSize
		NumberOfEntries = NumberOfEntries-1

	if (PrintResParams):
		clb._log.result(", see below for the results..")
		clb._log.result("|--|----|------------------------------------------|--|-----------|-----------|")
		if (CmdSubType == clb.CLI_KNOB_LOAD_DEFAULTS):
			clb._log.result("|VI|Ofst|                 Knob Name                |Sz|PreviousVal|RestoredVal|")
		else:
			clb._log.result("|VI|Ofst|                 Knob Name                |Sz|   DefVal  |   CurVal  |")
		clb._log.result("|--|----|------------------------------------------|--|-----------|-----------|")
		offset = 0
		for KnobCount in range (0, len(KnobsDict)):   # read and print the return knobs entry parameters from CLI's response buffer
			if(KnobsDict[KnobCount]['Type'] == 'string'):
				DefStr = binascii.unhexlify(hex(KnobsDict[KnobCount]['DefValue'])[2::].strip('L').zfill(KnobsDict[KnobCount]['Size']*2).upper().strip('L').replace('00', ''))[::-1]
				OutStr = binascii.unhexlify(hex(KnobsDict[KnobCount]['OutValue'])[2::].strip('L').zfill(KnobsDict[KnobCount]['Size']*2).upper().strip('L').replace('00', ''))[::-1]
				clb._log.result("|%2X|%4X|%42s|%2X| L\"%s\" | L\"%s\" |" %(KnobsDict[KnobCount]['VarId'], KnobsDict[KnobCount]['Offset'], KnobsDict[KnobCount]['KnobName'], KnobsDict[KnobCount]['Size'], DefStr, OutStr))
			else:
				clb._log.result("|%2X|%4X|%42s|%2X| %8X  | %8X  |" %(KnobsDict[KnobCount]['VarId'], KnobsDict[KnobCount]['Offset'], KnobsDict[KnobCount]['KnobName'], KnobsDict[KnobCount]['Size'], KnobsDict[KnobCount]['DefValue'], KnobsDict[KnobCount]['OutValue']))
			clb._log.result("|--|----|------------------------------------------|--|-----------|-----------|")
	else:
		clb._log.result(", Print Parameter buff is disabled..")

	ReturnVal = 0
	if( KnobsVerify and (CmdSubType != clb.CLI_KNOB_LOAD_DEFAULTS) ):
		VerifyErrCnt = 0
		for KnobCount in range (0, len(KnobsDict)):
			if(KnobsDict[KnobCount]['InValue'] != KnobsDict[KnobCount]['OutValue']):
				VerifyErrCnt = VerifyErrCnt + 1
				clb._log.result("Verify Fail: Knob = %s  ExpectedVal = 0x%X    CurrVal = 0x%X " %(KnobsDict[KnobCount]['KnobName'], KnobsDict[KnobCount]['InValue'], KnobsDict[KnobCount]['OutValue']))
		if (VerifyErrCnt == 0):
			clb._log.result("Verify Passed!")
		else:
			clb._log.result("Verify Failed!")
			ReturnVal = 1
	clb.CloseInterface()
	return ReturnVal

def gangesProgKnobs(ItpAsHif=0, xmlfilename=clb.PlatformConfigXml, inifilename=clb.KnobsIniFile):
	clb.InitInterface()
	DRAM_MbAddr = clb.GetDramMbAddr() # Get DRam MAilbox Address.
	DramSharedMBbuf = clb.memBlock(DRAM_MbAddr,0x200) # Read/save parameter buffer

	if ( clb.SaveXml(xmlfilename, 1, MbAddr=DRAM_MbAddr) == 1 ):   # Check and Save the GBT XML knobs section.
		clb._log.error("Aborting due to Error!")
		clb.CloseInterface()
		return 1

	LegMailboxAddrOffset = clb.readLegMailboxAddrOffset(DramSharedMBbuf)
	if(ItpAsHif):
		LegMailboxAddr = (DRAM_MbAddr + LegMailboxAddrOffset);          # set Legacy mailbox address pointing to DRAM, since its ITP as HIF
	else:
		LegMailboxAddr = clb.ReadBuffer(DramSharedMBbuf, + (LegMailboxAddrOffset + 0x4C), 4, clb.HEX)+ LegMailboxAddrOffset   # Read SvHif Cards Legacy Mailbox address from Dram Shared Mailbox
	clb._log.info( "Legacy Mailbox Addr = 0x%X  " %(LegMailboxAddr))
	if ( (LegMailboxAddr == 0) or (LegMailboxAddr == 0xFFFFFFFF) ):
		clb._log.error("Legacy Mailbox Addr is not valid, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	basepath = os.path.dirname(xmlfilename)
	binfile=os.sep.join([basepath, "biosKnobsdata.bin"])
	tmpBuff = prs.parseGangesinixml(xmlfilename, inifilename, binfile)
	clb.load_data(binfile, LegMailboxAddr)                # load the mailbox map
	clb.memwrite( LegMailboxAddr, 4, 0xB10590B5)          # program Mailbox valid signature
	clb._log.info("Legacy SvHif Mailbox programmed, waiting for CpuSv BIOS Hooks to respond now..")

	for retryCnt in range(0x0,10,1):		# set number of retries here, currently set to 3
		clb.runcpu()
		time.sleep(3)
		clb.haltcpu()
		PostDone = clb.memread(LegMailboxAddr+0x12, 2)
		if (PostDone == 0xF4):
			clb._log.result("Ganges flow posted 0xF4 (Power-Good reset required)...")
			clb.CloseInterface()
			return 0
		if (PostDone == 0xF6):
			clb._log.result("Ganges flow posted 0xF6 (Ready to proceed)...")
			clb.CloseInterface()
			return 0

	clb.CloseInterface()
	clb._log.error("Python Based loader timed-out, Aborting....")
	return 1

# Descriptor_Region                 = 0
# BIOS_Region                       = 1
# ME_Region                         = 2
# GBE_Region                        = 3
# PDR_Region                        = 4
# Device_Expan_Region               = 5
# Sec_BIOS_Region                   = 6
def CompareFlashRegion(RefBiosFile, NewBiosFile, Region=fwp.ME_Region):
	BiosRomFile = open(RefBiosFile, "rb")
	DescRegionListBuff = list(BiosRomFile.read(0x1000))		# first 4K region is Descriptor region.
	BiosRomFile.close()
	fwp.FlashRegionInfo(DescRegionListBuff, False)
	if(fwp.FwIngrediantDict['FlashDescpValid'] != 0):
		Offset = fwp.FwIngrediantDict['FlashRegions'][Region]['BaseAddr']
		RegionSize = (fwp.FwIngrediantDict['FlashRegions'][Region]['EndAddr'] - Offset + 1)
		RefBiosRomFile = open(RefBiosFile, "rb")
		tmpBuff = RefBiosRomFile.read(Offset)
		RefRegionBuffList = list(RefBiosRomFile.read(RegionSize))
		RefBiosRomFile.close()
		NewBiosRomFile = open(NewBiosFile, "rb")
		tmpBuff = NewBiosRomFile.read(Offset)
		NewRegionBuffList = list(NewBiosRomFile.read(RegionSize))
		NewBiosRomFile.close()
		clb._log.info("Comparing Region \"%s\" at Flash binary Offset: 0x%X  Size: 0x%X " %(fwp.FlashRegionDict[Region], Offset, RegionSize))
		if(RefRegionBuffList == NewRegionBuffList):
			clb._log.result("Region \"%s\" matches between the two binaries" %(fwp.FlashRegionDict[Region]))
			return 0
		else:
			clb._log.result("Region \"%s\" is different between the two binaries" %(fwp.FlashRegionDict[Region]))
			return 1

def FetchSpi(filename, BlkOffset, BlockSize, FetchAddr=0):
	clb.InitInterface()
	DRAM_MbAddr = clb.GetDramMbAddr()  # Get DRam MAilbox Address.
	DramSharedMBbuf = clb.memBlock(DRAM_MbAddr,0x110) # REad/save parameter buffer
	CLI_ReqBuffAddr = clb.readclireqbufAddr(DramSharedMBbuf)  # Get CLI Request Buffer Adderss
	CLI_ResBuffAddr = clb.readcliresbufAddr(DramSharedMBbuf)  # Get CLI Response Buffer Address
	clb._log.info("CLI Request Buffer Addr = 0x%X   CLI Response Buffer Addr = 0x%X" %(CLI_ReqBuffAddr, CLI_ResBuffAddr))
	if ( (CLI_ReqBuffAddr == 0) or (CLI_ResBuffAddr == 0) ):
		clb._log.error("CLI buffers are not valid or not supported, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	# Clear CLI Command & Response buffer headers
	clb.ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr)

	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 8, clb.FETCH_BIOS_CMD_ID )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4, 0x10 )					# program Parameter size
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 0x4, 4, int(BlkOffset) )	# program Offset
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 0x8, 4, int(BlockSize) )	# program BIOS Image Size
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 0xC, 8, int(FetchAddr) )	# program Fetch Address for the BIOS Image Size
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_SIG_OFF, 4, clb.CLI_REQ_READY_SIG )
	clb._log.info("CLI Mailbox programmed, now issuing S/W SMI to program BIOS..")
	clb.triggerSMI(0xF6)	# trigger S/W SMI for CLI
	if (clb.WaitForCliResponse(CLI_ResBuffAddr, 8) == 1):
		clb._log.error("CLI Response not ready, Aborting....")
		clb.CloseInterface()
		return 1

	ResStatus = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_STATUS_OFF, 4))
	ResFlags = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_FLAGS_OFF, 2))
	if( ((ResFlags & 0x3) != 0) or (ResStatus != 0) ):
		clb._log.error("CLI Command returned with some errors, Aborting!")
		clb.CloseInterface()
		return 1

	ResParamSize = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4))
	if (ResParamSize == 0):
		clb._log.error("CLI Response buffer's Parameter size is 0, hence Aborting..")
		clb.CloseInterface()
		return 1

	ResParambuff = clb.memBlock((CLI_ResBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE), ResParamSize)
	srcAddr = int(clb.ReadBuffer(ResParambuff, 0, 8, clb.HEX))
	clb.memsave(filename, srcAddr, int(BlockSize))

	clb.CloseInterface()
	return 0

# TmpBuffPtr is only used for 'read' operation, optional if read size is less than 1 MB
def SpiFlash(Operation='read', FilePtr='', Region=fwp.Invalid_Region, RegionOffset=0, RegionSize=0, TmpBuffPtr=0x20000000):
	clb.InitInterface()
	DRAM_MbAddr = clb.GetDramMbAddr()  # Get DRam MAilbox Address.
	DramSharedMBbuf = clb.memBlock(DRAM_MbAddr,0x110) # REad/save parameter buffer
	CLI_ReqBuffAddr = clb.readclireqbufAddr(DramSharedMBbuf)  # Get CLI Request Buffer Adderss
	CLI_ResBuffAddr = clb.readcliresbufAddr(DramSharedMBbuf)  # Get CLI Response Buffer Address
	clb._log.info("CLI Request Buffer Addr = 0x%X   CLI Response Buffer Addr = 0x%X" %(CLI_ReqBuffAddr, CLI_ResBuffAddr))
	if ( (CLI_ReqBuffAddr == 0) or (CLI_ResBuffAddr == 0) ):
		clb._log.error("CLI buffers are not valid or not supported, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	# Clear CLI Command & Response buffer headers
	clb.ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr)

	if (Region != fwp.Invalid_Region):
		if (Region == fwp.Descriptor_Region):
			RegionOffset = 0
			RegionSize = 0x1000
		else:
			clb._log.info("Fetching Region Offset & Size from the Descriptor section")
			DescFile = os.sep.join([clb.TempFolder, "DescFile.bin"])
			Status = FetchSpi(DescFile, 0, 0x1000, (CLI_ResBuffAddr+0x100))
			if(Status == 0):
				DescBinFile = open(DescFile, "rb")
				DescBinListBuff = list(DescBinFile.read())
				DescBinFile.close()
				UpdateOnlyBiosRegion = False
				Status = fwp.FlashRegionInfo(DescBinListBuff, False)
				clb.RemoveFile(DescFile)
				if(Status == 0):
					if (fwp.FwIngrediantDict['FlashDescpValid'] != 0):
						RegionOffset = fwp.FwIngrediantDict['FlashRegions'][Region]['BaseAddr']
						RegionSize = fwp.FwIngrediantDict['FlashRegions'][Region]['EndAddr'] + 1 - RegionOffset
					else:
						Status = 1
			if(Status != 0):
				clb._log.error("Descriptor section not Valid, please provide RegionOffset & size, Aborting due to Error!")
				clb.CloseInterface()
				return 1
	elif ((RegionSize == 0) and (Operation == 'write')):
		RegionSize = os.path.getsize(FilePtr)

	if (RegionSize == 0):
		clb._log.error("Invalid request, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	clb._log.info("RegionOffset = 0x%X  RegionSize = 0x%X" %(RegionOffset, RegionSize))
	if(Operation == 'read'):
		if(RegionSize <= 0x100000):
			TargetFileCopyAddr = (CLI_ResBuffAddr+0x100)
		else:
			TargetFileCopyAddr = TmpBuffPtr
		Status = FetchSpi(FilePtr, RegionOffset, RegionSize, TargetFileCopyAddr)
		clb.CloseInterface()
		return Status

	OneMbBlock = 0x100000
	loopcount = RegionSize/OneMbBlock
	Remainder = RegionSize%OneMbBlock
	BinFile = open(FilePtr, "rb")
	tmpFilePart = BinFile.read(0)
	Offset = RegionOffset
	Count = 0
	for Count in range (0, loopcount):
		filePart = BinFile.read(OneMbBlock)
		BinFileName = os.path.join( os.environ.get('TEMP'), "BinPart" + "_" + str(Count) + ".bin")
		binPart = open(BinFileName, "wb")
		binPart.write(filePart)
		binPart.close()
	if(Remainder != 0):
		filePart = BinFile.read(Remainder)
		BinFileName = os.path.join( os.environ.get('TEMP'), "BinPart" + "_" + str(Count) + ".bin")
		binPart = open(BinFileName, "wb")
		binPart.write(filePart)
		binPart.close()
		loopcount = loopcount + 1
	BinFile.close()
	for Count in range (0, loopcount):
		BinFileName = os.path.join( os.environ.get('TEMP'), "BinPart" + "_" + str(Count) + ".bin")
		if ( (Count == (loopcount-1)) and (Remainder != 0) ):
			BytesToFlash = Remainder
		else:
			BytesToFlash = OneMbBlock
		# Clear CLI Command & Response buffer headers
		clb.ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr)
		clb.load_data(BinFileName, (CLI_ReqBuffAddr + 0x100))
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 8, clb.PROG_BIOS_CMD_ID )
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4, 0x10 )
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 0x4, 8, (CLI_ReqBuffAddr + 0x100) )
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 0x10, 4, BytesToFlash )
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 0xC, 4, Offset )
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_SIG_OFF, 4, clb.CLI_REQ_READY_SIG )
		clb._log.info("CLI Mailbox programmed, now issuing S/W SMI to program SPI..")
		clb.triggerSMI(0xF6)	# trigger S/W SMI for CLI
		if (clb.WaitForCliResponse(CLI_ResBuffAddr, 8, 0) == 1):
			clb._log.error("CLI Response not ready, Aborting....")
			clb.CloseInterface()
			return 1

		ResStatus = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_STATUS_OFF, 4))
		ResFlags = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_FLAGS_OFF, 2))
		if( ((ResFlags & 0x3) != 0) or (ResStatus != 0) ):
			clb._log.error("CLI Command returned with some errors, Aborting!")
			clb.CloseInterface()
			return 1

		clb._log.result("Flashed 0x%X bytes at Rom Binary Offset = 0x%X " %(BytesToFlash, Offset))
		Offset = Offset + BytesToFlash
		clb.RemoveFile(BinFileName)
	clb._log.result("SPI Region Flashed successfully, Clearing CMOS..")
	clb.clearcmos()
	clb.CloseInterface()
	return 0

# save entire/complete Target XML to desired file.
def savexml(filename=clb.PlatformConfigXml, BiosBin=0):
	if(BiosBin == 0):
		Status = clb.SaveXml(filename)
	else:
		Status = fwp.GetsetBiosKnobsFromBin(BiosBin, 0, "genxml", filename)
	return Status

def CmpBiosBinKnobs(RefBiosBinFile, MyBiosBinFile, OutFile=r"KnobsDiff.log", CmpTag="default"):
	RefXml = clb.KnobsXmlFile.replace("BiosKnobs", "RefBiosKnobs")
	MyXml = clb.KnobsXmlFile.replace("BiosKnobs", "MyBiosKnobs")
	savexml(RefXml, RefBiosBinFile)
	savexml(MyXml, MyBiosBinFile)
	prs.GenKnobsDelta(RefXml, MyXml, OutFile, CmpTag)

def CreateTmpIniFile(KnobString):
	if (KnobString == 0):
		return clb.KnobsIniFile
	else:
		IniFilePart = open(clb.TmpKnobsIniFile, "w")
		IniFilePart.write(";-----------------------------------------------------------------\n")
		IniFilePart.write("; ESS Sv BIOS contact: amol.shinde@intel.com\n")
		IniFilePart.write("; XML Shared MailBox settings for SV BIOS CLI based setup\n")
		IniFilePart.write("; The name entry here should be identical as the name from the XML file (retain the case)\n")
		IniFilePart.write(";-----------------------------------------------------------------\n")
		IniFilePart.write("[BiosKnobs]\n")
		KnobString = KnobString.replace(",", "\n")
		IniFilePart.write("%s\n" %KnobString)
		IniFilePart.close()
		return clb.TmpKnobsIniFile
	return clb.KnobsIniFile

# Program given BIOS knobs for CV.
def CvProgKnobs(KnobStr=0, BiosBin=0, BinOutSufix=0, UpdateHiiDbDef=False, BiosOut=""):
	IniFile = CreateTmpIniFile(KnobStr)
	if(BiosBin == 0):
		Status = cliProcessKnobs(clb.PlatformConfigXml, IniFile, clb.CLI_KNOB_APPEND, 0, 1, KnobsVerify=True)
	else:
		Status = fwp.GetsetBiosKnobsFromBin(BiosBin, BinOutSufix, "prog", clb.PlatformConfigXml, IniFile, UpdateHiiDbDef, BiosOut)
	return Status

# Restore & then modify given BIOS knobs for CV.
def CvRestoreModifyKnobs(KnobStr=0, BiosBin=0):
	IniFile = CreateTmpIniFile(KnobStr)
	if(BiosBin == 0):
		Status = cliProcessKnobs(clb.PlatformConfigXml, IniFile, clb.CLI_KNOB_RESTORE_MODIFY, 0, 1, KnobsVerify=True)
	else:
		clb._log.error("Restore modify operation is not supported in Offline mode, please use CvProgKnobs with pristine Bios binary to get the same effect")
		Status = 0
	return Status

# Load Default BIOS knobs for CV.
def CvLoadDefaults(BiosBin=0):
	if(BiosBin == 0):
		Status = cliProcessKnobs(clb.PlatformConfigXml, clb.KnobsIniFile, clb.CLI_KNOB_LOAD_DEFAULTS, 0, 1, KnobsVerify=False)
	else:
		clb._log.error("Load Defaults operation is not supported in Offline mode, please use pristine Bios binary instead")
		Status = 0
	return Status

# Read BIOS knobs for CV.
def CvReadKnobs(KnobStr=0, BiosBin=0):
	IniFile = CreateTmpIniFile(KnobStr)
	if(BiosBin == 0):
		Status = cliProcessKnobs(clb.PlatformConfigXml, IniFile, clb.CLI_KNOB_READ_ONLY, 0, 1, KnobsVerify=True)
	else:
		Status = fwp.GetsetBiosKnobsFromBin(BiosBin, 0, "readonly", clb.PlatformConfigXml, IniFile)
	return Status

def GenBootOrderDict(PcXml, NewBootOrderStr=""):
	Tree = prs.ET.parse(PcXml)
	BootOrderDict = {}
	BootOrderDict['OptionsDict'] = {}
	BootOrderDict['OrderList'] = {}
	OrderIndex = 0
	for SetupKnobs in Tree.getiterator(tag="biosknobs"):
		for BiosKnob in SetupKnobs.getchildren():
			SETUPTYPE = (prs.nstrip(BiosKnob.get("setupType"))).upper()
			KnobName = prs.nstrip(BiosKnob.get('name'))
			if (KnobName[0:10] == "BootOrder_"):
				BootOrderDict['OrderList'][OrderIndex] = int(prs.nstrip(BiosKnob.get('CurrentVal')), 16)
				if ( (SETUPTYPE == "ONEOF") and (OrderIndex == 0) ):
					OptionsCount = 0
					for options in BiosKnob.getchildren():
						for option in options.getchildren():
							BootOrderDict['OptionsDict'][OptionsCount] = { 'OptionText': prs.nstrip(option.get("text")), 'OptionVal': int(prs.nstrip(option.get("value")), 16) }
							OptionsCount = OptionsCount + 1
				OrderIndex = OrderIndex + 1
	if (NewBootOrderStr == ""):
		BootOrderLen = len(BootOrderDict['OrderList'])
		if(BootOrderLen == 0):
			clb._log.result("\tBoot Order Variable not found in XML!")
			return 1
		else:
			if(len(BootOrderDict['OptionsDict']) == 0):
				clb._log.result("\tBoot Order Options is empty!")
				return 1
			BootOrderString = ""
			for count in range (0, BootOrderLen):
				if (count == 0):
					BootOrderString = BootOrderString + "%02X" %(BootOrderDict['OrderList'][count])
				else:
					BootOrderString = BootOrderString + "-%02X" %(BootOrderDict['OrderList'][count])
			clb._log.result("\n\tThe Current Boot Order: %s" %BootOrderString)
			clb._log.result("\n\tList of Boot Devices in the Current Boot Order")
			for count in range (0, BootOrderLen):
				for count1 in range (0, len(BootOrderDict['OptionsDict'])):
					if(BootOrderDict['OrderList'][count] == BootOrderDict['OptionsDict'][count1]['OptionVal']):
						clb._log.result("\t\t%02X - %s" %(BootOrderDict['OptionsDict'][count1]['OptionVal'], BootOrderDict['OptionsDict'][count1]['OptionText']))
						break
	else:
		NewBootOrder = NewBootOrderStr.split('-')
		KnobString = ""
		if(len(NewBootOrder) != len(BootOrderDict['OrderList'])):
			clb._log.error("\tGiven Boot order list length doesn't match current, aborting")
			return 1
		for count in range (0, len(NewBootOrder)):
			KnobString = KnobString + "BootOrder_" + "%d" %count + " = 0x" + NewBootOrder[count] + ", "
		CvProgKnobs("%s" %KnobString)
	return 0

def GetBootOrder():
	Return=savexml(clb.PlatformConfigXml)
	if (Return==0):
		GenBootOrderDict(clb.PlatformConfigXml)
		clb._log.result("\n\tRequested operations completed successfully.\n")
		return 0
	else:
		clb._log.error("\n\tRequested operation is Incomplete\n")
		return 1

def SetBootOrder(NewBootOrderStr=""):
	try:
		Return=savexml(clb.PlatformConfigXml)
		if (Return==0):
			set_operation=GenBootOrderDict(clb.PlatformConfigXml, NewBootOrderStr)
			if (set_operation==0):
				 Return1=savexml(clb.PlatformConfigXml)
				 if (Return1==0):
					 GenBootOrderDict(clb.PlatformConfigXml)
					 clb._log.result("\tRequested operations completed successfully.\n")
					 return 0
				 else:
					 clb._log.error("\tRequested operation is Incomplete\n")
			else:
				clb._log.error("\n\tRequested operation is Incomplete\n")
		else:
			clb._log.error("\n\tRequested operation is Incomplete\n")
	except IndexError:
			clb._log.error("\n\tInvalid format to bootorder!!!\n")
	return 1

# Random Warm/Cold reset test.
def RandResetTest():
	import random
	from components import ComponentManager
	sv=ComponentManager(["socket"])
	sv.refresh()
	ResetCount=0
	WarmResetCnt=0
	ColdResetCnt=0
	clb.InitInterface(1)
	while(1):
		time.sleep(5)
		SvPostCode = (sv.socket0.uncore0.biosnonstickyscratchpad7 >> 16)
		if (SvPostCode == 0x57F5):
			ResetCount += 1
			clb._log.result("| Total Reboot's = %d | Warm Reset Cnt = %d | Cold Reset Cnt = %d |" %(ResetCount, WarmResetCnt, ColdResetCnt))
			if(random.getrandbits(1)):
				clb._log.result("Perform Warm Reset")
				WarmResetCnt += 1
				clb.warmreset()
			else:
				clb._log.result("Perform Cold Reset")
				ColdResetCnt += 1
				clb.coldreset()

	clb.CloseInterface(1)
	return 0

def ExeSvCode(Codefilename, ArgBinFile=0):
	clb.InitInterface()
	DRAM_MbAddr = clb.GetDramMbAddr()  # Get DRam MAilbox Address.
	DramSharedMBbuf = clb.memBlock(DRAM_MbAddr,0x110) # REad/save parameter buffer
	CLI_ReqBuffAddr = clb.readclireqbufAddr(DramSharedMBbuf)  # Get CLI Request Buffer Adderss
	CLI_ResBuffAddr = clb.readcliresbufAddr(DramSharedMBbuf)  # Get CLI Response Buffer Address
	clb._log.info("CLI Request Buffer Addr = 0x%X   CLI Response Buffer Addr = 0x%X" %(CLI_ReqBuffAddr, CLI_ResBuffAddr))
	if ( (CLI_ReqBuffAddr == 0) or (CLI_ResBuffAddr == 0) ):
		clb._log.error("CLI buffers are not valid or not supported, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	# Clear CLI Command & Response buffer headers
	clb.ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr)
	for Count in range (0, 0x20):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + Count, 8, 0 )
	SvSpecificCodePtr = (CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + 0x100)
	basepath = os.path.dirname(Codefilename)
	RetBuffDatafile=os.sep.join([basepath, "RetBuffdata.bin"])
	clb.load_data(Codefilename, SvSpecificCodePtr)
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 8, clb.EXE_SV_SPECIFIC_CODE_OPCODE )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4, 0x100 )			# program Parameter size
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 4, 8, SvSpecificCodePtr )
	if (ArgBinFile != 0):
                if(os.path.isfile(ArgBinFile)):
                        ArgFile = open(ArgBinFile, "rb")
                        ArgBuff = list(ArgFile.read())
                        ArgBuffSize = len(ArgBuff)
                        ArgFile.close()
                        for Loop in range (0, 0xE8):
                                if (Loop == ArgBuffSize):
                                        break
                                clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 4 + 8 + Loop, 1, int(binascii.hexlify(ArgBuff[Loop]), 16) )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_SIG_OFF, 4, clb.CLI_REQ_READY_SIG )
	clb._log.info("CLI Mailbox programmed, now issuing S/W SMI to execute the given command..")
	clb.triggerSMI(0xF6)	# trigger S/W SMI for CLI
	if (clb.WaitForCliResponse(CLI_ResBuffAddr, 8) == 1):
		clb._log.error("CLI Response not ready, Aborting....")
		clb.CloseInterface()
		return 1

	ResStatus = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_STATUS_OFF, 4))
	ResFlags = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_FLAGS_OFF, 2))
	if( ((ResFlags & 0x3) != 0) or (ResStatus != 0) ):
		clb._log.error("Command returned with some errors, Aborting!")
		clb.CloseInterface()
		return 1

	ResParamSize = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4))
	if (ResParamSize == 0):
		clb._log.error("CLI Response buffer's Parameter size is 0, hence Aborting..")
		clb.CloseInterface()
		return 1

	RetParamBuffPtr = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 4, 4))
	RetParamBuffSize = int(clb.memread(RetParamBuffPtr, 4))
	if(RetParamBuffSize != 0):
		clb.memsave(RetBuffDatafile, (RetParamBuffPtr+4), RetParamBuffSize)
	clb._log.info("RetParamBuffPtr = 0x%X    RetParamBuffSize = 0x%X" %(RetParamBuffPtr, RetParamBuffSize))
	clb.CloseInterface()
	return 0

def ProcessBrtTable(operation, BrtBinFile=0):
	clb.InitInterface()
	DRAM_MbAddr = clb.GetDramMbAddr()  # Get Dram Mailbox Address.
	DramSharedMBbuf = clb.memBlock(DRAM_MbAddr,0x110) # Read/save parameter buffer
	CLI_ReqBuffAddr = clb.readclireqbufAddr(DramSharedMBbuf)  # Get CLI Request Buffer Adderss
	CLI_ResBuffAddr = clb.readcliresbufAddr(DramSharedMBbuf)  # Get CLI Response Buffer Address
	clb._log.info("CLI Request Buffer Addr = 0x%X   CLI Response Buffer Addr = 0x%X" %(CLI_ReqBuffAddr, CLI_ResBuffAddr))
	if ( (CLI_ReqBuffAddr == 0) or (CLI_ResBuffAddr == 0) ):
		clb._log.info("CLI buffers are not valid or not supported, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	# Clear CLI Command & Response buffer headers
	clb.ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr)
	for Count in range (0, 0x20):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + Count, 8, 0 )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 8, operation )
	if( (operation == clb.CREATE_FRESH_BRT_OPCODE) or (operation == clb.ADD_BRT_OPCODE) or (operation == clb.DIS_BRT_OPCODE) ):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4, os.path.getsize(BrtBinFile) )
		if(os.path.getsize(BrtBinFile) != 0):
			clb.load_data(BrtBinFile, CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF + 4)
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_SIG_OFF, 4, clb.CLI_REQ_READY_SIG )
	clb._log.info("CLI Mailbox programmed, now issuing S/W SMI to execute the given command..")
	clb.triggerSMI(0xF6)	# trigger S/W SMI for CLI
	if (clb.WaitForCliResponse(CLI_ResBuffAddr, 8) == 1):
		clb._log.error("CLI Response not ready, Aborting....")
		clb.CloseInterface()
		return 1

	ResStatus = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_STATUS_OFF, 4))
	ResFlags = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_FLAGS_OFF, 2))
	if( ((ResFlags & 0x3) != 0) or (ResStatus != 0) ):
		clb._log.error("CLI Command returned with some errors, Aborting!")
		clb.CloseInterface()
		return 1

	ResParamSize = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4))
	if (ResParamSize == 0):
		clb._log.error("CLI Response buffer's Parameter size is 0, hence Aborting..")
		clb.CloseInterface()
		return 1

	basepath = os.path.dirname(clb.PlatformConfigXml)
	RetBuffDatafile=os.sep.join([basepath, "RetBrtTabledata.bin"])
	clb.memsave(RetBuffDatafile, (CLI_ResBuffAddr+clb.CLI_REQ_RES_BUFF_HEADER_SIZE), ResParamSize)
	clb._log.info("ResParamSize = 0x%X " %(ResParamSize))
	clb.CloseInterface()
	return 0

def ReadBrtTable():
	result = ProcessBrtTable(clb.READ_BRT_OPCODE)
	return result

def CreateFreshBrtTable(BrtBinFile):
	result = ProcessBrtTable(clb.CREATE_FRESH_BRT_OPCODE, BrtBinFile)
	return result

def AddBrtTable(BrtBinFile):
	result = ProcessBrtTable(clb.ADD_BRT_OPCODE, BrtBinFile)
	return result

def DeleteAllBrtTables():
	result = ProcessBrtTable(clb.DEL_BRT_OPCODE)
	return result

def DisableBrtTable(Index1=0, Index2=0, Index3=0, Index4=0, Index5=0, Index6=0, Index7=0, Index8=0, Index9=0, Index10=0, Index11=0):
	basepath = os.path.dirname(clb.PlatformConfigXml)
	ReqBuffDatafile=os.sep.join([basepath, "DisBrtReqBuff.bin"])
	if (Index1 == 0):
		DisBrtReqlistbuff = []
	else:
		DisBrtReqlistbuff = [chr(Index1), chr(Index2), chr(Index3), chr(Index4), chr(Index5), chr(Index6), chr(Index7), chr(Index8), chr(Index9), chr(Index10), chr(Index11)]
	count = 0
	for index in range (0, len(DisBrtReqlistbuff)):
		if(count >= len(DisBrtReqlistbuff)):
			break
		if(DisBrtReqlistbuff[count] == chr(0)):
			DisBrtReqlistbuff.pop(count)
		else:
			count = count + 1
	Req_file = open(ReqBuffDatafile, "wb") # opening for writing
	Req_file.write(string.join(DisBrtReqlistbuff, ''))
	Req_file.close()
	result = ProcessBrtTable(clb.DIS_BRT_OPCODE, ReqBuffDatafile)
	return result

def MsrAccess(operation, MsrNumber=0xFFFFFFFF, ApicId=0xFFFFFFFF, MsrValue=0):
	if( (MsrNumber == 0xFFFFFFFF) or (ApicId == 0xFFFFFFFF)):
		return 0
	clb.InitInterface()
	DRAM_MbAddr = clb.GetDramMbAddr()  # Get Dram Mailbox Address.
	DramSharedMBbuf = clb.memBlock(DRAM_MbAddr,0x110) # Read/save parameter buffer
	CLI_ReqBuffAddr = clb.readclireqbufAddr(DramSharedMBbuf)  # Get CLI Request Buffer Adderss
	CLI_ResBuffAddr = clb.readcliresbufAddr(DramSharedMBbuf)  # Get CLI Response Buffer Address
	clb._log.info("CLI Request Buffer Addr = 0x%X   CLI Response Buffer Addr = 0x%X" %(CLI_ReqBuffAddr, CLI_ResBuffAddr))
	if ( (CLI_ReqBuffAddr == 0) or (CLI_ResBuffAddr == 0) ):
		clb._log.error("CLI buffers are not valid or not supported, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	# Clear CLI Command & Response buffer headers
	clb.ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr)
	for Count in range (0, 0x20):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + Count, 8, 0 )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 8, operation )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4, 8 )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE, 4, MsrNumber )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + 4, 4, ApicId )
	if (operation == clb.WRITE_MSR_OPCODE):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + 8, 8, MsrValue )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_SIG_OFF, 4, clb.CLI_REQ_READY_SIG )
	clb._log.info("CLI Mailbox programmed, now issuing S/W SMI to execute the given command..")
	clb.triggerSMI(0xF6)	# trigger S/W SMI for CLI
	if (clb.WaitForCliResponse(CLI_ResBuffAddr, 8) == 1):
		clb._log.error("CLI Response not ready, Aborting....")
		clb.CloseInterface()
		return 1

	ResStatus = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_STATUS_OFF, 4))
	ResFlags = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_FLAGS_OFF, 2))
	if( ((ResFlags & 0x3) != 0) or (ResStatus != 0) ):
		clb._log.error("CLI Command returned with some errors, Aborting!")
		clb.CloseInterface()
		return 1

	if (operation == clb.READ_MSR_OPCODE):
		MsrValue = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE, 8))
	clb._log.result("Msr No. 0x%X  ApicId = 0x%X  MsrValue = 0x%X " %(MsrNumber, ApicId, MsrValue))
	clb.CloseInterface()
	return 0

def IoAccess(operation, IoPort=0xFFFF, Size=0xFF, IoValue=0):
	if( (IoPort == 0xFFFF) or (Size == 0xFF)):
		return 0
	clb.InitInterface()
	DRAM_MbAddr = clb.GetDramMbAddr()  # Get Dram Mailbox Address.
	DramSharedMBbuf = clb.memBlock(DRAM_MbAddr,0x110) # Read/save parameter buffer
	CLI_ReqBuffAddr = clb.readclireqbufAddr(DramSharedMBbuf)  # Get CLI Request Buffer Adderss
	CLI_ResBuffAddr = clb.readcliresbufAddr(DramSharedMBbuf)  # Get CLI Response Buffer Address
	clb._log.info("CLI Request Buffer Addr = 0x%X   CLI Response Buffer Addr = 0x%X" %(CLI_ReqBuffAddr, CLI_ResBuffAddr))
	if ( (CLI_ReqBuffAddr == 0) or (CLI_ResBuffAddr == 0) ):
		clb._log.error("CLI buffers are not valid or not supported, Aborting due to Error!")
		clb.CloseInterface()
		return 1

	# Clear CLI Command & Response buffer headers
	clb.ClearCliBuff(CLI_ReqBuffAddr, CLI_ResBuffAddr)
	for Count in range (0, 0x8):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + Count, 8, 0 )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_CMD_OFF, 8, operation )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_PARAMSZ_OFF, 4, 8 )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE, 4, IoPort )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + 4, 4, Size )
	if (operation == clb.IO_WRITE_OPCODE):
		clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE + 8, 4, IoValue )
	clb.memwrite( CLI_ReqBuffAddr + clb.CLI_REQ_RES_READY_SIG_OFF, 4, clb.CLI_REQ_READY_SIG )
	clb._log.info("CLI Mailbox programmed, now issuing S/W SMI to execute the given command..")
	clb.triggerSMI(0xF6)	# trigger S/W SMI for CLI
	if (clb.WaitForCliResponse(CLI_ResBuffAddr, 8) == 1):
		clb._log.error("CLI Response not ready, Aborting....")
		clb.CloseInterface()
		return 1

	ResStatus = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_STATUS_OFF, 4))
	ResFlags = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_READY_FLAGS_OFF, 2))
	if( ((ResFlags & 0x3) != 0) or (ResStatus != 0) ):
		clb._log.error("CLI Command returned with some errors, Aborting!")
		clb.CloseInterface()
		return 1

	if (operation == clb.IO_READ_OPCODE):
		IoValue = int(clb.memread(CLI_ResBuffAddr + clb.CLI_REQ_RES_BUFF_HEADER_SIZE, 8))
	clb._log.result("IO Port 0x%X  Size = 0x%X  Value = 0x%X " %(IoPort, Size, IoValue))
	clb.CloseInterface()
	return 0

FIT_TBL_ENTRY_TYPE_0               = 0
UCODE_ENTRY_TYPE_1                 = 1
ACM_ENTRY_TYPE_2                   = 2
START_UP_BIOS_MODULE_TYPE_ENTRY_7  = 7
TPM_POLICY_TYPE_8                  = 8
BIOS_POLICY_TYPE_9                 = 9
TXT_POLICY_TYPE_A                  = 0xA
KEY_MANIFEST_TYPE_B                = 0xB
BOOT_POLICY_TYPE_C                 = 0xC
BIOS_DATA_AREA_TYPE_D              = 0xD
UNUSED_ENTRY_TYPE_7F               = 0x7F
FitDict     = {FIT_TBL_ENTRY_TYPE_0: 'FIT', UCODE_ENTRY_TYPE_1: 'Ucode', ACM_ENTRY_TYPE_2: 'ACM', START_UP_BIOS_MODULE_TYPE_ENTRY_7: 'Start Up Bios Module', TPM_POLICY_TYPE_8: 'TPM Policy', BIOS_POLICY_TYPE_9: 'Bios Policy', TXT_POLICY_TYPE_A: 'TXT Policy', KEY_MANIFEST_TYPE_B: 'Key Manifest', BOOT_POLICY_TYPE_C: 'Boot Policy', BIOS_DATA_AREA_TYPE_D: 'Bios Data', UNUSED_ENTRY_TYPE_7F: 'Unused'}
AcmTypeDict = {0x4: 'NPW', 0x8: 'Debug'}

def GetFitTableEntries(BiosBinListBuff):
	fwp.FwIngrediantDict['FIT']={}
	fwp.FwIngrediantDict['FitTablePtr'] = 0
	FitTableDict = {}
	if (BiosBinListBuff != 0):
		BinSize = len(BiosBinListBuff)
	else:
		BinSize = 0
	FitSig = 0
	FitTablePtr = int(clb.ReadBios(BiosBinListBuff, BinSize, 0xFFFFFFC0, 4))
	if (FitTablePtr >= 0xFF000000):
		FitSig = clb.ReadBios(BiosBinListBuff, BinSize, FitTablePtr, 8)
	if (FitSig == 0x2020205F5449465F): # "_FIT_   "
		fwp.FwIngrediantDict['FitTablePtr'] = FitTablePtr & 0xFFFFFFF0  # Has to be 16 Byte aligned address
		Entries = clb.ReadBios(BiosBinListBuff, BinSize, FitTablePtr+8, 4) & 0xFFFFFF
		if(clb.ReadBios(BiosBinListBuff, BinSize, FitTablePtr+0x0E, 1) & 0x80):		# FIT Table Checksum bit Valid?
			CheckSum = clb.ReadBios(BiosBinListBuff, BinSize, (FitTablePtr+0x0F), 1)
			CurChkSum = 0
			for bytecount in range (0, ((Entries*16))):
				CurChkSum = (CurChkSum + clb.ReadBios(BiosBinListBuff, BinSize, (FitTablePtr+bytecount), 1)) & 0xFF 
			if(CurChkSum != 0):
				clb._log.error("FIT Table checksum (0x%X) is not valid, Table seems to be corrupted, Aborting!" %(CheckSum))
				return FitTableDict
		for  Count in range (0, Entries):
			EntryAddr = clb.ReadBios(BiosBinListBuff, BinSize, (FitTablePtr+(Count*0x10)), 8)
			Size = (clb.ReadBios(BiosBinListBuff, BinSize, FitTablePtr+(Count*0x10)+0x8, 4) & 0xFFFFFF)
			EntryType = (clb.ReadBios(BiosBinListBuff, BinSize, FitTablePtr+(Count*0x10)+0x0E, 1) & 0x7F)
			FitTableDict[Count] = {'Type': EntryType, 'Name': FitDict.get(EntryType, 'reserved') , 'Address': EntryAddr, 'Size': Size}
	fwp.FwIngrediantDict['FIT'] = FitTableDict
	return FitTableDict

def FlashAcmInfo(UefiFwBinListBuff, PrintEn=True):
	fwp.FwIngrediantDict['ACM']={}
	BinSize = len(UefiFwBinListBuff)
	FitTableEntries = GetFitTableEntries(UefiFwBinListBuff)
	AcmBase = 0
	for count in FitTableEntries:
		if(FitTableEntries[count]['Type'] == ACM_ENTRY_TYPE_2):
			AcmBase = FitTableEntries[count].get('Address', 0)
			break
	if(AcmBase == 0):
		clb._log.result("ACM Entry not Found in FIT Table!")
		return
	ACM_ModuleId = clb.ReadBios(UefiFwBinListBuff, BinSize, AcmBase+0x0C, 4)
	ACM_VendorId = clb.ReadBios(UefiFwBinListBuff, BinSize, AcmBase+0x10, 4)
	ACM_Bld_Date = clb.ReadBios(UefiFwBinListBuff, BinSize, AcmBase+0x14, 4)
	ACM_Bld_Date_Str = "%02X.%02X.%04X" %(((ACM_Bld_Date >> 8) & 0xFF), (ACM_Bld_Date & 0xFF), ((ACM_Bld_Date >> 16) & 0xFFFF))
	fwp.FwIngrediantDict['ACM'] = {'VendorId': ACM_VendorId, 'ModuleId': ACM_ModuleId, 'Version': "??.??.??", 'Date': ACM_Bld_Date_Str, 'Type': AcmTypeDict.get(((ACM_ModuleId >> 28) & 0xFF), "???")}
	if PrintEn:
		clb._log.info("ACM Module Id = 0x%X  ACM Vendor Id = 0x%X  ACM Build Date = %s" %(ACM_ModuleId, ACM_VendorId, ACM_Bld_Date_Str))

def ProcessUcode(Operation="READ", BiosBinaryFile=0, UcodeFile=0, ReqCpuId=0, outPath='', BiosBinListBuff=0, PrintEn=True):
	fwp.FwIngrediantDict['Ucode']={}
	Entry = 0
	Operation = Operation.upper()
	FixFitTable = 0
	BiosFileExt = ".bin"
	basepath = os.path.dirname(clb.PlatformConfigXml)
	OrgUcodeFvFile   = os.sep.join([basepath, "OrgUcodeFv.bin"])
	ChgUcodeFvFile   = os.sep.join([basepath, "ChgUcodeFv.bin"])
	ChgFitSecFile    = os.sep.join([basepath, "ChgFitSec.bin"])
	ChgSecFitPtrFile = os.sep.join([basepath, "ChgSecFitPtr.bin"])
	clb.OutBinFile = ""

	if(Operation == "FIXFIT"):
		Opeartion = "READ"
		FixFitTable = 1
	ErrorFlag = 0
	ReqPatchInfo = ""
	if (BiosBinaryFile == 0):
		clb.InitInterface()
		BiosBinListBuff = 0
		BinSize = 0x1000000		# default Bios Flash size as 16 MB
		if (clb.memread((0x100000000 - BinSize), 8) == 0):	# found Zero vector
			if (clb.memread((0x100000000 - BinSize + 8), 8) == 0):	# found Zero vector, FV & BIOS region starts from here
				BinSize = BinSize*2		# Size is 32 MB
	else:
		if(BiosBinListBuff == 0):
			BiosFileExt = os.path.splitext(BiosBinaryFile)[1]
			BiosBinFile = open(BiosBinaryFile, "rb")
			BiosBinListBuff = list(BiosBinFile.read())
			BiosBinFile.close()
			UpdateOnlyBiosRegion = False
			BiosRegionBase = 0
			BiosEnd = len(BiosBinListBuff)
			IfwiBinListBuff = []
			Status = fwp.FlashRegionInfo(BiosBinListBuff, False)
			if(Status == 0):
				if (fwp.FwIngrediantDict['FlashDescpValid'] != 0):
					BiosEnd = fwp.FwIngrediantDict['FlashRegions'][fwp.BIOS_Region]['EndAddr'] + 1
					if(BiosEnd != len(BiosBinListBuff)):
						BiosRegionBase = fwp.FwIngrediantDict['FlashRegions'][fwp.BIOS_Region]['BaseAddr']
						clb._log.result("Bios Region Range 0x%X - 0x%X" %(BiosRegionBase, BiosEnd))
						IfwiBinListBuff = BiosBinListBuff
						BiosBinListBuff = BiosBinListBuff[BiosRegionBase:BiosEnd]
						UpdateOnlyBiosRegion = True
		BinSize = len(BiosBinListBuff)
		SaveNewBiosBinFile = 0

	if PrintEn:
		clb._log.info("Fetch MicroCode Firmware Volume Base Address from FIT table")
	UcodeFVbase = 0
	FitTableEntries = GetFitTableEntries(BiosBinListBuff)
	for count in FitTableEntries:
		if(FitTableEntries[count]['Type'] == UCODE_ENTRY_TYPE_1):
			UcodeFVbase = (FitTableEntries[count].get('Address', 0) & 0xFFFF0000)
			break
	if (UcodeFVbase != 0):
		FvGuid_low  = clb.ReadBios(BiosBinListBuff, BinSize, UcodeFVbase+0x10, 8)
		FvGuid_high = clb.ReadBios(BiosBinListBuff, BinSize, UcodeFVbase+0x18, 8)
		UcodeFVsize = clb.ReadBios(BiosBinListBuff, BinSize, UcodeFVbase+0x20, 4)
		if ( ( FvGuid_low == 0x4F1C8A3D8C8CE578 ) and ( FvGuid_high == 0xD32DC38561893599 ) ):
			if (BiosBinaryFile == 0):
				UcodeFVlistbuff = list(clb.memBlock(int(UcodeFVbase),int(UcodeFVsize)))
			else:
				UcodeFVlistbuff = BiosBinListBuff[(BinSize-(0x100000000-UcodeFVbase)):((BinSize-(0x100000000-UcodeFVbase)) + UcodeFVsize )]
		else:
			clb._log.error("Microcode Firmware Volume not found, Aborting due to error!")
			ErrorFlag = 1
	else:
		ErrorFlag = 1
	if (ErrorFlag == 1):
		clb._log.error("UcodeFVbase = 0, Aborting due to error!")
		if (BiosBinaryFile == 0):
			clb.CloseInterface()
		return 1

	if (Operation == "UPDATE"):
		FileExt = os.path.splitext(UcodeFile)[1]
		if( (FileExt == ".pdb") or (FileExt == ".mcb") or (FileExt == ".PDB") or (FileExt == ".MCB") ):
			UcodePdbFile = UcodeFile
		elif( (FileExt == ".inc") or (FileExt == ".INC") ):
			UcodePdbFile = os.sep.join([clb.TempFolder, "TempPdbFile.pdb"])
			RetStatus = ConvertInc2pdb(UcodeFile, UcodePdbFile)
			if (RetStatus):
				clb._log.error("Error Converting inc to pdb format, aborting..")
				return RetStatus
		else:
			clb._log.error("Wrong Ucode Patch File, we expect the Ucode patch file in either .inc or .pdb or .mcb format ")
			return 1
		PdbFile = open(UcodePdbFile, "rb")
		PdbFileListBuff = list(PdbFile.read())
		PdbFile.close()
		ReqUcodeSize = clb.ReadList(PdbFileListBuff, 0x20, 4)
		Date = clb.ReadList(PdbFileListBuff, 0x8, 4)
		ReqCpuId = clb.ReadList(PdbFileListBuff, 0xC, 4)
		ReqPatchId = clb.ReadList(PdbFileListBuff, 0x4, 4)
		PatchStr = "%08X" %(ReqPatchId)
		ReqPatchInfo = "__newUc_" + hex(ReqCpuId)[2:] + "_" + PatchStr
		CheckSum=0
		for  Count in range (0, (ReqUcodeSize/4)):
			CheckSum = (CheckSum + clb.ReadList(PdbFileListBuff, (Count*4), 4)) & 0xFFFFFFFF
		if PrintEn:
			clb._log.info("Ucode Patch Update requested for following entry, PDB Ucode file checksum = 0x%X" %(CheckSum))
		if (CheckSum != 0):
			clb._log.error("Warning: Found invalid checksum for the given PDB file, aborting !")
			return 1
		if PrintEn:
			clb._log.result("|----------|------------|------------|---------|------------|")
			clb._log.result("|  CPUID   |  Patch ID  | mm.dd.yyyy |  Size   | Operation  |")
			clb._log.result("| 0x%06X | 0x%08X | %02X.%02X.%04X | 0x%04X | %-9s  |" %(ReqCpuId, ReqPatchId, (Date>>24), ((Date>>16)& 0xFF), (Date & 0xFFFF), ReqUcodeSize, Operation))
			clb._log.result("|----------|------------|------------|---------|------------|")

	CheckSum = 0
	for  Count in range (0, (0x48/2)):
		CheckSum = (CheckSum + clb.ReadList(UcodeFVlistbuff, (Count*2), 2)) & 0xFFFF
	if PrintEn:
		clb._log.info("UcodeFVbase = 0x%08X  UcodeFVsize  = 0x%08X  FVcheckSum = 0x%04X" %(UcodeFVbase, UcodeFVsize, CheckSum))
	if ( ( clb.ReadList(UcodeFVlistbuff, 0x48, 8) == 0x4924F856197DB236 ) and ( clb.ReadList(UcodeFVlistbuff, 0x50, 8) == 0xF375B82FF1CDF890 ) ):
		org_file = open(OrgUcodeFvFile, "wb") # opening for writing
		org_file.write(string.join(UcodeFVlistbuff, ''))
		org_file.close()
		UcodeUpdated = 0
		UcodeFFSsize = (clb.ReadList(UcodeFVlistbuff, 0x48+0x14, 4) & 0xFFFFFF)
		UcodeFFSbaseOff = 0x48
		UcodeDataOff = UcodeFFSbaseOff+0x18
		FFsHdrCheckSum = 0
		for  Count in range (0, 0x17):    # Ignore offset 0x11 & 0x18 of the FFS header to calculate FFS header Checksum.
			if (Count != 0x11):
				FFsHdrCheckSum = (FFsHdrCheckSum + int(clb.ReadList(UcodeFVlistbuff, UcodeFFSbaseOff+Count, 1))) & 0xFF
		if PrintEn:
			clb._log.result("Ucode Space Availaible = 0x%08X  FFSHdrcheckSum = 0x%04X" %((UcodeFVsize - 0x48 - UcodeFFSsize), FFsHdrCheckSum))
		FFsFileState = clb.ReadList(UcodeFVlistbuff, UcodeFFSbaseOff+0x17, 1)
		if ( (FFsHdrCheckSum == 0) and PrintEn ):
			clb._log.info("Current FFsHdrCheckSum is Valid!")
		CurrUcodeSize = clb.ReadList(UcodeFVlistbuff, UcodeFFSbaseOff+0x20, 4)
		DeletedSize = 0
		AddedSize = 0
		if PrintEn:
			clb._log.result("|----------|------------|------------|---------|-----------|")
			clb._log.result("|  CPUID   |  Patch ID  | mm.dd.yyyy |  Size   | Operation |")
			clb._log.result("|----------|------------|------------|---------|-----------|")
		UcodeFFSendOff = UcodeFFSbaseOff+UcodeFFSsize
		for count in range (0, 20):
			if( UcodeDataOff >= (UcodeFFSendOff) ):
				break
			CurrUcodeSize = clb.ReadList(UcodeFVlistbuff, UcodeDataOff+0x20, 4)
			Date = clb.ReadList(UcodeFVlistbuff, UcodeDataOff+0x8, 4)
			CpuId = clb.ReadList(UcodeFVlistbuff, UcodeDataOff+0xC, 4)
			PatchId = clb.ReadList(UcodeFVlistbuff, UcodeDataOff+0x4, 4)
			if( (CurrUcodeSize > UcodeFFSsize) or (Date == 0xFFFFFFFF) or (CpuId == 0xFFFFFFFF) or (PatchId == 0xFFFFFFFF) ):
				break
			CurrOp = "READ"
			if (Operation == "DELETE"):
				if (CpuId == ReqCpuId):
#					for index in range (UcodeDataOff, UcodeDataOff+CurrUcodeSize):
#						UcodeFVlistbuff.pop(UcodeDataOff)     # delete the old Ucode entries
					del UcodeFVlistbuff[UcodeDataOff:UcodeDataOff+CurrUcodeSize]
					DeletedSize = DeletedSize + CurrUcodeSize
					UcodeFFSendOff = UcodeFFSendOff - CurrUcodeSize
					UcodeUpdated = 1
					PatchStr = "%08X" %(PatchId)
					ReqPatchInfo = ReqPatchInfo + "__delUc_" + hex(CpuId)[2:] + "_" + PatchStr
					CurrOp = Operation

			if (Operation == "SAVE"):
				if (CpuId == ReqCpuId):
					ProcessorFlg = clb.ReadList(UcodeFVlistbuff, UcodeDataOff+0x18, 4)
					PatchStr = "%08X" %(PatchId)
					SaveUcodeFile=os.sep.join([basepath, "m"+hex(ProcessorFlg)[2:]+hex(CpuId)[2:]+"_"+PatchStr+".pdb"])
					patch_file = open(SaveUcodeFile, "wb") # opening for writing
					patch_file.write(string.join(UcodeFVlistbuff[UcodeDataOff : UcodeDataOff+CurrUcodeSize], ''))
					patch_file.close()
					clb._log.result("file saved as %s" %(SaveUcodeFile))
					CurrOp = Operation
					UcodeDataOff = UcodeDataOff + CurrUcodeSize

			if (Operation == "UPDATE"):
				if (CpuId == ReqCpuId):
					del UcodeFVlistbuff[UcodeDataOff:UcodeDataOff+CurrUcodeSize]
					DeletedSize = DeletedSize + CurrUcodeSize
					UcodeFFSendOff = UcodeFFSendOff - CurrUcodeSize
					if(UcodeUpdated == 0):
						UcodeFVlistbuff[UcodeDataOff:UcodeDataOff] = PdbFileListBuff
						UcodeDataOff = UcodeDataOff + ReqUcodeSize
						AddedSize = AddedSize + ReqUcodeSize
						UcodeFFSendOff = UcodeFFSendOff + ReqUcodeSize
						UcodeUpdated = 1
					CurrOp = Operation

			if (CurrOp == "READ"):
				UcodeDataOff = UcodeDataOff + CurrUcodeSize
			DateStr = "%02X.%02X.%04X" %((Date>>24), ((Date>>16)& 0xFF), (Date & 0xFFFF))
			fwp.FwIngrediantDict['Ucode'][Entry] = {'CpuId':CpuId , 'Version':PatchId , 'Date':DateStr, 'UcodeSize':CurrUcodeSize, 'Operation': CurrOp}
			Entry = Entry + 1
			if PrintEn:
				clb._log.result("| 0x%06X | 0x%08X | %s | 0x%04X | %-9s |" %(CpuId, PatchId, DateStr, CurrUcodeSize, CurrOp))
		if PrintEn:
			clb._log.result("|----------|------------|------------|---------|-----------|")
		NewUcodeFFSsize = UcodeFFSsize
		if ( (Operation == "UPDATE") or (Operation == "DELETE") ):
			if ( (Operation == "UPDATE") and (UcodeUpdated == 0) ):
				UcodeFVlistbuff[UcodeDataOff:UcodeDataOff] = PdbFileListBuff
				AddedSize = AddedSize + ReqUcodeSize
				UcodeUpdated = 1

			if ( (DeletedSize != 0) or (AddedSize != 0) ):
				UnusedFVspace = UcodeFVsize - (0x48 + UcodeFFSsize)
				InsertInUnusedFV = 0
				RemoveFromUnusedFV = 0
				if (DeletedSize != AddedSize):
					UcodeFFSsize = (clb.ReadList(UcodeFVlistbuff, 0x48+0x14, 4) & 0xFFFFFF)
					if (DeletedSize > AddedSize):
						InsertInUnusedFV = DeletedSize - AddedSize
						NewUcodeFFSsize = ( (UcodeFFSsize - InsertInUnusedFV) & 0xFFFFFF )
						for index in range (UcodeFFSbaseOff+NewUcodeFFSsize, UcodeFFSbaseOff+NewUcodeFFSsize+InsertInUnusedFV):
							UcodeFVlistbuff.insert(index, '\xFF')
					else:
						RemoveFromUnusedFV = AddedSize - DeletedSize
						NewUcodeFFSsize = ( (UcodeFFSsize + RemoveFromUnusedFV) & 0xFFFFFF )
						if (NewUcodeFFSsize > (UcodeFVsize - 0x48)):
							clb._log.error("not enough space in FV, please create some space by deleting some entries")
							clb._log.error("Aborting due to above error !")
							clb.RemoveFile(OrgUcodeFvFile)
							return 1
						del UcodeFVlistbuff[UcodeFFSbaseOff+NewUcodeFFSsize:UcodeFFSbaseOff+NewUcodeFFSsize+RemoveFromUnusedFV]
					UcodeFVlistbuff.pop(0x48+0x14)
					UcodeFVlistbuff.pop(0x48+0x14)
					UcodeFVlistbuff.pop(0x48+0x14)
					UcodeFVlistbuff.insert(0x48+0x14, str(chr(NewUcodeFFSsize & 0xFF)))
					UcodeFVlistbuff.insert(0x48+0x15, str(chr((NewUcodeFFSsize >> 8) & 0xFF)))
					UcodeFVlistbuff.insert(0x48+0x16, str(chr((NewUcodeFFSsize >> 16) & 0xFF)))
					FFsHdrCheckSum = 0
					for  Count in range (0, 0x17):
						if (Count != 0x11) and (Count != 0x10):
							FFsHdrCheckSum = (FFsHdrCheckSum + int(clb.ReadList(UcodeFVlistbuff, UcodeFFSbaseOff+Count, 1))) & 0xFF
					FFsHdrCheckSum = (0x100 - FFsHdrCheckSum) & 0xFF
					UcodeFVlistbuff.pop(0x48+0x10)
					UcodeFVlistbuff.insert(0x48+0x10, str(chr(FFsHdrCheckSum)))
				if PrintEn:
					clb._log.info(" New UnusedFVspace = 0x%X  InsertInUnusedFV = 0x%X  RemoveFromUnusedFV = 0x%X  NewFFsHdrCkSum[0x10] = 0x%04X" %((UcodeFVsize - 0x48 - NewUcodeFFSsize), InsertInUnusedFV, RemoveFromUnusedFV, FFsHdrCheckSum))

			chg_file = open(ChgUcodeFvFile, "wb") # opening for writing
			chg_file.write(string.join(UcodeFVlistbuff, ''))
			chg_file.close()
			if (filecmp.cmp(ChgUcodeFvFile, OrgUcodeFvFile)):
				clb._log.result("No changes detected, Skip writing back to the BIOS!")
				UcodeUpdated = 0
			else:
				UcodeUpdated = 1
				clb._log.result("Changes Detected!, Writing back the Updated FV to BIOS")

			if ( (len(UcodeFVlistbuff) == UcodeFVsize) and (UcodeUpdated == 1) ):
				FixFitTable = 1
				if (BiosBinaryFile == 0):
					SpiFlash('write', ChgUcodeFvFile, RegionOffset=(UcodeFVbase & (BinSize-1)))
				else:
					BiosBinListBuff[(BinSize-(0x100000000-UcodeFVbase)):((BinSize-(0x100000000-UcodeFVbase)) + UcodeFVsize )] = UcodeFVlistbuff[0:UcodeFVsize]
					SaveNewBiosBinFile = 1

		NewUcodeDict = {}
		NewAddr = 0x60
		UcodeSize = 0
		Entry = 0
		UcodeFFSsize = (clb.ReadList(UcodeFVlistbuff, 0x48+0x14, 4) & 0xFFFFFF)
		while(NewAddr < UcodeFFSsize):
			NewPatchId = clb.ReadList(UcodeFVlistbuff, NewAddr+0x4, 4)
			NewCpuId = clb.ReadList(UcodeFVlistbuff, NewAddr+0xC, 4)
			UcodeSize = clb.ReadList(UcodeFVlistbuff, NewAddr+0x20, 4)
			if( (UcodeSize > UcodeFFSsize) or (NewCpuId == 0xFFFFFFFF) or (NewPatchId == 0xFFFFFFFF) ):
				break
			NewUcodeDict[Entry] = {'CpuId':NewCpuId, 'Version':NewPatchId, 'Address':(UcodeFVbase+NewAddr), 'UcodeSize': UcodeSize, 'FoundInFit': 0}
			Entry = Entry + 1
			NewAddr = NewAddr + UcodeSize
		for count in FitTableEntries:
			if(FitTableEntries[count]['Type'] == UCODE_ENTRY_TYPE_1):
				FitUcodeAddr = FitTableEntries[count].get('Address', 0)
				for Entry in NewUcodeDict:
					if ( (NewUcodeDict[Entry]['FoundInFit'] == 0) and (NewUcodeDict[Entry]['Address'] == FitUcodeAddr) ):
						NewUcodeDict[Entry]['FoundInFit'] = 1
		UpdateFitTable = 0
		for Entry in NewUcodeDict:
			if(NewUcodeDict[Entry]['FoundInFit'] == 0):
				clb._log.error("CpuId 0x%X entry, Addr = 0x%X was not found in FIT table, needs update" %(NewUcodeDict[Entry]['CpuId'], NewUcodeDict[Entry]['Address']))
				UpdateFitTable = 1
		if (UpdateFitTable == 0):
			if PrintEn:
				clb._log.result("FIT Table verification for Ucode entries was successfull")
		else:
			if(FixFitTable):
				clb._log.error("FIT Table verification failed, fixing it...")
			else:
				clb._log.error("FIT Table verification failed, use \"fixfit\" as first arg to ProcessUcode().")
		FitTablePtr = fwp.FwIngrediantDict['FitTablePtr']
		FitTableSector = (fwp.FwIngrediantDict['FitTablePtr'] - 0x400) & 0xFFFFF000
		FitSectorSize = 0x1000
		AndMask = 0xFFF
		NewFitTblBase = 0
		if ( (UpdateFitTable == 1) and (FixFitTable == 1) ):
			if ( (FitTablePtr+0x200) >= (FitTableSector+FitSectorSize) ):
				FitSectorSize = 0x2000
			if (BiosBinaryFile == 0):
				FitListbuff = list(clb.memBlock(FitTableSector, FitSectorSize))
			else:
				FitListbuff = BiosBinListBuff[(BinSize-(0x100000000-FitTableSector)):((BinSize-(0x100000000-FitTableSector)) + FitSectorSize)]
			NewFitTblList = FitListbuff[(FitTablePtr & AndMask) : ((FitTablePtr & AndMask) + (len(FitTableEntries)*0x10))]
			FitSigOrg = clb.ReadList(FitListbuff, ((FitTablePtr & AndMask) + 0x200), 8)
			if (FitSigOrg == 0x47524F5F5449465F): # "_FIT_ORG"
				NewFitTblBase = FitTablePtr		# means the Orignal table already existed and current one is already an override, so init it to all F's and re-use it.
				for entrycnt in range (0, len(FitTableEntries)):
					FitListbuff[(NewFitTblBase & AndMask) + (entrycnt * 0x10) : ((NewFitTblBase & AndMask) + (entrycnt * 0x10) + 0x10)] = ['\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF']
			if(NewFitTblBase != FitTablePtr):
				if((FitTablePtr-0x240) > FitTableSector):
					NewFitTblBase = FitTablePtr - 0x200
				else:
					if((FitTablePtr-0x100) > FitTableSector):
						NewFitTblBase = FitTablePtr - 0x100
					else:
						UpdateFitTable = 0
		if ( (UpdateFitTable == 1) and (FixFitTable == 1) ):
			NewFitTblList.pop(0xF)
			NewFitTblList.insert(0xF, '\x00')	# clear the checksum to 0
			FitSig = clb.ReadList(NewFitTblList, 0, 8)
			if (FitSig == 0x2020205F5449465F): # "_FIT_   "
				Entries = clb.ReadList(NewFitTblList, 8, 4) & 0xFFFFFF
				for  Count in range (0, Entries):
					if ((Count*0x10) >= len(NewFitTblList)):
						break
					EntryType = (clb.ReadList(NewFitTblList, (Count*0x10)+0x0E, 1) & 0x7F)
					while (EntryType == UCODE_ENTRY_TYPE_1):
						for index in range (0, 0x10):
							NewFitTblList.pop((Count*0x10))
						Entries = Entries - 1
						if ((Count*0x10) >= len(NewFitTblList)):
							break
						EntryType = (clb.ReadList(NewFitTblList, (Count*0x10)+0x0E, 1) & 0x7F)

			NewEntries = len(NewFitTblList)/0x10
			for Entry in NewUcodeDict:
				NewEntries = NewEntries + 1
				TmpUcodeList = [(NewUcodeDict[Entry]['Address'] & 0xFF), ((NewUcodeDict[Entry]['Address'] >> 8) & 0xFF), ((NewUcodeDict[Entry]['Address'] >> 16) & 0xFF), 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00]
				for index in range (0, 0x10):
					NewFitTblList.insert(0x10+(0x10*Entry)+index, str(chr(TmpUcodeList[index])))
			NewFitTblList.pop(8)
			NewFitTblList.insert(8, str(chr(NewEntries & 0xFF)))	# update number of entries
			NewChksm = 0
			for Index in range (0, len(NewFitTblList)):
				NewChksm = (NewChksm + clb.ReadList(NewFitTblList, Index, 1)) & 0xFF
			NewChksm = 0x100 - NewChksm
			NewFitTblList.pop(0xF)
			NewFitTblList.insert(0xF, str(chr(NewChksm)))
			SkipOrgSig = False
			if (NewFitTblBase != FitTablePtr):
				# Verify if the new Fit Table Base is really free or if we are accidently overriding something else.
				for BuffCount in range (0, len(NewFitTblList)):
					if(FitListbuff[(NewFitTblBase & AndMask) + BuffCount] != '\xFF'):
						NewFitTblBase = FitTablePtr + (len(FitTableEntries)*0x10) - len(NewFitTblList) # init it apropriately.
						if(NewFitTblBase > FitTablePtr):
							Bytes2Clear = NewFitTblBase - FitTablePtr
							for  Count in range (0, Bytes2Clear):
								NewFitTblList.insert(len(NewFitTblList), '\xFF')
							NewFitTblBase = FitTablePtr
						SkipOrgSig = True
						break
			FitListbuff[(NewFitTblBase & AndMask) : ((NewFitTblBase & AndMask) + len(NewFitTblList))] = NewFitTblList[0 : len(NewFitTblList)]
			if ((NewFitTblBase != FitTablePtr) and (SkipOrgSig == False)):
				FitListbuff.pop((FitTablePtr & AndMask) + 5)
				FitListbuff.insert((FitTablePtr & AndMask) + 5, 'O')
				FitListbuff.pop((FitTablePtr & AndMask) + 6)
				FitListbuff.insert((FitTablePtr & AndMask) + 6, 'R')
				FitListbuff.pop((FitTablePtr & AndMask) + 7)
				FitListbuff.insert((FitTablePtr & AndMask) + 7, 'G')
			chgFit_file = open(ChgFitSecFile, "wb") # opening for writing
			chgFit_file.write(string.join(FitListbuff, ''))
			chgFit_file.close()
			FitPtrList = [str(chr((NewFitTblBase & 0xFF))), str(chr(((NewFitTblBase >> 8) & 0xFF))), str(chr(((NewFitTblBase >> 16) & 0xFF))), str(chr(((NewFitTblBase >> 24) & 0xFF)))]
			if (BiosBinaryFile == 0):
				SpiFlash('write', ChgFitSecFile, RegionOffset=(FitTableSector & (BinSize-1)))
				FvSecListbuff = list(clb.memBlock(0xFFFFF000, 0x1000))
				FvSecListbuff[0xFC0 : 0xFC4] = FitPtrList[0:4]
				chgFitPtr_file = open(ChgSecFitPtrFile, "wb") # opening for writing
				chgFitPtr_file.write(string.join(FvSecListbuff, ''))
				chgFitPtr_file.close()
				SpiFlash('write', ChgSecFitPtrFile, RegionOffset=((BinSize-1) & 0xFFFFF000))
			else:
				BiosBinListBuff[(BinSize-(0x100000000-FitTableSector)): ((BinSize-(0x100000000-FitTableSector))+FitSectorSize)] = FitListbuff[0:FitSectorSize]
				BiosBinListBuff[(0xFFFFFFC0 & (BinSize-1)): ((0xFFFFFFC0 & (BinSize-1))+4)] = FitPtrList[0:4]
				SaveNewBiosBinFile = 1
		if PrintEn:
			clb._log.info(" UcodeFVdataDeleted = 0x%X  UcodeFVdataAdded = 0x%X" %(DeletedSize, AddedSize))
		if (BiosBinaryFile == 0):
			clb.CloseInterface()
		else:
			if(SaveNewBiosBinFile):
				OrgBinFileName = os.path.splitext(os.path.basename(BiosBinaryFile))[0]
				if ( (UpdateFitTable == 1) and (FixFitTable == 1) ):
					ChgBinFileName = OrgBinFileName + ReqPatchInfo + "_NewFit" + BiosFileExt
				else:
					ChgBinFileName = OrgBinFileName + ReqPatchInfo + BiosFileExt
				ChgBiosBinFile=os.sep.join([basepath, ChgBinFileName])
				clb.OutBinFile = ChgBiosBinFile
				clb._log.result("Saving the output Binary file as %s" %(ChgBiosBinFile))
				chgBios_file = open(ChgBiosBinFile, "wb") # opening for writing
				if(UpdateOnlyBiosRegion):
					IfwiBinListBuff[BiosRegionBase:BiosEnd] = BiosBinListBuff
					chgBios_file.write(string.join(IfwiBinListBuff, ''))
				else:
					chgBios_file.write(string.join(BiosBinListBuff, ''))
				chgBios_file.close()
				import shutil
				if outPath != '':
					shutil.move(ChgBiosBinFile,outPath)
		clb.RemoveFile(OrgUcodeFvFile)
		clb.RemoveFile(ChgUcodeFvFile)
		clb.RemoveFile(ChgFitSecFile)
		clb.RemoveFile(ChgSecFitPtrFile)

		return 0
	if (BiosBinaryFile == 0):
		clb.CloseInterface()
	return 1

def patchUcOnAllBioses(inBiosFolder='',outBiosFolder='',patchFile=''):
	for dirpath, dirnames, filenames in os.walk(inBiosFolder):
		for file in filenames:
			if not file.endswith('bin'): 
				continue
			try:
				BiosBinFile = os.path.join(dirpath, file)
				ProcessUcode(Operation="update", BiosBinaryFile=BiosBinFile, UcodeFile=patchFile, ReqCpuId=0, outPath=outBiosFolder)
			except WindowsError:
				clb._log.error("There was an issue accessing the paths specified. %s"%curpath)

def AutomateProUcode(InBios, outBiosFolder, PdbFile, CpuId=0):
	import glob, shutil
	if(os.path.isdir(InBios)):
		InBiosBinFileList = glob.glob(InBios+os.sep+"*.bin")
		if(len(InBiosBinFileList) == 0):
			InBiosBinFileList = glob.glob(InBios+os.sep+"*.rom")
	elif(os.path.isfile(InBios)):
		InBiosBinFileList = [InBios]
	for BiosBinFile in InBiosBinFileList:
		print "Processing BIOS file = %s" %BiosBinFile
		if(CpuId):
			ProcessUcode("delete", BiosBinFile, ReqCpuId=CpuId)
			TmpOutFileToDelete = clb.OutBinFile
			ProcessUcode("update", clb.OutBinFile, PdbFile)
			clb.RemoveFile(TmpOutFileToDelete)
		else:
			ProcessUcode("update", BiosBinFile, PdbFile)
		TmpOutFileToDelete = clb.OutBinFile
		if (clb.OutBinFile != ""):
			print clb.OutBinFile
			shutil.move(clb.OutBinFile, outBiosFolder)
		clb.RemoveFile(TmpOutFileToDelete)

def AutomateProgKnobs(InBios, IniFolder, outBiosFolder, NewMajVer="", NewMinorVer=""):
	import glob, shutil
	BiosKnobsFileList = glob.glob(IniFolder+os.sep+"*.ini")
	OrgKnobsIniFile = clb.KnobsIniFile
	if(os.path.isdir(InBios)):
		InBiosBinFileList = glob.glob(InBios+os.sep+"*.bin")
	elif(os.path.isfile(InBios)):
		InBiosBinFileList = [InBios]
	for KnobsIni in BiosKnobsFileList :
		clb.KnobsIniFile = KnobsIni
		SuffixText = os.path.splitext(os.path.basename(KnobsIni))[0]
		for BiosBinFile in InBiosBinFileList:
			clb._log.info("Processing BIOS file = %s" %BiosBinFile)
			CvProgKnobs(0, BiosBinFile, SuffixText, True)
			TmpOutFileToDelete = clb.OutBinFile
			fwp.UpdateBiosId(clb.OutBinFile, NewMajVer, NewMinorVer)
			if (clb.OutBinFile != ""):
				shutil.move(clb.OutBinFile, outBiosFolder)
			clb.RemoveFile(TmpOutFileToDelete)
	clb.KnobsIniFile = OrgKnobsIniFile

def GenDnvCpuSvBios(InBiosFile,ExtraKnobs='',Tag='',OutputDir=''):
	BiosBinFile = open(InBiosFile, "rb")
	BiosBinListBuff = list(BiosBinFile.read())
	BiosBinFile.close()
	BasePtr = len(BiosBinListBuff) - 0x1000
	FoundSig = False
	for Retry in range (0, 10):
		for CurrPtr in range (0, 0xFF8):
			Signature = clb.ReadList(BiosBinListBuff, (BasePtr+CurrPtr), 8)
			# jmp NotWarmStart
			# dd  0E9B13E21h
			# dd  0DE9FE209h
			if(Signature == 0xDE9FE209E9B13E21):
				clb._log.info("Found the Signature in Early Code at Offset 0x%X, Now Patching the binary to enable Merlin Code!" %(BasePtr+CurrPtr))
				NopOpcode = str(chr(0x90))
				BiosBinListBuff[(BasePtr+CurrPtr-2): (BasePtr+CurrPtr+8)] = [NopOpcode, NopOpcode, NopOpcode, NopOpcode, NopOpcode, NopOpcode, NopOpcode, NopOpcode, NopOpcode, NopOpcode]
				FoundSig = True
				break
		BasePtr = BasePtr - 0x1000
	if(FoundSig):
		TmpBiosBin = os.sep.join([clb.XmlCliPath, "out", os.path.basename(InBiosFile)])
		TmpBiosBinFile = open(TmpBiosBin, "wb") # opening for writing
		TmpBiosBinFile.write(string.join(BiosBinListBuff, ''))
		TmpBiosBinFile.close()
		if (ExtraKnobs==''):
                        ExtraKnobs = "SvBootMode=0xDC"
                else:
                        ExtraKnobs += ",SvBootMode=0xDC"
                if (Tag==""):
                        Tag = "CpuSv"
                else:
                        Tag += "_CpuSv"
		CvProgKnobs(ExtraKnobs, TmpBiosBin, Tag, True, OutputDir)
		clb.RemoveFile(TmpBiosBin)
		return 0
	else:
		clb._log.error("Merlin Signature not found in Early Code, Are you sure its INT version of DNV BIOS??  Aborting!!!")
	return 1

def ConvertInc2pdb(IncFile, PdbFile):
	with open(IncFile, "r") as f:
		lines = f.readlines()
	with open(PdbFile, "wb") as f:
		clb._log.info("  ConvertInc2pdb: start writing to pdb file...")
		for line in lines:
			line = line.split(";")[0].strip()
			if line:
				if (line[:4] == "dd 0" and line[12:] == "h"):
					value = int(line[4:12], 16)
					f.write(struct.pack("<L", value))
				else:
					return 1 #AssertionError
	clb._log.info("  ConvertInc2pdb: done..")
	return 0

def GenCcgXmlCliBios(InBiosFile):
	BiosBinFile = open(InBiosFile, "rb")
	BiosBinListBuff = list(BiosBinFile.read())
	BiosBinFile.close()
	BasePtr = 0
	FoundSig = False
	for CurrPtr in range (0, len(BiosBinListBuff)):
		Signature = clb.ReadList(BiosBinListBuff, (BasePtr+CurrPtr), 4)
		# ITST
		if(Signature == 0x54535449):
			clb._log.info("Found the ITST Signature at Offset 0x%X, Now Patching the binary to enable Intel Test Menu!" %(BasePtr+CurrPtr))
			TestMenuEn = str(chr(0xFE))
			if(clb.ReadList(BiosBinListBuff, (BasePtr+CurrPtr+8), 1) == 0xFF):
				BiosBinListBuff[(BasePtr+CurrPtr+8): (BasePtr+CurrPtr+9)] = TestMenuEn
				clb._log.info("Test Menu Enabled")
			FoundSig = True
			break
	if(FoundSig):
		TmpBiosBin = os.sep.join([clb.XmlCliPath, "out", os.path.basename(InBiosFile)])
		TmpBiosBinFile = open(TmpBiosBin, "wb") # opening for writing
		TmpBiosBinFile.write(string.join(BiosBinListBuff, ''))
		TmpBiosBinFile.close()
		CvProgKnobs("FlexCon=1", TmpBiosBin, "XmlCli", True)
		clb.RemoveFile(TmpBiosBin)
		return 0
	else:
		clb._log.info("Test Menu Signature not found!")
	return 1

def GetPostCodeFromMailbox ():
	ShardeMBAddress = clb.GetDramMbAddr()
	if ShardeMBAddress == 0:
		print "Wrong ShardeMBAddress, returning\n"
		return 0
	DramSharedMBbuf = clb.memBlock(ShardeMBAddress,0x100)
	if len(DramSharedMBbuf) < 0x24:
		print "DramSharedMBbuf too short, returning\n"
		return 0
	LegacyMBAddress = clb.readLegMailboxAddrOffset(DramSharedMBbuf)
	if LegacyMBAddress == 0:
		print "LegacyMBAddress not found, returning\n"
		return 0
	PostCodeAddres = ShardeMBAddress + LegacyMBAddress + 0x12
	PostCodeValue = clb.memread(PostCodeAddres, 1)
	return PostCodeValue
