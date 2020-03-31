#ifndef _VUPG_H_
#define _VUPG_H_
#include "common.h"

#include "QueueCommon.h"

#define UPG_USE_POINTER   1


#ifndef VUPG_DEBUG
	#define VUPG_DEBUG 0
#endif

#if VUPG_DEBUG 	
	#define VUPGLOG(format,...) printf("== VUPG == FILE: %s, FUN: %s, LINE: %d "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define VUPG_NO(format,...) printf(format,##__VA_ARGS__)
#else	
	#define VUPGLOG(format,...)
	#define VUPG_NO(format,...)
#endif




#pragma pack(1)

typedef struct  VUPG_InfoHead_Tag												 
{	
	uint8_t			   		HeadFlag;
	uint16_t			   	BodyLen; 	 
	uint8_t			   		CmdID;	
	uint16_t			   	SerialNumber;   
	uint8_t			   		Attr[2]; 						  
}VUPG_InfoHead_ST;

typedef struct V_UPG_Info_Tag
{
	uint8_t  Type;
	uint32_t	 Len;
	uint16_t 	 crc;
	uint8_t filenameLen;
	uint8_t filename[0];
	uint8_t fileverLen;
	uint8_t fileversion[0];	
}V_UPG_Info_ST;

typedef struct OTA_UpgReqData_Tag
{  
 
  uint32_t m_offset;
  uint16_t m_ReqLen;
  
}OTA_UpgReqData_ST;

typedef struct OTA_UpgHostSendData_Tag
{  
  uint8_t ackType;
  OTA_UpgReqData_ST data;
  
}OTA_UpgHostSendData_ST;



typedef struct OTA_SendMcuData_Tag
{  
  OTA_UpgHostSendData_ST Req;
  uint8_t  buf[512];
  
}OTA_SendMcuData_ST;


typedef struct  VUPG_SendUpgradeData_Tag												  
{  
	VUPG_InfoHead_ST		Head;	
	OTA_SendMcuData_ST 		data;	
	uint16_t		   		CheckCrc;
	uint8_t			   		EndFlag;
	
}VUPG_SendUpgradeData_ST;

typedef union  VUPG_UNSendUpgradeData_Tag												  
{  
	uint8_t buf[MSG_MAX_LEN];
	VUPG_SendUpgradeData_ST stInfo;
	
}VUPG_UNSendUpgradeData_UN;



#pragma pack()

//extern uint16_t VUPG_GetFileCRC(void);
//extern uint32_t VUPG_GetFileLen(void);

#if UPG_USE_POINTER
OTA_SocSendInfo_ST*  VUPG_GetUpgHostReq(void);

#else
extern uint8_t  VUPG_GetUpgHostReq(OTA_SocSendInfo_ST *pData);
#endif

extern uint8_t VUPG_Init(void);
extern uint8_t VUPG_GetInitSts(void);

extern uint8_t UPG_PackageSendUpgData(uint16_t *len, uint8_t *buf, uint16_t serialNO);

static uint8_t vupg_Analysis(uint8_t *outBuf,uint8_t *inBuf,uint16_t inLen ,uint16_t *outLen);

extern uint8_t  VUPG_UpdateUpgHostReq(OTA_SocSendInfo_ST *pData);

extern uint8_t  VUPG_UpdateSlaveReqData(OTA_UpgReqData_ST *pData);
#if UPG_USE_POINTER
extern OTA_UpgReqData_ST* VPUG_GetSlaveReqData(void);
#else
extern uint8_t VPUG_GetSlaveReqData(OTA_UpgReqData_ST* pos,uint32_t* len);
#endif
extern uint8_t  VUPG_UpdateUpgHostData(OTA_SendMcuData_ST *pData);
//extern uint8_t  VUPG_GetUpgHostData(OTA_SendMcuData_ST *pData);
extern OTA_SendMcuData_ST*  VUPG_GetUpgHostData(void);



#endif
