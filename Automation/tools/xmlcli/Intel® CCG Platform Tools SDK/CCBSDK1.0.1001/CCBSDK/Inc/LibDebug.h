/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
 * This header defines all declarations for debug Library
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
	LibDebug.h
Abstract:
	This header defines all declarations for debug Library
 */

/*! 
 *  \file        LibDebug.h
 *  \brief       API's for Debug Library functionalities.
 *  \details     This provides Interface to debug and log information.
 *  \author      Intel Corporation.
 *  \version     1.0.0.1000
 *  \date        2011-2015
 *  \bug         TBD
 *  \copyright   Copyright (c)  2012 Intel Corporation. All rights reserved
 *  \dontinclude LibDebug.h
 */

/*!\mainpage About LibDebug
 *
 * \section  Debug Library. This static library API's useful to debug and log information.
 *           
 *
 * \warning  This library intended to be used with in the Organization.
 *           Neither library nor documentation can be distributed outside the organization.
 */


/** @defgroup DebugLibops Debug Library General
 *  API's for Debug Library Capability.
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS

#if (defined APPLE && defined KERNEL) || defined __KERNEL__ || (defined _WIN32_WINNT && !defined _WIN32_)
#define KERNEL_MODE
/***************************************************************************************

*****************************  KERNEL MODE ***********************************************

****************************************************************************************/
// Specific Kernel Includes
#if defined _WIN32_WINNT && !defined _WIN32_

#include <ntddk.h>

#elif defined __KERNEL__ 

#include <linux/kernel.h>

#elif (defined APPLE && defined KERNEL)
#include <IOKit/IOLib.h>
#include <IOKit/IOReturn.h>

#endif

#include OSKERNELINCLUDES
#include KERNELERRORLIB


#else
/***************************************************************************************

*****************************  USER MODE ***********************************************

****************************************************************************************/
#include OSINTERFACE
#include ERRORLIB

#include <stdio.h>

#endif


/*******************************************************************************************
*********************** COMMON DECLARATIONS *************************************************
********************************************************************************************/

#include BASICTYPES



typedef enum
{
	eDEBUG_TYPE_EXIT        = 0x20,
	eDEBUG_TYPE_ERROR  		= 0x10,
	eDEBUG_TYPE_WARN		= 0x08,
	eDEBUG_TYPE_PARAM 		= 0x04,
	eDEBUG_TYPE_INFO 		= 0x02,
	eDEBUG_TYPE_ENTRY		= 0x01,
	eDEBUG_TYPE_ALL		    = 0x20 | 0x10 | 0x08 | 0x04 | 0x02 | 0x01
} eTraceType;

typedef enum
{
	eDEBUG_OUTPUT_NONE      = 0x0,
	eDEBUG_OUTPUT_CONSOLE 	= 0x01,
	eDEBUG_OUTPUT_FILE 		= 0x02,
	eDEBUG_OUTPUT_INTERFACE	= 0x04,
	eDEBUG_OUTPUT_ALL 		= 0x04 | 0x02 | 0x01
}eOutputType;

typedef enum
{
	eDEBUG_TRACE_FORMAT_NONE         = 0x0,
	eDEBUG_TRACE_FUNCTION			 = 0x01,
	eDEBUG_TRACE_LINE       		 = 0x02,
	eDEBUG_TRACE_TIMESTAMP 	 		 = 0x04,
	eDEBUG_TRACE_TRACE_DESCRIPTION 	 = 0x08,
	eDEBUG_TRACE_FORMAT_ALL			 = 0x01 | 0x02 | 0x04 |0x08
}eTraceFormatType;


typedef enum
{
	eDEBUG_FLAG_NONE                    = 0x0,
	eDEBUG_FLAG_SYNC                 	= 0x01,
	eDEBUG_FLAG_LOCKED              	= 0x02,
	eDEBUG_FLAG_THREAD              	= 0x04,
	eDEBUG_FLAG_LOCAL_GLOBAL_FILE 		= 0x08,
	eDEBUG_FLAG_ALL                     = 0x01 | 0x02 | 0x04 |0x08
}eDebugFlags;

typedef void (*fnPtrTypeConsole) ( char* ,... );
typedef void (*fnPtrTypeLog) ( char* , ... );
typedef void (*fnPtrTypeLogFile) (PtrType FileHandle ,  char* Buffer , ... );
typedef void (*fnPtrTypeUserFunction)( eTraceType  , char *  , UInt32 StringLength );

typedef void *  DbgHandle;

#ifdef __cplusplus
extern "C"
{
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbSetFilePtr(DbgHandle myDebugHdl , PtrType myFilePtr);
#else
eKernelErrorCode DbSetFilePtr(DbgHandle myDebugHdl , PtrType myFilePtr);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbSetGlobalFilePtr( PtrType myFilePtr);
#else
eKernelErrorCode DbSetGlobalFilePtr( PtrType myFilePtr);
#endif


#if (!defined KERNEL_MODE)
eUserErrorCode DbInit(Int8Ptr pModName, DbgHandle * pHdl);
#else
eKernelErrorCode DbInit(Int8Ptr pModName, DbgHandle * pHdl);
#endif 

#if (!defined KERNEL_MODE)
eUserErrorCode DbExit (DbgHandle  pHdl);
#else
eKernelErrorCode DbExit (DbgHandle  pHdl);
#endif 


#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableSetFlags( DbgHandle myDebugHdl ,eDebugFlags  myFlags, UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableSetFlags( DbgHandle myDebugHdl ,eDebugFlags  myFlags, UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableGlobalSyncFlag(  UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableGlobalSyncFlag( UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableTraceFormatType( DbgHandle myDebugHdl ,eTraceFormatType  myTraceFormatType, UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableTraceFormatType( DbgHandle myDebugHdl ,eTraceFormatType  myTraceFormatType, UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableTraceType( DbgHandle myDebugHdl ,eTraceType  myTraceType, UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableTraceType( DbgHandle myDebugHdl ,eTraceType  myTraceType, UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableOutputType( DbgHandle myDebugHdl ,eOutputType  myOutputType, UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableOutputType( DbgHandle myDebugHdl ,eOutputType  myOutputType, UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableGlobalTraceType( eTraceType  myTraceType, UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableGlobalTraceType(eTraceType  myTraceType, UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableGlobalOutputType( eOutputType  myTraceType, UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableGlobalOutputType(eOutputType  myTraceType, UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableGlobalTraceFormatType( eTraceFormatType  myTraceFormatType, UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableGlobalTraceFormatType(eTraceFormatType  myTraceFormatType, UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbEnableDisableGlobalSetFlags( eDebugFlags  myFlags, UInt8 on_off);
#else
eKernelErrorCode DbEnableDisableGlobalSetFlags(eDebugFlags  myFlags, UInt8 on_off);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode  DebugOutputLogs(DbgHandle myDebugHdl , const  char* myFile , const char * myFunction , UInt32 myLineNumber,  UInt32 myTracetype , const char * format , ...);
#else
eKernelErrorCode  DebugOutputLogs(DbgHandle myDebugHdl , const  char* myFile , const char * myFunction , UInt32 myLineNumber,  UInt32 myTracetype , const char * format , ...);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbSetUserDefinedFunction(DbgHandle myDebugHdl  , fnPtrTypeUserFunction);
#else
eKernelErrorCode DbSetUserDefinedFunction(DbgHandle myDebugHdl , fnPtrTypeUserFunction);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbSetUserFunctionTraceType(DbgHandle myDebugHdl  , UInt32 myTracetype);
#else
eKernelErrorCode DbSetUserFunctionTraceType(DbgHandle myDebugHdl  , UInt32 myTracetype);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode HexDump (DbgHandle  myDebugHdl,  UInt32 myTracetype , void * pData, UInt64 pSize,  const  char* myFile , const char * myFunction , UInt32 myLineNumber);
#else
eKernelErrorCode HexDump (DbgHandle  myDebugHdl,   UInt32 myTracetype , void * pData, UInt64 pSize ,const  char* myFile , const char * myFunction , UInt32 myLineNumber);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbSetConsoleFunction (DbgHandle  myDebugHdl, fnPtrTypeConsole myConoleFunction );
#else
eKernelErrorCode DbSetConsoleFunction (DbgHandle  myDebugHdl,  fnPtrTypeConsole myConoleFunction);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbSetLogFileFunction (DbgHandle  myDebugHdl, fnPtrTypeLogFile myLogFileFunction );
#else
eKernelErrorCode DbSetLogFileFunction (DbgHandle  myDebugHdl,  fnPtrTypeLogFile myLogFileFunction);
#endif

#if (!defined KERNEL_MODE)
eUserErrorCode DbSetInterfaceFunction (DbgHandle  myDebugHdl, fnPtrTypeLog myInterfaceFunction );
#else
eKernelErrorCode DbSetInterfaceFunction (DbgHandle  myDebugHdl,  fnPtrTypeLog myInterfaceFunction);
#endif


#ifdef __cplusplus
}
#endif

/*!         
 * \def DEBUG_DECL_CODE 
 * \brief Expansion of variable arguments.
 */
#define DEBUG_DECL_CODE(varArg,...)     __VA_ARGS__

/*!         
 * \def HEXDUMP 
 * \brief Displays information in hex dump format.
 */
#define HEXDUMP(hdl,myTraceType , data,size)          HexDump( hdl , myTraceType, data, size , __FILE__ , __FUNCTION__ ,__LINE__  )

/*!         
 * \def DEBUG_INIT
 * \brief Initialize library for logging information.
 */

//DEBUG_INIT - This is the legacy macro which does not work for EFI builds. 
//CCDEBUG_INIT - This is the new macro. All CCB packages/products should move to this.
#ifndef UEFI
#define DEBUG_INIT(modName , hdlPtr )         DbInit(modName,hdlPtr)    
#endif
#define CCDEBUG_INIT(modName , hdlPtr )         DbInit(modName,hdlPtr)  


#define DEBUG_PRINT(DbgHdl , myTraceType , format , ... )      DebugOutputLogs((DbgHdl) , __FILE__ , __FUNCTION__, __LINE__ ,(myTraceType), format , ## __VA_ARGS__ )

/****************  WIll be removed once all converted to new Debug Lib *********************************/
#define DEBUG_FN_ENTRY(hdl,fmt, ...)    	DEBUG_PRINT(hdl , eDEBUG_TYPE_ENTRY ,fmt, ## __VA_ARGS__) 
#define DEBUG_FN_INFO(hdl,fmt, ...)    		DEBUG_PRINT(hdl , eDEBUG_TYPE_INFO  ,fmt, ## __VA_ARGS__) 
#define DEBUG_FN_PARAM(hdl,fmt, ...)    	DEBUG_PRINT(hdl , eDEBUG_TYPE_PARAM, fmt, ## __VA_ARGS__) 
#define DEBUG_FN_ERROR(hdl,fmt, ...)    	DEBUG_PRINT(hdl , eDEBUG_TYPE_ERROR, fmt, ## __VA_ARGS__) 
#define DEBUG_FN_WARN(hdl,fmt, ...)     	DEBUG_PRINT(hdl , eDEBUG_TYPE_WARN, fmt, ## __VA_ARGS__) 
#define DEBUG_FN_EXIT(hdl,fmt, ...)     	DEBUG_PRINT(hdl , eDEBUG_TYPE_EXIT, fmt, ## __VA_ARGS__) 

#define DEBUG_SET_ERROR_ON_OFF(hdl,on_off)     		DbEnableDisableTraceType(hdl , eDEBUG_TYPE_ERROR ,on_off)
#define DEBUG_SET_WARN_ON_OFF(hdl,on_off)     		DbEnableDisableTraceType(hdl , eDEBUG_TYPE_WARN , on_off)
#define DEBUG_SET_PARAM_ON_OFF(hdl,on_off)     		DbEnableDisableTraceType(hdl , eDEBUG_TYPE_PARAM , on_off)
#define DEBUG_SET_INFO_ON_OFF(hdl,on_off)      		DbEnableDisableTraceType(hdl , eDEBUG_TYPE_INFO , on_off)
#define DEBUG_SET_EE_ON_OFF(hdl,on_off)        		DbEnableDisableTraceType(hdl , eDEBUG_TYPE_EXIT | eDEBUG_TYPE_ENTRY ,  on_off)
#define DEBUG_SET_ALL_ON_OFF(hdl, on_off)			DbEnableDisableTraceType(hdl , eDEBUG_TYPE_ALL ,  on_off)

/****************  End of WIll be removed once all converted to new Debug Lib *********************************/

#define DEBUG_EXIT(hdl)                 DbExit(hdl)

#define DEBUG_SET_CONSOLE_FUNCTION(Handle, fnPtr)                      DbSetConsoleFunction(Handle , fnPtr)
#define DEBUG_SET_INTERFACE_FUNCTION(Handle , fnPtr)                   DbSetInterfaceFunction(Handle , fnPtr)
#define DEBUG_SET_LOGFILE_FUNCTION(Handle , fnPtr)					   DbSetLogFileFunction(Handle , fnPtr)
#define DEBUG_SET_OUTPUT_INTERFACE(hdl , myOutputType,  on_off)        DbEnableDisableOutputType(hdl , myOutputType , on_off)
#define DEBUG_SET_TRACE_TYPE(hdl , myTraceType, on_off)                DbEnableDisableTraceType(hdl , myTraceType, on_off)
#define DEBUG_SET_TRACE_ELEMENTS(hdl , myTraceFormatType , on_off)     DbEnableDisableTraceFormatType(hdl , myTraceFormatType, on_off)
#define DEBUG_SET_FLAGS(hdl , myDebugFlags,  on_off)                   DbEnableDisableSetFlags(hdl , myDebugFlags, on_off)
#define DEBUG_SET_LOCAL_FILE_HANDLE(hdl , FilePointer)                 DbSetFilePtr(hdl, FilePointer)
#define DEBUG_SET_USER_FUNCTION(hdl, fnPtrTypeUserFunction )           DbSetUserDefinedFunction(hdl , fnPtrTypeUserFunction)
#define DEBUG_SET_USER_FUNCTION_TRACE_TYPE(hdl  , myTraceType )        DbSetUserFunctionTraceType(hdl , myTraceType)

/********************************* GLOBALS ***********************************************************************/

#define DEBUG_GLOBAL_SET_TRACE_TYPE(myTraceType , on_off)              DbEnableDisableGlobalTraceType(myTraceType , on_off)
#define DEBUG_GLOBAL_SET_OUTPUT_INTERFACE(myOutputType, on_off)        DbEnableDisableGlobalOutputType( myOutputType , on_off)
#define DEBUG_GLOBAL_SET_TRACE_ELEMENTS(myTraceFormatType, on_off)     DbEnableDisableGlobalTraceFormatType( myTraceFormatType , on_off )
#define DEBUG_GLOBAL_SET_FLAGS(myDebugFlags, on_off)                   DbEnableDisableGlobalSetFlags(myDebugFlags, on_off)
#define DEBUG_SET_GLOBAL_FILE_SYNC_FLAG(on_off)                        DbEnableDisableGlobalSyncFlag(on_off)
#define DEBUG_SET_GLOBAL_FILE_HANDLE(FilePointer)                	   DbSetGlobalFilePtr((FilePointer))




#define POINTER_CHECK(x)    if ((x) == NULL) return(eNullPointer);

#endif  // end of LIB_DEBUG

