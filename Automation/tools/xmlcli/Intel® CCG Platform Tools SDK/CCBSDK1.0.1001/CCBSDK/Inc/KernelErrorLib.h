/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
 * This headers defines all  the errors propogated across CommonCodeBase in Kernel Mode
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
	KernelErrorLib.h
Abstract:
	This headers defines all  the errors propogated across CommonCodeBase in Kernel Mode
 */

#ifndef ERRORCODE_H
#define ERRORCODE_H

#ifdef APPLE
#include <IOKit/IOReturn.h>

typedef enum _t_KernelErrorCode
{
    eNoError        = kIOReturnSuccess,
    eBadArgument   = kIOReturnBadArgument,
    eNullPointer    = kIOReturnBadArgument,
	eNoMemory		= kIOReturnNoMemory,
	eNotSupported	= kIOReturnUnsupported,
    eInsaneList     = 0xC0000282
} eKernelErrorCode;
#endif /* OS == MacOS */

#ifdef _WIN32_WINNT
#include "Ntddk.h"

typedef enum _KernelErrorCode
{
	eNoError 		= STATUS_SUCCESS,
	eBadArgument 	= STATUS_INVALID_PARAMETER,
	eNullPointer  	= STATUS_NO_MORE_ENTRIES,
	eNoMemory	  	= STATUS_NO_MEMORY,
	eNotSupported 	= STATUS_NOT_SUPPORTED,
	eInsaneList		= STATUS_RANGE_LIST_CONFLICT
} eKernelErrorCode;

#endif /* OS == Windows */


#ifdef __unix__

typedef enum _KernelErrorCode
{
	eNoError 		=  0,
	eBadArgument 	= -EINVAL,
	eNullPointer  	= -EINVAL,
	eNoMemory	  	= -ENOMEM,
	eNotSupported 	= -ENODEV,
	eInsaneList		= -E2BIG

} eKernelErrorCode;
#endif
	


#endif /* ERRORCODE_H */
