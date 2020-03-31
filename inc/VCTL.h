#ifndef VCTL_H
#define VCTL_H


typedef enum SubitemCode_Tag
{
	//车身
	VehicleBody_LockID 				= 	0x01,	//车锁
	VehicleBody_WindowID 			= 	0x02,	//车窗
	VehicleBody_SunroofID 			= 	0x03,	//天窗
	VehicleBody_TrackingCarID		=	0x04,	//寻车
	VehicleBody_LowbeamID			=	0x05,	//近光灯
	Airconditioner_ControlID 		= 	0x06,	//空调控制
	Power_SwitchID 					= 	0x07,	//动力状态启停
	SleepTime_ID                	=   0x0A,  //sleep time
	SleepNow_ID                 	=   0x0B,  //sleepnow
	EngineState_SwitchID			=	0x0F,	//发动机状态启停

	DriverHeat_ID            		=   0x10,  
	CopilotHeat_ID              	=   0x11, 
	ChargeNow_ID                	=   0x12,
	ChargeOrder_ID              	=   0x13,
	ChargeUregnt_ID             	=   0x14,
	WifiSts_ID                 		=   0x15,
	VehicleAutoOUT_ID              	=   0x16,
   	VehicleBody_LuggageCarID    	=   0x17,
	VehicleSeat_DrivingSeatMomeryID	= 	0x18

}SubitemCode_ENUM;

typedef enum AcpConfigItemID_Tag
{
	ReportTime_ID 	= 	0x02,	
	SetCar_ID 		= 	0x60,	

}AcpConfigItemID_ENUM;


#pragma pack(1)

typedef struct  VCTL_RemoteCtrl_Tag												  
{  
	uint8_t 				CtrlCode;  
//	uint16_t				CtrlSerialNumber;
	uint8_t 				CtrlParameter;
}VCTL_RemoteCtrl_ST;


typedef struct  VCTL_VeichleConfig_Tag												  
{  
	uint8_t 				ParameterID;
	uint8_t 				ParameterLen;
	uint8_t 				ParameterValue;
}VCTL_VeichleConfig_ST;
#pragma pack()
uint8_t VCTL_Dispatch(uint8_t *CtlTotal, uint8_t *CfgTotal);

VCTL_RemoteCtrl_ST VCTL_GetRemoteCtrl(uint8_t index);

VCTL_VeichleConfig_ST VCTL_GetVeichleConfig(uint8_t index);


#endif

