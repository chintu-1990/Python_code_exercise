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

OptionHandllers.h

Abstract:
This file contains function declarations pertaining to CCB HWAPI Test application.
These functions call into the HWAPI library interface to access the HAPI driver functionality.
*/

#ifndef TESTAPP_OPTIONHANDLERS_H
#define TESTAPP_OPTIONHANDLERS_H

eUserErrorCode pciCfgReadHandler();
eUserErrorCode pciCfgWriteHandler();
eUserErrorCode mmioReadHandler();
eUserErrorCode mmioWriteHandler();
eUserErrorCode portReadHandler();
eUserErrorCode portWriteHandler();
eUserErrorCode msrReadHandler();
eUserErrorCode msrWriteHandler();
eUserErrorCode cpuidHandler();
eUserErrorCode pciExReadHandler();
eUserErrorCode memReadHandler();
eUserErrorCode memWriteHandler();

#endif