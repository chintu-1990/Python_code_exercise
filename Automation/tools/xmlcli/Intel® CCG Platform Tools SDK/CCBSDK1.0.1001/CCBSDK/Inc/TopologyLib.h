/*
    Copyright (c)  2012 Intel Corporation. All rights reserved
    This software and associated documentation (if any) is furnished
    under a license and may only be used or copied in accordance
    with the terms of the license. Except as permitted by such
    license, no part of this software or documentation may be
    reproduced, stored in a retrieval system, or transmitted in any
    form or by any means without the express written consent of
    Intel Corporation.

Module Name:

    TopologyLib.h

Abstract:

    Library Header file.
*/

#ifndef _TOPOLOGYLIB_H
#define _TOPOLOGYLIB_H

/*! 
 *  \file 	    TopologyLib.h
 *  \brief      Topology access API's
 *  \details    This provides Interface to access CPU Package,Core and Thread information.
 *  \author     Intel Corporation.
 *  \version    1.0.0.1000
 *  \date       2011-2015
 *  \copyright  Copyright (c)  2012 Intel Corporation. All rights reserved
 *  \bug        TBD
 */
#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif
#include PACKAGE_SPECS
#include BASICTYPES
#include ERRORLIB
#include BASICSTRUCTS
#include BASICENUMS
#include LIBDEBUG
#define MAX_SUPPORTED_THREADS eCPU_Any

/*! 
 *  \struct   PROC_DETAILS
 *  \brief    Structure to hold Information - Number of Logical cores, Number of Physical cores, Number of Physical Processors, Physical core mapping and Logical core mapping.
 */
 
 #ifdef __cplusplus
extern "C" {
#endif

#define MAX_GRP_COUNT				32		// Group, socket, NUMA node are mostly same and 32 should be fine for the life of this tool :-)
#define MAX_PROC_COUNT				1024	// Just a big number not to be crossed in the far future
#define MAX_PKG_COUNT				4		// Package = Socket. Currently maximum socket is 4

typedef struct _ProcTopoInfo{
	UInt32 OSCoreNumber;
	UInt32 Packageid;			// Physical Package containing the Core
	UInt32 CoreID;				// Core Containing the Logical Processor 
	UInt32 SMTID;
	UInt32 APICID;				// APIC ID is unique assigned by the BIOS and is stored is MSR 0x1A in every logical processor. This is hardware feature
	UInt32 LogProc;				// Logical Processor number within the group 
	UInt32 SysProcNum;			// Logical Processor number for the whole system, the highest this can be is = ActiveProcCount. This increases monotonically spawning sockets/groups
	UInt32 GroupNum;			// This is the group number under which the logical processor falls.
	UInt32 Stepping;			// Processor stepping. This is hardware feature
}ProcTopoInfo_t, * ProcTopoInfoPtr_t;

typedef struct _PROC_DETAILS {
    UInt16 theLogicalCoreCount;                             /**<  [OUT] Logical Core Count */
    UInt16 thePhysicalCoreCount;                            /**<  [OUT] Physical Core Count */
    UInt16 thePhysicalProcessorCount;                       /**<  [OUT] Physical Processor Count */
    UInt16 thePhysicalThreadList[MAX_SUPPORTED_THREADS];    /**<  [OUT] Physical Thread List */
    UInt16 theLogicalThreadList[MAX_SUPPORTED_THREADS];     /**<  [OUT] Logical Thread List */
	ProcTopoInfo_t myProcTopoInfo[MAX_SUPPORTED_THREADS];
	UInt32 SocketCount;						// Physical distinct Socket count in the system. 
	UInt32 MaxProcCount;					// Maximum possible number of processors per core. This is the max possible and will differ form Active count. 
	UInt32 ActiveProcCount;					// Active number of Logical processors per system including all sockets.
	UInt32 LogProcPerSocket[MAX_PKG_COUNT];	// Number of Logical Processors per socket. Can be asymmetrical 
	UInt32 LogProcPerCore;					// When HT enabled each processor can have multiple logical processors. Assumed 2 and constant for all the sockets
	UInt32 CorePerSocket[MAX_PKG_COUNT];	// Core count per Socket(NUMA Node). This is hardware feature.  Can be asymmetrical 
	UInt32 MaxGroupCount;					// This the Maximum Number of Logical Processor Groups possible on this system. This can change on reboot
	UInt32 ActiveGroupCount;				// This the actual Active Number of Logical Processor Groups right now on this system. This can change on reboot
	UInt32 NumaNodeCount;					// This is a group of cores and can cross Sockets. Normally  NumaNode count and Socket count are same. 
	UInt64 AffinityMask[MAX_GRP_COUNT];		// This is a Bitmap 64 bits wide. Each bit represents 1 logical processor. P0 to P63 Right to Left. This is per group
	UInt32 ProcPerGroup[MAX_GRP_COUNT];		// This the Number of Logical Processors per Group. This facilitates accommodating processor count > 64.
	UInt32 SockPerGroup[MAX_GRP_COUNT];		// This the Number of Sockets per Group. Single group can accommodate multiple sockets
	ProcTopoInfo_t ProcIDMap[MAX_PROC_COUNT];		// This maps the APIC ID, Processor Stepping to a logical processor
	UInt64Ptr PServerProcInfo;						/**<  [OUT] This points ta a Server structure which has the topology info*/
} PROC_DETAILS, *PROC_DETAILSPtr;


/*! \fn       eUserErrorCode	LibTopologyInit()
 *  \brief    To initialise the Topology Library.
 *  \return   eUserErrorCode - returns eNoError on success, eFailure - on failure
 *  \see	  LibTopologyTerminate()
 */
eUserErrorCode LibTopologyInit(void);


/*! \fn		  eUserErrorCde     LibTopologygetProcDetails (PROC_DETAILS* pPROC_DETAILS)
 *	\brief	  Returns system information in a structure.
 *	\param    [OUT]  \a PROC_DETAILS* - Info containing LogicalCoreCount, PhysicalCoreCount, PhysicalProcessorCount, PhysicalThreadList, LogicalThreadList.
 *	\return   eUserErrorCode - returns eNoError on success, eFailure - on failure
 */ 
eUserErrorCode LibTopologygetProcDetails (PROC_DETAILSPtr pPROC_DETAILS);


/*! \fn       eUserErrorCode	LibTopologyTerminate()
 *  \brief    To finalise the topology library and free memory.
 *  \return   eUserErrorCode - returns eNoError on success, eFailure - on failure
 *  \see      LibTopologyInit()
 */
eUserErrorCode LibTopologyTerminate(void);

#ifdef __cplusplus
}
#endif

#endif
