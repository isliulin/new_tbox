#ifndef BPAK_H
#define BPAK_H




//add chen
#include "common.h"

#define BPAK_VERSION_FLAG	1		//0,测试;1,正式
#define BPAK_VEICHLE_DATA_LEN  		256
#define BPAK_REPROT_VECIHLE_INFORMATION	2
#define BPAK_SIGNCODE_TOTAL		28

#define BPAK_VEICHLEINFO_APPID	2

#define BPAK_VEICHLEINFO_CMDID	2
#define BPAK_VEICHLEINFO_CMDID	2
#define BPAK_VEICHLEINFO_CMDID	2

typedef struct  BPAK_InfoHead_Tag												 
{	
	uint8_t			   		HeadFlag;
	uint16_t			   	BodyLen; 	
	uint8_t			   		CmdID;	 /*3 */
	uint16_t			   	SerialNumber;   /*4 */
	uint8_t			   		Attr[2]; 			 /*6 */ 
	
}BPAK_InfoHead_ST;

typedef enum BPAK_MessageType_Tag
{
		BPAK_MessageType_GPSCarSpead			=	1,	
		BPAK_MessageType_VeichleInfo			=	2,	
		BPAK_MessageType_ALL,
}BPAK_MessageType_ENUM;
		
typedef enum BPAK_AcpCryptFlag_Tag
{
	BPAK_AcpCryptFlag_NULL			=	0,	//非加密
	BPAK_AcpCryptFlag_IS 			=	1,	//加密
	BPAK_AcpCryptFlag_ALL,
}BPAK_AcpCryptFlag_ENUM;


//车况信号编码ID列表
typedef enum BPAK_VeichleInfoNum_Tag
{
		BPAK_VeichleInfo_GPSNum				=	0,	//GPS信息
		BPAK_VeichleInfo_RemainedOilNum		=	1,	//剩余油量
		BPAK_VeichleInfo_OdometerNum		=	2,	//总里程
		BPAK_VeichleInfo_MaintVolNum		=	3,	//蓄电池电压
		BPAK_VeichleInfo_CarSpeedNum		=	4,	//实时车速
		BPAK_VeichleInfo_DoorStatusNum		=	5,	//车门状态
		BPAK_VeichleInfo_LockStatusNum		=	6,	//车锁状态
		BPAK_VeichleInfo_RoofStatusNum		=	7, //天窗状态
		BPAK_VeichleInfo_WindowStateNum		=	8,	//车窗状态
		BPAK_VeichleInfo_LampStautsNum		=	9,	//车灯状态
		BPAK_VeichleInfo_EngineStateNum		=	10, //发动机状态
		BPAK_VeichleInfo_EngineSpeedNum		=	11, //发动机转速
		BPAK_VeichleInfo_GearStateNum 		=	12, //档位信息
		BPAK_VeichleInfo_HandBrakeStateNum	=	13, //手刹状态
		BPAK_VeichleInfo_IgnInfoNum			=	14, //供匙
		BPAK_VeichleInfo_BPStsNum			=	15, //动力电池状态
		BPAK_VeichleInfo_ChargeStateNum	 	=	16,//充电状态
		BPAK_VeichleInfo_ChargeGunStatusNum	=	17,//充电枪连接状态
		BPAK_VeichleInfo_BreakStatusNum		=	18,//制动踏板开关
		BPAK_VeichleInfo_RemainEnduranceNum	=	19,//剩余续航里程
		BPAK_VeichleInfo_BeltStatusNum		=	20, //安全带状态
		BPAK_VeichleInfo_MotorWorkStateNum	=	21,//电机工作状态 
		BPAK_VeichleInfo_MotorSpeedNum		=	22, //电动机转速
		BPAK_VeichleInfo_PowerdNum			=	23, //动力状态
		BPAK_VeichleInfo_VerTboxOSNum 	 	=	24,//TBox_OS版本
		BPAK_VeichleInfo_VerTboxMCUNum		=	25, //TBox_MCU版本
		BPAK_VeichleInfo_VerTboxBLENum		=	26, //TBox_BLE版本
		BPAK_VeichleInfo_GwNodeMissNum		=	27, //节点是否丢失
		BPAK_VeichleInfo_4GSIGNALVALUENum 	=	28, //4G信号强度值

}BPAK_VeichleInfoNum_ENUM;



typedef union BPAK_ElementLenInfo_Tag
{
	uint8_t Element_Len;
	struct{ 
		uint8_t DataLen 	:5; //数据长度
		uint8_t MoreFlag	:1;	//数据长度标识选择
		uint8_t Identifier  :2; //元素数据标识
	}elementLenBit;
}BPAK_ElementLenInfo_UN;


typedef struct BPAK_TimeStamp_Tag
{
	uint8_t Year 	:6; //年(0=1990,1=1991,62=2052)
	uint8_t Month	:4;	//月
	uint8_t Day 	:5;	//日
	uint8_t Hour 	:5;	//时
	uint8_t Minutes :6;	//分
	uint8_t Seconds :6;	//秒
	uint16_t msSeconds :16;//毫秒0-1000
}BPAK_TimeStamp_St;




typedef union BPAK_GPSInfo_Tag
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
}BPAK_GPSInfo_UN;
typedef union BPAK_RemainOil_Tag
{
	uint16_t Oil;
	struct		
	{
		uint8_t Backup:8;
		uint8_t RemainOilVule: 8; 	
	} BYTES;
}BPAK_RemainOil_UN;
typedef union BPAK_DOORStatus_Tag 
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
}BPAK_DOORState_UN;
typedef union BPAK_LOCKStatus_Tag 
{
    uint8_t LockStatus;                    
    struct                                                          
    {  
        uint8_t  RightRearLock	 	: 2;              
        uint8_t  LeftRearLock	 	: 2;                      
        uint8_t  CopilotLock	 	: 2;              
        uint8_t  DrivingLock 	 	: 2;                                 			               
     }BYTES;
}BPAK_LOCKState_UN;
typedef union BPAK_WINDOSStatus_Tag 
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
}BPAK_WINDOSState_UN;

typedef union BPAK_LAMPStatus_Tag 
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
}BPAK_LAMPState_UN;

typedef union BPAK_BPSts_Tag
{
	uint8_t BatPow[8];
	struct		
	{
		uint16_t BPVol: 16; 
		uint16_t BPSur: 16; 
		uint16_t BPSoc: 16;	
	} BYTES;
}BPAK_BPSts_UN;


typedef union BPAK_AcpAppHeader_Tag
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
}BPAK_AcpAppHeader_UN;

#pragma pack(1)

typedef struct BPAK_AcpTimeStamp_Tag
{
	BPAK_ElementLenInfo_UN	Element_LenInfo;//长度
	BPAK_TimeStamp_St				TimeStampInfo;
}BPAK_AcpTimeStamp_St;


typedef struct  BPAK_VeichleInfo_Tag
{
	BPAK_AcpAppHeader_UN		Header;
	uint8_t				SendMode;
	uint8_t				CollectPacketSN;
	uint8_t				TimeStamp[6];
	uint8_t 			SignCodeTotal[2];
	uint16_t 			GPSInfoSignNum; 
	BPAK_GPSInfo_UN 	GPSInfo;
	uint16_t 			RemainOilSignNum; 
	BPAK_RemainOil_UN 	RemainOil;
	uint16_t 			OdoSignNum; 
	uint32_t 			Odo; 
	uint16_t 			MaintVolSignNum; 
	uint16_t 			MaintVol;
	uint16_t 			CarSpeedSignNum; 
	uint16_t 			CarSpeed;
	uint16_t 			DoorStatusSignNum; 
	BPAK_DOORState_UN 	DoorStatus;
	uint16_t 			LockStatusSignNum; 
	BPAK_LOCKState_UN 	LockStatus;
	uint16_t 			RoofStatusSignNum; 
	uint8_t 			RoofStatus;	 
	uint16_t 			WindowStatusSignNum; 
	BPAK_WINDOSState_UN WindowStatus;
	uint16_t 			LampStautsSignNum; 
	BPAK_LAMPState_UN 	LampStauts;
	uint16_t 			EngineStatusSignNum; 
	uint8_t 			EngineStatus;
	uint16_t 			EngineSpeedSignNum; 
	uint16_t 			EngineSpeed;
	uint16_t 			GearInforSignNum; 
	uint8_t 			GearInfor;
	uint16_t 			HandParkStatusSignNum; 
	uint8_t 			HandParkStatus;
	uint16_t 			IgnInfoSignNum; 
	uint8_t 			IgnInfo;
	uint16_t 			BPStsSignNum; 
	BPAK_BPSts_UN		BPSts;
	uint16_t 			ChargeStatusSignNum; 
	uint8_t 			ChargeStatus;
	uint16_t 			ChargeGunStatusSignNum; 
	uint8_t 			ChargeGunStatus;
	uint16_t 			BreakStatusSignNum; 
	uint8_t 			BreakStatus;
	uint16_t 			RemainEnduranceSignNum; 
	uint16_t 			RemainEndurance;
	uint16_t 			BeltStatusSignNum; 
	uint8_t 			BeltStatus;
	uint16_t 			MotorStatusSignNum; 
	uint8_t 			MotorStatus;
	uint16_t 			MotorSpeedSignNum; 
	uint16_t 			MotorSpeed;
	uint16_t 			PowerForceStsSignNum; 
	uint8_t 			PowerForceSts;
	uint16_t 			OsVersionSignNum; 
	uint8_t 			OsVersion[12];
	uint16_t 			McuVersionSignNum; 
	uint8_t 			McuVersion[12];
	uint16_t 			BleVersionSignNum; 
	uint8_t 			BleVersion[12];
	uint16_t 			CarGwNodeMissSignNum; 
	uint8_t 			CarGwNodeMiss;
	uint16_t 			BPAK_4GValueSignNum; 
	uint8_t 			BPAK_4GValue;
}BPAK_VeichleInfo_ST;

#pragma pack()

typedef union BPAK_VeichleInformation_Tag
{
	uint8_t Buf[BPAK_VEICHLE_DATA_LEN];	
	BPAK_VeichleInfo_ST      stBpakInfo;

}BPAK_VeichleInformation_UN;


void bpak_TimeDeal(uint8_t* pTemp, uint8_t isFlagLen);
void bpak_TimeStampPart(uint8_t* pTemp, BPAK_TimeStamp_St *pTimeStamp);
uint8_t BPAK_Init();
uint8_t BPAK_DeInit();
uint8_t BPAK_Printf(void);
uint8_t bpak_Pakage(uint8_t *buf);
uint8_t BPAK_PackageData(uint8_t PAKID,uint8_t* pData);


#endif

