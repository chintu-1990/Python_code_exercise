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

OptionHandllers.c

Abstract:
This file contains functions pertaining to CCB HWAPI Test application.
These functions call into the HWAPI library interface to access the HAPI driver functionality.

*/

#include PACKAGE_SPECS
#include ENVIRONMENT
#include BASICSTRUCTS
#include HWAPISTRUCTS
#include OSINTERFACE
#include ERRORLIB
#include LIBDEBUG
#include "OptionUtilities.h"

#define MAX_BUFFER_SIZE 5000

eUserErrorCode pciCfgReadHandler() {

	eUserErrorCode status = eNoError; 

	PCIConfAddr pciConfAddr;
	UInt32 registerOffset = 0;
	UInt32 size = 0;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getPCIParams(&pciConfAddr);
	getRegisterOffset(&registerOffset);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {
		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}
	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}
	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;

	if (eNoError != (status = PCIRead(pciConfAddr, registerOffset, dataElementPtr))) {
		printf("\nError in calling PCIRead ret = %d \n", status);
	}
	else {
		displayData(dataElementPtr);
	}

	free(dataElementPtr); 	
	return status;
}

eUserErrorCode pciCfgWriteHandler() {

	eUserErrorCode status = eNoError;

	PCIConfAddr pciConfAddr;
	UInt32 registerOffset = 0;
	UInt32 size = 0;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getPCIParams(&pciConfAddr);
	getRegisterOffset(&registerOffset);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {

		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}

	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}

	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;

	getData(dataElementPtr->Data, size);
	
	if (eNoError != (status = PCIWrite(pciConfAddr, registerOffset, dataElementPtr))) {
		printf("\nError in calling PCIWrite ret = %d ", status);
	}
	else {
		printf("\nPCIWrite completed successfully\n");
	}

	free(dataElementPtr);
	return status;
}

eUserErrorCode mmioReadHandler() {

	eUserErrorCode status = eNoError;

	UInt64 barAddress = 0;
	UInt32 registerOffset = 0;
	UInt32 size = 0;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getBarAddress(&barAddress);
	getRegisterOffset(&registerOffset);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {
		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}

	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}

	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;

	if (eNoError != (status = PCIMMIORead(barAddress, registerOffset, dataElementPtr)))
	{
		printf("\nError in calling PCI MMIO read ret = %d\n", status);		
	}
	else {
		displayData(dataElementPtr);
	}

	free(dataElementPtr);
	return status;
}

eUserErrorCode mmioWriteHandler() {

	eUserErrorCode status = eNoError;

	UInt64 barAddress = 0;
	UInt32 registerOffset = 0;
	UInt32 size = 0;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getBarAddress(&barAddress);
	getRegisterOffset(&registerOffset);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {
		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}

	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}

	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;
	getData(dataElementPtr->Data, size);

	if (eNoError != (status = PCIMMIOWrite(barAddress, registerOffset, dataElementPtr)))
	{
		printf("\nError in calling PCI MMIO Write. ret = %d\n", status);
	}
	else {
		printf("\nPCIMMIOWrite completed Sucessfully\n");
	}

	free(dataElementPtr);
	return status;

}

eUserErrorCode portReadHandler() {

	eUserErrorCode status = eNoError;

	UInt16 portAddress = 0;
	UInt32 size = 0;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getPortAddress(&portAddress);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {
		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}

	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}

	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;

	if (eNoError != (status = ReadPort(portAddress, dataElementPtr))) {
		printf("\nError in calling ReadPort ret = %d\n", status);
	}
	else {
		displayData(dataElementPtr);
	}

	free(dataElementPtr);
	return status;
}

eUserErrorCode portWriteHandler() {

	eUserErrorCode status = eNoError;

	UInt16 portAddress = 0;
	UInt32 size = 0;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getPortAddress(&portAddress);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {
		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}

	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}

	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;
	getData(dataElementPtr->Data, size);

	if (eNoError != (status = WritePort(portAddress, dataElementPtr))) {
		printf("\nError in calling WritePort ret = %d\n", status);
	}
	else {
		displayData(dataElementPtr);
	}

	free(dataElementPtr);
	return status;
}

eUserErrorCode msrReadHandler() {

	eUserErrorCode status = eNoError;
	MSRInfoBlock msrInfoBlock;

	getMSRInfo(&msrInfoBlock);
	
	if (eNoError != (status = ReadMSR(&msrInfoBlock))) {
		printf("\nError in calling ReadMSR ioctl\n");
	}
	else {
		printf("\nValue returned hi 0x%x lo 0x%x \n", msrInfoBlock.hiContent, msrInfoBlock.loContent);
	}
	return status;
}

eUserErrorCode msrWriteHandler() {

	eUserErrorCode status = eNoError;
	MSRInfoBlock msrInfoBlock;

	getMSRInfo(&msrInfoBlock);
	getMSRData(&msrInfoBlock);

	if (eNoError != (status = WriteMSR(&msrInfoBlock))) {
		printf("\nError in calling WriteMSR ioctl\n");
	}
	else {
		printf("\nMSR Written successfully \n");
	}

	return status;
}

eUserErrorCode cpuidHandler() {

	eUserErrorCode status = eNoError;
	IA32RegBlock regBlock;

	getCPUIdRegs(&regBlock);

	if (eNoError != (status = ReadCPUID(eCPU_Any, &regBlock)))
	{
		printf("\nError in calling ReadCPUID ioctl\n");
	}
	else {
		printf("\n eax= 0x%x ebx = 0x%x ecx = 0x%x edx = 0x%x\n"
			, regBlock.eax, regBlock.ebx, regBlock.ecx, regBlock.edx);
	}
	fflush(stdin);

	return status;
}


eUserErrorCode pciExReadHandler() {

	eUserErrorCode status = eNoError;

	PCIConfAddr pciConfAddr;
	UInt32 size = 0;
	UInt32 registerOffset = 0;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getPCIParams(&pciConfAddr);
	getRegisterOffset(&registerOffset);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {
		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}

	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}

	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;

	if (eNoError != (status = PCIExRead(pciConfAddr, registerOffset, dataElementPtr))) {
		printf("\nError in calling PCIRead ret = %d \n", status);
	}
	else {
		displayData(dataElementPtr);
	}

	free(dataElementPtr);
	return status;
}

eUserErrorCode memReadHandler() {
	eUserErrorCode status = eNoError;

	UInt32 size = 0;
	UInt64 barAddress = 0;
	eCPUNumber theCPU = eCPU_Any;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getBarAddress(&barAddress);
	getCPUNumber(&theCPU);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {
		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}

	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}

	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;

	if (eNoError != (status = PhysicalMemRead(theCPU, barAddress, dataElementPtr)))
	{
		printf("\nError in calling PhysicalReadMem.  ret = %d\n", status);
	}
	else {
		displayData(dataElementPtr);
	}

	free(dataElementPtr);
	return status;

}

eUserErrorCode memWriteHandler() {

	eUserErrorCode status = eNoError;

	UInt64 address = 0;
	UInt32 size = 0;
	eCPUNumber theCPU = eCPU_Any;
	Int32 bufferSize = 0;
	Int32 dataElementSize = sizeof(DataElement);

	getAddress(&address);
	getCPUNumber(&theCPU);
	getDataSize(&size);

	bufferSize = dataElementSize + size;

	if (0 >= bufferSize || 0 >= size || MAX_BUFFER_SIZE <= bufferSize) {
		printf("\nInvalid argument: size is 0\n");
		return eBadArgument;
	}

	DataElementPtr dataElementPtr = (DataElementPtr)malloc(bufferSize);
	if (NULL == dataElementPtr) {
		return eNoMemory;
	}

	memset(dataElementPtr, 0, sizeof(DataElement)+size);
	dataElementPtr->Size = size;
	getData(dataElementPtr->Data, size);

	if (eNoError != (status = PhysicalMemWrite(theCPU, address, dataElementPtr)))
	{
		printf("\nError in calling PhysicalMemWrite Write. ret = %d\n", status);
	}
	else {
		printf("\nPhysicalMemWrite completed Sucessfully\n");
	}

	free(dataElementPtr);
	return status;
}

