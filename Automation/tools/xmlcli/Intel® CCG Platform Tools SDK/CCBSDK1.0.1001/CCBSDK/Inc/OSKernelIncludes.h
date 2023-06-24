/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
 * This header helps abstracts OS-functionalities across Different OS in kernel Space
 
 *  Copyright (c) 2013, Intel Corporation.
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
	OSKernelIncludes.h
Abstract:
	This header helps abstracts OS-functionalities across Different OS in kernel Space
 */

#ifndef _OSKERNEL_INCLUDES
#define _OSKERNEL_INCLUDES

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#define osMutex_t UInt32 //// DOne for time being 
#ifdef APPLE
#ifdef __cplusplus
#include <libkern/c++/OSObject.h>
#include <IOKit/IOService.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/IOMemoryDescriptor.h>
#include <IOKit/IOUserClient.h>
#endif
#include <IOKit/IOLib.h>
#include <mach/kern_return.h>
#include <libkern/OSMalloc.h>

#ifdef __cplusplus
extern "C"
{
#endif
extern thread_t current_thread(void);
extern processor_t cpu_to_processor(int);
extern void thread_bind(processor_t);
#ifdef __cplusplus
};
#endif 

typedef thread_t	osThread_t;
typedef	uint32_t	osCpuMask_t;	
#define OS_INIT_CPUMASK(theCpuMask)                              (theCpuMask) = 0;
#define os_udelay(microdelay)									IODelay(microdelay)

#define osGetCurrentThread()								current_thread()
#define	osGetCurrentCPUMask(theThread, theCpuMaskPtr)		
#define osSetThreadAffinity(theThread, theCPU, theCPUMask)	thread_bind(cpu_to_processor((theCPU)))
#define osUnsetThreadAffinity(theThread, theCpuMask)		thread_bind(0);

#define os_alloc_generic(MemSize ,TAG)		                    OSMalloc((MemSize) ,(TAG))														 
#define os_free_generic(MemPtr,MemSize ,TAG)                    OSFree(MemPtr,MemSize,(TAG))
#define os_memset(ptr,value,size)								memset((ptr),(value),(size))

#endif /// #ifdef APPLE ends 

#ifdef __unix__

//#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/cpumask.h>
#include <asm/io.h>


typedef struct task_struct *   osThread_t;
#ifndef __VMKLNX__
typedef struct cpumask         osCpuMask_t;
#define OS_INIT_CPUMASK(theCpuMask)                                      cpumask_clear(&(theCpuMask));
#else
typedef	uint32_t	osCpuMask_t;	
#define OS_INIT_CPUMASK(theCpuMask)                              (theCpuMask) = 0;
#endif
/*
#define osGetCurrentThread()						current
#define	osGetCurrentCPUMask(theThread, theCpuMaskPtr)			*(theCpuMaskPtr) = theThread->cpus_allowed;
#define osSetThreadAffinity(theThread, theCpu, theCpuMask)		{ osCpuMask_t new; cpumask_set_cpu(theCpu,&new); set_cpus_allowed_ptr(theThread,&new); }
#define osUnsetThreadAffinity(theThread, theCpuMask)			 set_cpus_allowed_ptr(theThread,&theCpuMask) 
*/

#ifndef __VMKLNX__
#define osGetCurrentThread()						get_current();
#define	osGetCurrentCPUMask(theThread, theCpuMaskPtr)			*(theCpuMaskPtr) = theThread->cpus_allowed;
#define osSetThreadAffinity(theThread, theCpu, theCpuMask)		{ osCpuMask_t new; cpumask_set_cpu(theCpu,&new); set_cpus_allowed_ptr(theThread,&new); }
#define osUnsetThreadAffinity(theThread, theCpuMask)			 set_cpus_allowed_ptr(theThread,&theCpuMask)
#else
#define osGetCurrentThread()					        0
#define	osGetCurrentCPUMask(theThread, theCpuMaskPtr)			{printk(KERN_ALERT"dummy, osGetCurrentCPUMask\n");*(theCpuMaskPtr) = -1; }// unsupported
#define osSetThreadAffinity(theThread, theCpu, theCpuMask)		{printk(KERN_ALERT"dummy, osSetThreadAffinity\n");} // Not supported in VMKernel
#define osUnsetThreadAffinity(theThread, theCpuMask)			{printk(KERN_ALERT"dummy, osUnsetThreadAffinity\n");} //Not Supported in VMKernel
#endif
#define os_memset(ptr,value,size)								memset((ptr),(value),(size))
#define os_udelay(microdelay)									udelay(microdelay)
#define os_alloc_generic(MemSize ,TAG)							kmalloc( (MemSize),GFP_KERNEL)									 
#define os_free_generic(MemPtr,MemSize ,TAG)					kfree((MemPtr))

#endif

#ifdef _WIN32_WINNT
#include PACKAGE_SPECS
#include KERNELERRORLIB
#include ENVIRONMENT 
#include BASICTYPES

#include"ntddk.h"
typedef PKTHREAD	    osThread_t;
#if _AMD64_
typedef	GROUP_AFFINITY	osCpuMask_t;
eKernelErrorCode AffinitizeWinKernel(UInt16 theCpu , osCpuMask_t * myOldMaskPtr);
#else
typedef 	KAFFINITY  osCpuMask_t;
#endif




#define osGetCurrentThread()								 KeGetCurrentThread();
#define	osGetCurrentCPUMask(theThread, theCpuMaskPtr)       
#define os_udelay(microdelay)								 KeStallExecutionProcessor(microdelay)
#ifdef _AMD64_
#define OS_INIT_CPUMASK(theCpuMask)                          memset(&theCpuMask, 0 , sizeof(osCpuMask_t) );
#define osSetThreadAffinity(theThread, theCpu, theCpuMask)	 AffinitizeWinKernel(theCpu, &(theCpuMask) );
#define osUnsetThreadAffinity(theThread, theCpuMask)		 KeRevertToUserGroupAffinityThread(&(theCpuMask));
#else
#define OS_INIT_CPUMASK(theCpuMask)                          (theCpuMask) = 0;
#define osSetThreadAffinity(theThread, theCpu, theCpuMask)	 { (theCpuMask) = KeSetSystemAffinityThreadEx(1 << (theCpu) ); }
#define osUnsetThreadAffinity(theThread, theCpuMask)		 KeRevertToUserAffinityThreadEx((theCpuMask));
#endif /* _AMD64 */

#define os_memset(ptr,value,size)							 RtlFillMemory((PVOID)(ptr),(SIZE_T)(size),(UCHAR)(value));
#define os_alloc_generic(MemSize,TAG)						 ExAllocatePoolWithTag(PagedPool,MemSize,'BEEF')	 
#define os_free_generic(MemPtr,MemSize ,TAG)                 ExFreePool((MemPtr))

#endif

#endif	/* _OSKERNEL_INCLUDES */
