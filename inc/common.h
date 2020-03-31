#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
//#include "TBoxDataPool.h"

//版本由:项目名编码+平台号编码+GBT32960支持+ACP支持+文件版本+日期  eg. GV1350.7600.00.01.Ver1.0.00.20180430
#define PROJECT_NAME                  "GV1350"
#define PLATFORM_CODE                	"7600"
#define SUPPORT_GBT32960_VER   			"00"
#define SUPPORT_ACP_VER             	"01" 
#define FILE_VER_PREFIX               	"Ver"
#define FILE_VERSION                    "MPU_V1.1.02"

#define RET_OK       0
#define RET_FAIL     1
#define RET_INVALID  2

#define TRUE 1
#define FALSE 0



#ifndef DEBUG
	#define DEBUG 1
#endif

#if DEBUG
    #define DEBUGLOG(format,...) printf("### SYSLOG ### %s,%s[%d] "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define DEBUG_NO(format,...) printf(format,##__VA_ARGS__)
#else
	#define DEBUGLOG(format,...)
	#define DEBUG_NO(format,...)
#endif




#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE 	(1)
#endif

#ifndef FALSE 
#define FALSE 	(0)
#endif



#define SUCCESS  0
#define FAIL   (-1)


#define RET_OK       0
#define RET_FAIL     1
#define RET_INVALID  2



#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char     int8_t;
typedef short int       int16_t;
typedef int             int32_t;
# if __WORDSIZE == 64
typedef long int        int64_t;
# else
__extension__
typedef long long int   int64_t;
# endif
#endif

/* Unsigned. */
typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int        uint32_t;
# define __uint32_t_defined
#endif
#if __WORDSIZE == 64
typedef unsigned long int   uint64_t;
#else
__extension__
typedef unsigned long long int uint64_t;
#endif

//
#define TBOX_4G_VERSION				  "16031"
#define  REMOTE_UPDATA_OS_KEY_WORD      "ver031"

#define     QUEUE_TEST_MACRO                    0

//#define MQTT_OSID				1	//

#define RELEASE_NOTE             "TBOX_A_"
#define BUILD_NOTE               "_Build_"

#define SLEEP_MS(X)             usleep(1000*X)
#define SLEEP_S(X)               sleep(X)

#define NEW_UPGRADE_MCU_MODE	0

#define VIVIAN_ADD_MONITOR      0

#define MCU_SYNC_CNT   			100

extern const unsigned char sysVerNumber[3];
extern unsigned char mcuVerNumber[3];


/***************************************************************************
* Function Name: getSoftwareVerion 					
* Function Introduction: get system software version	
* Parameter description:                               
*     pBuff: passed an array address                   
*     size : array size                                
* Function return value: 0	:success   					
* Data : 2017.09.08									
****************************************************************************/
#define   MODEM_STATE_INIT   		 				0
#define   MODEM_STATE_CHECK_PS_CS    				1
#define   MODEM_STATE_DATA_CALL      				2
#define   MODEM_STATE_DATA_CALL_CHECK			3
#define   MODEM_STATE_IDLE_CHECK					4
#define   MODEM_STATE_ENABLE_CFUN					5
#define   MODEM_STATE_REBOOT						6

extern void Set_Reboot_Value(uint8_t value);

extern uint8_t Get_Reboot_Value();

extern int getSoftwareVerion(char *pBuff, unsigned int size);

extern int sys_mylog(char *plog);

extern int get_Read8090LiveStatus();

extern void SetGlDebugLog(unsigned char log);
extern unsigned char GetGlDebugLog(void);

extern char *GetFAWACPRecverrStr();

extern char *GetFAWACPSenderrStr();

extern char *GetFAWACPTimeout_eventStr();
	
extern char *GetFAWACPRemoteCtlStr();

extern uint8_t GetFAWACP_Recverrvalue();

extern uint8_t GetFAWACP_Senderrvalue();

extern uint8_t GetFAWACP_Timeouteventvalue();

extern uint8_t GetFAWACP_RemoteAwakevalue();

extern void SetFAWACP_Recverrvalue(uint8_t value);

extern void SetFAWACP_Senderrvalue(uint8_t value);

extern void SetFAWACP_Timeouteventvalue(uint8_t	value);

extern void SetFAWACP_RemoteAwakevalue(uint8_t	value);

extern void FAWACP_ERROR_Log(int status, int errcode,int times);

extern uint8_t GetLogFileUpFinish(void);

extern void SetLogFileFinishFlag(uint8_t value);

extern void tbox_log(int flag, uint8_t msgid);

#endif // _COMMON_H_

