/*
	Copyright (c)  2016 Intel Corporation. All rights reserved
	This software and associated documentation (if any) is furnished
	under a license and may only be used or copied in accordance
	with the terms of the license. Except as permitted by such
	license, no part of this software or documentation may be
	reproduced, stored in a retrieval system, or transmitted in any
	form or by any means without the express written consent of
	Intel Corporation.

Module Name:

	HWAPIUserClient.h

Abstract:
	This header contains the function numbers for the User Client

*/

#ifndef _HWAPIMACUSERCLIENT_H
#define _HWAPIMACUSERCLIENT_H

/*
 * Indices of functions exposed by HWAPI Driver & User Client
 * THESE MUST AGREE WITH THE DISPATCH TABLE or the user will be
 * invoking the wrong functions.
 */

//This is a copy of the eDriverFunctions in HWAPIIoctl.h and is used only on the driver side of Mac Client/Server implementation of driver functions
//We need to get rid of these two versions of the enum for Mac. We need to include this file for Mac for both client (HWAPI driver) and user builds (HWAPIlib). 
enum eDriverFunctions
{
	kBindThread,
	kClose,
	kGetCPUIDInfo,
	kGetVersion,
	kHaltCPU,
	kInitialize,
	kInvalidateCaches,
	kInvalidateTLB,
	kInvalidateWriteBack,
	kMicrocodeUpdate,
	kOpen,
	kReadAR,
	kReadControlReg,
	kReadDebugReg,
	kReadEFlags,
	kReadGDT,
	kReadIDT,
	kReadIOBus,
	kReadLDT,
	kReadMemory,
	kReadMSRReg,
	kReadMSW,
	kReadPCIConfig,
	kReadPMC,
	kReadSL,
	kReadTR,
	kReadTSC,
	kUnbindThread,
	kWriteControlReg,
	kWriteDebugReg,
	kWriteEFlags,
	kWriteGDT,
	kWriteIDT,
	kWriteIOBus,
	kWriteLDT,
	kWriteMemory,
	kWriteMSRReg,
	kWriteMSW,
	kWritePCIConfig,
	kWriteTR,
	kReadDRAM,
	kWriteDRAM,
	kNumDriverFunctions
};

#endif /* _HWAPIUSERCLIENT_H */

