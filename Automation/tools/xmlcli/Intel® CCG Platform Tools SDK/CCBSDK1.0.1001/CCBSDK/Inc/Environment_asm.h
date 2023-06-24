/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
 * This headers helps defining  the Environment we are working on
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
	Environment_asm.h
Abstract:
	This headers helps defining  the Environment we are working on
 */
 

#ifndef _ENVIRONMENT_H_ASM
#define _ENVIRONMENT_H_ASM

/*
	These are the supported CPUTYPE values
*/
#define kCPU_Undefined	0
#define kCPU_i386		1
#define kCPU_x86_64		2
#define kCPU_Atom		3
#define kCPU_Itanium	4
#define kCPU_ARM		5

/*
	These are the supported OS Values
*/
#define kOS_Undefined	0
#define kOS_MacOSX		1
#define kOS_Linux		2
#define kOS_Win32		3
#define kOS_WinXP		4
#define kOS_Win7		5
#define kOS_Win8		6
#define kOS_iOS			7
#define kOS_BSD			8
#define kOS_SysV		9
#define kOS_VxWorks		10

/*
	Values for CONFIG
*/
#define kDebug			1
#define kRelease		2

#endif

