#ifndef _OSABSTRACTION_H_
#define _OSABSTRACTION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PACKAGE_SPECS
#define PACKAGE_SPECS "PackageSpecsExt.h"
#endif
#include PACKAGE_SPECS
#include BASICTYPES
#include ERRORLIB
#include BASICSTRUCTS

typedef struct _os_time
{
	UInt8 tm_sec;
	UInt8 tm_min;
	UInt8 tm_hour;
	UInt8 tm_month;
	UInt16 tm_year;
	UInt8 tm_date;
}OSTime_t, *OSTimePtr_t;

typedef enum
{
	eUserRead = 00400,
	eUserWrite = 00200,
	eGroupRead = 00040,
	eGroupWrite = 00020,
	eOthersRead = 00004,
	eOthersWrite = 00002,
} ePermissionMode;

#if defined(__unix__) || defined(APPLE) || defined(__APPLE__)
#include <sys/time.h>

#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>

#ifndef ANDROID
#include <spawn.h>
#endif

#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>


#ifdef ANDROID
#include <linux/sched.h>
#include <linux/kernel.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#endif


eUserErrorCode  LinuxCheckMultiInstance(char * name);
void LinuxCheckMultiInstanceClose(char * name);


typedef UInt32      osWinthread_t;
typedef pthread_t   osThread_t;
typedef void*       ososExitCode;
typedef pid_t        osProcessHdl;


#if defined(APPLE) || defined (__APPLE__)
#ifdef MACUSEBSDSIDE
typedef int os_handle;
#else
typedef mach_port_t os_handle;
#endif
#else
typedef int         os_handle;
#endif   // 44- defined(APPLE) || defined (__APPLE__)

typedef void* threadStatus_t;
typedef void* threadArg_t;
#define OS_THRD_EXIT_STATE_CANCELLATION									PTHREAD_CANCELED
#define ospthread_create(thread,attr,start_routine,arg)                pthread_create(thread,attr,start_routine,arg)  
#define ospthread_cancel(thread)                                       pthread_cancel(thread)
#define ospthread_exit(osExitCode)                                     pthread_exit(osExitCode)
#define ospthread_join(thread, osExitCodePtr)                          pthread_join(thread, osExitCodePtr)
#define ospthread_self()	                                           pthread_self()
#define ospthread_waitForThread(threadhandle)                          pthread_join(threadhandle, NULL)
#define os_ChangePermissions(Path,Mode)								   chmod(Path, Mode);
#define os_sync(FilePtr)											   fflush((FILE *)(FilePtr))
#define os_GlobalSync(FilePtr)                                                sync() 


/*  Affinity Abstractions Linux*/
#if defined(APPLE) || defined (__APPLE__)
// This Stuff still needs out to be figured for MAC
#define osGetMaxCPUSupportedOnOS(myCpuCountPtr)                       {int temp = sizeof(int); sysctlbyname("hw.logicalcpu", myCpuCountPtr, &temp , NULL, 0);}1;
typedef Int32   osAffinityMask_t;                              
#define ospthread_setAffinity_Server(theCPU)                           BindCPU(theCPU)
#define ospthread_setAffinity(thread, AffinityMaskPtr)                 { if(*(AffinityMaskPtr) != -1) BindCPU(*(AffinityMaskPtr)); else UnBindCPU(); }
#define ospthread_ClearAffinityMask(AffinityMaskPtr)                   *(AffinityMaskPtr) = -1
#define ospthread_SetAffinityMask(theCPU,AffinityMaskPtr)               *(AffinityMaskPtr) = (theCPU)
#define ospthread_getaffinity(Thread, AffinityMaskPtr)                  *(AffinityMaskPtr) = -1
#else
#if defined (ANDROID)
typedef cpu_set_t   osAffinityMask_t;
#define ospthread_setAffinity(thread, AffinityMaskPtr)                 sched_setaffinity( __pthread_gettid(thread), sizeof(osAffinityMask_t), (AffinityMaskPtr))
#define ospthread_ClearAffinityMask(AffinityMaskPtr)                   CPU_ZERO(AffinityMaskPtr);
#define ospthread_SetAffinityMask(theCPU,AffinityMaskPtr)              CPU_SET(theCPU, (AffinityMaskPtr))
#define ospthread_getaffinity(Thread, AffinityMaskPtr)                 sched_getaffinity( __pthread_gettid(Thread), sizeof(osAffinityMask_t), (AffinityMaskPtr))  
#define osGetMaxCPUSupportedOnOS(myCpuCountPtr)                        *(myCpuCountPtr) = sysconf(_SC_NPROCESSORS_CONF);    
#define ospthread_setAffinity_Server(theCPU)                           LinuxSetAffinityHigherCore(theCPU)                     
#else
typedef cpu_set_t   osAffinityMask_t;
#define ospthread_setAffinity(thread, AffinityMaskPtr)                 pthread_setaffinity_np((thread), sizeof(osAffinityMask_t), (AffinityMaskPtr))
#define ospthread_ClearAffinityMask(AffinityMaskPtr)                   CPU_ZERO(AffinityMaskPtr);
#define ospthread_SetAffinityMask(theCPU,AffinityMaskPtr)              CPU_SET(theCPU, (AffinityMaskPtr))
#define ospthread_getaffinity(Thread, AffinityMaskPtr)                 pthread_getaffinity_np((Thread), sizeof(osAffinityMask_t), (AffinityMaskPtr))  
#define osGetMaxCPUSupportedOnOS(myCpuCountPtr)                        *(myCpuCountPtr) = sysconf(_SC_NPROCESSORS_CONF);    
#define ospthread_setAffinity_Server(theCPU)                           LinuxSetAffinityHigherCore(theCPU)                     
#endif   //Android
#endif   // 57 - defined(APPLE) || defined (__APPLE__)

 /*! Memory Routine*/
#define os_memcpy(dest,src,len) 	memcpy((dest),(src),(len))
#define os_memset(ptr,value,size)	memset((ptr),(value),(size))
#define os_malloc(size)				malloc((size))
#define os_memcmp(dest,src,len) 	memcmp((dest),(src),(len))

/*! IOCTL Routine
*/
#define os_ioctl(desc,opcode,inparam,insize,outparam, outsize, byteret, flag,Scalar_struct)\
	ioctl((desc),(opcode),(inparam)) 

#if defined (__APPLE__) || defined(APPLE)
#ifdef os_ioctl
#undef os_ioctl
#endif

#ifdef MACUSEBSDSIDE

#define os_ioctl(desc,opcode,inparam,insize,outparam, outsize, byteret, flag,Scalar_struct)\
ret = ioctl((desc),(opcode),&(inparam));

#else // MACUSEBSDSIDE
#define os_ioctl(desc,opcode,inparam,insize,outparam, outsize, byteret, flag,Scalar_struct)\
*(byteret) = outsize;\
ret = IOConnectCallStructMethod(desc,opcode,inparam,insize,outparam,byteret);

#endif
#endif

/*! Error code Routine
*/


#define ospthread_setschedparam(theThread, policy, paramPtr)           pthread_setschedparam(theThread, policy, paramPtr)
#define ospthread_getschedparam(theThread, PolicyPtr, ParamPtr)        pthread_getschedparam(theThread,PolicyPtr, ParamPtr)  

//mutex
typedef pthread_mutex_t osMutex_t;
#define ospthread_mutex_init(mutexPtr,attribPtr,NAME)                 pthread_mutex_init(mutexPtr,attribPtr)
#define ospthread_mutex_lock(mutexPtr)                                pthread_mutex_lock(mutexPtr)
#define ospthread_mutex_unlock(mutexPtr)                              pthread_mutex_unlock(mutexPtr)  
#define ospthread_mutex_exit(mutexPtr)                                pthread_mutex_destroy(mutexPtr)

// Linux 
//Create a folder AppNAME in folder /usr/bin for multi instance to work properly on Linux and MAC
#define os_check_multi_instance(mutexPtr,attribPtr,AppNAME)              LinuxCheckMultiInstance(AppNAME)  
#define os_clear_multi_instance(mutexPtr,AppNAME)                        LinuxCheckMultiInstanceClose(AppNAME)

//Process
#ifndef ANDROID
typedef posix_spawnattr_t   osAttribute_t;
#endif
typedef pid_t               osHandle;
typedef char *              osPathPtr;
typedef char **             osArg;
typedef char **             osEnvArguments;


/************************************************************************************************
******************     DYNAMIC LIBRARY **********************************************************
*************************************************************************************************/

typedef void *   osLibHdl;
typedef void *   osLibProcAddr;

#define  OS_DYNLIB_OPEN( myDynLibName,myLibHdl)                  	       myLibHdl = dlopen(myDynLibName, RTLD_NOW | RTLD_LOCAL);
#define  OS_DYNLIB_GET_SYMBOL( myLibHdl, myFuncSymName , myLibProcAddr)    myLibProcAddr = dlsym(myLibHdl, myFuncSymName);                     
#define  OS_DYNLIB_CLOSE(myLibHdl)                                         dlclose(myLibHdl);

//Timing
#define  osmsleep(MilliSec)                                           usleep(MilliSec * 1000)

// Get Uid 
#define os_check_root_priv()                                          (( 0 == geteuid())? true:false)
#define os_getchar()												  getchar()



#undef os_errno
//#define os_errno() 		errno()
#define os_errno() (-errno)

/**************************************************************************************************
******** LOAD/UNLOAD Drivers from code ************************************************************
Note -: Presently to be used only for HWAPI as windows HWAPI driver start code is embedded in HWAPI
***************************************************************************************************/
#ifdef __unix__
#define LOAD_DRIVER(myDriverExecutableName, myDriverName, myDriverFileName)  if(system("cat /proc/modules | grep " myDriverName " > /dev/NULL") != 0 )\
                                                                             {\
                                                                                system("insmod " myDriverExecutableName ".ko" );\
                                                                                system("chmod 777 " myDriverFileName);\
                                                                             }1;
#define UNLOAD_DRIVER(myDriverExecutableName,myDriverName)                   if(system("cat /proc/modules | grep " myDriverName " > /dev/NULL") == 0 )\
                                                                                system("rmmod " myDriverExecutableName );
#endif

#if defined (__APPLE__) || defined(APPLE)
#define LOAD_DRIVER(myDriverExecutableName, myDriverName , myDriverFileName)    if( system("kextstat -l | grep  " myDriverName  "  > /dev/null")){\
																						system("chown -Rf root:wheel "myDriverExecutableName ".kext");\
																						system("kextutil " myDriverExecutableName ".kext");\
																						system ("chmod 777 " myDriverFileName);\
																				}1;
#define UNLOAD_DRIVER(myDriverExecutableName,myDriverName )                      if(!( system("kextstat -l | grep  " myDriverName  "  > /dev/null"))){\
																						system("kextunload " myDriverExecutableName ".kext");
#endif

#endif  //  defined(__unix__) || defined(APPLE) || defined(__APPLE__)




#ifdef _WIN32_
#include <windows.h>
#include <winioctl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <Shlobj.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef HANDLE  osWinthread_t;
typedef void*   osThread_t;
typedef HANDLE	os_handle;

typedef PROCESS_INFORMATION   osProcessHdl;
typedef DWORD threadStatus_t;
typedef LPVOID threadArg_t;

eUserErrorCode WinSetAffinityHigherCore(UInt32 myCore);
typedef struct
{
    UInt32 sched_priority;
}sched_param, *sched_paramPtr;

#define OS_THRD_EXIT_STATE_CANCELLATION									0x5A
#define ospthread_create(threadPtr,attr,start_routine,arg)              *threadPtr = CreateThread( attr, 0,(LPTHREAD_START_ROUTINE)start_routine, arg, 0,0 )
#define ospthread_exit(osExitCode)                                       ExitThread((UInt32)osExitCode)
#define ospthread_cancel(thread)                                        TerminateThread(thread,OS_THRD_EXIT_STATE_CANCELLATION)
#define ospthread_join(threadhandle, osExitCodePtr)                     GetExitCodeThread(threadhandle, osExitCodePtr)
#define ospthread_waitForThread(threadhandle)                           WaitForSingleObject(threadhandle, INFINITE)
#define os_sync(FilePtr)												fflush((FILE *)(FilePtr))
#define os_GlobalSync(FilePtr)											1

#define ospthread_setschedparam(theThread, policy, priority)            SetPriority(priority->sched_priority)
#define ospthread_getschedparam(theThread, PolicyPtr, priority)         GetPriority(&(priority->sched_priority))   
#define ospthread_self()                                                GetCurrentThread()
#define osGetMaxCPUSupportedOnOS(myCpuCountPtr)            				 *(myCpuCountPtr) = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);

/*  Affinity Abstractions Windows*/
typedef UInt32  osAffinityMask_t;
#define ospthread_ClearAffinityMask(AffinityMaskPtr)                    (*AffinityMaskPtr = 0);
#define ospthread_SetAffinityMask(theCPU,AffinityMaskPtr)               *(AffinityMaskPtr) = 1 << (theCPU)
#define ospthread_setAffinity(thread, AffinityMaskPtr)                  SetThreadAffinityMask(thread, *AffinityMaskPtr)
#define ospthread_getaffinity(thread, AffinityMaskPtr)              	*AffinityMaskPtr = 1; *AffinityMaskPtr = SetThreadAffinityMask(thread,(DWORD_PTR) AffinityMaskPtr) ;   SetThreadAffinityMask(thread,(DWORD_PTR) AffinityMaskPtr);                    

#define ospthread_setAffinity_Server(theCPU)                            WinSetAffinityHigherCore(theCPU)
  
#define os_ChangePermissions(Path,Mode)									_chmod(Path,(( ((Mode/64) == 2) || ((Mode/64) == 6) || (((Mode%64)/8) == 2) || (((Mode%64)/8) == 6) || (Mode%8 == 2) || (Mode%8 == 6)) ? _S_IWRITE : _S_IREAD));
/*! Memory Routine 
*/
#define os_memcmp(dest,src,len) 	memcmp((dest),(src),(len))
#define os_memcpy(dest,src,len) 	memcpy((dest),(src),(len))
#define os_memset(ptr,value,size)	memset((ptr),(value),(size))
#define os_malloc(size)				malloc((size))


/*! IOCTL Routine
*/
#define os_ioctl(desc,opcode,inparam,insize,outparam, outsize, byteret, flag,Scalar_struct)\
	DeviceIoControl((HANDLE)(desc),(DWORD)(opcode),(LPVOID)(inparam),(DWORD)(insize),(LPVOID)(outparam),(DWORD)(outsize),(LPDWORD) (byteret),(LPOVERLAPPED) (flag)) 
		
/*! Error code Routine
*/
#define os_errno() GetLastError()

//mutex
typedef   HANDLE  osMutex_t;
#define ospthread_mutex_init(mutexPtr,attribPtr,NAME)                   ((*mutexPtr = CreateMutex(NULL,FALSE,(LPCTSTR)(NAME))) == NULL)? eFailure:eNoError
#define ospthread_mutex_lock(mutexPtr)                                  WaitForSingleObject(*mutexPtr, INFINITE)
#define ospthread_mutex_unlock(mutexPtr)                                ReleaseMutex(*mutexPtr)
#define ospthread_mutex_exit(mutexPtr)  								CloseHandle(*mutexPtr)



#define os_check_multi_instance(mutexPtr,attribPtr,AppNAME)              ((*mutexPtr = CreateMutex(NULL,FALSE,(LPCTSTR)(AppNAME))) == NULL)? eFailure : ( (os_errno() == eAlreadyExsists)? eFailure:eNoError)  ;
#define os_clear_multi_instance(mutexPtr,AppNAME)                       CloseHandle(*mutexPtr)          

//Process
typedef LPPROCESS_INFORMATION   osHandle;
typedef LPSTARTUPINFO           osAttribute_t;
typedef LPCTSTR                 osPathPtr;
typedef LPCTSTR                 osArg;
typedef LPCTSTR                 osEnvArguments;  

/************************************************************************************************
******************     DYNAMIC LIBRARY **********************************************************
*************************************************************************************************/

typedef  HMODULE    osLibHdl;
typedef  FARPROC    osLibProcAddr;

#define  OS_DYNLIB_OPEN( myDynLibName,myLibHdl)                  	       myLibHdl = LoadLibrary(myDynLibName);
#define  OS_DYNLIB_GET_SYMBOL( myLibHdl, myFuncSymName , myLibProcAddr)    myLibProcAddr = GetProcAddress(myLibHdl, myFuncSymName);
#define  OS_DYNLIB_CLOSE(myLibHdl)                                         FreeLibrary(myLibHdl);

//Timing
#define  osmsleep(MilliSec)                                           Sleep(MilliSec)


#define os_check_root_priv()                                          (IsUserAnAdmin() ? true : false)
#define os_getchar()												  getchar()   
        
/**************************************************************************************************
******** LOAD/UNLOAD Drivers from code ************************************************************
Note -: Presently to be used only for HWAPI as windows HWAPI driver start code is embedded in HWAPI
***************************************************************************************************/
#define LOAD_DRIVER(myDriverExecutableName, myDriverName , myDriverFileName)               1;
#define UNLOAD_DRIVER(myDriverExecutableName)                                              1;
		
#endif

//##########################    UEFI Support ####################################
#ifdef UEFI
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <uefi.h>

typedef void *   	osThread_t;
typedef void*       ososExitCode;
typedef UInt32      osProcessHdl;
typedef UInt32      os_handle;
typedef UInt32      osHandle;

typedef char **             osArg;
typedef char **             osEnvArguments;
typedef char *              osPathPtr;
typedef char*    osAttribute_t;

typedef void* threadStatus_t;
typedef void* threadArg_t;


extern eUserErrorCode EFIIoctl(os_handle fd, UInt8 x1, void * param, size_t temp, void * ver, size_t s, UInt64Ptr x, void * y, UInt8 temp1 );
extern eUserErrorCode EfiGetMaxSupportedonOS(Int32Ptr MaxSupportedCPUPtr);
extern eUserErrorCode EfiBindCPU(eCPUNumber theCPU);
extern eUserErrorCode EfiUnBindCPU();
extern eUserErrorCode EFIMillSecSleep(UInt32 MilliSeconds);
extern UInt8 EFIGetKeyStroke();


#define OS_THRD_EXIT_STATE_CANCELLATION									1
#define ospthread_create(thread,attr,start_routine,arg)                eNotSupported  
#define ospthread_cancel(thread)                                       eNotSupported
#define ospthread_exit(osExitCode)                                     eNotSupported
#define ospthread_join(thread, osExitCodePtr)                          eNotSupported
#define ospthread_self()	                                           (void*) 1
#define ospthread_waitForThread(threadhandle)                          eNotSupported
#define os_ChangePermissions(Path,Mode)								   eNotSupported

typedef Int32   osAffinityMask_t;   

extern eUserErrorCode EfiSetAffinity(osAffinityMask_t * );
extern eUserErrorCode EfiGetCurrentBSP(osAffinityMask_t * );

#define osGetMaxCPUSupportedOnOS(myCpuCountPtr)                         EfiGetMaxSupportedonOS((myCpuCountPtr))                         
#define ospthread_setAffinity_Server(theCPU)                            EfiBindCPU(theCPU)
#define ospthread_setAffinity(thread, AffinityMaskPtr)                  EfiSetAffinity((AffinityMaskPtr))
#define ospthread_ClearAffinityMask(AffinityMaskPtr)                    (*AffinityMaskPtr = 0);
#define ospthread_SetAffinityMask(theCPU,AffinityMaskPtr)               *(AffinityMaskPtr) = 1 << (theCPU)
#define ospthread_getaffinity(Thread, AffinityMaskPtr)                  EfiGetCurrentBSP((AffinityMaskPtr))
#define ospthread_setschedparam(theThread, policy, paramPtr)           	eNotSupported
#define ospthread_getschedparam(theThread, PolicyPtr, ParamPtr)         eNotSupported 

//mutex
/// As Multithreading is not supported Mutex within application can be just a simple variable
typedef Int32 osMutex_t;
#define ospthread_mutex_init(mutexPtr,attribPtr,NAME)                 *(mutexPtr) = 0
#define ospthread_mutex_lock(mutexPtr)                                *(mutexPtr) = 1
#define ospthread_mutex_unlock(mutexPtr)                              *(mutexPtr) = 1  
#define ospthread_mutex_exit(mutexPtr)                                 eNoError

#define os_sync(FilePtr)											   fflush((FILE *)(FilePtr))
#define os_GlobalSync(FilePtr)                                         1

#define os_malloc(size)				malloc(size)
//#define os_strcpy(destination,  source)     strcpy((destination) , (source))
#define os_memset(ptr,value,size)	memset((ptr),(value),(size))
#define os_memcpy(dest,src,len) 	memcpy((dest),(src),(len))
#define os_ioctl(desc,opcode,inparam,insize,outparam, outsize, byteret, flag,Scalar_struct) EFIIoctl(desc,opcode,inparam,insize,outparam, outsize, byteret, flag,Scalar_struct)
//								EFIIoctl()

#define os_check_multi_instance(mutexPtr,attribPtr,AppNAME)               eNoError
#define os_clear_multi_instance(mutexPtr,AppNAME)                         eNoError    
#define os_errno() 														  (EFIerrno)


typedef  Int32    osLibHdl;
typedef  UINTN    osLibProcAddr;

#define  OS_DYNLIB_OPEN( myDynLibName,myLibHdl)                  	       eNotSupported;
#define  OS_DYNLIB_GET_SYMBOL( myLibHdl, myFuncSymName , myLibProcAddr)    eNotSupported;
#define  OS_DYNLIB_CLOSE(myLibHdl)                                         eNotSupproted;

#define  osmsleep(MilliSec)                                           EFIMillSecSleep(MilliSec)
#define os_check_root_priv()                                          true
#define os_getchar()												  EFIGetKeyStroke()   




#endif /// UEFI

eUserErrorCode osposix_spawn(osHandle osHandle_t, osPathPtr osPathPtr_t,osArg osArg_t , osEnvArguments osEnvArguments_t );
eUserErrorCode osKill(osHandle osHandle_t, UInt32 * uExitCode);
eUserErrorCode osGetCurrentTimeInMilliSec(UInt64 *TimeInMilliSecPtr);


/**
 * @defgroup OSABS OS Abstraction Layer
 * Features to provide OS-level functionality without calling OS directly
 */


typedef void* os_PFILE;

#ifndef NULL
#define NULL 0
#endif

#define os_stdout stdout
#define os_stdin stdin
#define os_stderr stderr

//#define UInt64 unsigned long long BASICTYPES -> UInt64
#include BASICTYPES

/**  Memory Free Method
 * @ingroup OSABS
 * Deallocates the space previously allocated by os_malloc or os_realloc
 * @param ptr pointer to the memory to deallocate
 * @see os_malloc
 * @see os_realloc
*/
void os_free( void* ptr );

/**  Memory Realloc Method
 * @ingroup OSABS
 * Reallocates the given area of memory. The contents of the area remain unchanged up\n
 * to the lesser of the new and old sizes. If the area is expanded, the contents of the\n
 * new part of the array are undefined.
 * @param ptr pointer to the memory area to be reallocated. It must be previously returned from os_malloc or otherwise the results are undefined.
 * @param size the total size desired for the new allocation of memory
 * @return void * to newly allocated memory. The pointer must be deallocated with os_free
 * @return NULL if there is insufficient memory or an error has occurred.
 * @see os_malloc
 * @see os_free
*/
void * os_realloc( void *ptr, UInt64 size );

unsigned int os_strlen(  const char * str );

char * os_strcpy ( char * destination, const char * source );

char * os_strcat ( char * destination, const char * source );

char * os_strncpy ( char * destination, const char * source, unsigned int num );

int os_strcmp ( const char * str1, const char * str2 );

UInt8Ptr os_strchr(const UInt8Ptr str, UInt8 character);

UInt8Ptr os_strrchr(const UInt8Ptr str, UInt8 character);

int os_snprintf(char *str, unsigned int size, const char *format, ...);

int os_vsnprintf(char *str, unsigned int size, const char *format, void* pVaList);

int os_printf (const char * format, ... );

int os_vprintf(const char * format, void* pVaList);

int os_fprintf(os_PFILE stream, const char * format, ... );

UInt64 os_fwrite(const void * ptr, UInt64 size, UInt64 count, os_PFILE stream);

UInt64 os_fread(void * ptr, UInt64 size, UInt64 count, os_PFILE stream);

int os_vfprintf(os_PFILE stream, const char * format, void* pVaList);

os_PFILE os_fopen(const char * filename, const char * mode);

int  os_fflush(os_PFILE stream);

int os_fclose(os_PFILE stream);

int os_setvbuf (os_PFILE stream, char* buffer, int mode, UInt64 size);

void _os_PrintAllocationList();

int os_fgetc(os_PFILE stream);

char * os_fgets(char * str, int num, os_PFILE stream);

int os_fputs (const char * str, os_PFILE stream);

int os_fseek(os_PFILE stream, Int64 offset, int origin);

UInt64 os_ftell(os_PFILE stream);

char * os_strtok(char * str, const char * delimiters);

/////////////////////
//PState Interfaces//
/////////////////////

eUserErrorCode    osInitializePowerScheme(void);

eUserErrorCode    osRestorePowerScheme(void);

eUserErrorCode    osGetPStateTable(PPROCESSOR_STATE PStateInformation,UInt32 *NumPStates);
eUserErrorCode    osSetPState(PPROCESSOR_STATE myProcStatePtr);

eUserErrorCode    osGetCurrentPState(UInt32Ptr CurrentPState, UInt32 NumPStates,PPROCESSOR_STATE PStateInformation);

/*Process handling*/
eUserErrorCode osSpawnProcess(const Int8 *appName, Int8 *const argv[],Int8 *const envp[], osProcessHdl *hdl);
eUserErrorCode execProcess(const Int8 *fileName, Int8 *const argv[]);
eUserErrorCode osKillProcess(osProcessHdl myProcHdl);
eUserErrorCode osWaitForProcessExit(osProcessHdl myProcHdl);

/*OS Time*/
eUserErrorCode osGetTimeStruct(OSTimePtr_t myOSTimePtr);
eUserErrorCode os_GetDateTime(Int8 **dateTime);

eUserErrorCode os_GetFileNames(Int8 *directoryName, Int8 ***fileList, UInt32 *count);

eUserErrorCode os_FreeFileNames(Int8 ***fileList, UInt32 count);

/*OS Info*/
eUserErrorCode osGetOSVersion(Int8 **osVersion);
eUserErrorCode osGetSystemName(Int8 **systemName);
eUserErrorCode OSGetMemoryLimit(UInt64 *TotalPhysicalMem);

#ifdef __cplusplus
}
#endif

#endif

 
