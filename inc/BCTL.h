#ifndef BCTL_H
#define BCTL_H

//add chen
#include "common.h"

#if 1



#define BCTL_CTRL_DATA_LEN  256
#define BCTL_HEAD_DATA_LEN  7
#define BCTL_INFO_CMDID  3


#define BCTL_BLE_GENERAL_ACK_CMD       	0x01	//从机通用应答
#define BCTL_BLE_ORDEN_CMD				0x02
#define BCTL_BLE_CHECK_ACK_CMD	        0x03
#define BCTL_BLE_CTRL_ACK_CMD			0x04//从机控制应答
#define BCTL_REQ_UPGRADE_DATA_CMD			0x88
#define BCTL_SEND_COMPLETE       0x06
#define BCTL_SEND_COMPLETE       0x0A

//#define START_SYNC_CMD          0x00	/




#pragma pack(1)

typedef struct  BCTL_InfoHead_Tag												 
{	
	uint8_t			   		HeadFlag;
	uint16_t			   	BodyLen; 	
	uint8_t			   		CmdID;	
	uint16_t			   	SerialNumber;   
	uint8_t			   		Attr[2]; 			
	
}BCTL_InfoHead_ST;




typedef struct  BCTL_BLEGeneralACK_Tag													
{  
	 
	uint8_t 		ACKID; 	 
	uint16_t		ACKNumber;  
	uint8_t 		ACKResult; 	
}BCTL_BLEGeneralACK_ST;
 


typedef struct  BCTL_RemoteCtrlACK_Tag												  
{  	   	   
	uint8_t	  				CtrlCode;  
	uint8_t	  				CtrlResult;
}BCTL_RemoteCtrlACK_ST;
   
typedef struct	BCTL_TimeSync_Tag												  
{		   
	uint8_t 				Byte[5];
}BCTL_TimeSync_ST;
typedef struct	BCTL_BLEOrder_Tag												  
{		   
	uint8_t 				Order;
}BCTL_BLEOrder_ST;

typedef struct	BCTL_BLECtrl_Tag												  
{		   
	uint8_t 				Ctrl;
}BCTL_BLECtrlACK_ST;

typedef struct	BLECheckACK_Tag												  
{		   
	uint8_t 				CMDID;
	uint8_t 				SubitemTotal;
	uint8_t 				Parameter;
}BCTL_BLECheckACK_ST;


typedef union BCTL_InfoBody_Tag
{
	BCTL_BLECheckACK_ST				BLECheckACK;
	BCTL_BLEOrder_ST				BLEOrder;
	BCTL_BLEGeneralACK_ST			BLEGeneralACK;
	BCTL_BLECtrlACK_ST				BLECtrlACK;
}BCTL_InfoBody_UN;

typedef struct BCTL_RemoteCtrl_Tag
{

	BCTL_InfoHead_ST				Head;	
	BCTL_InfoBody_UN    			Body;
}BCTL_RemoteCtrl_ST;



typedef union BCTL_RemoteCtrlInformation_Tag
{
	uint8_t Buf[BCTL_CTRL_DATA_LEN];	
	BCTL_RemoteCtrl_ST    	stBctlInfo;
}BCTL_RemoteCtrlInformation_UN;


#pragma pack()

uint8_t BCTL_Init(void);
uint8_t BCTL_DeInit(void);
uint8_t BCTL_Parse(uint8_t *pBuf,uint8_t len, BCTL_RemoteCtrlInformation_UN *mcuVi);
uint8_t BCTL_Printf(void);

uint16_t BCTL_GetBodyLen(void);
uint8_t BCTL_GetCmdID(void);
uint16_t BCTL_GetSerialNumber(void);
uint8_t BCTL_GetAttr(uint8_t *pData,uint16_t &len);	
#endif

#endif


