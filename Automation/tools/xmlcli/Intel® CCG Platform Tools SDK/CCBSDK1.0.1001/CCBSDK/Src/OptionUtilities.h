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

OptionUtilities.h

Abstract:
This file contains function declarations pertaining to command line UI for CCB Test applications
*/

#ifndef TESTAPP_OPTIONUTILITIES_H
#define TESTAPP_OPTIONUTILITIES_H

#include PACKAGE_SPECS
#include BASICSTRUCTS
#include HWAPISTRUCTS

void getPCIParams(PCIConfAddr * pciConfAddr);
void getRegisterOffset(UInt32 *offset);
void getBarAddress(UInt64 *barAddress);
void getPortAddress(UInt16 *portAddress);
void getDataSize(UInt32 *size);
void getData(UInt8 *dataPtr, UInt32 size);
void getMSRInfo(MSRInfoBlock *msrInfoBlock);
void getMSRData(MSRInfoBlock *msrInfoBlock);
void getACPITableName(UInt8* name, Int32 size);
void getCPUIdRegs(IA32RegBlockPtr reg);
void getAddress(UInt64 *address);

void displayData(DataElementPtr dataElementPtr);

#endif