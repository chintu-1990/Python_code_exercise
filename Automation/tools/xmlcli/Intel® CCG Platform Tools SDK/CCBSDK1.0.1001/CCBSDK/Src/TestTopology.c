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

TestTopology.c

Abstract:
This file contains the entry point to TestTopology application.
It displays the logical core count, physical core count, physical processor count and per thread details (Proc, pack, Core, LP, APICID)
*/

#include PACKAGE_SPECS
#include <stdio.h>
#include BASICTYPES
#include BASICSTRUCTS
#include BASICENUMS
#include HWAPILIB
#include TOPOLOGYLIB
#include ERRORLIB
 
int main()
{	
	eUserErrorCode status = eNoError;

	PROC_DETAILS * procDetails = (PROC_DETAILS *)malloc(sizeof(PROC_DETAILS));
	if (NULL == procDetails) {
		return eFailure;
	}
	memset(procDetails, 0, sizeof(procDetails));

	//Initializize TopologyLib
	if (eNoError != (status = LibTopologyInit())) {
		printf("TopologyLib initialization failed\n");
		free(procDetails);
		return status;
	}

	//Get Proc details using TopologyLib
	if (eNoError != (status = LibTopologygetProcDetails(procDetails))) {
		printf("LibTopologygetProcDetails failed\n");
		free(procDetails);
		LibTopologyTerminate();
		return status;
	}

	//Display Proc details
	printf("\nLogical Core Count %d ", procDetails->theLogicalCoreCount);
	printf("\nPhysical Core Count %d ",procDetails->thePhysicalCoreCount);
	printf("\nPhysical Processor Count %d ", procDetails->thePhysicalProcessorCount);
	for(int i=0;i<procDetails->thePhysicalCoreCount;i++)
		printf("\nPhysical Thread List %d ", procDetails->thePhysicalThreadList[i]);
	for(int i=0;i<procDetails->thePhysicalCoreCount;i++)
		printf("\nLogical Thread List %d ",procDetails->theLogicalThreadList[i]);
	
	printf("\n\nProc\tPack\tCore\tLP\tAPICID\n");	
	for(int i=0; i < procDetails->theLogicalCoreCount ; i ++)
	{
		printf("%d\t%d\t%d\t%d\t%d\t\n" , procDetails->myProcTopoInfo[i].OSCoreNumber , procDetails->myProcTopoInfo[i].Packageid , procDetails->myProcTopoInfo[i].CoreID
										, procDetails->myProcTopoInfo[i].SMTID,procDetails->myProcTopoInfo[i].APICID);
	}
	
	//Terminate TopologyLib
	free(procDetails);
	LibTopologyTerminate();
	
    return eNoError;
}
