#ifndef CONTAINERLIB_H
#define CONTAINERLIB_H

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif

#include PACKAGE_SPECS
#include BASICTYPES
#include OSINTERFACE
#include ERRORLIB
#include LIBDEBUG


#ifdef __cplusplus
extern "C" {
#endif

/*! 
 *  \file 	   ContainerLib.h
 *  \brief     API's for Container functionalities.
 *  \details   This provides Interface to access Container functionalities.
 *  \author    Intel Corporation.
 *  \version   1.0.0.1000
 *  \date      2011-2015
 *  \bug       TBD
 *  \copyright Copyright (c)  2012 Intel Corporation. All rights reserved
 
 */
 
  /*! \mainpage About ContainerLib
 *
 * \section   Container Library. This static library API's useful to create and use the container.
 *			
 *
 *  \warning   This library intended to be used with in the Organization.
 *			   Neither library nor documentation can be distributed outside the organization.
 */
 
  /*! \example containertestcase.c
 * This is an example of how to use the Container functions.
 */
  
 /** @defgroup Containerops Container General
 *  API's for Container Capability.
 */
 /*! \enum   ImplementationType
 *  \brief    enum for type of implementations 
 *  \ingroup  Containerops
 */
typedef enum ImplementationType
{
	LinkedList,
	SortedLinkedList,
	RBTree,
	AVLTree
	
}eImplementationType;

/*! \enum   CompareResult
 *  \brief    enum for comparison results 
 *  \ingroup  Containerops
 */
typedef enum CompareResult
{
	IsLessThan=-1,
	IsEqual=0,
	IsGreater=1
}eCompareResult;

/*! \enum   DataType
 *  \brief    enum for datatype 
 *  \ingroup  Containerops
 */
typedef enum DataType
{
	Integer,
	Character,
	Float,
	Custom
}eDataType;

/*! \enum   SearchType
 *  \brief    enum for searchtype 
 *  \ingroup  Containerops
 */
typedef enum SearchType
{
	Linear,
	CustomAlgo
}eSearchType;

typedef void * ContainerNode_t;

/*! \struct   ContainerHandle_t
 *  \brief    structure for ContainerHandle_t
 *  \ingroup  Containerops
 */
typedef struct ContainerHandle ContainerHandle_t,*ContainerHandlePtr_t;

typedef eCompareResult (*CompareFp)(ContainerNode_t,ContainerNode_t);

typedef Int32 (*SearchFp)(ContainerHandlePtr_t,ContainerNode_t);

/*! \struct   ContainerParameters_t
 *  \brief    structure for container parameters
 *  \ingroup  Containerops
 */
typedef struct ContainerParam
{
	Int8 ListName[128];
	eImplementationType ImplementationChoice;
	eDataType datatype;
	eSearchType SearchAlgo;
	CompareFp ComparisonFp;
	SearchFp Find;
}ContainerParameters_t, *ContainerParametersPtr_t;

/*! \fn       eUserErrorCode InitContainer(ContainerParametersPtr_t ContainerParams,ContainerHandlePtr_t *hdl)
 *  \brief    Initialize the handle. Should be called before using any other API.
 *  \param    [IN] \a ContainerParams- structure of all parameters required for initialization.
 *  \param    [OUT]  \a TheContainerHandlePtr - Pointer to the handle.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      UnInitContainer()
 */
eUserErrorCode InitContainer(ContainerParametersPtr_t ContainerParams,ContainerHandlePtr_t *hdl , UInt8 InitalizeDebug);


/*! \fn       eUserErrorCode InsertAtIndex(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t data, Int32 index);
 *  \brief    Insert generic data at any index. Index should be greater than or equal to 0.
 *  \param    [IN] \a TheContainerHandle-handle for which memory has been already allocated.
 *  \param    [IN] \a data - pointer to the data. Pointer should have memory allocated.
 *  \param    [IN] \a index - index at which to enter the data.Index starts from 0. 
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      RetrieveFromIndex(),DeleteAtIndex(),DeleteByData()
 */
eUserErrorCode InsertAtIndex(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t data, Int32 index);

/*! \fn       eUserErrorCode Insert(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t data)
 *  \brief    This function inserts data in sorted order. For unsorted linked-list, it inserts data at the end.
 *  \param    [IN] \a TheContainerHandle- handle for which memory has been already allocated.
 *  \param    [IN] \a data - pointer to the data. Pointer should have memory allocated.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      RetrieveFromIndex(),DeleteAtIndex(),DeleteByData()
 */
eUserErrorCode Insert(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t data);

/*! \fn       eUserErrorCode RetrieveFromIndex(ContainerHandlePtr_t TheContainerHandle_t,ContainerNode_t *data,Int32 index)
 *  \brief    Retrieve data from any index. Index should be greater than or equal to 0.
 *  \param    [IN] \a TheContainerHandle-handle for which memory has been already allocated.
 *  \param    [OUT] \a data - pointer to retrieved data.
 *  \param    [IN] \a index - index from which to retrieve data.Index starts from 0.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      InsertAtIndex()
 */
eUserErrorCode RetrieveFromIndex(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t *data,Int32 index);

/*! \fn       eUserErrorCode RetrieveFromEnd(ContainerHandlePtr_t TheContainerHandle_t,ContainerNode_t *data)
 *  \brief    Retrieve data from the end.
 *  \param    [IN] \a TheContainerHandle- handle for which memory has been already allocated.
 *  \param    [OUT] \a data - pointer to retrieved data.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      Push()
 */
eUserErrorCode RetrieveFromEnd(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t *data);

/*! \fn       eUserErrorCode DeleteAtIndex(ContainerHandlePtr_t TheContainerHandle_t,Int32 index)
 *  \brief    Delete data at any index. Index should be greater than or equal to 0.
 *  \param    [IN] \a TheContainerHandle-handle for which memory has been already allocated.
 *  \param    [IN] \a index - index at which to delete data.Index starts from 0. 
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      InsertAtIndex()
 */
eUserErrorCode DeleteAtIndex(ContainerHandlePtr_t TheContainerHandle,Int32 index);

/*! \fn       eUserErrorCode DeleteByData(ContainerHandlePtr_t TheContainerHandle_t,ContainerNode_t data)
 *  \brief    Delete a list node or tree node by the node data.
 *  \param    [IN] \a TheContainerHandle-handle for which memory has been already allocated.
 *  \param    [IN] \a data - pointer to node to be deleted.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      InsertAtIndex(),Insert()
 */
eUserErrorCode DeleteByData(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t data);

/*! \fn       eUserErrorCode Push(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t data)
 *  \brief    Push data to the list.
 *  \param    [IN] \a TheContainerHandle-handle for which memory has been already allocated.
 *  \param    [IN] \a data - pointer to the data. Pointer should have memory allocated.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      Pop()
 */
eUserErrorCode Push(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t data);

/*! \fn       eUserErrorCode Pop(ContainerHandlePtr_t TheContainerHandle_t,ContainerNode_t *data)
 *  \brief    Pop data from the list, and return the Popped data.
 *  \param    [IN] \a TheContainerHandle-handle for which memory has been already allocated.
 *  \param    [OUT] \a data - pointer to Popped data.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      Push()
  */
eUserErrorCode Pop(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t *data);

/*! \fn       eUserErrorCode Pop(ContainerHandlePtr_t TheContainerHandle_t,ContainerNode_t prevdata,ContainerNode_t newdata )
 *  \brief    Update a node with a given data
 *  \param    [IN] \a TheContainerHandle-handle for which memory has been already allocated.
 *  \param    [IN] \a prevdata - pointer to the data which is to be updated.
 *  \param    [IN] \a newdata - pointer to the new data.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 */
eUserErrorCode UpdateNode(ContainerHandlePtr_t TheContainerHandle,ContainerNode_t prevdata,ContainerNode_t newdata );

/*! \fn       eUserErrorCode UnInitContainer(ContainerHandlePtr_t containerHdl);
 *  \brief    Uninitialize handle-containerHdl. Should be called before using any other API.
 *  \param    [IN]  \a containerHdl-handle for which memory has been already allocated.
 *  \return   \a eUserErrorCode
 *  \ingroup  Containerops
 *  \see      InitContainer()
 */
eUserErrorCode UnInitContainer(ContainerHandlePtr_t containerHdl);

#ifdef __cplusplus
}
#endif

#endif