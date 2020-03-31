#ifndef __MCU_H__
#define __MCU_H__

#include "BaseThread.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include "framework.h"
#include "VIF.h"


//add chen
#include "FAWACP.h"
#include "RingBuffer.h"
#include "WDSControl.h"    //uint8
#include "SMSControl.h"    //uint16

#ifndef MCU_DEBUG
	#define MCU_DEBUG 0
#endif

#if MCU_DEBUG 	
	#define MCULOG(format,...) printf("== MCU == FILE: %s, FUN: %s, LINE: %d "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define MCU_NO(format,...) printf(format,##__VA_ARGS__)
#else	
	#define MCULOG(format,...)
	#define MCU_NO(format,...)
#endif


//#ifndef MCU_UART_DEVICE
#define MCU_UART_DEVICE	        (char*)"/dev/ttyHS1"    /* for mcu uart */
#define MCU_UART_SPEED	        	460800
#define MCU_UART_STOP_BITS       	1
#define MCU_UART_DATA_BITS      	8
#define MCU_UART_CHECK_BIT      	'n'
#define MCU_UART_TIMEOUT_MSECONDS   50
//#endif

#ifndef PLC_UPGRADE_FILE
#define PLC_UPGRADE_FILE        (char*)"/data/plc.bin"
#endif

#ifndef MCU_UPGRADE_FILE
#define MCU_UPGRADE_FILE        (char*)"/data/MCU.bin"
#endif

#define BUFF_LEN                   			4096
#define UART_SNED_TO_MCU_LEN	   		    640
#define MCU_CMD_MAX_COUNT					10

#define  SYSTEM_BATTRY_STATUS_NORMAL        0
#define  SYSTEM_BATTRY_STATUS_LOW        	1
#define  SYSTEM_BATTRY_STATUS_HIGH       	2


//**************************************************************************
//**************************************************************************
//****************delete****************************************************
#define  MCU_4G_SYNC_SHAKEHAND_FAIL  		0
#define  MCU_4G_SYNC_SHAKEHAND_OK   		1

#define  MCU_GPS_STATUS_NO_ERR    			0
#define  MCU_GPS_STATUS_ERR					1
#define  MCU_GPS_STATUS_RECOVERY			2

#define  MCU_WAIT_UPGRADE_MAX_COUNT		    20

#define MCU_Factory_Test    	0x82 
#define TBOX_REQ_CFGINFO    	0x92
#define	TBOX_QUERY_PARA_CMD		0x83


/* mcu upgrade tbox*/
#define MCU_SND_UPGRADE_INFO    0x0D
#define TBOX_REPLY_ID           0x8E
#define MCU_SND_UPGRADE_DATA    0x0E
#define MCU_SND_UPGRADE_CMPL    0x08     //COMPLETE
#define TBox_CHECK_REPORT	    0xA0
#define MCU_CHECK_REPORT	    0x20

#define TBOX_SEND_UPGRADE_CMD   0x8a
#define TBOX_RECV_MCU_APPLY_FOR 0x86
#define MCU_SEND_COMPLETE       0x0A
#define MCU_REPORT_DRIVERDATA	0x07	//从机驾驶行为数据上报


typedef enum MCU_ACC_Status_TAG{
  STATUS_ACC_OFF = 0x00, 
  STATUS_ACC_ON,  
  STATUS_ACC_ALL
}MCU_ACC_Status_ENUM;

typedef enum MCU_WAKEUP_STATE_TAG{
  STATUS_WAKEUP_INIT = 0x00, 
  STATUS_WAKEUP_OPENUART,  
  STATUS_WAKEUP_SYNC,
  STATUS_WAKEUP_COMPLETE,
  STATUS_WAKEUP_ALL
}MCU_WAKEUP_STATE_ENUM;

typedef enum MCU_WAKEUP_EVENT_TAG{
  STATUS_MCU_NORMAL = 0x00, 
  STATUS_WAKEUP_EVENT,
  STATUS_SLEEP_EVENT,
  STATUS_MCU_ALL
}MCU_WAKEUP_EVENT_ENUM;

typedef enum MCU_UPGRADE_STATE_TAG{
  MCU_UPGRADE_STATE_IDLE = 0x00, 
  MCU_UPGRADE_STATE_SNED_UPGREADE_INFO,
  MCU_UPGRADE_STATE_WAIT_MCU_REQ,
  MCU_UPGRADE_STATE_SEND_UPGRADE_DATA,    
  MCU_UPGRADE_STATE_SEND_UPGRADE_ERR,  
  MCU_UPGRADE_STATE_FINISH,
  MCU_UPGRADE_STATE_IDLE_ALL
}MCU_UPGRADE_STATE_ENUM;

typedef struct
{
	uint16_t MainPower;
	uint8_t DeputyPower;
	uint8_t CANStatus;
	uint8_t GyroscopeData[12];
	uint8_t GpsAntenna;
	uint8_t GpsPoist;
	uint8_t GpsSatelliteNumber;
	uint16_t GpsSatelliteInter[40];	//Gps??n????????????	
	uint8_t  GpsNumber[40];	//Gpsnumber
	uint8_t  GpsStrength[40];	//Gpsstrength
	uint16_t GPStime;		//gps启动时间

}McuStatus_t;

typedef struct
{
	uint8_t Satelliate_ID;
	uint8_t Satelliate_RSSI;
}Satelliate_State_t;

typedef struct 
{
	uint8_t Antenna_State;
	uint8_t Satelliate_Number;
	Satelliate_State_t Satelliate[255];
}GPS_Info_t;
//**************************************************************************
//**************************************************************************
//**************************************************************************


class CMcu : public TiotThread
{
public:
    static CMcu *GetInstance();
    static void  FreeInstance();
    static void *Mcu_RevQueue_Process(void *arg);
    CMcu();
    virtual ~CMcu();
    BOOL Init();
	BOOL GetInitSts();
    void Deinit();
    void Run();
	void close_uart();
	int setSystemTime(struct tm *pTm);
	uint32_t checkMcuUartData(unsigned char *pData, unsigned int size);
	unsigned int Crc16Check(unsigned char* pData, uint32_t len);
	
	//********************delete**********************
	void Set_Uart_Sync_Value(int value);
	int  get_Uart_Sync_Value();
	uint8 GetSystemBatteryStatus();
	void Set_mcuWakeupEventValue(int value); 
	int Get_mcuWakeupEventValue();						
	int packProtocolData(uint8_t responeCmd, uint8_t subCmd, uint8_t *needToPackData, uint16_t len, uint16_t serialNum); 
	int escape_mcuUart_data(unsigned char *pData, int len);
	int pack_mcuUart_upgrade_data(unsigned char cmd, bool isReceivedDataCorrect, int mcuOrPlcFlag);
	unsigned short int pack_upgrade_cmd(unsigned char *pData, int len, int mcuOrPlcFlag);
	unsigned short int pack_upgrade_data(unsigned char *pData, int len, bool isReceivedDataCorrect, int mcuOrPlcFlag);
	int SendPowerSwitchSame(void);
	static int   Mcu_RemoteCtrlRes();
	static int   Mcu_RemoteConfigRes(uint8_t SubitemCode, uint16_t SubitemVal);
	//************************************************
	
protected:
    static CMcu* m_instance;
    BOOL    m_binit;
    BOOL Processing();
    BOOL TimeoutProcessing();
     void SendQueueTestTask();
     void ReceiveQueueTestTask();
private:
    int m_FdUart;
    int rev_msg_len;
    pthread_t   Mcu_RevQueue_Id;
    VIF_VeichleInformation_UN 	g_stMcuVif;
	
    int UartInit();
    int SetUartParity(int fd, int databits, int stopbits, int parity);
    int SetUartSpeed(int fd, int speed);
    int mcu_ReceiveQueue(uint8_t Id);

    int mcu_Receive_FW_Sync(void);
	int mcu_Receive_FW_Veichle_Ack(void);
	int mcu_Receive_FW_Remote_Ctl(void);
    int mcu_Sync_Init(void);
    int mcu_Sync_Process_200ms(void);

public:
	int 			mcuWakeupevent;
	RingBuffer		m_RingBuff;
	Framework*      mFramework;

    int 			mcuUpgradeMode;//delete
    uint8_t		    AccStatus;//delete
    uint8_t 		RCtrlErrorCode;//delete
    uint8_t 		u8GpsErrFlag;//delete
    uint8_t			flagGetTboxCfgInfo;//delete
    uint8_t         u8RemoteControlFlag;//delete

    uint8_t 		mcuSyncTimeOutCnt;
    uint8_t 		mcuSyncFlag;
};

#endif
