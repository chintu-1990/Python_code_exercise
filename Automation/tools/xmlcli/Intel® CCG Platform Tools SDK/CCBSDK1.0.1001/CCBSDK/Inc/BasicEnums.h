/* Dual Licensed File -:
	User can choose between GPL-2.0 and Intel Proprietary License,
	as appropriate after consulting with Products/HWAPIDriver/License_Use.txt
	
 
 ****************** GPL-2.0 *********************************************
    This header contains all enum declarartions which can be used 
	across all packages and Product
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
	BasicEnums.h
Abstract:
	This header contains all enum declarartions which can be used 
	across all packages and Product
 */


#ifndef __BASICENUM_H_
#define __BASICENUM_H_

/*
 *  \file 	   BasicEnums.h
 *  \brief     basic C enums.
 *  \details   This provides different "C" enumerations can be used by libraries and Applications.
 *  \author    Intel Corporation.
 *  \version   1.0.0.1000
 *  \date      2011-2015
 *  \bug       TBD
 *  \copyright Copyright (c)  2012 Intel Corporation. All rights reserved
 *  \dontinclude BasicEnums.h
 */
 
  /** @defgroup BasicEnum enumerations
 *  enum's to be used by Libs and Apps.
 */
 

  /*! \enum   eBlockType
   * \brief CPU number enumeration 0-31,eCPU_Any.Structures to aid in the passing of registers in and out
       of the driver.
   *  \ingroup BasicEnum
 */
typedef enum
{
	e32Bit, /**< 32bit block */
	e64Bit  /**<  64bit block */
} eBlockType;



/*! \enum   eControlRegister
 *  \brief Control register enumeration
 *  \ingroup BasicEnum
 *  \see eDebugRegister
 */
typedef enum
{
	eCR0 = 0,/**< Control Register 0 */
	eCR2 = 2,/**< Control Register 2 */
	eCR3,	 /**< Control Register 3 */
	eCR4,	 /**< Control Register 4 */
	eCR8 = 8,/**< Control Register 8 */
} eControlRegister;


/*! \enum   eDebugRegister
 *  \brief debug register enumeration
 *  \ingroup BasicEnum
 *  \see eControlRegister
 */
typedef enum
{
	eDR0 = 0,
	eDR1,
	eDR2,
	eDR3,
	eDR4,
	eDR5,
	eDR6,
	eDR7
} eDebugRegister;

/*! \enum   eCPUNumber
 *  \brief CPU number enumeration 0-31,eCPU_Any
 *  \ingroup BasicEnum
 */
typedef enum
{
	eCPU_0 = 0,	eCPU_1,  eCPU_2,  eCPU_3,  eCPU_4,  eCPU_5,  eCPU_6,  eCPU_7,  eCPU_8,	eCPU_9,  eCPU_10, eCPU_11, eCPU_12, eCPU_13, eCPU_14, eCPU_15,
	eCPU_16,	eCPU_17, eCPU_18, eCPU_19 ,eCPU_20, eCPU_21, eCPU_22, eCPU_23 ,eCPU_24, eCPU_25, eCPU_26, eCPU_27, eCPU_28, eCPU_29, eCPU_30, eCPU_31,
	eCPU_32,	eCPU_33, eCPU_34, eCPU_35 ,eCPU_36, eCPU_37, eCPU_38, eCPU_39 ,eCPU_40, eCPU_41, eCPU_42, eCPU_43, eCPU_44, eCPU_45, eCPU_46, eCPU_47,
	eCPU_48,	eCPU_49, eCPU_50, eCPU_51 ,eCPU_52, eCPU_53, eCPU_54, eCPU_55 ,eCPU_56, eCPU_57, eCPU_58, eCPU_59, eCPU_60, eCPU_61, eCPU_62, eCPU_63,
	eCPU_64,	eCPU_65, eCPU_66, eCPU_67 ,eCPU_68, eCPU_69, eCPU_70, eCPU_71 ,eCPU_72, eCPU_73, eCPU_74, eCPU_75, eCPU_76 ,eCPU_77, eCPU_78, eCPU_79,
	eCPU_80,	eCPU_81, eCPU_82, eCPU_83 ,eCPU_84, eCPU_85, eCPU_86, eCPU_87 ,eCPU_88, eCPU_89, eCPU_90, eCPU_91, eCPU_92 ,eCPU_93, eCPU_94, eCPU_95,
	eCPU_96,	eCPU_97, eCPU_98, eCPU_99 ,eCPU_100,eCPU_101,eCPU_102,eCPU_103 ,eCPU_104, eCPU_105, eCPU_106, eCPU_107, eCPU_108 ,eCPU_109, eCPU_110, eCPU_111,
	eCPU_112,	eCPU_113, eCPU_114, eCPU_115 ,eCPU_116,eCPU_117,eCPU_118, eCPU_119 ,eCPU_120, eCPU_121, eCPU_122, eCPU_123, eCPU_124, eCPU_125 ,eCPU_126, eCPU_127,
	eCPU_128,	eCPU_129, eCPU_130, eCPU_131 ,eCPU_132,eCPU_133,eCPU_134, eCPU_135 ,eCPU_136, eCPU_137, eCPU_138, eCPU_139, eCPU_140, eCPU_141 ,eCPU_142, eCPU_143,
	eCPU_144,	eCPU_145, eCPU_146, eCPU_147 ,eCPU_148,eCPU_149,eCPU_150, eCPU_151 ,eCPU_152, eCPU_153, eCPU_154, eCPU_155, eCPU_156, eCPU_157 ,eCPU_158, eCPU_159,
	eCPU_160,	eCPU_161, eCPU_162, eCPU_163 ,eCPU_164,eCPU_165,eCPU_166, eCPU_167 ,eCPU_168, eCPU_169, eCPU_170, eCPU_171, eCPU_172, eCPU_173 ,eCPU_174, eCPU_175,
	eCPU_176,	eCPU_177, eCPU_178, eCPU_179 ,eCPU_180,eCPU_181,eCPU_182, eCPU_183 ,eCPU_184, eCPU_185, eCPU_186, eCPU_187, eCPU_188, eCPU_189 ,eCPU_190, eCPU_191,
	eCPU_192,	eCPU_193, eCPU_194, eCPU_195 ,eCPU_196,eCPU_197,eCPU_198, eCPU_199 ,eCPU_200, eCPU_201, eCPU_202, eCPU_203, eCPU_204, eCPU_205 ,eCPU_206, eCPU_207,
	eCPU_208,	eCPU_209, eCPU_210, eCPU_211 ,eCPU_212,eCPU_213,eCPU_214, eCPU_215 ,eCPU_216, eCPU_217, eCPU_218, eCPU_219, eCPU_220, eCPU_221 ,eCPU_222, eCPU_223,
	eCPU_224,	eCPU_225, eCPU_226, eCPU_227 ,eCPU_228,eCPU_229,eCPU_230, eCPU_231 ,eCPU_232, eCPU_233, eCPU_234, eCPU_235, eCPU_236, eCPU_237 ,eCPU_238, eCPU_239,
	eCPU_240,	eCPU_241, eCPU_242, eCPU_243 ,eCPU_244,eCPU_245,eCPU_246, eCPU_247 ,eCPU_248, eCPU_249, eCPU_250, eCPU_251, eCPU_252, eCPU_253 ,eCPU_254, eCPU_255,
	eCPU_Any
} eCPUNumber;



#endif  //__BASICENUM_H_
