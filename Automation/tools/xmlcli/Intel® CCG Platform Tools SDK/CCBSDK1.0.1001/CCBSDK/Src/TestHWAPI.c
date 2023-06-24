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

TestHWAPI.c

Abstract:
This file contains the entry point to TestHWAPI application.
*/

#include <stdio.h>
#include <stdlib.h>
#include PACKAGE_SPECS
#include HWAPILIB
#include ERRORLIB
#include "OptionHandlers.h"

eUserErrorCode(*optionHandlers[13])() = {
	&pciCfgReadHandler,
	&pciCfgWriteHandler,
	&mmioReadHandler,
	&mmioWriteHandler,
	&portReadHandler,
	&portWriteHandler,
	&msrReadHandler,
	&msrWriteHandler,
	&cpuidHandler,
	&pciExReadHandler,
	&memReadHandler,
	&memWriteHandler
};


int main(int argc, char*  argv[]) {

  eUserErrorCode status = eNoError;
  
  printf("HWAPI Initialize\n");
  
  if (eNoError != (status = HWAPIInitialize())) {
	  printf("Error in HWAPI initialization");
	  return status;
  }

  status = runInteractive();

  printf("HWAPI Terminate\n");
  if (eNoError != (status = HWAPITerminate())) {
	  printf("Error in terminating HWAPI");
  }
  return status;
}


int runInteractive() {
	UInt32 userInput;
	while (1)
	{
		printf("\n Please provide input\n");
		printf("1 for PCI Config Space Read\n");
		printf("2 for PCI Config Space Write\n");
		printf("3 for MMIO Read\n");
		printf("4 for MMIO Write\n");
		printf("5 for Read Port\n");
		printf("6 for Write Port\n");
		printf("7 for Read MSR\n");
		printf("8 for Write MSR\n");
		printf("9 for Execute CPUID\n");
		printf("10 for PCIEx Config Space Read\n");
		printf("11 for Memory Read\n");
		printf("12 for Memory Write\n");
		printf("Any other number to exit\n");
		fflush(stdin);
		scanf("%u", &userInput);

		fflush(stdin);

		if (userInput > 0 && userInput < 14) {
			(*optionHandlers[userInput-1])();
		}
		else {
			return 0;
		}
	}
}



