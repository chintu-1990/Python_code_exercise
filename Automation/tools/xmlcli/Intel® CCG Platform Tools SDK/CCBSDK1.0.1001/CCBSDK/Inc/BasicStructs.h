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
	BasicStruct.h
Abstract:
	This header contains the structures which can be used across 
	all Packages and products

 */

#ifndef __BASICSTRUCT_H_
#define __BASICSTRUCT_H_
/*
 Structures to aid in the passing of registers in and out
 of the driver.
 */

 /*
 *  \file 	   BasicStructs.h
 *  \brief     basic C structs.
 *  \details   This provides different "C" structures can be used by libraries and Applications.
 *  \author    Intel Corporation.
 *  \version   1.0.0.1000
 *  \date      2011-2015
 *  \bug       TBD
 *  \copyright Copyright (c)  2012 Intel Corporation. All rights reserved
 *  \dontinclude BasicStructs.h
 */

 /** @defgroup BasicStructs structures
 *  structs's to be used by Libs and Apps.
 */
 
#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS
#include BASICTYPES
#include BASICENUMS
#include ENVIRONMENT


/*! \struct   KeyValue 
 *  \brief    Contains parsed xml details as per initialised library 
 */

typedef struct _KeyValue
{
    Int8 Key[64];
    UInt32 Value;
    struct _KeyValue * Next;
}KeyValue, *KeyValuePtr;


typedef union
{
	struct
	{
		UInt32 MajVer:16;
		UInt32 MinVer:16;
	}Ver;
	UInt32 u;
	
}LibVer, *LibVerPtr;

 /*! 
 *  \struct   IA32RegBlock,IA32RegBlockPtr
 *  \brief IA32 register block
 *  \ingroup BasicStructs
 */
typedef struct
{
	UInt32		Version;
	// Version 1.
	UInt32		eax;/**< eax */
	UInt32		ebx;/**< ebx */
	UInt32		ecx;/**< ecx */
	UInt32		edx;/**< edx */
	// Version 2.
} IA32RegBlock, *IA32RegBlockPtr;

 /*! 
 *  \struct   IA64RegBlock,IA64RegBlockPtr
 *  \brief IA64 register block
 *  \ingroup BasicStructs
 */
typedef struct
{
	UInt32		Version;
	// Vesion 1.
	UInt64		rax;/**< register rax */
	UInt64		rbx;/**< register rbx */
	UInt64		rcx;/**< register rcx */
	UInt64		rdx;/**< register rdx */
	// Version 2.
} IA64RegBlock, *IA64RegBlockPtr;

 /*! 
 *  \struct   RegBlock,RegBlockPtr
 *  \brief  register block  structure
 *  \ingroup BasicStructs
 */
 
typedef struct
{
	UInt32		Version;
	// Version 1.
	eCPUNumber	cpu;   /**< CPU/core number */
	eBlockType	kind;  /**< selection for below union \a Regs */
	union
	{
		IA32RegBlock	IA32Regs; /**< \a  IA32RegBlock*/
		IA64RegBlock	IA64Regs; /**< \a  IA64RegBlock*/
	} Regs;
	// Version 2.
} RegBlock, *RegBlockPtr;


/*!  \def REG_BLOCK_SIZE 
  Computes the size of RegBlock.
*/


#define	REG_BLOCK_SIZE	sizeof(RegBlock) /*!< Computes the size of \a RegBlock. */

/*
 Some handy Macros to operate on register blocks
 */
#define SET_IA32REG_BLOCK(block, a, b, c, d, e) \
	(block)->cpu = (a); \
	(block)->kind = e32Bit; \
	(block)->Regs.IA32Regs.eax = (b); \
	(block)->Regs.IA32Regs.ebx = (c); \
	(block)->Regs.IA32Regs.ecx = (d); \
	(block)->Regs.IA32Regs.edx = (e);

#define SET_IA64REG_BLOCK(block, a, b, c, d, e) \
	(block)->cpu = (a); \
	(block)->kind = e64Bit; \
	(block)->Regs.IA32Regs.rax = (b); \
	(block)->Regs.IA32Regs.rbx = (c); \
	(block)->Regs.IA32Regs.rcx = (d); \
	(block)->Regs.IA32Regs.rdx = (e);

#define COPY_REG_BLOCK(fromBlockPtr, toBlockPtr) \
	(toBlockPtr)->cpu = (fromBlockPtr)->cpu; \
	if (((fromBlockPtr)->kind == e32Bit) && ((toBlockPtr)->kind == e32Bit)) \
		(toBlockPtr)->Regs.IA32Regs = (fromBlockPtr)->Regs.IA32Regs; \
	else \
	{ \
		if (((fromBlockPtr)->kind == e64Bit) && ((toBlockPtr)->kind == e64Bit)) \
			(toBlockPtr)->Regs.IA64Regs = (fromBlockPtr)->Regs.IA64Regs; \
		else \
		{ \
			if (((fromBlockPtr)->kind == e64Bit) && ((toBlockPtr)->kind == e32Bit)) \
			{ \
				(toBlockPtr)->Regs.IA32Regs.eax = (UInt32)(fromBlockPtr)->Regs.IA64Regs.rax; \
				(toBlockPtr)->Regs.IA32Regs.ebx = (UInt32)(fromBlockPtr)->Regs.IA64Regs.rbx; \
				(toBlockPtr)->Regs.IA32Regs.ecx = (UInt32)(fromBlockPtr)->Regs.IA64Regs.rcx; \
				(toBlockPtr)->Regs.IA32Regs.edx = (UInt32)(fromBlockPtr)->Regs.IA64Regs.rdx; \
			} \
			else \
			{ \
				(toBlockPtr)->Regs.IA64Regs.rax = (UInt64)(fromBlockPtr)->Regs.IA32Regs.eax; \
				(toBlockPtr)->Regs.IA64Regs.rbx = (UInt64)(fromBlockPtr)->Regs.IA32Regs.ebx; \
				(toBlockPtr)->Regs.IA64Regs.rcx = (UInt64)(fromBlockPtr)->Regs.IA32Regs.ecx; \
				(toBlockPtr)->Regs.IA64Regs.rdx = (UInt64)(fromBlockPtr)->Regs.IA32Regs.edx; \
			} \
		} \
	}

/*
	Segment Selector Stuff
*/
typedef UInt16	SegmentSelector, *SegementSelectorPtr;
#define SEG_RPL(x)			((x) & 0x03)
#define SEG_TI(x)			((x) & 0x04)
#define SEG_LIMIT(x)		((x) & 0xFFF8)
#define GET_SEG_LIMIT(x)	(SEG_LIMIT((x)) >> 3)
#define SET_SEG_LIMIT(x, y)	(((x) << 3) | (y))

 /*! 
 *  \struct   SegmentDescriptor,SegmentDescriptorPtr
 *  \brief  SegmentDescriptor structure
 */
typedef struct
{
	char	desc[10]; /**<  10byte segment descriptor */
} SegmentDescriptor, *SegmentDescriptorPtr;

/*
	MSR related information
*/
 /*! 
 *  \struct   MSRInfoBlock,MSRInfoBlockPtr
 *  \brief  MSR related information
 *  \ingroup BasicStructs
 */
typedef struct
{
	UInt32		Version;
	// Version 1.
	eCPUNumber	cpu; /**<  [IN] CPU number */
	UInt32		msr; /**<  [IN] msr number */
	UInt32		hiContent;/**<  [IN -Write][OUT-Read] high word */
	UInt32		loContent;/**<  [IN -Write][OUT-Read] low word */
	// Version 2.
} MSRInfoBlock, *MSRInfoBlockPtr;

/*!
  *\def MSR_BLOCK_SIZE
  *\brief Computes size  of \a MSRInfoBlock
  *\ingroup BasicStructs
*/
#define	MSR_BLOCK_SIZE	sizeof(MSRInfoBlock)


/*!
  *\def SET_MSR_BLOCK(block, a, b, c, d)
  *\brief fills  \a MSRInfoBlock  with \a cpu \a msr, \a high word \a low word
  *\ingroup BasicStructs
*/
#define SET_MSR_BLOCK(block, a, b, c, d) \
	(block)->cpu = (a); \
	(block)->msr = (b); \
	(block)->hiContent = (c); \
	(block)->loContent = (d);


/*
 Structures for uCode Patching
 */
#define MAX_PATCH_BUFFER_SIZE	(4*4096)
#define PATCH_BLOCK_SIZE		4096
#define UCODE_HEADER_SIZE		48

 /*! 
 *  \struct   CPUPatchData,CPUPatchDataPtr
 *  \brief    structure to be passed for CPU micro code update
 *  \ingroup BasicStructs
 */
 
typedef struct
{
	UInt32	fHeader;			/* 00-03 */  /**<  header */
	UInt32	fPatchID;			/* 04-07 */  /**<  patchID */
	UInt32	fDate;				/* 08-0b */  /**<  patch created date */
	UInt32	fCPUID;				/* 0c-0f */  /**<  CPUID - which patch created for */
	UInt32	fCkSum;				/* 10-13 */  /**<   check sum */
	UInt32	fLoaderVersion;		/* 14-17 */  /**<   loader version */
	UInt32	fPlatformId;		/* 18-1b */  /**<   platform ID */
	UInt32	fDataSize;			/* 1c-1f */  /**<   patch data size */
	UInt32	fTotalSize;			/* 20-23 */
	UInt32	fReserved[3];		/* 24-2f */
	UInt8	fBuffer[MAX_PATCH_BUFFER_SIZE - UCODE_HEADER_SIZE];   /**<   micro code that to be written */
} CPUPatchData, *CPUPatchDataPtr;

/*! 
 *  \struct   InvalCacheStruct,pInvalCacheStruct
 *  \brief    structure to be passed to invalidate cache address block for CPU.
 *  \ingroup BasicStructs
 */
typedef struct {
	eCPUNumber theCPU; /**<   CPU number */
	void * address;    /**<   address */
}InvalCacheStruct , *pInvalCacheStruct;

/*! 
 *  \struct   DataElement,DataElementPtr
 *  \brief    structure to pass data or to retrieve data.
 *  \ingroup BasicStructs
 */

typedef struct
{
    UInt32  	Size; /**<   size of the data */
    UInt8	Data[DATA_ELEMENT_DATA_LENGTH];  /**<   data */
}DataElement , *DataElementPtr;


/*! 
 *  \struct   PCIConfAddr,PCIConfAddrPtr,PCIConfAddrHdl
 *  \brief    PCI Config address structure
 *  \ingroup BasicStructs
 */
typedef struct 
{
	UInt32		Bus		    :8; /**<   Bus number */
	UInt32		Device		:5; /**<   Device number */
	UInt32		Function	:3; /**<   Function number */
	UInt32		Reserved	:16;/**<   reserved for future use */
} PCIConfAddr, *PCIConfAddrPtr, **PCIConfAddrHdl;

/*! 
 *  \struct   NonTurboData,NonTurboDataPtr
 *  \brief    Non Turbo data requires for frequency calculation
 *  \ingroup BasicStructs
 */

typedef struct 
{
	eCPUNumber Core;
	UInt32 MsrAddress;
	UInt32 PMTimerAddress;
	UInt32 pcdiff;
	UInt32 ppcdiff;
}NonTurboProductData,*NonTurboProductDataPtr;

// PStateLib Structures
typedef struct _PROCESSOR_STATE
{
    UInt32           Frequency;
    UInt32           Flags;
    UInt32           PercentFrequency;
} PROCESSOR_STATE, *PPROCESSOR_STATE;


typedef struct acpi_table_header
{
    Int8                   Signature[4];          
    UInt32                 Length;                
    Int8                   Revision;              
    Int8                   Checksum;              
    Int8                   OemId[6];            
    Int8                   OemTableId[8]; 
    UInt32                 OemRevision;   
    Int8                   AslCompilerId[4];      
}AcpiTableHeader_t , * AcpiTableHeaderPtr_t ;

/*! 
 *  \struct   KscCompletePack,KscCompletePackPtr
 *  \brief    KSC and KSC Control files data to be flashed in elevated IRQL.
 *  \ingroup  BasicStructs
 */

typedef struct
{
	//KSC Control buffer size.
	UInt32 FcntrlSize;
	//KSC buffer size.
	UInt32 KscBinSize;
	//Base EC or Lid EC - In case of single EC machine value is always 0.
	//Incase of dual ECs (e.g., KSL SDS) For flashing Lid EC pass 0 and
	//for flashing Base EC pass 1.
	UInt8 bIsBaseEC;
	//Flag to enable/ disable logging in driver.
	UInt8 bEnableDriverLogging;
	//KSC Control buffer.
	UInt8 FcntrlFile[2048];
	//KSC buffer.
	UInt8 KscBinFile[1];
}KscCompletePack, *KscCompletePackPtr;

#endif // __BASICSTRUCT_H_

