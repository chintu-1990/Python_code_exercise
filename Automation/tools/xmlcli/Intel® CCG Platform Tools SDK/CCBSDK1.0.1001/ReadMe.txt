*******************************************************************
Intel(R) CCBSDK
Version 1.0.1001
July  25th  2016
Intel Confidential
*******************************************************************
README Contents
===============
1.  Introduction
2.  System Requirements
3.  Release Notes
4.  Installation Instructions
5.  Known Issues and Bug Reporting
6.  License Agreement

*******************************************************************

1.  Introduction
================

Intel(R) CCBSDK provides a framework for Intel(R) platform debug tool development

2.  System Requirements
=======================
The Intel(R) CCBSDK is currently supported on Windows only.
The Intel(R) CCBSDK development kit packages support the following platforms: 
	SkyLake platform, KabyLake platform, ApolloLake platform
	   
3.  Release Notes
=================

Release 1.0.1001
	Contains 
	- HWAPI Driver (CCHWAPI.sys) Version 2.0.2
        - HWAPILib library
	- TopologyLib library
	- ContainerLib library
	- OSInterface library
	- Debug library
      
4.  Installation and Usage Instructions 
=======================================

On Windows (Win7, Win8, Win10)
===================

1) Boot to Windows.
2) Download tool and unzip it
3) cd to tool folder
4) Run Setup.exe
5) cd to tool installation folder 
6)   cd to Src folder to view the sample code that uses the CCBSDK libraries.
     cd to Winx64\Release\sample_dll_link (for x64 platform)
	This folder contains Makefile that demonstrates how to build an application by dynamically linking to the CCBSDK libraries
	Run TestHWAPI to see a demo of HWAPILib and HWAPIDriver functionlity
	Run TestTopology to see a demo of TopologyLib functionality
7)   cd to Winx64\Release\sample_lib_link (for x64 platform)
	This folder contains Makefile that demonstrates how to build an application by statically linking to the CCBSDK libraries
	Run TestHWAPI to see a demo of HWAPILib and HWAPIDriver functionlity
	Run TestTopology to see a demo of TopologyLib functionality

5. Known Issues and Bug Reporting
==================================
To report a bug on this utility, please submit the bug report to
your Intel Field Representative.

*******************************************************************
This ReadMe file as well as the software described in it is furnished 
under license and may only be used or copied in accordance with the 
terms of the license. The information in this manual is furnished for 
informational use only, is subject to change without notice, and 
should not be construed as a commitment by Intel Corporation. 
Intel Corporation assumes no responsibility or liability for any 
errors or inaccuracies that may appear in this document or any 
software that may be provided in association with this document. 

Except as permitted by such license, no part of this document may be 
reproduced, stored in a retrieval system, or transmitted in any form 
or by any means without the express written consent of 
Intel Corporation. 

INFORMATION IN THIS DOCUMENT IS PROVIDED IN CONNECTION WITH INTEL®
PRODUCTS. NO LICENSE, EXPRESS OR IMPLIED, BY ESTOPPEL OR OTHERWISE,
TO ANY INTELLECTUAL PROPERTY RIGHTS IS GRANTED BY THIS DOCUMENT.
EXCEPT AS PROVIDED IN INTEL TERMS AND CONDITIONS OF SALE FOR SUCH
PRODUCTS, INTEL ASSUMES NO LIABILITY WHATSOEVER, AND INTEL DISCLAIMS
ANY EXPRESS OR IMPLIED WARRANTY, RELATING TO SALE AND/OR USE OF INTEL
PRODUCTS INCLUDING LIABILITY OR WARRANTIES RELATING TO FITNESS FOR A
PARTICULAR PURPOSE, MERCHANTABILITY, OR INFRINGEMENT OF ANY PATENT,
COPYRIGHT OR OTHER INTELLECTUAL PROPERTY RIGHT.
UNLESS OTHERWISE AGREED IN WRITING BY INTEL, THE INTEL PRODUCTS ARE
NOT DESIGNED NOR INTENDED FOR ANY APPLICATION IN WHICH THE FAILURE
OF THE INTEL PRODUCT COULD CREATE A SITUATION WHERE PERSONAL INJURY 
OR DEATH MAY OCCUR.
Intel may make changes to specifications and product descriptions at any time,
without notice. Designers must not rely on the absence or characteristics of any
features or instructions marked reserved or undefined. Intel reserves these for 
future definition and shall have no responsibility whatsoever for conflicts or 
incompatibilities arising from future changes to them. The information here is 
subject to change without notice. Do not finalize a design with this information.

The products described in this document may contain design defects or errors
known as errata which may cause the product to deviate from published specifications. 
Current characterized errata are available on request.
Contact your local Intel sales office or your distributor to obtain the latest 
specifications and before placing your product order. 

45-nm products are manufactured on a lead-free process. Lead-free per EU RoHS 
directive July, 2006. Some E.U. RoHS exemptions may apply to other components 
used in the product package. Residual amounts of halogens are below 
November, 2007 proposed IPC/JEDEC J-STD-709 standards.

Reverse engineering or disassembly is prohibited.

Intel, and the Intel logo are trademarks of Intel Corporation in the U.S. and other countries.

*Other names and brands may be claimed as the property of others.

Copyright © 2016, Intel Corporation. All rights reserved.
*******************************************************************