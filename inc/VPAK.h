#ifndef VPAK_H
#define VPAK_H

#include "common.h"
#include "VCTL.h"
#include "VUPG.h"


#define VPAK_RemoteCtrl_DATA_NUM			21
#define VPAK_RemoteCtrl_DATA_LEN			256

#define VPAK_REMOTECTRL_CMDID				0x84
#define VPAK_VeichleConfig_CMDID			0x82
#define VPAK_HostStatusReport_CMDID			0x85
#define VPAK_HostACK_CMDID					0x81
#define VPAK_UpgradeDateAACK_CMDID			0x86
#define VPAK_SendUpgrade_CMDID				0x8A



#pragma pack(1)



typedef struct  VPAK_InfoHead_Tag												 
{	
	uint8_t			   		HeadFlag;
	uint16_t			   	BodyLen; 	 
	uint8_t			   		CmdID;	
	uint16_t			   	SerialNumber;   
	uint8_t			   		Attr[2]; 						  
}VPAK_InfoHead_ST;


typedef struct  VPAK_SlaveHeart_Tag												  
{  
	VPAK_InfoHead_ST		Head;	   	   
}VPAK_SlaveHeart_ST;


typedef struct  VPAK_RemoteCtrl_Tag												  
{  
	VPAK_InfoHead_ST		Head;	   	   
	VCTL_RemoteCtrl_ST		Parameter;
	uint16_t			   	CheckCrc;
	uint8_t			   		EndFlag;
}VPAK_RemoteCtrl_ST;

typedef struct  VPAK_VeichleConfig_Tag												  
{  
	VPAK_InfoHead_ST			Head;	   	   
	uint8_t	  					ParameterTotal;  
	VCTL_VeichleConfig_ST		ParameterList[46];
	uint16_t			   		CheckCrc;
	uint8_t			   			EndFlag;
}VPAK_VeichleConfig_ST;

typedef struct  VPAK_HostACK_Tag												  
{  
	VPAK_InfoHead_ST			Head;	   	   
	uint8_t	  					ACKID;  
	uint16_t					ACKNumber;
	uint8_t						Result;
	uint16_t			   		CheckCrc;
	uint8_t			   			EndFlag;
}VPAK_HostACK_ST;


typedef struct  VPAK_HostStatusReport_Tag												  
{  
	VPAK_InfoHead_ST		Head;	   	   
	uint16_t	  			HostSerialNumber;  
	uint8_t	  				HardwareSts;
	uint8_t			   		PhoneSts;
	uint8_t			   		VPAK_4GValue;
	uint8_t	  				WifiFault;
	uint8_t			   		SIMSts;
	uint8_t	  				USBFault;
	uint8_t	  				BTFault;
	uint8_t	  				LogSwitch;
	uint8_t			   		HostEngineSVersions[11];
	uint16_t			   	CheckCrc;
	uint8_t			   		EndFlag;
}VPAK_HostStatusReport_ST;



typedef struct  VPAK_SendUpgradeReq_Tag												  
{  
	VPAK_InfoHead_ST		Head;	   	   
	uint8_t  				Type;
	uint8_t	 				filelen[4];
	uint8_t 	 				crc[2];
	uint8_t 				filenameLen;
	uint8_t 				filename[13];
	uint8_t 				fileverLen;
	uint8_t 				fileversion[4];

	uint16_t			   	CheckCrc;
	uint8_t			   		EndFlag;
}VPAK_SendUpgradeReq_ST;


typedef struct  VPAK_SendUpgradeData_Tag												  
{  
	VPAK_InfoHead_ST		Head;
	/*
	uint8_t AckType;
	uint32_t offset;
	uint16_t reqLen;
	uint8_t buf[512];
	*/
	OTA_SendMcuData_ST data;	
	uint16_t		   CheckCrc;
	uint8_t			   EndFlag;
	
}VPAK_SendUpgradeData_ST;




typedef union VPAK_RemoteCtrlInformation_Tag
{
	uint8_t Buf[VPAK_RemoteCtrl_DATA_LEN];	
	VPAK_RemoteCtrl_ST			stVpakCtrl;
	VPAK_VeichleConfig_ST		stVpakConfig;
	VPAK_HostStatusReport_ST	stVpakHostSts;
	VPAK_SlaveHeart_ST			stVpakSlaveHeart;
	VPAK_HostACK_ST				stVpakHostACK;
	VPAK_SendUpgradeReq_ST      stVpakSendUpgReq;
	VPAK_SendUpgradeData_ST		stVpakSendUpgData;
}VPAK_RemoteCtrlInformation_UN;

#pragma pack()

uint8_t VPAK_Init(void);
uint8_t VPAK_DeInit(void);
uint8_t VPAK_PackageData(uint16_t *len, uint8_t PAKID, uint8_t* pData, uint8_t ACKID = 0, uint8_t ACKRes = 0);

uint8_t vpak_PackageHostACK(uint16_t *len, uint8_t *buf, uint8_t ACKID, uint16_t serialNO);
uint8_t vpak_PackageHostData(uint16_t *len, uint8_t *buf, uint16_t serialNO);
uint8_t vpak_PackageConfigData(uint16_t *len, uint8_t *buf, uint8_t Total, uint16_t serialNO);
uint8_t vpak_PackageCtrlData(uint16_t *len, uint8_t *buf, uint8_t index, uint16_t serialNO);
uint8_t vpak_PackageSendUpgReq(uint16_t *len, uint8_t *buf, uint16_t serialNO);
uint8_t vpak_PackageSendUpgData(uint16_t *len, uint8_t *buf, uint16_t serialNO);

uint16_t vpak_Crc16Check(uint8_t *pBuf,uint8_t len);
uint8_t vpak_Analysis(uint8_t *outBuf,uint8_t *inBuf,uint16_t inLen ,uint16_t *outLen);






#endif

