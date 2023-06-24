/*
	Copyright (c)  2007-2012 Intel Corporation. All rights reserved
	This software and associated documentation (if any) is furnished
	under a license and may only be used or copied in accordance
	with the terms of the license. Except as permitted by such
	license, no part of this software or documentation may be
	reproduced, stored in a retrieval system, or transmitted in any
	form or by any means without the express written consent of
	Intel Corporation.

Module Name:

	MarginStruct.h

Abstract:
	This header contains the structures which can be used across 
	all Electrical Validation packages which perform margining.

*/
#ifndef __MARGINTYPES_H_
#define __MARGINTYPES_H_

 /*
 *  \file 	   MarginTypes.h
 *  \brief     basic C defines.
 *  \details   This provides different "C" structures can be used by margining Packages and products.
 *  \author    Intel Corporation.
 *  \version   1.0.0.1000
 *  \date      2011-2015
 *  \bug       TBD
 *  \copyright Copyright (c)  2012 Intel Corporation. All rights reserved
 *  \dontinclude MarginTypes.h
 */
 
   /** @defgroup MarginDef Margining Structs
 *  enum's to be used by Libs and Apps.
 */
#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS 
#include BASICTYPES
#include BASICENUMS


#define		MARGIN_ALL_LANES		0xFF
#define		MARGIN_ALL_PORTS		0xFF
#define		LANE_NUMBER_NOT_VALID	0xFF
#define		PORT_NUMBER_NOT_VALID	0xFF
#define		NA						0xFF
#define		MARGIN_NOT_SUPPORTED	0xF0

#define LPTH						0x1
#define LPTM						0x2
#define LPTLP						0x3


/*! \enum   eAlgorithm
 *  \brief Algorithms that are currenly impemented for margining.
 *  \ingroup MarginDef
 */   
typedef enum
{
    VOCAdjustmentAlgo = 1,/**< Voc Adjustment Algorithm */
	JitterInjectionAlgo,/**< Jitter injection algorithm using timing table */
	ClosedLoopPeakAlgo,
	VOCAdjustmentViaDataSamplersAlgo,
	VOCAdjustmentViaErrorSamplersAlgo,
	VOCAdjustmentViaTxSwingAlgo,
	PhaseInterpolatorAlgo
	
}eAlgorithm;


/*! \enum   eMarginType
 *  \brief Type of margining.
 *  \ingroup MarginDef
 */   
typedef enum
{
	ePerPortMarigin = 0,/**< per port margin - apply \a eAlgorithm on all lanes once */
	eLaneMarigin/**< per Lane margin - apply \a eAlgorithm lane by lane basis */
} eMarginType;

/*! \enum   eMarginFunctonality
 *  \brief margining functionality Voltage or timing...
 *  \ingroup MarginDef
 */
typedef enum
{
	eVoltageMargin,
	eTimingMargin
}eMarginFunctionality;

/*! \struct   MarginType_t, MarginTypePtr_t
 *  \brief  request for a particuler type of margining. for ePerPortMarigin-PortNumber,LaneNumber are invalid.
 *          for LaneNumber = \a MARGIN_ALL_LANES,margining will be done for all lanes else only for the given lane.
 *			similerly PortNumber= \a MARGIN_ALL_PORTS, margining will be done on all ports else only for the given port.if port number
 *			not applicable for a particuler algorithm then this param will not be used.
 *  \ingroup MarginDef
 */
typedef struct
{
	eMarginType 	MarginType;/**< margining Type */
	UInt16			PortNumber;/**< Port Number, \a MARGIN_ALL_PORTS for all ports */
	UInt16			LaneNumber;/**< Lane Number, \a MARGIN_ALL_LANES for all lanes */
}MarginType_t, *MarginTypePtr_t;


/*! \struct   RegisterDetails_t,RegisterDetailsPtr_t
 *  \brief  Information for each register.
 *  \ingroup MarginDef
 */
typedef struct
{
    UInt32                	 Version;/**< Structure version number */
    UInt8Ptr                 RegisterName;/**< Register Name */
    UInt16                   RegisterAddress;/**< Register Address or offset */
}RegisterDetails_t, *RegisterDetailsPtr_t;


/*! \struct   ParameterDetails_t,ParameterDetailsPtr_t
 *  \brief  user input Parametre details.
 *  \ingroup MarginDef
 */
typedef struct
{
    UInt32                	 Version;       /**< Structure version number */
    UInt8Ptr                 ParameterName; /**< Parametre Name */
    UInt32                   NoOfParameters;/**< No of values for this parametre */
	UInt16					 Parameter[0];  /**< array of values */
}ParameterDetails_t, *ParameterDetailsPtr_t;

/*! \struct   MarginResult_t,MarginResultPtr_t
 *  \brief  resulting data after margining.
 *  \ingroup MarginDef
 */
typedef struct
{
    UInt32               	Version;/**< Structure version number */
    UInt8                   Count;  /**< No of Values available*/
	UInt8Ptr                HighPtr;/**< high margin value for each lane, for port margining  0-offset will have the result*/
	UInt8Ptr                LowPtr;/**< low margin value for each lane, for port margining  0-offset will have the result*/
}MarginResult_t, *MarginResultPtr_t;


/*! \struct   MarginingTable_t,MarginingTablePtr_t
 *  \brief  table of input parametres to start margining.
 *  \ingroup MarginDef
 */
typedef struct
{   
    UInt32              	Version;  /**< Structure version number */
    eAlgorithm              Algorithm;/**< algorithm that should be used in margining */
    UInt32                  RegisterCount;/**< no of user defined registers */
    RegisterDetailsPtr_t    RegisterDetails;/**< list of registers */
    UInt32                  ParameterCount;/**< no of user defined parametres */
    ParameterDetailsPtr_t   *ParameterDetails; /**< list of input parametres */ 
}MarginingTable_t,*MarginingTablePtr_t;

/*! \struct   TimingTable_t,TimingTablePtr_t
 *  \brief  input data for timing margin.
 *  \ingroup MarginDef
 */
typedef struct
{
	UInt32					Version;/**< Structure version number */
	UInt8    				MaxDepth;/**< Max depth or no of values for step or SpareBit */
	UInt32   				*Step;  /**< step values for timing margin */
	UInt8    				*SpareBit;/**< SpareBit values for timing margin, NULL if it doesn't exist.*/
}TimingTable_t, *TimingTablePtr_t;

/*! \enum   eDirection
 *  \brief  Current direction of margining.
 *  \ingroup MarginDef
 */
typedef enum
{
	eHigh = 1,/**< marginig UP Words */
	eLow/**< marginig down Words */
}eDirection;


/*! \struct   StepResult_t,StepResultPtr_t
 *  \brief  Data for each step of margining.These are intermediate values.
 *  \ingroup MarginDef
 */
typedef struct
{
	UInt32					Version;/**< Structure version number */
	eDirection 				Direction;/**< Current margining direction UP/DOWN */
	UInt32					CurrentStep;/**< current Step Value */
	eMarginFunctionality	MarginFunctionality;/**< is the result of voltage/timing margin.. */
	UInt8					PortNumber;/**< port number for which the current margining done,PORT_NUMBER_NOT_VALID if this param not applicable */
	UInt8					LaneNumber;/**< Lane number for which the current margin done. LANE_NUMBER_NOT_VALID if this param not applicable*/
	
}StepResult_t, *StepResultPtr_t;

/*! \typedef  void (*MarginUpdateInfo_t)(StepResultPtr_t MrgnResult )
 *  \brief    call back function that should be implemented in product to get intermediate values.
 *  \param    [OUT] \a MrgnResult		- margin result for each step.
 *  \return   None
 *  \ingroup  CPUDMI
 */ 

typedef enum
{
	GEN_ONE = 1,
	GEN_TWO,
	GEN_THREE
}Negotiated_SPEED;


typedef struct
{
	UInt32					Version;/**< Structure version number */
	UInt32    				PortNo;/**< Port No */
	UInt8   				PortStatus[64];  /**< Port State */
	UInt32    				PortSpeed;/**< Port Speed*/
}PortInfo_t, *PortInfoPtr_t;


typedef void (*MarginUpdateInfo_t)(StepResultPtr_t MrgnResult );

bool IsTxMargin;
bool IsVoltageMarginRequested;
bool IsLPTDetected;
bool IsWPTDetected;
UInt32 LPTType;

bool IsSATAPMChanged[6];

UInt32 PollPeriod;

bool SATALog;

#endif



