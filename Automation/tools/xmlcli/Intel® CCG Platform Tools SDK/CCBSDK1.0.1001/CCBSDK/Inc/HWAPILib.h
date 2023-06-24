#ifndef _HWAPILIB_H
#define _HWAPILIB_H

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS
#include BASICTYPES
#include BASICSTRUCTS
#include ERRORLIB
#include HWAPISTRUCTS

/*! 
 *  \file 	   HWAPILib.h
 *  \brief     CPU,PCI,IO access API's
 *  \details   This provides Interface to access CPU,PCI,IO functionalities.
 *  \author    Intel Corporation.
 *  \version   1.0.0.1000
 *  \date      2011-2015
 *  \copyright  Copyright (c)  2012 Intel Corporation. All rights reserved
 *  \bug       TBD
 *  \dontinclude HWAPILib.h
 */

 /*! \mainpage About HWAPILib
 *
 * \section   HWAPILib Library to access any hardware registers , memory, PCI.
 * 			  HWAPI includes  userspace library and driver.
 *
 *  \warning   This library intended to be used with in the Organization.
 *			   Neither library nor documentation can be distributed outside the organization.
 */
 
   /*! \example HWAPILib_test.c
 * This is an example of how to use the HWAPI library.
 */
 

  /** @defgroup MSROps MSR R/W Operations
 *  API's for MSR Operations.
 */

  /** @defgroup PCIOps PCI R/W Operations
 *  API's to access PCI memory and config registers
 */ 

 /** @defgroup PortIO Port IO Operations
 *  API's to access IO Ports
 */  

  /** @defgroup HWAPIGen HWAPI general Operations
 *  genaral API's that doesn't fall in to any specific category
 */  

 #ifdef __cplusplus
extern "C" {
#endif


/*! \fn       eUserErrorCode	HWAPIInitialize()
 *  \brief    init for HWAPI. This must be called before using any API from HWAPI
 *  \param    None
 *  \return   eUserErrorCode - returns eSuccess on success,eHWAPIInitializationFailed - on failure
 *  \ingroup  HWAPIGen
 *  \see      HWAPITerminate()
 */
eUserErrorCode		HWAPIInitialize();


/*! \fn       eUserErrorCode	HWAPITerminate()
 *  \brief    terminate or exit function for HWAPI. Called when HWAPI functionality not required
 *  \param    None
 *  \return   eUserErrorCode - returns eSuccess on success,eHWAPITerminationFailed - on failure
 *  \ingroup  HWAPIGen
 *  \see      HWAPIInitialize()
 */
eUserErrorCode		HWAPITerminate();


/*! \fn       eUserErrorCode	GetHWAPIDriverVersion(LibVerPTr VersionPtr)
 *  \brief    to get the version of the current HWAPI driver. 
 *  \param    [OUT] \a VersionPtr - Major:Minor version of driver
 *  \return   eUserErrorCode - returns eSuccess on success,eHWAPITerminationFailed - on failure
 *  \ingroup  HWAPIGen
 *  \see      HWAPIInitialize()
 */
eUserErrorCode GetHWAPIDriverVersion(LibVerPtr VersionPtr);

/*! \fn       eUserErrorCode	GetPCILibVersion(LibVerPTr VersionPtr)
 *  \brief    to get the version of the current PCI library of the HWAPI driver. 
 *  \param    [OUT] \a VersionPtr - Major:Minor version of library
 *  \return   eUserErrorCode - returns eSuccess on success,eHWAPITerminationFailed - on failure
 *  \ingroup  HWAPIGen
 *  \see      HWAPIInitialize()
 */
eUserErrorCode GetPCILibVersion(LibVerPtr VersionPtr);

/*! \fn       eUserErrorCode PCIExRead(PCIConfAddr PciAddress, UInt16 RegisterOffset, DataElementPtr DataValue)
*  \brief    to Read any PCI extended configuration register values using B:D:F
*  \param    [IN] \a PciAddress    - BUS:DEVICE:FUNC to address a register uniquely
*  \param    [IN]    RegisterOffset - Register address
*  \param    [OUT] \a DataValue - requested data
*  \return   eUserErrorCode - returns eSuccess on success,ePCIReadFail - on failure
*  \ingroup  PCIOps
*  \see      PCIWrite(),PCIMMIORead(),PCIMMIOWrite(),ReadMSR()
*/
eUserErrorCode PCIExRead(PCIConfAddr PciAddress, UInt32 RegisterOffset, DataElementPtr pDataValue);

/*! \fn       eUserErrorCode PCIRead(PCIConfAddr PciAddress, UInt16 RegisterOffset, DataElementPtr DataValue)
 *  \brief    to Read any PCI register values using B:D:F
 *  \param    [IN] \a PciAddress    - BUS:DEVICE:FUNC to address a register uniquely
 *  \param    [IN]    RegisterOffset - Register address
 *  \param    [OUT] \a DataValue - requested data
 *  \return   eUserErrorCode - returns eSuccess on success,ePCIReadFail - on failure
 *  \ingroup  PCIOps
 *  \see      PCIWrite(),PCIMMIORead(),PCIMMIOWrite(),ReadMSR()
 */
eUserErrorCode		PCIRead(PCIConfAddr PciAddress, UInt32 RegisterOffset, DataElementPtr DataValue);


/*! \fn       eUserErrorCode		PCIWrite(PCIConfAddr PciAddress, UInt16 RegisterOffset, DataElement DataValue)
 *  \brief    to write data to any PCI register values using B:D:F
 *  \param    [IN] \a PciAddress    - BUS:DEVICE:FUNC to address a register uniquely
 *  \param    [IN]    RegisterOffset - Register address
 *  \param    [IN] \a DataValue    - data to write
 *  \return   eUserErrorCode - returns eSuccess on success,ePCIWriteFail - on failure
 *  \ingroup  PCIOps
 *  \see      PCIRead(),PCIMMIORead(),PCIMMIOWrite(),WriteMSR()
 */
eUserErrorCode		PCIWrite(PCIConfAddr PciAddress, UInt32 RegisterOffset, DataElementPtr DataValue);

/*! \fn       eUserErrorCode ReadMMIO(UInt32 pBarAddress, UInt16 pRegisterOffset, Int8Ptr pDest,UInt32 noOfBytes)
*  \brief    To read 1 byte from mapped io space.
*  \param    [IN]  /a UInt32   - Address to map
*  \param    [IN]  /a UInt16   - Offset
*  \param    [OUT] /a UInt8Ptr - Pointer to data read.
*  \param    [IN]  /a UInt32   - noOfBytes to read
*  \return   eUserErrorCode - returns eNoError/eFailure
*/
eUserErrorCode ReadMMIO(PtrType BarAddress, UInt32 RegisterOffset, Int8Ptr pDest, UInt32 noOfBytes);

/*! \fn       eUserErrorCode WriteMMIO(UInt32 pBarAddress, UInt16 pRegisterOffset, Int8Ptr pSource,UInt32 noOfBytes)
*  \brief    To write 2 byte to mapped io space.
*  \param    [IN] /a UInt32   - Address to map
*  \param    [IN] /a UInt16   - Offset
*  \param    [IN] /a UInt16   - Data to be written.
*  \param    [IN] /a UInt32   - noOfBytes to write.
*  \return   eUserErrorCode - returns eNoError/eFailure
*/
eUserErrorCode WriteMMIO(PtrType BarAddress, UInt32 RegisterOffset, Int8Ptr pSource, UInt32 noOfBytes);

/*! \fn       eUserErrorCode		PhysicalMemRead(UInt32  BarAddress, UInt16	RegisterOffset, DataElementPtr DataValue)
*  \brief    to Read any Physical address.
*  \param    [IN]    theCpu - cpu number to set affinity
*  \param    [IN]    BaseAddress     - physical address to read. 
*  \param    [OUT] \a DataValue    - data retrieved from physical address
*  \return   eUserErrorCode - returns eSuccess on success, eFailure - on failure
*  \ingroup  PCIOps
*  \see      PhysicalMemWrite()
*/
eUserErrorCode		PhysicalMemRead(eCPUNumber theCpu, PtrType BaseAddress, DataElementPtr pDataValue);

/*! \fn       eUserErrorCode		PhysicalMemWrite(UInt32  BarAddress, UInt16	RegisterOffset, DataElementPtr DataValue)
*  \brief    to Write to any Physical address.
*  \param    [IN]    theCpu - cpu number to set affinity
*  \param    [IN]    BaseAddress     - physical address to write to.
*  \param    [OUT] \a DataValue    - data to be copied to physical address
*  \return   eUserErrorCode - returns eSuccess on success, eFailure - on failure
*  \ingroup  PCIOps
*  \see      PhysicalMemRead()
*/
eUserErrorCode	    PhysicalMemWrite(eCPUNumber theCpu, PtrType BaseAddress, DataElementPtr pDataValue);

/*! \fn       eUserErrorCode		PCIMMIORead(UInt32  BarAddress, UInt16	RegisterOffset, DataElementPtr DataValue)
 *  \brief    to Read any PCI-MMIO address.
 *  \param    [IN]    BarAddress     - MMIO address to read. probably got from \a PCIRead()
 *  \param    [IN]    RegisterOffset - Register address to read
 *  \param    [OUT] \a DataValue    - data retrieved from MMIO address
 *  \return   eUserErrorCode - returns eSuccess on success,ePCIMMIOReadFail - on failure
 *  \ingroup  PCIOps
 *  \see      PCIRead(),PCIWrite(),PCIMMIOWrite()
 */
eUserErrorCode		PCIMMIORead(PtrType  BarAddress, UInt32	RegisterOffset, DataElementPtr DataValue);

/*! \fn       eUserErrorCode		PCIMMIOWrite(UInt32 BarAddress, UInt16	RegisterOffset, DataElement DataValue);
 *  \brief    to Write data to any PCI-MMIO address.
 *  \param    [IN]    BarAddress     - MMIO address to write. probably got from \a PCIRead()
 *  \param    [IN]    RegisterOffset - Register address to write
 *  \param    [IN] \a DataValue    - data to write
 *  \return   eUserErrorCode - returns eSuccess on success,ePCIMMIOWriteFail - on failure.
 *  \ingroup  PCIOps
 *  \see      PCIRead(),PCIWrite(),PCIMMIORead()
 */
eUserErrorCode	    PCIMMIOWrite(PtrType BarAddress, UInt32	RegisterOffset, DataElementPtr DataValue);

/*! \fn       eUserErrorCode		ReadPort(UInt16	PortAddress, DataElementPtr DataValue)
 *  \brief    to read IO port
 *  \param    [IN]    PortAddress     - port number/Address
 *  \param    [OUT] \a DataValue     - data retrieved from port
 *  \return   eUserErrorCode - returns eSuccess on success,eReadPortFail - on failure.
 *  \ingroup  PortIO
 *  \see      WritePort(),PCIRead(),ReadMSR()
 */
eUserErrorCode	    ReadPort(UInt16	PortAddress,  DataElementPtr DataValue);

/*! \fn       eUserErrorCode		WritePort(UInt16	PortAddress, DataElement DataValue)
 *  \brief    to write data to IO port
 *  \param    [IN]    PortAddress     - port number/Address
 *  \param    [IN] \a DataValue     - data to write
 *  \return   eUserErrorCode - returns eSuccess on success,eWritePortFail - on failure.
 *  \ingroup  PortIO
 *  \see      ReadPort(), PCIWrite(),WriteMSR()
 */
eUserErrorCode		WritePort(UInt16	PortAddress, DataElementPtr DataValue);

/*! \fn       eUserErrorCode		ReadMSR(MSRInfoBlockPtr	    MsrDetails)
 *  \brief    read CPU MSR register
 *  \param    [IN,OUT] \a  MsrDetails - same structure will hold the read data.
 *  \return   eUserErrorCode - returns eSuccess on success,eReadMSRFail - on failure.
 *  \ingroup  MSROps
 *  \see      WriteMSR(), PCIRead(), ReadPort()
 */
eUserErrorCode		ReadMSR(MSRInfoBlockPtr  MsrDetails);

/*! \fn       eUserErrorCode	    WriteMSR(MSRInfoBlockPtr	MsrDetails)
 *  \brief    write data to CPU MSR register
 *  \param    [IN] \a  MsrDetails - same structure will hold the data to write
 *  \return   eUserErrorCode - returns eSuccess on success,eWriteMSRFail - on failure.
 *  \ingroup  MSROps
 *  \see      ReadMSR(), PCIWrite(),WritePort()
 */
eUserErrorCode	    WriteMSR(MSRInfoBlockPtr MsrDetails);

/*! \fn       eUserErrorCode      ReadCPUID(eCPUNumber	theCpu , IA32RegBlockPtr	pIA32RegBlock)
 *  \brief    Excute the CPUID instruction
  *  \param   [IN]    theCpu     - CPU Number on which to execute
 *  \param    [IN,OUT] \a  pIA32RegBlock - Input outpur of 32 bit registers
 *  \return   eUserErrorCode - returns eSuccess on success,eWriteMSRFail - on failure.
 *  \ingroup  MSROps
 *  \see      ReadMSR(), PCIWrite(),WritePort()
 */
eUserErrorCode      ReadCPUID(eCPUNumber	theCpu , IA32RegBlockPtr	pIA32RegBlock);

/*! \fn       eUserErrorCode      GenerateSMI( UInt16   PortAddress, DataElementPtr DataValue ,  RegBlockPtr myRegBlockPtr);
 *  \brief    Generated a SMI 
 *  \param    [IN]    PortAddress     - port number/Address
 *  \param    [IN] \a DataValue     - data to write
 *  \param    [IN] \a myRegBlockPtr     - The register block to read/write registers
 *  \return   eUserErrorCode - returns eSuccess on success,eWriteMSRFail - on failure.
 *  \ingroup  PortIO
 *  \see      ReadMSR(), PCIWrite(),WritePort()
 */

eUserErrorCode      GenerateSMI( UInt16   PortAddress, DataElementPtr DataValue ,  RegBlockPtr myRegBlockPtr);

/*! \fn       eUserErrorCode      BindCPU(eCPUNumber	theCpu , IA32RegBlockPtr	pIA32RegBlock)
 *  \brief    Bind the the thread to a specific CPU
  *  \param   [IN]    theCpu     - CPU Number on which thread needs to be binded
 *  \return   eUserErrorCode - returns eSuccess on success,eWriteMSRFail - on failure.
 *  \ingroup  MSROps
 */
eUserErrorCode  BindCPU(eCPUNumber	theCpu);

/*! \fn       eUserErrorCode      UnBindCPU(eCPUNumber	theCpu , IA32RegBlockPtr	pIA32RegBlock)
 *  \brief    UnBind the the thread to a specific CPU
 *  \param    [IN]    theCpu     - CPU Number on which thread needs to be unbinded
 *  \return   eUserErrorCode - returns eSuccess on success,eWriteMSRFail - on failure.
 *  \ingroup  MSROps
 *  \see      ReadMSR(), PCIWrite(),WritePort()
 */
eUserErrorCode  UnBindCPU(void);

/*! \fn       eUserErrorCode CalcFreqNonTurbo(NonTurboProductDataPtr pNonTurboData)
 *  \brief    To Find Non Turbo Frequency
 *  \param    [IN] NonTurboProductDataPtr - Pointer to NonTurboData
 *  \return   eUserErrorCode - returns eNoError/eFailure
 */
eUserErrorCode CalcFreqNonTurbo(NonTurboProductDataPtr pNonTurboData);

/*! \fn       eUserErrorCode InvalidateWriteBack(eCPUNumber	theCpu)
 *  \brief    To write-back and invalidate cache
 *  \param    [IN]    theCpu     - CPU Number on which to write-back and invalidate the cache
 *  \return   eUserErrorCode - returns eNoError/eFailure
 */
eUserErrorCode InvalidateWriteBack(eCPUNumber theCpu);

/*! \fn       eUserErrorCode LockAndFlashKsc(KscCompletePackPtr pKscCompletePack, Int32Ptr pErrorCode)
 *  \brief    KSC flashing in elevated IRQL.
 *  \param    [IN]    pKscCompletePack     - KSC information required for flashing.
 *  \param    [OUT]   pErrorCode		   - What went wrong in KSC flashing.
 *  \return   eUserErrorCode - returns eNoError/eFailure
 */
eUserErrorCode LockAndFlashKsc(KscCompletePackPtr pKscCompletePack, Int32Ptr pErrorCode);

/*! \fn       eUserErrorCode GetACPITable(char* TableName, DataElementPtr *  DataValuePtrPtr  )
 *  \brief    Used to read ACPI Tables 
 *  \param    [IN]    TableName     -     ACPI Table Name to be read 
 *  \param    [OUT]   DataValuePtrPtr - Pointer to DataElementPtr -- API will populate this .. Responsbility of User to free memory 
 *  \return   eUserErrorCode - returns eNoError/eFailure
 */
eUserErrorCode GetACPITable(char* TableName, DataElementPtr *  DataValuePtrPtr  );

/*! \fn       eUserErrorCode  GetPCIExCfgBaseAddr(UInt64 *pBarAddress)
*  \brief    Used to read PCI Extended Config Space Base Address
*  \param    [IN OUT]    pBarAddress     -  Pointer to PCI Extended Config Space Base Address value
*  \param    [IN]	segmentNumber: the segment number in the ACPI MCFG table that corresponds to a PCI Host Controller whose memory mapped area's base address is to be read (valid values 0 to MAX_SEG_COUNT).
*  \return   eUserErrorCode - returns eNoError/eFailure
*/
eUserErrorCode GetPCIExCfgBaseAddr(UInt64* pBarAddress, Int32 segmentNumber);

/*! \fn       eUserErrorCode  GetSegBusInfo(SegBusInfo(*segBusInfo)[MAX_SEG_COUNT])
*  \brief    Used to read PCI Extended Config Space Segment and Bus information from ACPI table MCFG
*  \param    [IN OUT]    segBusInfo  -  Pointer to an array of SEG_BUS_INFO with MAX_SEG_COUNT elements to be passed in and the function will copy Segment/Bus information in to it. 
*  \return   eUserErrorCode - returns eNoError/eFailure
*/
eUserErrorCode GetSegBusInfo(SegBusInfo (*segBusInfo)[MAX_SEG_COUNT]);

#ifdef UEFI
eUserErrorCode TopologyProcessorInfo();
eUserErrorCode GetMaxCPUSupported(Int32Ptr);
eUserErrorCode EfiGetActiveCore(UInt32 * CurrentCore );
#endif

#ifdef __cplusplus
}
#endif

#endif //_HWAPI_LIB_H

	








 
