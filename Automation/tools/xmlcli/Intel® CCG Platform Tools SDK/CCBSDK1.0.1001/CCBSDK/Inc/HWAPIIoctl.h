/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
 * This header provides a commmonInterface across all OS for all IOCTL number
 * Copyright (c) 2016, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

 
************************* IPL ************************************************
 
 Copyright (c)  2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
Module Name:
	HWAPIIoctl.h
Abstract:
	This header provides a commmonInterface across all OS for all IOCTL number
 */

#ifndef _HWAPIIOCTL_H
#define _HWAPIIOCTL_H

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS
#include BASICTYPES



#if defined(APPLE) || defined(__APPLE__)
#include <sys/ioccom.h>
#define CTL_CODE(a, b, c, d) _IOWR(0xF2, (b), UInt64)
#endif

#ifdef __unix__
#define CTL_CODE(a, b, c, d) _IOWR(0xF2, (b), UInt64)
#endif

#ifdef _WIN32_
#include "Windows.h"
#endif

#if defined _WIN32_ || defined _WIN32_WINNT
#define HW_ABSTRACTION_TYPE 0x8024
#endif

#ifdef UEFI
#define CTL_CODE(a, b, c, d) (b)
#endif

#if (defined _WIN32_ || defined _WIN32_WINNT) || defined __unix__ || ( defined(MACUSEBSDSIDE) && (defined(APPLE) || defined(__APPLE__))) || defined UEFI

//This is used for Windows, Unix alone or Unix mode of Mac OS implementation and EFI 
//This is not used for Mac User/Client implementation of driver calls

#define kHWAPIGetCPUIDInfo			CTL_CODE( HW_ABSTRACTION_TYPE, 0x00, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIGetVersion			CTL_CODE( HW_ABSTRACTION_TYPE, 0x01, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIHaltCPU				CTL_CODE( HW_ABSTRACTION_TYPE, 0x02, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIInvalidateCaches		CTL_CODE( HW_ABSTRACTION_TYPE, 0x03, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIInvalidateTLB			CTL_CODE( HW_ABSTRACTION_TYPE, 0x04, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIInvalidateWriteBack	CTL_CODE( HW_ABSTRACTION_TYPE, 0x05, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIMicrocodeUpdate		CTL_CODE( HW_ABSTRACTION_TYPE, 0x06, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadAR				CTL_CODE( HW_ABSTRACTION_TYPE, 0x07, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadControlReg		CTL_CODE( HW_ABSTRACTION_TYPE, 0x08, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadDebugReg			CTL_CODE( HW_ABSTRACTION_TYPE, 0x09, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadEFlags			CTL_CODE( HW_ABSTRACTION_TYPE, 0x0A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadGDT				CTL_CODE( HW_ABSTRACTION_TYPE, 0x0B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadIDT				CTL_CODE( HW_ABSTRACTION_TYPE, 0x0C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadIOBus				CTL_CODE( HW_ABSTRACTION_TYPE, 0x0D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadLDT				CTL_CODE( HW_ABSTRACTION_TYPE, 0x0E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadMemory			CTL_CODE( HW_ABSTRACTION_TYPE, 0x0F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadMSRReg			CTL_CODE( HW_ABSTRACTION_TYPE, 0x10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadMSW				CTL_CODE( HW_ABSTRACTION_TYPE, 0x11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadPCIConfig			CTL_CODE( HW_ABSTRACTION_TYPE, 0x12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadPMC				CTL_CODE( HW_ABSTRACTION_TYPE, 0x13, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadSL				CTL_CODE( HW_ABSTRACTION_TYPE, 0x14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadTR				CTL_CODE( HW_ABSTRACTION_TYPE, 0x15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadTSC				CTL_CODE( HW_ABSTRACTION_TYPE, 0x16, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIUnbindThread			CTL_CODE( HW_ABSTRACTION_TYPE, 0x17, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteControlReg		CTL_CODE( HW_ABSTRACTION_TYPE, 0x18, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteDebugReg			CTL_CODE( HW_ABSTRACTION_TYPE, 0x19, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteEFlags			CTL_CODE( HW_ABSTRACTION_TYPE, 0x1A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteGDT				CTL_CODE( HW_ABSTRACTION_TYPE, 0x1B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteIDT				CTL_CODE( HW_ABSTRACTION_TYPE, 0x1C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteIOBus			CTL_CODE( HW_ABSTRACTION_TYPE, 0x1D, METHOD_BUFFERED, FILE_READ_ACCESS)
#define kHWAPIWriteLDT				CTL_CODE( HW_ABSTRACTION_TYPE, 0x1E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteMemory			CTL_CODE( HW_ABSTRACTION_TYPE, 0x1F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteMSRReg			CTL_CODE( HW_ABSTRACTION_TYPE, 0x20, METHOD_BUFFERED, FILE_READ_ACCESS)
#define kHWAPIWriteMSW				CTL_CODE( HW_ABSTRACTION_TYPE, 0x21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWritePCIConfig		CTL_CODE( HW_ABSTRACTION_TYPE, 0x22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteTR				CTL_CODE( HW_ABSTRACTION_TYPE, 0x23, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIGetCPULibVersion		CTL_CODE( HW_ABSTRACTION_TYPE, 0x24, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIGetIOLibVersion		CTL_CODE( HW_ABSTRACTION_TYPE, 0x25, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIGetPCILibVersion		CTL_CODE( HW_ABSTRACTION_TYPE, 0x26, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIGetNonTurboData		CTL_CODE( HW_ABSTRACTION_TYPE, 0x27, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIClose                 CTL_CODE( HW_ABSTRACTION_TYPE, 0x28, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIOpen                  CTL_CODE( HW_ABSTRACTION_TYPE, 0x29, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIGenerateSMI           CTL_CODE( HW_ABSTRACTION_TYPE, 0x2A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIFlashKSC				CTL_CODE( HW_ABSTRACTION_TYPE, 0x2B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIMBIRead				CTL_CODE( HW_ABSTRACTION_TYPE, 0x2C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIMBIWrite				CTL_CODE( HW_ABSTRACTION_TYPE, 0x2D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIReadDRAM				CTL_CODE( HW_ABSTRACTION_TYPE, 0x2E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define kHWAPIWriteDRAM				CTL_CODE( HW_ABSTRACTION_TYPE, 0x2F, METHOD_BUFFERED, FILE_ANY_ACCESS)


#endif

#if  (!(defined(MACUSEBSDSIDE)) && (defined(__APPLE__) || defined(APPLE) )) && !(defined _WIN32_ || defined _WIN32_WINNT || defined __unix__)


/*
 * Indices of functions exposed by HWAPI Driver & User Client
 * THESE MUST AGREE WITH THE DISPATCH TABLE or the user will be
 * invoking the wrong functions.
 */
//This is used for Unix alone or Unix mode of Mac OS implementation and for MAC OS USer/Client implementation of driver calls
enum eDriverFunctions
{
	kHWAPIBindThread,  //0
	kHWAPIClose,       //1
	kHWAPIGetCPUIDInfo, //2
	kHWAPIGetVersion,   //3
	kHWAPIHaltCPU,      //4
//	kHWAPIInitialize,
	kHWAPIInvalidateCaches, //5
	kHWAPIInvalidateTLB,
	kHWAPIInvalidateWriteBack,
	kHWAPIMicrocodeUpdate,
	kHWAPIOpen,             //9 
	kHWAPIReadAR,
	kHWAPIReadControlReg,
	kHWAPIReadDebugReg,    //12
	kHWAPIReadEFlags,
	kHWAPIReadGDT,
	kHWAPIReadIDT,         //15
	kHWAPIReadIOBus,
	kHWAPIReadLDT,
	kHWAPIReadMemory,       //18
	kHWAPIReadMSRReg,
	kHWAPIReadMSW,
	kHWAPIReadPCIConfig,     //21
	kHWAPIReadPMC,
	kHWAPIReadSL,
	kHWAPIReadTR,
	kHWAPIReadTSC,
	kHWAPIUnbindThread,
	kHWAPIWriteControlReg,
	kHWAPIWriteDebugReg,
	kHWAPIWriteEFlags,
	kHWAPIWriteGDT,
	kHWAPIWriteIDT,
	kHWAPIWriteIOBus,
	kHWAPIWriteLDT,
	kHWAPIWriteMemory,
	kHWAPIWriteMSRReg,
	kHWAPIWriteMSW,
	kHWAPIWritePCIConfig,
	kHWAPIWriteTR,
	kHWAPIReadDRAM,
	kHWAPIWriteDRAM,
	kHWAPINumDriverFunctions,
	kHWAPIGetPCILibVersion,
	kHWAPIGetCPULibVersion,
	kHWAPIGetIOLibVersion,
	kHWAPIGetNonTurboData,    /// Need to be added in driver code for MAC
    kHWAPIMBIRead,
    kHWAPIMBIWrite
};


#endif //#if  defined(__APPLE__) || defined(APPLE) 

#endif /* _HWAPIIOCTL_H */
