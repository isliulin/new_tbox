#include "VUPG.h"
#include "tboxComnFun.h"
#include "VPAK.h"
OTA_SocSendInfo_ST m_stUpgHostReq;
OTA_UpgReqData_ST  m_stUpgSlaveReq;
//OTA_SendMcuData_ST m_stUpgHostdata;

uint8_t upg_Inits = FALSE;


VUPG_UNSendUpgradeData_UN m_stHostSend;

#if 0
uint32_t VUPG_GetFileLen(void)
{
	return m_stUpgHostReq.m_FileLen;
}


uint16_t VUPG_GetFileCRC(void)
{
	return m_stUpgHostReq.m_nCRC;
}

#endif

uint8_t VUPG_Init(void)
{

	upg_Inits = TRUE;
	memset(&m_stUpgHostReq,0,sizeof(OTA_SocSendInfo_ST));
	memset(&m_stUpgSlaveReq,0,sizeof(OTA_UpgReqData_ST));
	memset(&m_stHostSend,0,sizeof(VUPG_SendUpgradeData_ST));
	return upg_Inits;
	
}

uint8_t VUPG_GetInitSts(void)
{

	return upg_Inits;
}


uint8_t  VUPG_UpdateUpgHostReq(OTA_SocSendInfo_ST *pData)
{
	if(pData == NULL) return RET_INVALID;
	else m_stUpgHostReq = *pData;
	return RET_OK;
}
#if (UPG_USE_POINTER==0)

uint8_t  VUPG_GetUpgHostReq(OTA_SocSendInfo_ST *pData)
{
	pData = &m_stUpgHostReq;
	return RET_OK;
}

#else
OTA_SocSendInfo_ST*  VUPG_GetUpgHostReq(void)
{
	return &m_stUpgHostReq;
}
#endif


uint8_t  VUPG_UpdateSlaveReqData(OTA_UpgReqData_ST *pData)
{

	uint32_t offset;
	uint16_t reqLen = 0;
	if(pData == NULL )return RET_INVALID;

	uint8_t HHB;
	uint8_t HLB;
	uint8_t LHB;
	uint8_t LLB;

	offset = pData->m_offset;
	reqLen = pData->m_ReqLen;

	HHB = (offset >>24 )&0XFF;
	HLB = (offset >>16 )&0XFF;
	LHB = (offset >>8 )&0XFF;
	LLB = (offset >>0 )&0XFF;

	//printf("\r\nVUPG_UpdateUpgData %0x\r\n",offset);
	//printf("VUPG_UpdateUpgData %0x\r\n",reqLen);

	#if 1

	m_stUpgSlaveReq.m_offset = (((LLB <<24)&0xFF000000) |\
								((LHB <<16)&0xFF0000)|\
								((HLB << 8)&0xFF00) |\
								((HHB << 0)&0xFF)
								);

	LHB = (reqLen >> 8 )&0XFF;								
	LLB = (reqLen >> 0 )&0XFF;


	m_stUpgSlaveReq.m_ReqLen = (((LLB << 8)&0xFF00) |\
								((LHB << 0)&0xFF)
								);

   #endif

	VUPGLOG("\r\nVUPG_UpdateUpgData %0x\r\n",m_stUpgSlaveReq.m_offset);
	VUPGLOG("VUPG_UpdateUpgData %0x\r\n",m_stUpgSlaveReq.m_ReqLen);

	return RET_OK;
}


#if (UPG_USE_POINTER==0)
uint8_t VPUG_GetSlaveReqData(OTA_UpgReqData_ST* pos,uint32_t* len)
{	
		pos = &m_stUpgSlaveReq;
		
		if(len == NULL)
			return RET_INVALID;
		else
		*len = sizeof(OTA_UpgReqData_ST);
		
		return RET_OK;
}
#else
OTA_UpgReqData_ST* VPUG_GetSlaveReqData(void)
{	
		return &m_stUpgSlaveReq;
}

#endif
uint8_t  VUPG_UpdateUpgHostData(OTA_SendMcuData_ST *pData)
{
	uint8_t HHB;
	uint8_t HLB;
	uint8_t LHB;
	uint8_t LLB;
	
	if(pData == NULL) return RET_INVALID;
	 else m_stHostSend.stInfo.data = *pData;
	 
#if 1
	HHB = (pData->Req.data.m_offset >>24)&0xFF;
	HLB = (pData->Req.data.m_offset >>16)&0xFF;
	LHB = (pData->Req.data.m_offset >>8)&0xFF;
	LLB = (pData->Req.data.m_offset >>0)&0xFF;

	m_stHostSend.stInfo.data.Req.data.m_offset= (((LLB <<24)&0xFF000000) |\
										((LHB <<16)&0xFF0000)|\
										((HLB << 8)&0xFF00) |\
										((HHB << 0)&0xFF)
										);

	LHB = (pData->Req.data.m_ReqLen >> 8 )&0XFF;								
	LLB = (pData->Req.data.m_ReqLen >> 0 )&0XFF;


	m_stHostSend.stInfo.data.Req.data.m_ReqLen = (((LLB << 8)&0xFF00) |\
										((LHB << 0)&0xFF)
										);
	#endif

	return RET_OK;
}


OTA_SendMcuData_ST*  VUPG_GetUpgHostData(void)
{
	return &m_stHostSend.stInfo.data;
}




uint8_t vupg_Analysis(uint8_t *outBuf,uint8_t *inBuf,uint16_t inLen ,uint16_t *outLen)
{
	uint16_t i = 0;
	uint16_t tempLen = 0;
	uint8_t ret = RET_FAIL;

 if((NULL == outBuf) ||(NULL == inBuf) || (inLen > MSG_MAX_LEN))
	{	
		ret = RET_INVALID;
	}
	else
	{
		*(outBuf + tempLen) = 0x7e;
		tempLen ++;
		for(i = 1; i < inLen + 1; i ++)
		{
			if(0x7d == *(inBuf + i))
			{
				*(outBuf + tempLen) = 0x7d;				
				tempLen ++;	
				*(outBuf + tempLen) = 0x01;				
				tempLen ++;	
			}
			else if(0x7e == *(inBuf + i))
			{
				*(outBuf + tempLen) = 0x7d;				
				tempLen ++;	
				*(outBuf + tempLen) = 0x02;				
				tempLen ++;	
			}
			else
			{
				*(outBuf + tempLen) = *(inBuf + i);
				tempLen ++;
			}
		}
		*(outBuf + tempLen) = 0x7e;
		tempLen ++;
		
		*outLen = tempLen;
		ret = RET_OK;

	}
	

	return ret;
}



uint8_t UPG_PackageSendUpgData(uint16_t *len, uint8_t *buf, uint16_t serialNO)
{
	uint8_t Value = 0 ;
	uint16_t tempLen = 0 ;
	uint16_t tempCount = 0 ;
	uint8_t	ret = RET_FAIL;
	static uint8_t CollectPacketSN = 1;
	uint16_t nLen = 0;
	uint16_t crc = 0;
	OTA_SendMcuData_ST* pData = NULL;

	uint8_t *pos  = (uint8_t *)&m_stHostSend.buf;

	if(len == NULL) VUPGLOG("null");
	

	m_stHostSend.stInfo.Head.HeadFlag	 	= 0x7e;
	m_stHostSend.stInfo.Head.CmdID			= VPAK_UpgradeDateAACK_CMDID;
	
	m_stHostSend.stInfo.Head.SerialNumber  = ((serialNO << 8) & 0xFF00) |
											 ((serialNO  >> 8) & 0x00FF);
	
	m_stHostSend.stInfo.Head.Attr[0]		= 0x00;
	m_stHostSend.stInfo.Head.Attr[1]		= 0x00;

	nLen = sizeof(OTA_SendMcuData_ST);
	m_stHostSend.stInfo.Head.BodyLen = ((nLen << 8) & 0xFF00) | ((nLen >> 8) & 0x00FF);
	crc = Crc16Check((unsigned char *)(pos +1 ),nLen + 7,0xffff);
	m_stHostSend.stInfo.EndFlag = 0x7e;
	m_stHostSend.stInfo.CheckCrc  = ((crc << 8) & 0xFF00) |\
							  		((crc >> 8) & 0x00FF);
#if 0
				printf("[SUCCESS] UPG BEFORE\r\n");
				printf("\r\n");
				printf("UPG######UPG_PackageSendUpgData#######\n");
				for(int i =0; i< nLen + MCU_PTC_HEAD_LEN+MCU_PTC_TAIL_LEN; i++)
				printf("%02x ",pos[i]);
				
				printf("\r\n");
				
				printf("UPG#####UPG_PackageSendUpgData#########\n");
#endif

	VUPGLOG("UPG######len:%d#######\n",nLen);

	ret = vupg_Analysis(buf,(uint8_t *)&(m_stHostSend.buf),nLen + 9,&tempLen);	
	
	*len = tempLen;
	
#if 1
			VUPGLOG("[SUCCESS] UPG after\r\n");
			VUPGLOG("\r\n");
			VUPGLOG("UPG######upg_Analysis#######\n");
			for(int i =0; i< nLen + MCU_PTC_HEAD_LEN + MCU_PTC_TAIL_LEN; i++)				
				VUPGLOG("%02x ",pos[i]);			
			VUPGLOG("\r\n");			
			VUPGLOG("UPG#####after#########\n");
			
#endif
			
	return ret;
}




