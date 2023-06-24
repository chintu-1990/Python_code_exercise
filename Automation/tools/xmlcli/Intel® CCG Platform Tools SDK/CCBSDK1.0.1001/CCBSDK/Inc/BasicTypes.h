/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
	This header contains the structures which can be used across 
	all Packages and products
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
	BasicTypes.h
Abstract:
	This header contains the definitions of data types based
	on size and sign.
 */



 /*
 *  \file 	   BasicTypes.h
 *  \brief     basic C defines.
 *  \details   This provides different "C" structures can be used by libraries and Applications.
 *  \author    Intel Corporation.
 *  \version   1.0.0.1000
 *  \date      2011-2015
 *  \bug       TBD
 *  \copyright Copyright (c)  2012 Intel Corporation. All rights reserved
 *  \dontinclude BasicTypes.h
 */
 

#ifndef _BASICTYPES_H
#define _BASICTYPES_H

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS
#include ENVIRONMENT


#if defined(__APPLE__) || defined(APPLE)
/*
    The following is to prevent name space collisions and
    symbol redefinition on the MacOS.
*/
#ifdef __MACTYPES__
#error "You must include BasicTypes before MacTypes"
#endif /* __MACTYPES__ */
#ifdef __TYPES__
#error "You must include BasicTypes before types"
#endif /* __TYPES__ */
#include <libkern/OSTypes.h>
#ifndef _OS_OSTYPES_H
#error "OSTypes is not defining _OS_OSTYPES_H. Check on why."
#endif /* _OS_OSTYPES_H */
#endif /* APPLE */

#ifndef kNULL
#define kNULL	(void*)0
#endif

#ifndef true
#define true	1
#endif

#ifndef kTRUE
#define kTRUE	1
#endif

#ifndef false
#define false	0
#endif

#ifndef kFALSE
#define kFALSE	0
#endif

#ifndef _LINUX_STDDEF_H
#ifndef __cplusplus
#ifndef bool

typedef char	bool;
#endif
#endif
#endif

#ifdef ANDROID
#ifndef __cplusplus
#ifndef bool

typedef char	bool;

#endif
#endif
#endif

#define kCHAR_SIZE		sizeof(char)
#define kSHORT_SIZE		sizeof(short)
#define kINT_SIZE		sizeof(int)
#define kLONG_SIZE		sizeof(long)
#define kLONGLONG_SIZE	sizeof(long long)

/*
	A note on volatile types:

	There is a difference in a volatile pointer and a pointer
	to a volatile. The placement of the 'V' in the data type
	should differentiate the data from the pointer.
*/
#ifdef UEFI
typedef CHAR8 Int8;
#else	
typedef char				Int8;
#endif
typedef volatile Int8		VInt8;
typedef Int8*				Int8Ptr;
typedef VInt8*				VInt8Ptr;
typedef volatile Int8Ptr	Int8VPtr;

#ifdef UEFI
typedef UINT8				UInt8;
#else 
#ifndef _OS_OSTYPES_H
typedef unsigned char		UInt8;
#endif
#endif	

typedef volatile UInt8		VUInt8;
typedef UInt8*				UInt8Ptr;
typedef UInt8Ptr*			UInt8PtrPtr;
typedef VUInt8*				VUInt8Ptr;
typedef volatile UInt8Ptr	UInt8VPtr;

#ifdef UEFI
typedef INT16         		Int16;    
#else
typedef short				Int16;
#endif
typedef volatile Int16		VInt16;
typedef Int16*				Int16Ptr;
typedef VInt16*				VInt16Ptr;
typedef volatile Int16Ptr	Int16VPtr;

#ifdef UEFI 
typedef   UINT16            UInt16;
#else 
#ifndef _OS_OSTYPES_H
typedef unsigned short		UInt16;
#endif
#endif	

typedef volatile UInt16		VUInt16;
typedef UInt16*				UInt16Ptr;
typedef VUInt16*			VUInt16Ptr;
typedef volatile UInt16Ptr	UInt16VPtr;

#ifdef UEFI
typedef     INT32           Int32;
#else
typedef 	int				Int32;
#endif
typedef volatile Int32		VInt32;
typedef Int32*				Int32Ptr;
typedef VInt32*				VInt32Ptr;
typedef volatile Int32Ptr	Int32VPtr;

#ifdef UEFI 
typedef   UINT32            UInt32;
#else
#ifndef _OS_OSTYPES_H
#if(Is64Bit == 1)
typedef unsigned int       	UInt32;
#else
typedef unsigned long      	UInt32;
#endif
#endif
#endif

typedef volatile UInt32		VUInt32;
typedef UInt32*				UInt32Ptr;
typedef VUInt32*			VUInt32Ptr;
typedef volatile UInt32Ptr	UInt32VPtr;

#ifdef UEFI
typedef   INT64              Int64;
#else
typedef long long			Int64;
#endif

typedef volatile Int64		VInt64;
typedef Int64*				Int64Ptr;
typedef VInt64*				VInt64Ptr;
typedef volatile Int64Ptr	Int64VPtr;

#ifdef UEFI
typedef UINT64              UInt64;
#else 
#ifndef _OS_OSTYPES_H
typedef unsigned long long	UInt64;
#endif
#endif

typedef volatile UInt64		VUInt64;
typedef UInt64*				UInt64Ptr;
typedef VUInt64*			VUInt64Ptr;
typedef volatile UInt64Ptr	UInt64VPtr;

#ifdef UEFI
typedef UINTN               PtrType;
#else

#if(Is64Bit == 1)
typedef UInt64				PtrType;
#else
typedef UInt32				PtrType;
#endif
#endif


#define ADDRESS_DIFF(x, y)		((PtrType)((x)) - (PtrType)((y))
#define ADDRESS_OFFSET(x, y)	((PtrType)((x)) + (PtrType)((y))

#endif	/* _BASICTYPES_H */
