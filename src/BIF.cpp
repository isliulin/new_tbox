#include "BIF.h"

#if 1
BIF_VeichleInformation_UN g_stBif;

extern uint8_t BIF_InitSts = FALSE;
uint8_t BIF_Init(void)
{
	/*if uart com is failure ,should be false*/
	BIF_InitSts = TRUE;
	memset(g_stBif.Buf,0,sizeof(g_stBif));
	return BIF_InitSts;
}
	
uint8_t BIF_DeInit(void)
{

	BIF_InitSts = FALSE;
	return BIF_InitSts;
}

uint8_t bif_Analysis(uint8_t *outBuf,uint8_t *inBuf,uint8_t inLen,uint8_t *outLen)
{
	uint8_t i = 0;
	uint8_t tempLen = 0;
	uint8_t ret = RET_FAIL;
	if(BIF_InitSts == FALSE)
	{
		return ret;
	}
	else if((NULL == outBuf) ||(NULL == inBuf) ||(NULL == outLen) || (inLen > BIF_BLESTS_DATA_LEN))
	{	
		ret = RET_INVALID;
	}
	else
	{
		
		for(i = 0; i < inLen; i ++)
		{
			
			if(0x7d == *(inBuf + i))
			{
				i ++;
				if(0x01 == *(inBuf + i))
				{
					*(outBuf + tempLen) = 0x7d;
				}
				else if(0x02 == *(inBuf + i))
				{
					*(outBuf + tempLen) = 0x7f;
				}
				else if(0x03 == *(inBuf + i))
				{
					*(outBuf + tempLen) = 0x7e;
				}
				else
				{
					
				}
				tempLen ++;
					
			}
			else
			{
				*(outBuf + tempLen) = *(inBuf + i);
				tempLen ++;
			}
		}
		*outLen = tempLen;
		ret = RET_OK;
	}
	return ret;
}

uint16_t bif_Crc16Check(uint8_t *pBuf,uint8_t len)
{
	uint16_t ui16InitCrc = 0xffff;
	uint16_t ui16Crc = 0;
	uint16_t ui16i;
	uint8_t ui8j;
	uint8_t ui8ShiftBit;
	
	for(ui16i = 0;ui16i<len;ui16i++)
	{
		
		ui16InitCrc ^= pBuf[ui16i];
		for(ui8j=0;ui8j<8;ui8j++)
		{
		   ui8ShiftBit = ui16InitCrc&0x01;
		   ui16InitCrc >>= 1;
		if(ui8ShiftBit != 0)
		   {
				ui16InitCrc ^= 0xa001;
				
		   }			
		}
	}
	ui16Crc = ui16InitCrc;
	return ui16Crc;

	
}


uint8_t BIF_Parse(uint8_t *pBuf, uint8_t len, BIF_VeichleInformation_UN *bleBif)
	
{
	uint8_t ret = RET_FAIL;
	uint8_t pData[sizeof(BIF_VeichleInformation_UN)] = {0};
	uint16_t checkCrc = 0;
	uint8_t bodylen = 0;
	uint8_t datalen = 0;
	int i= 0;

	if(BIF_InitSts == FALSE)
	{
		return ret;
	}
	else if((NULL == pBuf) || (len >= BIF_BLESTS_DATA_LEN))
	{	
		return RET_INVALID;
	}
	else
	{
		bif_Analysis(pData,pBuf,len,&datalen);
		bodylen = (pData[1] << 8) + pData[2];
		checkCrc = (pData[sizeof(BIF_InfoHead_ST) + bodylen] << 8) 
					+ pData[sizeof(BIF_InfoHead_ST)+ bodylen + 1];
		if(checkCrc != bif_Crc16Check(&pData[1],bodylen- 1 +sizeof(BIF_InfoHead_ST)))
		{
		    printf("%04x ",checkCrc);
		    printf("chen chen chen chen chen chen chen chen chen ===========================================");
			ret = RET_INVALID;
		}
		else
		{
			memcpy(g_stBif.Buf ,pData,datalen);
			memcpy(bleBif->Buf ,g_stBif.Buf,datalen);
			ret = RET_OK;
		}
		
	}

	return ret;
}



uint16_t BIF_GetBodyLen(void)
{
	return g_stBif.stBifInfo.Head.BodyLen;
}

uint8_t BIF_GetCmdID(void)
{
	return g_stBif.stBifInfo.Head.CmdID;
}

uint16_t BIF_GetSerialNumber(void)
{
	return g_stBif.stBifInfo.Head.SerialNumber;
}
uint8_t BIF_GetAttr(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((TRUE == BIF_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{
		pData = g_stBif.stBifInfo.Head.Attr;
		len = 2;
		ret = RET_OK;
	}
	return ret;
}

uint8_t BIF_GetMAC(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((FALSE == BIF_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{
		len = 6;		
		memcpy(pData ,g_stBif.stBifInfo.Body.BleState.MAC,len);
		ret = RET_OK;
	}
	return ret;
}

uint8_t BIF_GetWorkSts(void)
{
	return g_stBif.stBifInfo.Body.BleState.WorkSts;
}

uint8_t BIF_GetLinkSts(void)
{
	return g_stBif.stBifInfo.Body.BleState.LinkSts;
}

uint8_t BIF_GetSecretKey(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((FALSE == BIF_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{
		len = 6;		
		memcpy(pData ,g_stBif.stBifInfo.Body.BleState.SecretKey,len);
		ret = RET_OK;
	}
	return ret;
}

uint8_t BIF_GetBroadName(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((FALSE == BIF_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{
		len = 8;
		memcpy(pData, g_stBif.stBifInfo.Body.BleState.BroadName, len);
		ret = RET_OK;
	}
	return ret;
}

#endif

