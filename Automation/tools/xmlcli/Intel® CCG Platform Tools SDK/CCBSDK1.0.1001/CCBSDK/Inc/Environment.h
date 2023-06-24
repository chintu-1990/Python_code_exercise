/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
 * This headers helps defining  the Environment we are working on
 * Copyright (c) 2013, Intel Corporation.
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
 
 Copyright (c)  2013 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
Module Name:
	Environment.h
Abstract:
	This headers helps defining  the Environment we are working on
 */
 
 
#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS
#include ENVIRONMENT_ASM

/*
 Test for ANSI C
 */
#ifndef UEFI
#ifndef _WIN32_WINNT
#ifndef __STDC__
#error	"Compiler not ANSI compliant"
#endif /* __STDC__ */
#endif /* _WIN32_WINNT */
#endif

#ifdef	_WIN32_WINNT // Check if on user mode code, this define works
#ifdef	_AMD64_ // Check if on user mode code, this define works. For both if not, need to use || to define multiple ones
#define	Is64Bit	1
#else
#define	Is64Bit	0
#endif // _AMD64_
#endif // _WIN32_WINNT 

#if	defined(APPLE) || defined(__APPLE__)
#define	Is64Bit	defined(__LP64__)
#endif // APPLE

#ifdef __linux__
#define Is64Bit (defined(__LP64__) && (__LP64__ == 1))
#endif // __linux__

#define Is64Bit1 hi ## Is64Bit


#ifdef _WIN32_WINNT
#define CALLING_CONVENTION	__cdecl
#endif // _WIN32_WINNT

#ifdef __unix__
#if (Is64Bit == 0)
#define CALLING_CONVENTION	__attribute__((regparm(0)))
#else
#define CALLING_CONVENTION
#endif
#endif //__unix__

#ifdef APPLE
#define CALLING_CONVENTION
#endif //APPLE

#if (Is64Bit == 1)
#define Int32_FORMAT        "%d"
#define UInt32_FORMAT       "%u"
#define UInt32x_FORMAT      "%x"
#define UInt64_FORMAT       "%llu"
#define UInt64x_FORMAT      "%llx"
#define Int64_FORMAT        "%ld"
#define UInt32_08X_FORMAT   "%08X"
#define UInt32_08x_FORMAT   "%08x"
#define UInt32_x_FORMAT     "0x%08x"
#define PhysAddr_FORMAT     "0x%016llx"
#define PhysLen_FORMAT      "%llu"
#define VirtAddr_FORMAT     "0x%016llx"
#define ByteCount_FORMAT    "%llu"
#else
#define Int32_FORMAT        "%ld"
#define UInt32_FORMAT       "%lu"
#define UInt32x_FORMAT      "%lx"
#define UInt64_FORMAT       "%llu"
#define UInt64x_FORMAT      "%llx"
#define Int64_FORMAT        "%lld"
#define UInt32_08X_FORMAT   "%08lX"
#define UInt32_08x_FORMAT   "%08lx"
#define UInt32_x_FORMAT     "0x%08lx"
#define PhysAddr_FORMAT     "0x%08lx"
#define PhysLen_FORMAT      UInt32_FORMAT
#define VirtAddr_FORMAT     "0x%08x"
#define ByteCount_FORMAT    UInt32_FORMAT
#endif // (Is64Bit == 1)
#define PhysAddr64_FORMAT   (const char*)"0x%016llx"



//#include BASICTYPES
typedef struct
{
//	UInt32			fVersion;
	unsigned int	fVersion;
	unsigned char	_CPU;
	unsigned char	_OS;
	// Version 1.0.0.0
} RuntimeEnvironment, *RuntimeEnvironmentPtr;

extern RuntimeEnvironmentPtr	gRuntimeEnvironment;
#ifdef UEFI
#define DATA_ELEMENT_DATA_LENGTH 1
#else
#define DATA_ELEMENT_DATA_LENGTH 0
#endif

#endif	/* _ENVIRONMENT_H */



