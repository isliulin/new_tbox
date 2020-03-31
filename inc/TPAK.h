#ifndef TPAK_H
#define TPAK_H




//add chen
#include "common.h"

#define TPAK_VERSION_FLAG	1		//0,测试;1,正式
#define TPAK_VEICHLE_DATA_LEN  		256
//#define TPAK_REPROT_VECIHLE_INFORMATION	0x14
#define TPAK_SIGNCODE_TOTAL		28

#define TPAK_VEICHLEINFO_APPID		4
#define TPAK_RemoteCtrlACK_APPID	2
#define TPAK_TimeSync_APPID			9
#define TPAK_Upgrade_APPID			6
#define TPAK_Message_Head_Len       3


typedef union TPAK_SignCodeBit_Tag
{
	uint16_t SignCodeBit;
 	struct{
		uint8_t	SignCodeL	:8;	//信号编码低位
		uint8_t SignCodeH 	:5; //信号编码高位
		uint8_t Signflag	:3;	//信号标识(0:有效1:无效2:预留3:预留)
  	}codeBit;
}TPAK_SignCodeBit_UN;


#pragma pack(1)




typedef enum TPAK_MessageType_Tag
{
		TPAK_MessageType_GPSCarSpead			=	1,	
		TPAK_MessageType_VeichleInfo			=	2,	
		TPAK_MessageType_ALL,
}TPAK_MessageType_ENUM;
		
typedef enum TPAK_AcpCryptFlag_Tag
{
	TPAK_AcpCryptFlag_NULL			=	0,	
	TPAK_AcpCryptFlag_IS 			=	1,	
	TPAK_AcpCryptFlag_ALL,
}TPAK_AcpCryptFlag_ENUM;


//车况信号编码ID列表
typedef enum TPAK_VeichleInfoNum_Tag
{
		TPAK_VeichleInfo_GPSNum				=	0,	
		TPAK_VeichleInfo_RemainedOilNum		=	1,	
		TPAK_VeichleInfo_OdometerNum		=	2,	
		TPAK_VeichleInfo_MaintVolNum		=	3,	
		TPAK_VeichleInfo_CarSpeedNum		=	4,	
		TPAK_VeichleInfo_DoorStatusNum		=	5,	
		TPAK_VeichleInfo_LockStatusNum		=	6,	
		TPAK_VeichleInfo_RoofStatusNum		=	7, 
		TPAK_VeichleInfo_WindowStateNum		=	8,	
		TPAK_VeichleInfo_LampStautsNum		=	9,	
		TPAK_VeichleInfo_EngineStateNum		=	10, 
		TPAK_VeichleInfo_EngineSpeedNum		=	11, 
		TPAK_VeichleInfo_GearStateNum 		=	12, 
		TPAK_VeichleInfo_HandBrakeStateNum	=	13, 
		TPAK_VeichleInfo_IgnInfoNum			=	14, 
		TPAK_VeichleInfo_BPStsNum			=	15, 
		TPAK_VeichleInfo_ChargeStateNum	 	=	16,
		TPAK_VeichleInfo_ChargeGunStatusNum	=	17,
		TPAK_VeichleInfo_BreakStatusNum		=	18,
		TPAK_VeichleInfo_RemainEnduranceNum	=	19,
		TPAK_VeichleInfo_BeltStatusNum		=	20, 
		TPAK_VeichleInfo_MotorWorkStateNum	=	21,
		TPAK_VeichleInfo_MotorSpeedNum		=	22,
		TPAK_VeichleInfo_PowerdNum			=	23, 
		TPAK_VeichleInfo_VerTboxOSNum 	 	=	24,
		TPAK_VeichleInfo_VerTboxMCUNum		=	25, 
		TPAK_VeichleInfo_VerTboxBLENum		=	26, 
		TPAK_VeichleInfo_GwNodeMissNum		=	27, 
		TPAK_VeichleInfo_4GSIGNALVALUENum 	=	28, 

}TPAK_VeichleInfoNum_ENUM;


typedef union TPAK_ElementLenInfo_Tag
{
	uint8_t Element_Len;
	struct{ 
		uint8_t DataLen 	:5; //数据长度
		uint8_t MoreFlag	:1;	//数据长度标识选择
		uint8_t Identifier  :2; //元素数据标识
	}elementLenBit;
}TPAK_ElementLenInfo_UN;

typedef union TPAK_HeadTimeStamp_Tag
{
	uint8_t Data[7];					
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


}TPAK_HeadTimeStamp_UN;
typedef union TPAK_TimeStamp_Tag
{
	uint8_t Data[6];					
	struct															
	{  

		uint8_t Month		:4; //月
		uint8_t Day 		:5; //日
		uint8_t Hour		:5; //时
		uint8_t Minutes 	:6; //分
		uint8_t Seconds 	:6; //秒
		uint16_t msSeconds :16;//毫秒0-1000

							
	 }BYTES;


}TPAK_TimeStamp_UN;





typedef union TPAK_GPSInfo_Tag
{
	uint8_t Info[15];
	struct		
	{
		uint32_t Direction: 9; 
		uint32_t Sec: 6; 
		uint32_t Min: 6;
		uint32_t Hour: 5; 
		uint32_t Day: 5; 
		uint32_t Mon: 4; 
		uint32_t Year: 6; 
		uint32_t High: 14; 
		uint32_t LatSts: 2; 
		uint32_t LatDeg: 29; 
		uint32_t LonSts: 2;
		uint32_t LonDeg: 29; 
		uint32_t PosiVailed: 2; 	
	} BYTES;
}TPAK_GPSInfo_UN;
typedef union TPAK_RemainOil_Tag
{
	uint16_t Oil;
	struct		
	{
		uint8_t Backup:8;
		uint8_t RemainOilVule: 8; 	
	} BYTES;
}TPAK_RemainOil_UN;
typedef union TPAK_DOORStatus_Tag 
{
    uint8_t DoorStatus;                    
    struct                                                          
    {  
        uint8_t  DrivingDoor	 	: 1;              
        uint8_t  CopilotDoor	 	: 1;                      
        uint8_t  LeftRearDoor	 	: 1;              
        uint8_t  RightRearDoor 	 	: 1;                                 
        uint8_t  RearCanopy			: 1;                 
        uint8_t  EngineCover	 	: 1;			               
     }BYTES;
}TPAK_DOORState_UN;
typedef union TPAK_LOCKStatus_Tag 
{
    uint8_t LockStatus;                    
    struct                                                          
    {  
        uint8_t  RightRearLock	 	: 2;              
        uint8_t  LeftRearLock	 	: 2;                      
        uint8_t  CopilotLock	 	: 2;              
        uint8_t  DrivingLock 	 	: 2;                                 			               
     }BYTES;
}TPAK_LOCKState_UN;
typedef union TPAK_WINDOSStatus_Tag 
{
    uint8_t WindowStatus[2];                    
    struct                                                          
    {  
        uint16_t  LeftFrontWindow	 	: 3;              
        uint16_t  RightFrontWindow	 	: 3; 
    	uint16_t  LeftRearWindow		: 3;
		uint16_t  RightRearWindow		: 3; 
		uint16_t  Backup		 		: 4;	
                            
     }BYTES;
}TPAK_WINDOSState_UN;

typedef union TPAK_LAMPStatus_Tag 
{
    uint8_t LampStatus;                    
    struct                                                          
    {  
        uint8_t  Headlights	 		: 1;              
        uint8_t  Positionlights	 	: 1;                      
        uint8_t  Nearlights	 		: 1;              
        uint8_t  Rearfoglights 	 	: 1;                                 
        uint8_t  Frontfoglights		: 1;                 
        uint8_t  Leftlights	 		: 1;	
		uint8_t  Rightlights	 	: 1;	
     }BYTES;
}TPAK_LAMPState_UN;

typedef union TPAK_BPSts_Tag
{
	uint8_t BatPow[8];
	struct		
	{
		uint16_t BPVol: 16; 
		uint16_t BPSur: 16; 
		uint16_t BPSoc: 16;	
	} BYTES;
}TPAK_BPSts_UN;


typedef union TPAK_AcpAppHeader_Tag
{
	uint8_t AcpAppHeader[5];
	struct
	{
		//服务标识号
		uint8_t Acp_AppID 	:6; //Acp的服务标识号
		uint8_t PrivateFlag	:1;	//私有标识
		uint8_t Reserved_ID	:1;	//保留位	

		//服务消息标识(测试+加密+服务)
		uint8_t MsgType 	:5; //服务消息类型
		uint8_t CryptFlag	:1;	//加密标识
		uint8_t TestFlag	:1;	//测试报文标识
		uint8_t Reserved_T 	:1;	//保留位

		//消息控制位
		uint8_t MsgCtrlFlag :4; //消息控制掩码(0000/0010)
		uint8_t Version		:3;	//版本号(set 0-7)
		uint8_t VersionFlag	:1;	//启用版本号标识(0:启用,1:不用)

		//长度定义
		uint16_t	MsgLength	:16;//消息长度(AppHeader+Element)
	}BYTES;
}TPAK_AcpAppHeader_UN;





typedef struct  TPAK_VeichleInfoBody_Tag
{
	TPAK_HeadTimeStamp_UN	TimeStamp;
	uint8_t					SendMode;
	uint8_t					CollectPacketSN;
	TPAK_HeadTimeStamp_UN	GatherTime;
	uint8_t 				SignCodeTotal[2];
	uint16_t 			    GPSInfoSignNum; 
	TPAK_GPSInfo_UN 	    GPSInfo;
	uint16_t 			    RemainOilSignNum; 
	TPAK_RemainOil_UN 	    RemainOil;
	uint16_t 			    OdoSignNum; 
	uint32_t 			    Odo; 
	uint16_t 			    MaintVolSignNum; 
	uint16_t 			    MaintVol;
	uint16_t 			    CarSpeedSignNum; 
	uint16_t 			    CarSpeed;
	uint16_t 			    DoorStatusSignNum; 
	TPAK_DOORState_UN 	    DoorStatus;
	uint16_t 			    LockStatusSignNum; 
	TPAK_LOCKState_UN 	    LockStatus;
	uint16_t 			    RoofStatusSignNum; 
	uint8_t 			    RoofStatus;	 
	uint16_t 			    WindowStatusSignNum; 
	TPAK_WINDOSState_UN     WindowStatus;
	uint16_t 			    LampStautsSignNum; 
	TPAK_LAMPState_UN 	    LampStauts;
	uint16_t 			    EngineStatusSignNum; 
	uint8_t 			    EngineStatus;
	uint16_t 			    EngineSpeedSignNum; 
	uint16_t 			    EngineSpeed;
	uint16_t 			    GearInforSignNum; 
	uint8_t 			    GearInfor;
	uint16_t 			    HandParkStatusSignNum; 
	uint8_t 			    HandParkStatus;
	uint16_t 			    IgnInfoSignNum; 
	uint8_t 			    IgnInfo;
	uint16_t 			    BPStsSignNum; 
	TPAK_BPSts_UN		    BPSts;
	uint16_t 			    ChargeStatusSignNum; 
	uint8_t 			    ChargeStatus;
	uint16_t 			    ChargeGunStatusSignNum; 
	uint8_t 			    ChargeGunStatus;
	uint16_t 			    BreakStatusSignNum; 
	uint8_t 			    BreakStatus;
	uint16_t 			    RemainEnduranceSignNum; 
	uint16_t 			    RemainEndurance;
	uint16_t 			    BeltStatusSignNum; 
	uint8_t 			    BeltStatus;
	uint16_t 			    MotorStatusSignNum; 
	uint8_t 			    MotorStatus;
	uint16_t 			    MotorSpeedSignNum; 
	uint16_t 			    MotorSpeed;
	uint16_t 			    PowerForceStsSignNum; 
	uint8_t 			    PowerForceSts;
	uint16_t 			    OsVersionSignNum; 
	uint8_t 			    OsVersion[12];
	uint16_t 			    McuVersionSignNum; 
	uint8_t 			    McuVersion[12];
	uint16_t 			    BleVersionSignNum; 
	uint8_t 			    BleVersion[12];
	uint16_t 			    CarGwNodeMissSignNum; 
	uint8_t 			    CarGwNodeMiss;
	uint16_t 			    TPAK_4GValueSignNum; 
	uint8_t 			    TPAK_4GValue;
}TPAK_VeichleInfoBody_ST;



typedef struct  TPAK_VeichleInfo_Tag
{
	uint8_t 				HeadFlag;
	uint16_t				Length;
	TPAK_AcpAppHeader_UN	Header;
	TPAK_HeadTimeStamp_UN	TimeStamp;
	TPAK_VeichleInfoBody_ST Body;

}TPAK_VeichleInfo_ST;

typedef struct  TPAK_RemoteCtrlACKBody_Tag
{
	uint8_t                 Total;
	uint8_t                 CtrlCode;
	uint8_t                 CtrlParameter[256];
}TPAK_RemoteCtrlACKBody_ST;

typedef struct  TPAK_RemoteCtrlACK_Tag
{
	uint8_t 				    HeadFlag;
	uint16_t				    Length;
	TPAK_AcpAppHeader_UN	    Header;
	TPAK_HeadTimeStamp_UN	    TimeStamp;
	uint8_t                     ErrorElem;
	TPAK_RemoteCtrlACKBody_ST   Body;
}TPAK_RemoteCtrlACK_ST;

typedef struct  TPAK_TimeSync_Tag
{
	uint8_t 				HeadFlag;
	uint16_t				Length;
	TPAK_AcpAppHeader_UN	Header;
	TPAK_HeadTimeStamp_UN	TimeStamp;
}TPAK_TimeSync_ST;

typedef struct  TPAK_UpgradeACK_Tag
{
	uint8_t 				    HeadFlag;
	uint16_t				    Length;
	TPAK_AcpAppHeader_UN		Header;
	TPAK_HeadTimeStamp_UN	 	TimeStamp;
}TPAK_UpgradeACK_ST;

/*typedef struct  TPAK_DownResultACK_Tag
{
	uint8_t 				HeadFlag;
	uint16_t				Length;
	TPAK_AcpAppHeader_UN	Header;
	TPAK_HeadTimeStamp_UN	TimeStamp;
	uint8_t 				Result;
}TPAK_DownResultACK_ST;


typedef struct  TPAK_TimeSync_Tag
{
	uint8_t 				HeadFlag;
	uint16_t				Length;
	TPAK_AcpAppHeader_UN		Header;
	TPAK_HeadTimeStamp_UN	 	TimeStamp;
}TPAK_TimeSync_ST;*/

typedef struct TPAK_RemoteUpgrade_Tag
{
	uint8_t 					HeadFlag;
	uint16_t					Length;
	TPAK_AcpAppHeader_UN		Header;
	TPAK_HeadTimeStamp_UN	 	TimeStamp;
	uint8_t 					Result;
}TPAK_RemoteUpgrade_ST;


typedef union TPAK_VeichleInformation_Tag
{
	uint8_t Buf[TPAK_VEICHLE_DATA_LEN];	
	TPAK_VeichleInfo_ST     stTpakInfo;
	TPAK_TimeSync_ST		stTpakSync;
	TPAK_RemoteCtrlACK_ST	stTpakCtrlACK;
	TPAK_RemoteUpgrade_ST   stUpgrade;
}TPAK_VeichleInformation_UN;


#pragma pack()

void tpak_TimeDeal(uint8_t* pTemp, uint8_t isFlagLen);
void tpak_TimeStampPart(uint8_t* pTemp, TPAK_HeadTimeStamp_UN *pTimeStamp);
uint8_t TPAK_Init();
uint8_t TPAK_DeInit();
uint8_t TPAK_Printf(void);
uint8_t tpak_PakageVeichleData(uint16_t *len,uint8_t *buf,uint8_t msgType);
uint8_t tpak_PakageCtrlACKData(uint16_t *len,uint8_t* pData, uint8_t type);
uint8_t tpak_PakageSyncData(uint16_t *len,uint8_t* pData, uint8_t type);
uint8_t tpak_PakageUpgradeData(uint16_t *len,uint8_t* pData, uint8_t type, uint8_t result);
uint8_t TPAK_PackageData(uint16_t *len,uint8_t PAKID, uint8_t* pData, uint8_t type = 0,  uint8_t result = 0);



#endif

