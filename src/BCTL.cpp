#include "BCTL.h"
#if 1



uint8_t 						BCTL_InitSts = FALSE;
BCTL_RemoteCtrlInformation_UN 	g_stBctl;


uint8_t BCTL_Init(void)
{
	/*if uart com is failure ,should be false*/
	BCTL_InitSts = TRUE;
	memset(g_stBctl.Buf,0,sizeof(g_stBctl));
	return BCTL_InitSts;
}
	
uint8_t BCTL_DeInit(void)
{

	BCTL_InitSts = FALSE;
	return BCTL_InitSts;
}

uint8_t BCTL_Printf(void)
{
	int i = 0;
	for(i = 0; i < 150; i ++)
	{
		printf("Buf[%d]:%02X\n", i, g_stBctl.Buf[i]);
	}
	
	return 0;
	
}

uint8_t bctl_Analysis(uint8_t *outBuf ,uint8_t *inBuf ,uint8_t inLen ,uint8_t *outLen)
{
	uint8_t i = 0;
	uint8_t tempLen = 0;
	uint8_t ret = RET_FAIL;
	if(BCTL_InitSts == FALSE)
	{
		return ret;
	}
	else if((NULL == outBuf) ||(NULL == inBuf) ||(NULL == outLen) || (inLen > BCTL_CTRL_DATA_LEN))
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

uint16_t bctl_Crc16Check(uint8_t *pBuf,uint8_t len)
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
/*
uint8_t bctl_AllotInfo(uint8_t *pBuf,uint8_t len)
{
	uint8_t cmdID = 0;

	cmdID = *(pBuf + BCTL_INFO_CMDID);
	switch(cmdID)
	{
	case BCTL_REPROT_VECIHLE_INFORMATION:
		memcpy(g_stBctl.Buf ,pBuf,len);
		break;
	default:
    	break;
	}
}
*/
uint8_t BCTL_Parse(uint8_t *pBuf, uint8_t len, BCTL_RemoteCtrlInformation_UN *mcuBctl)
	
{
	uint8_t ret = RET_FAIL;
	uint8_t pData[BCTL_CTRL_DATA_LEN] = {0};
	uint16_t checkCrc = 0;
	uint8_t bodylen = 0;
	uint8_t datalen = 0;
	int i= 0;

	if(BCTL_InitSts == FALSE)
	{
		return ret;
	}
	else if((NULL == pBuf) || (len >= BCTL_CTRL_DATA_LEN))
	{	
		return RET_INVALID;
	}
	else
	{
		bctl_Analysis(pData,pBuf,len,&datalen);
		bodylen = (pData[1] << 8) + pData[2];
		checkCrc = (pData[1 + BCTL_HEAD_DATA_LEN + bodylen] << 8) 
					+ pData[1 + BCTL_HEAD_DATA_LEN + bodylen + 1];
		if(checkCrc != bctl_Crc16Check(&pData[1],bodylen+BCTL_HEAD_DATA_LEN))
		{
			ret = RET_INVALID;
		}
		else
		{
			memcpy(g_stBctl.Buf ,pData,datalen);
			memcpy(mcuBctl->Buf ,g_stBctl.Buf,datalen);
			ret = RET_OK;
		}
		
	}

	return ret;
}

uint16_t BCTL_GetBodyLen(void)
{
	return g_stBctl.stBctlInfo.Head.BodyLen;
}

uint8_t BCTL_GetCmdID(void)
{
	return g_stBctl.stBctlInfo.Head.CmdID;
}
uint16_t BCTL_GetSerialNumber(void)
{
	return g_stBctl.stBctlInfo.Head.SerialNumber;
}
uint8_t BCTL_GetAttr(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((TRUE == BCTL_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{
		pData = g_stBctl.stBctlInfo.Head.Attr;
		len = 2;
		ret = RET_OK;
	}
	return ret;
}

#endif


