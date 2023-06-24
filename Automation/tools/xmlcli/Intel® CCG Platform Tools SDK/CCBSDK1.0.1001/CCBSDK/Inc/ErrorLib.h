/*
	Copyright (c)  2008 Intel Corporation. All rights reserved
	This software and associated documentation (if any) is furnished
	under a license and may only be used or copied in accordance
	with the terms of the license. Except as permitted by such
	license, no part of this software or documentation may be
	reproduced, stored in a retrieval system, or transmitted in any
	form or by any means without the express written consent of
	Intel Corporation.

Module Name:

	ErrorLib.h

Abstract:
	This header contains the mappings of the error codes between
	individual OS'es and the Common Codebase

*/
#ifndef ERRORLIB_H
#define ERRORLIB_H

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS
#include BASICTYPES



/***  Defines to convert __LINE__ and __DATE__ to String  ***/


#define MAKESTRING(X)   #X
#define TOSTR(X)     MAKESTRING(X)

/***  End Defines to convert __LINE__ and __DATE__ to String  ***/


typedef UInt32 ccb_status;
ccb_status CCB_GetUnknownErrorStatus();
ccb_status CCB_GetSuccessStatus();
bool CCB_IsSuccessStatus(ccb_status Status);


#if defined(__APPLE__) || defined(APPLE)
#include <IOKit/IOReturn.h>

typedef enum _t_UserErrorCode  //Needs to be modified
{
	eNoError 		    = 0,
	eSuccess    	    = 0,
	eBadArgument        = 1,
	eNullPointer,
    eFailure,
	eNotSupported,
	eNoMemory,
	eAlreadyExsists,
	eDriverInitFailed,
	eBufferMissMatch,
	eAccessDenied,
	eInsufficientMemory,
	eOutOfRange,
	eIncorrectAlignment,
	eInvalidMACAddress,
	eRegionNotBlank

} eUserErrorCode;
#endif /* OS == MacOS */


#ifdef _WIN32_WINNT
#include "WinError.h"

typedef enum _UserErrorCode
{
	eNoError    	    = 1, //ERROR_SUCCESS,
    eBadArgument        = ERROR_BAD_ARGUMENTS,		
    eNoMemory			= ERROR_NOT_ENOUGH_MEMORY,
	eNullPointer,
    eFailure,
	eNotSupported,
	eAlreadyExsists     = ERROR_ALREADY_EXISTS,
	eDriverInitFailed,
	eBufferMissMatch,
	eAccessDenied,
	eInsufficientMemory,
	eOutOfRange,
	eIncorrectAlignment,
	eInvalidMACAddress,
	eRegionNotBlank
	
} eUserErrorCode;

#endif /* OS == Windows */

#ifdef UEFI
#include <Uefi/UefiBaseType.h>
typedef enum _UserErrorCode
{
	eNoError    	  = EFI_SUCCESS,
    eBadArgument ,		
    eNoMemory	,
	eNullPointer ,
    eFailure ,
	eNotSupported  ,
	eAlreadyExsists  ,
	eDriverInitFailed,
	eBufferMissMatch,
	eAccessDenied,
	eInsufficientMemory,
	eOutOfRange,
	eIncorrectAlignment,
	eInvalidMACAddress,
	eRegionNotBlank	
} eUserErrorCode;
#endif 

#ifdef __unix__
#include <errno.h>

typedef enum _UserErrorCode
{
	eNoError 		=  0,
	eBadArgument 	= -EINVAL,
	eNullPointer  	= -EINVAL,
	eNoMemory	  	= -ENOMEM,
	eNotSupported 	= -ENODEV,
	eInsaneList		= -E2BIG,  
	eAlreadyExsists = -EEXIST,
	eFailure,
	eDriverInitFailed,
	eBufferMissMatch,
	eAccessDenied,
	eInsufficientMemory,
	eOutOfRange,
	eIncorrectAlignment,
	eInvalidMACAddress,
	eRegionNotBlank

} eUserErrorCode;
#endif

#endif //  ERRORLIB_H	

