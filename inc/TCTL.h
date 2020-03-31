#ifndef TCTL_H
#define TCTL_H

#include "common.h"

#define FW_TCTL_DEMO  0
#define TSP_TCTL_DEMO 0



#define TCTL_RemoteCtrl_DATA_LEN	512

#define TCTL_CarLock			1
#define TCTL_FindCar			2
#define TCTL_SetPower			3
#define TCTL_SetEngine			4
#define TCTL_SetSleepTime		5
#define TCTL_ImmediatelySleep	6
#define TCTL_SetSleepUpStsTime	7
#define TCTL_CheckBleSts		8
#define TCTL_SetBroad			9
#define TCTL_DownKey			10
#define TCTL_ConfigCarType		11
#define TCTL_SetAlarmVol		12
#define TCTL_ResetEC3CAN		13
#define TCTL_SetDeviceNum		14
#define TCTL_FactoryReset		15
#define TCTL_ImmediatelyRouse	16
#define TCTL_SetLOG				17
#define TCTL_UpLOG				18
#define TCTL_UpGPS				19
#define TCTL_ReturnCar			20
#define TCTL_SetBleTokenTime	21


typedef union AcpAppHeader_Tag
{
	uint8_t Data[5];
	struct
	{
		uint8_t 	Acp_AppID 		:6; 
		uint8_t 	PrivateFlag		:1;	
		uint8_t 	Reserved_ID	:1;	
		uint8_t 	MsgType 		:5; 
		uint8_t 	CryptFlag		:1;	
		uint8_t 	TestFlag		:1;	
		uint8_t 	Reserved_T 		:1;
		uint8_t 	MsgCtrlFlag 		:4; 
		uint8_t 	Version			:3;	
		uint8_t 	VersionFlag		:1;	
		uint16_t	MsgLength		:16;
	}BYTES;
}AcpAppHeader_UN;



#pragma pack(1)
typedef union TCTL_TimeStamp_Tag
{

		uint8_t Data[6];					
		struct															
		{  
			uint8_t Year		:6; 
			uint8_t Month		:4; //月
			uint8_t Day 		:5; //日
			uint8_t Hour		:5; //时
			uint8_t Minutes 	:6; //分
			uint8_t Seconds 	:6; //秒
			uint16_t msSeconds :16;//毫秒0-1000
	
								
		 }BYTES;


}TCTL_TimeStamp_UN;

typedef union TCTL_HeadTimeStamp_Tag
{
	uint8_t Data[8];					
	struct															
	{  
		uint8_t IEID		:2; 
		uint8_t MoreFlag	:1; 
		uint8_t Length		:5;
		uint8_t Year		:6; 
		uint8_t Month		:4; //月
		uint8_t Day 		:5; //日
		uint8_t Hour		:5; //时
		uint8_t Minutes 	:6; //分
		uint8_t Seconds 	:6; //秒
		uint16_t msSeconds :16;//毫秒0-1000

							
	 }BYTES;


}TCTL_HeadTimeStamp_UN;

typedef struct  TCTL_RemoteCtrlIndex_Tag
{
	uint8_t					CtrlTotal;
	uint8_t					CtrlCode[21];
}TCTL_RemoteCtrlIndex_ST;

/*pedef struct  TCTL_RemoteCtrlInfo_Tag
{
 uint8_t CarLockCode;
 uint8_t CarLockParam;
 uint8_t FindCarCode;
 uint8_t FindCarParam;
 uint8_t SetPowerCode;
 uint8_t SetPowerParam;
 uint8_t SetEngineCode;
 uint8_t SetEngineParam;
 uint8_t SetSleepTimeCode;
 uint8_t SetSleepTimeParam;
 uint8_t ImmediatelySleepCode;
 uint8_t ImmediatelySleepParam;
 uint8_t SetSleepUpStsTimeCode;
 uint8_t SetSleepUpStsTimeParam;
 uint8_t CheckBleStsCode;
 uint8_t CheckBleStsParam;
 uint8_t SetBroadCode;
 uint8_t SetBroadParam;
 uint8_t DownKeyCode;
 uint8_t DownKeyParam;
 uint8_t ConfigCarTypeCode;
 uint8_t ConfigCarTypeParam;
 uint8_t SetAlarmVolCode;
 uint8_t SetAlarmVolParam;
 uint8_t ResetEC3CANCode;
 uint8_t ResetEC3CANParam;
 uint8_t SetDeviceNumCode;
 uint8_t SetDeviceNumParam;
 uint8_t FactoryResetCode;
 uint8_t FactoryResetParam;
 uint8_t ImmediatelyRouseCode;
 uint8_t ImmediatelyRouseParam;
 uint8_t SetLOGCode;
 uint8_t SetLOGParam;
 uint8_t UpLOGCode;
 uint8_t UpLOGParam;
 uint8_t ReturnCarCode;
 uint8_t ReturnCarParam;
 uint8_t SetBleTokenTimeCode;
 uint8_t SetBleTokenTimeParam;
}TCTL_RemoteCtrlInfo_ST;*/


typedef struct  TCTL_CtrlItem_Tag
{
	uint8_t				SubitemCode;
	uint8_t				SubitemParam[17];
}TCTL_CtrlItem_ST;

typedef struct  TCTL_ACKUpgrade_Tag
{

	AcpAppHeader_UN 			Header;
	TCTL_HeadTimeStamp_UN	    TimeStamp;
	uint8_t						ACKUpTest;
}TCTL_ACKUpgrade_ST;

typedef struct  TCTL_SendUpgradeData_Tag
{

	AcpAppHeader_UN 			Header;
	TCTL_HeadTimeStamp_UN	    TimeStamp;
	uint8_t						SendUpTest;
	uint8_t						OSID[2];
	uint8_t						MD5[16];
	uint8_t						UpFeilLen;
	uint8_t						UpFeil[256];
}TCTL_SendUpgradeData_ST;

typedef struct  TCTL_CheckVersion_Tag
{

	AcpAppHeader_UN 			Header;
	TCTL_HeadTimeStamp_UN	    TimeStamp;
	uint8_t						VersionTest;
}TCTL_CheckVersion_ST;


typedef struct  fwTCTL_RemoteCtrlInfo_Tag
{
	AcpAppHeader_UN 			Header;
	TCTL_HeadTimeStamp_UN	TimeStamp;
	uint8_t					CtrlTotal;
	TCTL_CtrlItem_ST		Item[20];
}fwTCTL_RemoteCtrlInfo_ST;


typedef union fwTCTL_RemoteCtrl_Tag
{
	uint8_t Buf[TCTL_RemoteCtrl_DATA_LEN];	
	fwTCTL_RemoteCtrlInfo_ST      stTctlInfo;
	TCTL_ACKUpgrade_ST			ACKUpgrade;
	TCTL_SendUpgradeData_ST		SendUpgradeData;
	TCTL_CheckVersion_ST		CheckVersion;

}fwTCTL_RemoteCtrl_UN;

typedef struct  tspTCTL_RemoteCtrlInfo_Tag
{
	uint8_t						HeadFlag;
	uint16_t					Length;
//	AcpAppHeader_UN 			Header;
	fwTCTL_RemoteCtrl_UN 		Ctrl;
}TCTL_RemoteCtrlInfo_ST;


typedef union tspTCTL_RemoteCtrl_Tag
{
	uint8_t Buf[TCTL_RemoteCtrl_DATA_LEN];	
	TCTL_RemoteCtrlInfo_ST      stTctlInfo;

}tspTCTL_RemoteCtrl_UN;


#pragma pack()

uint8_t TCTL_GetCtrlTotal(void);
uint8_t TCTL_Init(void);
uint8_t TCTL_DeInit(void);
TCTL_CtrlItem_ST TCTL_GetCtrlItem(uint8_t index);
uint8_t TCTL_Parse(uint8_t *pBuf, uint8_t len);

uint8_t TCTL_GetMsgType(void);
uint8_t TCTL_GetACKUpTest(void);
uint8_t TCTL_GetSendUpTest(void);
uint8_t TCTL_GetOSID(void);
uint8_t TCTL_GetOSID(uint8_t *pData,uint16_t &len);
uint8_t TCTL_GetMD5(uint8_t *pData,uint16_t &len);
uint8_t TCTL_GetUpFeilLen(void);
uint8_t TCTL_GetUpFeil(uint8_t *pData,uint16_t &len);
uint8_t TCTL_Receive(uint8_t *str, uint16_t dataLen);

#endif

