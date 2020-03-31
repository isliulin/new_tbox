#ifndef VIF_H
#define VIF_H

//add chen
#include "common.h"





#define VIF_VEICHLE_DATA_LEN  256
#define VIF_VEICHLE_DATA_HEAD_LEN  7
#define VIF_INFO_CMDID  3


#define VIF_MCU_GENERAL_RESP        0x01	//从机通用应答
#define VPAK_SlaveHeart_CMDID			0x02
#define CarInfo		            0x14
#define TBOX_REMOTECTRL_CMD		0x04	//从机控制应答
#define MCU_REQ_UPGRADE_DATA_CMD			0x06
#define MCU_SEND_COMPLETE       0x0A




typedef union DOORStatus_Tag 
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
}DOORState_UN;

typedef union LOCKStatus_Tag 
{
    uint8_t LockStatus;                    
    struct                                                          
    {  
        uint8_t  RightRearLock	 	: 2;              
        uint8_t  LeftRearLock	 	: 2;                      
        uint8_t  CopilotLock	 	: 2;              
        uint8_t  DrivingLock 	 	: 2;                                 			               
     }BYTES;
}LOCKState_UN;

typedef union WINDOSStatus_Tag 
{
    uint16_t WindowStatus;                    
    struct                                                          
    {  
    	uint8_t  LeftRearWindowHigh	: 1;
		uint8_t  RightRearWindow		: 3; 
		uint8_t  Backup		 		: 4;	
        uint8_t  LeftFrontWindow	 	: 3;              
        uint8_t  RightFrontWindow	 	: 3;                      
        uint8_t  LeftRearWindowLow	 	: 2;     



     }BYTES;
}WINDOSState_UN;

typedef union LAMPStatus_Tag 
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
}LAMPState_UN;

typedef union TyreSystem_Tag 
{
    uint8_t tyreSystem[4];                    
    struct                                                          
    {  
        uint8_t  LeftFrontTyrePress 		: 3;              
        uint8_t  LeftFrontTyreTemp		 	: 3;                      
        uint8_t  LeftFrontBackup			: 2;              
        uint8_t  RightFrontTyrePress  	 	: 3;                                 
        uint8_t  RightFrontTyreTemp			: 3;                 
        uint8_t  RightFrontBackup	 		: 2;	
        uint8_t  LeftRearTyrePress  	 	: 3;                                 
        uint8_t  LeftRearTyreTemp			: 3;                 
        uint8_t  LeftRearBackup	 			: 2;
	 	uint8_t  RightRearTyrePress  	 	: 3;                                 
        uint8_t  RightRearTyreTemp			: 3;                 
        uint8_t  RightRearBackup	 		: 2;

     }BYTES;
}TyreSystem_UN;




typedef union VIF_SendState_Tag 
{
	uint8_t State[4];
	struct		
	{
		uint8_t Lat: 1; /*纬度*/
		uint8_t Lon: 1; /*经度*/
		uint8_t GpsAntenna: 1; /*GPS天线故障*/
		//uint32_t GpsAntenna: 2; /*GPS天线故障,0:正常，1：开路，2：短路*/ //wangzhongpu
		uint8_t AutoLock: 1; /*曾自动锁车标志*/
		uint8_t AccPlcPowerType: 1; /*ACC\PLC上电*/
		uint8_t OpenCover: 1; /*开盖报警*/
		uint8_t BusFault: 1; /*总线故障报警*/
		uint8_t HostPowerCut: 1; /*主电源断电报警*/
		uint8_t BackBatPowerCut: 1; /*备份电池断电报警*/
		uint8_t BackBatLowPower: 1; /*备用电池欠压报警*/
		uint8_t Rs485BaudChange: 1; /*RS485总线波特率变化标志*/
		uint8_t CanBaudChange: 1; /*CAN总线波特率变化标志*/
		uint8_t DevHerar: 1; /*终端连接心跳包*/
		uint8_t DevSleep: 1; /*休眠状态*/
		uint8_t HostLowPower: 1; /*主电源欠压报警*/
		uint8_t Collision: 1; /*碰撞报警*/
		uint8_t Remove: 1; /*拆卸报警*/
		uint8_t PlcControlled: 1; /*PLC受控情况*/
		uint8_t PosiVailed: 1; /*GPS定位状态*/
		uint8_t CanTimerSendState: 1; //CAN定时输出状态
		uint8_t Rs485TimerSendState: 1; //RS485定时输出状态
		uint8_t Flashfault:1;	//flash故障			//wangzhongpu
		uint8_t CAN0fault:1;		//CAN0通信故障			//wangzhongpu
		uint8_t CAN1fault:1;		//CAN1通信故障			//wangzhongpu
		uint8_t PositionModuleState:1; //定位模块状态，0：正常，1：故障
		uint8_t CAN0BUSOFF:1;			//CAN0 BUSOFF，0：正常，1：故障
		uint8_t CAN1BUSOFF:1;			//CAN1 BUSOFF，0：正常，1：故障
		uint8_t MCUConnect4GError:1;	//MCU连接4G失败，0：正常，1：故障
		uint8_t Backup:4; /*备用*/
	} bitState_t;
}VIF_SendState_UN;

#pragma pack(1)

typedef struct  VIF_InfoHead_Tag												 
{	
	uint8_t			   		HeadFlag;
	uint16_t			   	BodyLen; 	
	uint8_t			   		CmdID;	 /*3 */
	uint16_t			   	SerialNumber;   /*4 */
	uint8_t			   		Attr[2]; 			 /*6 */ 
	
}VIF_InfoHead_ST;




typedef struct VIF_VeichleTroubleInfo_Tag
{
	uint8_t EngineSystem;
	uint8_t TCUSystem;
	uint8_t EmissionSystem;
	uint8_t ACUSystem;
	uint8_t ESPSystem;
	uint8_t ABSSystem;
	uint8_t EPSSystem;
	uint8_t OilPressSystem;
	uint8_t OilLowSystem; //8油量低报警
	uint8_t BreakFluidSystem;
	uint8_t ChargeSystem;
	uint8_t BreakSystem;
	uint8_t TyrePressSystem;
	uint8_t StartStopSystem;
	uint8_t LampSystem;
	uint8_t ELockSystem;
	uint8_t EngineTempSytem;
	uint8_t EPBSystem; // 17 电子驻车单元系统故障
	uint8_t UpperBeamSystem;
	uint8_t NavigateSystem;
	uint8_t CrashSystem;
	uint8_t RoalSystem;
	uint8_t BlindMonitorSystem;
	uint8_t OperateAC;
	uint8_t HighVolSystem;
	uint8_t InsulationSystem;
	uint8_t InterlockSystem; //26 高压互锁故障
	uint8_t CellSystem;
	uint8_t MotorSystem;
	uint8_t EParkSystem;
	uint8_t EQuantityForCellSystem;
	uint8_t TempeForCellSystem;
	uint8_t TempeForMotorSystem;
	uint8_t NaviCnstSpdSystem;
	uint8_t BattChargerSystem;//34 充电机故障
	uint8_t ACSystem;
	uint8_t AuxSystem;
	uint8_t EmergyBreakSystem;
	uint8_t RadarSystem;
	uint8_t ElcSystem;
	TyreSystem_UN TyreSystem;
	uint8_t DCDCSystem;
	uint8_t BCMSystem;
	uint8_t RelaySystem;
	uint8_t TSROutSpdSystem;
	uint8_t TSRLimSpdSystem;
	uint8_t AEBinvolvedSystem;
	uint8_t ABSinvolvedSystem;
	uint8_t ASRinvolvedSystem;
	uint8_t ESPinvolvedSystem;
	uint8_t DSMSystem;//53 DSM报警
	uint8_t LeaveWheelPromp;
	uint8_t ACCStatus;
	uint8_t AccSpdSystem;
	uint8_t FCWSystem;
	uint8_t FCWStatus;
	uint8_t TimerForFCWSystem;
	uint8_t BreakForFCWSystem;
	uint8_t LDWStatus;
	uint8_t LDWSystem;
	uint8_t WheelForLDWSystem;
	uint8_t LKAStatus;
	uint8_t LKAinvolvedSystem;
	uint8_t LKADriverPromp;
	uint8_t LKADriverForWheel;
	uint8_t BSDStatus;
	uint8_t LeftBSDSystem;
	uint8_t RightBSDSystem;//70 BSD右侧报警
	uint8_t TimeForWheelBSD;
	uint8_t TimeForBreakBSD;
	uint8_t AccelPedalForBSD;
	uint8_t LeftForFlowSystem;
	uint8_t RightForFlowSystem;
	uint8_t RearForFlowSystem;
	uint8_t BreakForFlowSystem;
	uint8_t AccelPedalForFlow;
	uint8_t LeftAUXSystem;
	uint8_t RightAUXSystem;
	uint8_t RearAUXSystem;
	uint8_t BreakAUXSystem;
	uint8_t AccelPedalAUXSystem;
	uint8_t Doorcrash;   //83车门入侵信号
	uint8_t Enginestop;  //84紧急熄火信号
	uint8_t Carcrash;    //85车辆碰撞信号
	uint8_t TrailCarEvent;   //86车辆拖动信号
	uint8_t SkyNotCloseAlm;   //87天窗未关提醒
}VIF_VeichleTroubleInfo_ST;


/*this struct DATA is reading from MCU UART*/
typedef struct  VIF_VeichleInfoBody_Tag
{					
	VIF_SendState_UN 	Sts;	/*8 */
	uint32_t 	LatDeg;       /*12 */
	uint32_t 	LonDeg;
	uint16_t 	Hight;         /*20 */
	uint16_t 	Direction;     /*22 */
	uint8_t 	Year;         /* 24 */
	uint8_t 	Mon;
	uint8_t 	Day;
	uint8_t 	Hour;
	uint8_t 	Min;
	uint8_t 	Sec;
	uint8_t 	RemainOil;   /* 30 */
	uint16_t 	BPSoc;      /* 31 */
	uint16_t 	CarSpeed;	  /* 33 */
	uint32_t 	Odo;		  /* 35 */
	DOORState_UN 	DoorStatus;	  /* 39 */
	LOCKState_UN 	LockStatus;	  /* 40 */
	uint8_t 	RoofStatus;	  /* 41 */
	WINDOSState_UN 	WindowStatus;/* 42 */
	LAMPState_UN 	LampStauts;	  /* 44 */
	uint8_t 	McuVersion[12];		  /* 45 */
	uint8_t 	MotorStatus;	  /* 57 */
	uint16_t 	MotorSpeed;  /* 58 */
	uint8_t 	EngineStatus;	  /* 60 */
	uint16_t 	EngineSpeed; /* 61 */
	uint8_t 	GearInfor;	  /* 63 */
	uint8_t 	IgnInfo;		  /* 64 */
	uint8_t 	HandParkStatus; /* 65 */
	uint8_t 	BeltStatus;	  /* 66 */
	uint8_t 	ChargeStatus;	  /* 67 */
	uint8_t 	ChargeGunStatus;/* 68 */
	uint8_t 	BreakStatus; /* 69 */
	uint16_t 	RemainEndurance;  /* 70 */
	uint16_t 	BPVol;	  /* 72 */
	uint16_t 	BPSur;	  /* 74 */
	uint16_t 	MaintVol;	  /* 76 */
	uint8_t 	PowerForceSts;	/* 78*/
	uint8_t 	CarIDState;	   /*79 */
	uint8_t 	CarGwNodeMiss;  /*80*/
//	VIF_VeichleTroubleInfo_ST	TroubleInfo;
	
}VIF_VeichleInfoBody_ST;

typedef struct  VIF_VeichleACK_Tag													
{  
	 
	uint8_t 		ACKID; 	 
	uint16_t		ACKNumber;  
	uint8_t 		ACKResult; 	
}VIF_VeichleACK_ST;
 
typedef struct  VIF_VeichleHeartBeat_Tag												 
{	
	 VIF_InfoHead_ST Head;		  	 
}VIF_VeichleHeartBeat_ST;


typedef struct  VIF_RemoteCtrlACK_Tag												  
{  	   	   
	uint8_t	  				CtrlCode;  
	uint8_t	  				CtrlResult;
}VIF_RemoteCtrlACK_ST;
   
typedef struct	VIF_TimeSync_Tag												  
{		   
	uint8_t 				Byte[5];
}VIF_TimeSync_ST;


typedef union VIF_InfoBody_Tag
{
	VIF_VeichleInfoBody_ST    	VifInfoBody;
	VIF_VeichleACK_ST 			VifACKBody;
//	VIF_VeichleHeartBeat_ST		VifHeartBody;
	VIF_RemoteCtrlACK_ST		VifCtrlACKBody;
	VIF_TimeSync_ST				VifSyncBody;

}VIF_InfoBody_UN;

typedef struct VIF_VeichleInfo_Tag
{

	VIF_InfoHead_ST				Head;	
	VIF_InfoBody_UN    			Body;
}VIF_VeichleInfo_ST;



typedef union VIF_VeichleInformation_Tag
{
	uint8_t Buf[VIF_VEICHLE_DATA_LEN];	
	VIF_VeichleInfo_ST    stVifInfo;
}VIF_VeichleInformation_UN;


#pragma pack()

uint8_t VIF_Init(void);
uint8_t VIF_DeInit(void);
uint8_t VIF_Parse(uint8_t *pBuf,uint8_t len, VIF_VeichleInformation_UN *mcuVi);
uint8_t VIF_Printf(void);

uint16_t VIF_GetBodyLen(void);
uint8_t VIF_GetCmdID(void);
uint16_t VIF_GetSerialNumber(void);
uint8_t VIF_GetAttr(uint8_t *pData,uint16_t &len);	
uint32_t VIF_GetStsLat(void);
uint32_t VIF_GetStsLon(void);
uint8_t VIF_GetStsGpsAntenna(void);
uint8_t VIF_GetStsAutoLock(void);
uint8_t VIF_GetStsAccPlcPowerType(void);
uint8_t VIF_GetStsOpenCover(void);
uint8_t VIF_GetStsBusFault(void);
uint8_t VIF_GetStsHostPowerCut(void);
uint8_t VIF_GetStsBackBatPowerCut(void);
uint8_t VIF_GetStsBackBatLowPower(void);
uint8_t VIF_GetStsRs485BaudChange(void);
uint8_t VIF_GetStsCanBaudChange(void);
uint8_t VIF_GetStsDevHerar(void);
uint8_t VIF_GetStsDevSleep(void);
uint8_t VIF_GetStsHostLowPower(void);
uint8_t VIF_GetStsCollision(void);
uint8_t VIF_GetStsRemove(void);
uint8_t VIF_GetStsPlcControlled(void);
uint8_t VIF_GetStsPosiVailed(void);
uint8_t VIF_GetStsCanTimerSendState(void);
uint8_t VIF_GetStsRs485TimerSendState(void);
uint8_t VIF_GetStsFlashfault(void);
uint8_t VIF_GetStsCAN0fault(void);
uint8_t VIF_GetStsCAN1fault(void);
uint8_t VIF_GetStsPositionModuleState(void);
uint8_t VIF_GetStsCAN0BUSOFF(void);
uint8_t VIF_GetStsCAN1BUSOFF(void);
uint8_t VIF_GetStsMCUConnect4GError(void);
uint32_t VIF_GetLatDeg(void);
uint32_t VIF_GetLonDeg(void);
uint16_t VIF_GetHight(void);
uint16_t VIF_GetDirection(void);
uint8_t VIF_GetYear(void);
uint8_t VIF_GetMon(void);
uint8_t VIF_GetDay(void);
uint8_t VIF_GetHour(void);
uint8_t VIF_GetMin(void);
uint8_t VIF_GetSec(void);
uint8_t VIF_GetRemainOil(void);
uint16_t VIF_GetBPSoc(void);
uint16_t VIF_GetCarSpeed(void);
uint32_t VIF_GetOdo(void);
uint8_t VIF_GetDoorStatusDrivingDoor(void);
uint8_t VIF_GetDoorStatusCopilotDoor(void);
uint8_t VIF_GetDoorStatusLeftRearDoor(void);
uint8_t VIF_GetDoorStatusRightRearDoor(void);
uint8_t VIF_GetDoorStatusRearCanopy(void);
uint8_t VIF_GetDoorStatusEngineCover(void);
uint8_t VIF_GetLockStatusRightRearLock(void);
uint8_t VIF_GetLockStatusLeftRearLock(void);
uint8_t VIF_GetLockStatusCopilotLock(void);
uint8_t VIF_GetLockStatusDrivingLock(void);
uint8_t VIF_GetRoofStatus(void);
uint8_t VIF_GetWindowStatusLeftFrontWindow(void);
uint8_t VIF_GetWindowStatusRightFrontWindow(void);
uint8_t VIF_GetWindowStatusLeftRearWindow(void);
uint8_t VIF_GetWindowStatusRightRearWindow(void);
uint8_t VIF_GetLampStautsHeadlights(void);
uint8_t VIF_GetLampStautsPositionlights(void);
uint8_t VIF_GetLampStautsNearlights(void);
uint8_t VIF_GetLampStautsRearfoglights(void);
uint8_t VIF_GetLampStautsFrontfoglights(void);
uint8_t VIF_GetLampStautsLeftlights(void);
uint8_t VIF_GetLampStautsRightlights(void);
uint8_t VIF_GetMcuVersion(uint8_t *pData,uint16_t &len);
uint8_t VIF_GetMotorStatus(void);
uint16_t VIF_GetMotorSpeed(void);
uint8_t VIF_GetEngineStatus(void);
uint16_t VIF_GetEngineSpeed(void);
uint8_t VIF_GetGearInfor(void);
uint8_t VIF_GetIgnInfo(void);
uint8_t VIF_GetHandParkStatus(void);
uint8_t VIF_GetBeltStatus(void);
uint8_t VIF_GetChargeStatus(void);
uint8_t VIF_GetChargeGunStatus(void);
uint8_t VIF_GetBreakStatus(void);
uint16_t VIF_GetRemainEndurance(void);
uint16_t VIF_GetBPVol(void);
uint16_t VIF_GetBPSur(void);
uint16_t VIF_GetMaintVol(void);
uint8_t VIF_GetPowerForceSts(void);
uint8_t VIF_GetCarIDState(void);
uint8_t VIF_GetCarGwNodeMiss(void);
/*
uint8_t VIF_GetEngineSystem(void);
uint8_t VIF_GetTCUSystem(void);
uint8_t VIF_GetEmissionSystem(void);
uint8_t VIF_GetACUSystem(void);
uint8_t VIF_GetESPSystem(void);
uint8_t VIF_GetABSSystem(void);
uint8_t VIF_GetEPSSystem(void);
uint8_t VIF_GetOilPressSystem(void);
uint8_t VIF_GetOilLowSystem(void);
uint8_t VIF_GetBreakFluidSystem(void);
uint8_t VIF_GetChargeSystem(void);
uint8_t VIF_GetBreakSystem(void);
uint8_t VIF_GetTyrePressSystem(void);
uint8_t VIF_GetStartStopSystem(void);
uint8_t VIF_GetLampSystem(void);
uint8_t VIF_GetELockSystem(void);
uint8_t VIF_GetEngineTempSytem(void);
uint8_t VIF_GetEPBSystem(void);
uint8_t VIF_GetUpperBeamSystem(void);
uint8_t VIF_GetNavigateSystem(void);
uint8_t VIF_GetCrashSystem(void);
uint8_t VIF_GetRoalSystem(void);
uint8_t VIF_GetBlindMonitorSystem(void);
uint8_t VIF_GetOperateAC(void);
uint8_t VIF_GetHighVolSystem(void);
uint8_t VIF_GetInsulationSystem(void);
uint8_t VIF_GetInterlockSystem(void);
uint8_t VIF_GetCellSystem(void);
uint8_t VIF_GetMotorSystem(void);
uint8_t VIF_GetEParkSystem(void);
uint8_t VIF_GetEQuantityForCellSystem(void);
uint8_t VIF_GetTempeForCellSystem(void);
uint8_t VIF_GetTempeForMotorSystem(void);
uint8_t VIF_GetNaviCnstSpdSystem(void);
uint8_t VIF_GetBattChargerSystem(void);
uint8_t VIF_GetACSystem(void);
uint8_t VIF_GetAuxSystem(void);
uint8_t VIF_GetEmergyBreakSystem(void);
uint8_t VIF_GetRadarSystem(void);
uint8_t VIF_GetElcSystem(void);
uint8_t VIF_GetLeftFrontTyrePress(void);
uint8_t VIF_GetLeftFrontTyreTemp(void);
uint8_t VIF_GetRightFrontTyrePress(void);
uint8_t VIF_GetRightFrontTyreTemp(void);
uint8_t VIF_GetLeftRearTyrePress(void);
uint8_t VIF_GetLeftRearTyreTemp(void);
uint8_t VIF_GetRightRearTyrePress(void);
uint8_t VIF_GetRightRearTyreTemp(void);
uint8_t VIF_GetDCDCSystem(void);
uint8_t VIF_GetBCMSystem(void);
uint8_t VIF_GetRelaySystem(void);
uint8_t VIF_GetTSROutSpdSystem(void);
uint8_t VIF_GetTSRLimSpdSystem(void);
uint8_t VIF_GetAEBinvolvedSystem(void);
uint8_t VIF_GetABSinvolvedSystem(void);
uint8_t VIF_GetASRinvolvedSystem(void);
uint8_t VIF_GetESPinvolvedSystem(void);
uint8_t VIF_GetDSMSystem(void);
uint8_t VIF_GetLeaveWheelPromp(void);
uint8_t VIF_GetACCStatus(void);
uint8_t VIF_GetAccSpdSystem(void);
uint8_t VIF_GetFCWSystem(void);
uint8_t VIF_GetFCWStatus(void);
uint8_t VIF_GetTimerForFCWSystem(void);
uint8_t VIF_GetBreakForFCWSystem(void);
uint8_t VIF_GetLDWStatus(void);
uint8_t VIF_GetLDWSystem(void);
uint8_t VIF_GetWheelForLDWSystem(void);
uint8_t VIF_GetLKAStatus(void);
uint8_t VIF_GetLKAinvolvedSystem(void);
uint8_t VIF_GetLKADriverPromp(void);
uint8_t VIF_GetLKADriverForWheel(void);
uint8_t VIF_GetBSDStatus(void);
uint8_t VIF_GetLeftBSDSystem(void);
uint8_t VIF_GetRightBSDSystem(void);
uint8_t VIF_GetTimeForWheelBSD(void);
uint8_t VIF_GetTimeForBreakBSD(void);
uint8_t VIF_GetAccelPedalForBSD(void);
uint8_t VIF_GetLeftForFlowSystem(void);
uint8_t VIF_GetRightForFlowSystem(void);
uint8_t VIF_GetRearForFlowSystem(void);
uint8_t VIF_GetBreakForFlowSystem(void);
uint8_t VIF_GetAccelPedalForFlow(void);
uint8_t VIF_GetLeftAUXSystem(void);
uint8_t VIF_GetRightAUXSystem(void);
uint8_t VIF_GetRearAUXSystem(void);
uint8_t VIF_GetBreakAUXSystem(void);
uint8_t VIF_GetAccelPedalAUXSystem(void);
uint8_t VIF_GetDoorcrash(void);
uint8_t VIF_GetEnginestop(void);
uint8_t VIF_GetCarcrash(void);
uint8_t VIF_GetTrailCarEvent(void);
uint8_t VIF_GetSkyNotCloseAlm(void);
*/
uint8_t VIF_GetResponseID(void);
uint8_t VIF_GetResponseNumber(void);
uint8_t VIF_GetResponseResult(void);

uint8_t VIF_GetCtrlCode(void);
uint8_t VIF_GetCtrlResult(void);

#endif
	
