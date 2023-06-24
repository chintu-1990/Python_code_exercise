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
This file contains functions pertaining to command line UI for CCB Test applications
*/

#include PACKAGE_SPECS
#include ENVIRONMENT
#include BASICSTRUCTS
#include HWAPISTRUCTS
#include OSINTERFACE
#include ERRORLIB
#include LIBDEBUG

void getPCIParams(PCIConfAddr * pciConfAddr) {

	UInt32 bus = 0, device = 0, function = 0;
	printf("\nPlease provide bus number(DEC) : ");
	scanf("%u", &bus);
	pciConfAddr->Bus = bus;

	printf("\nPlease provide device number(DEC) : ");
	scanf("%u", &device);
	pciConfAddr->Device = device;

	printf("\nPlease provide function(DEC) : ");
	scanf("%u", &function);
	pciConfAddr->Function = function;

	fflush(stdin);
}

void getBarAddress(UInt64 *barAddress) {

	printf("\nPlease provide bar address (HEX): ");
	scanf("%llux", barAddress);
	fflush(stdin);
}

void getAddress(UInt64 *address) {

	printf("\nPlease provide DRAM address (HEX): ");
	scanf("%llux", address);
	fflush(stdin);
}

void getCPUNumber(eCPUNumber *theCPU) {

	UInt32 cpuNumber = 0; 
	printf("\nProvide cpu(DEC) : ");
	scanf("%u", &cpuNumber);
	*theCPU = (eCPUNumber) cpuNumber;
	fflush(stdin);
}

void getRegisterOffset(UInt32 *offset) {

	printf("\nPlease provide register offset(HEX) : ");
	scanf("%ux", offset);
	fflush(stdin);
}

void getPortAddress(UInt16 *portAddress) {

	printf("\nPlease provide port address(HEX) : ");
	scanf("%hux", portAddress);
	fflush(stdin);
}

void getDataSize(UInt32 *size) {

	printf("\nPlease provide the data length(DEC) : ");
	scanf("%u", size);
	fflush(stdin);
}

void getMSRInfo(MSRInfoBlock *msrInfoBlock) {

	UInt32 cpuNumber = 0;
	msrInfoBlock->cpu = msrInfoBlock->msr = 0;
	printf("\nmsr(HEX) : ");
	scanf("%ux", &(msrInfoBlock->msr));
	printf("\nProvide cpu(DEC) : ");
	scanf("%u", &cpuNumber);
	msrInfoBlock->cpu = cpuNumber;
	fflush(stdin);
}

void getMSRData(MSRInfoBlock *msrInfoBlock) {

	msrInfoBlock->hiContent = msrInfoBlock->loContent = 0;
	printf("\nProvide values hi and lo.");
	printf("\nhi(HEX):  ");
	scanf("%ux", &(msrInfoBlock->hiContent));
	printf("\nlo(HEX):  ");
	scanf("%ux", &(msrInfoBlock->loContent));
	fflush(stdin);
}

void getCPUIdRegs(IA32RegBlockPtr regBlock) {

	printf("\nProvide values for registers eax and ecx.");
	printf("\neax(DEC) : ");
	scanf("%ux", &(regBlock->eax));
	printf("\necx(DEC) : ");
	scanf("%ux", &(regBlock->ecx));
	fflush(stdin);
}

void getData(UInt8 *dataPtr, UInt32 size) {

	UInt8 * tempDataPtr = dataPtr;

	printf("\nPlease enter data (DEC) : ");
	switch (size) {
	case 1:
		scanf("%hhu", ((UInt8Ptr)dataPtr));
		break;
	case 2:
		scanf("%hu", ((UInt16Ptr)dataPtr));
		break;
	case 4:
		scanf("%u", ((UInt32Ptr)dataPtr));
		break;
	default:
		for (int i = 0; i < size; i++)
		{
			printf("\nPlease enter data for %d position (DEC)", i);
			scanf("%hhu", tempDataPtr++);
		}
	}
	fflush(stdin);
}

void displayData(DataElementPtr dataElementPtr) {

	UInt32 size = dataElementPtr->Size;

	if (size == 1)
		printf("\nValue returned 0x%x", *((UInt8Ptr)dataElementPtr->Data));
	else if (size == 2)
		printf("\nValue returned 0x%x", *((UInt16Ptr)dataElementPtr->Data));
	else if (size == 4)
		printf("\nValue returned 0x%x", *((UInt32Ptr)dataElementPtr->Data));
	else
	{
		printf("\nValue Returned  : ------------------------------------------- \n ");
		for (Int32 temp_size = 0; temp_size < size; temp_size++)
		{
			if (!(temp_size % 4)) printf("\nOffset 0x%x : ", temp_size);
			printf("\t 0x%x", *((UInt8Ptr)dataElementPtr->Data + temp_size));
		}
	}
	fflush(stdout);
}

