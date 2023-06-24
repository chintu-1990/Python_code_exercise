/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
 * This header provides a commmon structures used as a Interface between User and Kernel Space
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
	HWAPIStructs.h
Abstract:
	This header provides a commmon structures used as a Interface between User and Kernel Space
 */



#ifndef _HWAPISTRUCTS_H
#define _HWAPISTRUCTS_H

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS
#include BASICSTRUCTS
/*! 
 *  \file 	   HWAPIStructs.h
 *  \brief     struct's for HWAPILib
 *  \author    Intel Corporation.
 *  \version   1.0.0.1000
 *  \date      2011-2015
 *  \copyright  Copyright (c)  2012 Intel Corporation. All rights reserved
 *  \bug       TBD
 *  \dontinclude HWAPIStruct.h
 */

#define MAX_SEG_COUNT               256
#define MCFG_HEADER_LEN			    44				// Number of bytes in the header (Totally 44 bytes). This is taken from the PCI Firmware spec

#pragma pack(1)


/*! \struct   PCIPack, PCIPackPtr
 *  \brief    structure to hold PCI data.
 *  \ingroup  PCIOps
 *  \see MSRDetails,PCIConfAddr
 */
typedef struct
{
	UInt32		Version;
	// Version 1.
	PCIConfAddr PciAddr; /**< PCI address */
	UInt32 	Offset;		 /**< offset from PCI Address */
	UInt32  Size;		/**< size of data */
	union{
		UInt8  value8;  /**< 1 byte Data */
		UInt16 value16; /**< 2 byte Data */
		UInt32 value32; /**< 4 byte Data */			
	}union_value;	
	// Version 2.
}PCIPack,*PCIPackPtr;

/*! \struct   MicroCodeUpdateStruct, pMicroCodeUpdateStruct
 *  \brief    structure to update microcode for a CPU.
 *  \ingroup  HWAPIGen
 *  \see MSRDetails,PCIConfAddr
 */
typedef struct {
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;
	UInt32        theMSRNumber;
	UInt32        theoffset;
	CPUPatchData  PatchData;
	// Version 2.
}MicroCodeUpdateStruct , *pMicroCodeUpdateStruct; 


/*! \struct   SegDescrStruct, pSegDescrStruct
 *  \brief    segment descriptor value for a CPU.
 *  \ingroup  HWAPIGen
 *  \see ContRegStruct,DebugRegStruct,IOStruct
 */
typedef struct {
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;           /**< CPU number */
	SegmentDescriptor SegDesData;/**< segment discritor */
	// Version 2
}SegDescrStruct , *pSegDescrStruct ; 


/*! \struct   ContRegStruct, pContRegStruct
 *  \brief    control register value for a CPU.
 *  \ingroup  HWAPIGen
 *  \see SegDescrStruct,DebugRegStruct,IOStruct
 */
typedef struct {
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;            /**< CPU number */
	eControlRegister theRegister; /**< control register structure */
	UInt64 theValue;	          /**< register Value */
	// Version 2.
}ContRegStruct , *pContRegStruct;


/*! \struct   DebugRegStruct, pDebugRegStruct
 *  \brief    control register value for a CPU.
 *  \ingroup  HWAPIGen
 *  \see ContRegStruct,SegDescrStruct,IOStruct
 */
typedef struct {
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;          /**< CPU number */
	eDebugRegister theRegister;/**< debug register structure */
	UInt64 theValue;	       /**< register Value */
	// Version 2
}DebugRegStruct , *pDebugRegStruct;


/*! \struct   IOStruct, pIOStruct
 *  \brief    struct for IO operations.
 *  \ingroup  HWAPIGen
 *  \see ContRegStruct,SegDescrStruct,DebugRegStruct
 */
typedef struct {
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;  /**< CPU number */
	UInt32 thePort;	    /**< IO port number */
	UInt32 theSize;     /**< size of data */
	union{
		UInt8  value8;  /**< 1 byte Data */
		UInt16 value16; /**< 2 byte Data */
		UInt32 value32;	/**< 4 byte Data */		
	}union_value;
	// Version 2.
}IOStruct, *pIOStruct;


/*! \struct   MSWStruct, pMSWStruct
 *  \brief    struct for IO operations.
 *  \ingroup  HWAPIGen
 *  \see ContRegStruct,SegDescrStruct,IOStruct
 */
typedef struct {
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;    /**< CPU number */
	UInt32     	theValue;  /**< Segment Limit value */
	// Version 2.
}MSWStruct, *pMSWStruct;

/*! \struct   SMIStruct_t, SMIStructPtr_t
 *  \brief    struct for Generating SMI.
 *  \ingroup  HWAPIGen
 *  \see RegBlock
 */
typedef struct {
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;  /**< CPU number */  
	UInt32 thePort;	    /**< IO port number */ 
	RegBlock myRegBlock; /**< Register block */                     
}SMIStruct_t, *SMIStructPtr_t;


/*! \struct   ReadWriteMemStruct, pReadWriteMemStruct
 *  \brief    struct for IO operations.
 *  \ingroup  HWAPIGen
 *  \see PCIPack,MSRDetails,IOStruct
 */
typedef struct
{
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;	     /**< CPU number */
	PtrType PhysicalAddress; /**< physical address  */	
	UInt32	size;            /**< size of data  */	
	UInt8 	Data[DATA_ELEMENT_DATA_LENGTH];         /**< data */	
	// Version 2.
}ReadWriteMemStruct, *pReadWriteMemStruct;

typedef struct 
{
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;
	UInt32     	theCounter;
	UInt64     	value;
	// Version 2.
}ReadPerfMonStruct, *ReadPerfMonStructPtr;

typedef struct 
{
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;
	UInt32     	theTaskReg;
	// Version 2.
}ReadTaskStruct, *ReadTaskStructPtr;

typedef struct 
{
	UInt32		Version;
	// Version 1.
	eCPUNumber 	theCPU;
	UInt64     	theTSC;
	// Version 2.
}ReadWriteTSCStruct,  *ReadWriteTSCStructPtr;

//
// All the 4 structures defined below are OS agnostic, is based of PCIE Firmware spec
// So Linux, MAC or any other OS need to get the pointer to the System Firnware table and 
// point it to the following structures and then rest fall into place. 

// Length specifies the entire length of the table. Initially read the length
// field allocate the full space. ConfigSpace will point to a series 
// of structure of the type MCFG_DESCRIPTOR. Each MCFG_DESCRIPTOR defines
// a PCIE segment. 36 bytes is the length of the structure + so many 12 
//

typedef struct _MCFG_TABLE {    // 44 bytes in length. Only one per system
	UInt32 Signature;        	// 4 bytes
	UInt32 Length;           // 4 bytes	        // = 44 + 16 + 16 ... goes on
	UInt8  Revison;          // 1 byte
	UInt8  Checksum;         // 1 byte
	UInt32 OEMID;		// 6 bytes
	UInt32 OEMTableID;   // 8 bytes
	UInt32 OEMRevision;      // 4 bytes
	UInt32 CreatorID;        // 4 bytes
	UInt32 CreatorRev;       // 4 bytes
	UInt32 Reserved;     // 8 bytes
} MCFGTable, *MCFGTablePtr;

typedef struct _MCFG_DESCRIPTOR {   // This is 16 bytes and is many as segments
	UInt32    LowBaseAdd;			// CfgBase Address can be 32/64 bit
	UInt32    HighBaseAdd;
	union {
		struct {
			UInt16 	SegGr;			// Segment number of this CfgSpace 
			UInt8   StartBus;
			UInt8   EndBus;
		} BusInfo;
		UInt32   FullBusInfo;
	}u;
	UInt32   RSVD;
} MCFGDescriptor, *MCFGDescriptorPtr;

//
// GetSystemFirmwareTable()in Windows returns a pointer to MCFG structure
//

typedef struct _MCFG {
	UInt32	 Header[MCFG_HEADER_LEN/4];				// This points to the MCFG_TABLE
	MCFGDescriptor McfgDescriptor;	// There can be as many as (Length - 44)/16 Segments or roots 
} MCFG, *MCFGPtr;

//
// The following struct and definition is for getting the Segment and Processor info.
// One structure per segment. SegCount will be same for all Segments. It is there to 
// facilitate easy access for the current structure. 
//

typedef struct _SEG_BUS_INFO {
	UInt32 Seg;                      // The current segment number
	UInt32 StartBus;                 // Start bus number need not be 0
	UInt32 EndBus;                   // End bus need not be 255
	UInt64 CfgBaseAdd;				 // Config base address of this bus in this segment
	UInt32 SegCount;                 // Total number of segment in this platform. 
} SegBusInfo, *SegBusInfoPtr;

#pragma pack()


#endif //_HWAPISTRUCTS_H
