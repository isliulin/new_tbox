#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/time.h>
#include "json.h"
#include "framework.h"
#include "TCTL.h"


fwTCTL_RemoteCtrl_UN  g_stTctl;
uint8_t Tctl_InitSts = FALSE;


uint8_t TCTL_Init(void)
{
	/*if uart com is failure ,should be false*/
	Tctl_InitSts = TRUE;
	memset(g_stTctl.Buf,0,sizeof(g_stTctl));
	return Tctl_InitSts;
}
	
uint8_t TCTL_DeInit(void)
{

	Tctl_InitSts = FALSE;
	return Tctl_InitSts;
}

uint8_t TCTL_Receive(uint8_t *str, uint16_t dataLen)
{
    uint8_t ret = RET_FAIL;
    if(Tctl_InitSts == FALSE)
    {
        return ret;
    }
    else if((NULL == str) || (FALSE == Tctl_InitSts))
    {   
        return RET_INVALID;
    }
    else
    {
        memcpy(g_stTctl.Buf ,str,dataLen);
		ret = RET_OK;
    }
    return ret;
}

	
/*uint8_t TCTL_Parse(uint8_t *pBuf, uint8_t len)
{
		uint8_t ret = RET_FAIL;
		uint8_t pData[TCTL_RemoteCtrl_DATA_LEN] = {0};
		uint16_t checkCrc = 0;
		uint8_t bodylen = 0;
		uint8_t datalen = 0;
		int i= 0;
	
		if(Tctl_InitSts == FALSE)
		{
			return ret;
		}
		else if((NULL == pBuf) || (len > TCTL_RemoteCtrl_DATA_LEN))
		{	
			return RET_INVALID;
		}
		else
		{
			for(i = 0; i < len; i++)
			{
				printf("pBuf[%d]:%02x\n",i,*(pBuf + i));
			}
			bodylen = (pData[1] << 8) + pData[2];
			memcpy(g_stTctl.Buf ,pData,datalen);
			ret = RET_OK;

		}
	
		return ret;

}*/



uint8_t TCTL_GetCtrlTotal(void)
{
	return g_stTctl.stTctlInfo.CtrlTotal;
}

TCTL_CtrlItem_ST TCTL_GetCtrlItem(uint8_t index)
{
	return g_stTctl.stTctlInfo.Item[index];
}

uint8_t TCTL_GetMsgType(void)
{
	return g_stTctl.ACKUpgrade.Header.BYTES.MsgType;
}

uint8_t TCTL_GetACKUpTest(void)
{
	return g_stTctl.ACKUpgrade.ACKUpTest;
}

uint8_t TCTL_GetVersionTest(void)
{
	return g_stTctl.CheckVersion.VersionTest;
}


uint8_t TCTL_GetSendUpTest(void)
{
	return g_stTctl.SendUpgradeData.SendUpTest;
}

uint8_t TCTL_GetOSID(void)
{
	return g_stTctl.SendUpgradeData.SendUpTest;
}


uint8_t TCTL_GetOSID(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((TRUE == Tctl_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{	
		len = 2;
		memcpy(pData,g_stTctl.SendUpgradeData.OSID,len);
		ret = RET_OK;
	}
	return ret;
}

uint8_t TCTL_GetMD5(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((TRUE == Tctl_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{	
		len = 16;
		memcpy(pData,g_stTctl.SendUpgradeData.MD5,len);
		ret = RET_OK;
	}
	return ret;
}

uint8_t TCTL_GetUpFeilLen(void)
{
	return g_stTctl.SendUpgradeData.UpFeilLen;
}

uint8_t TCTL_GetUpFeil(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((TRUE == Tctl_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{	
		len = 256;
		memcpy(pData,g_stTctl.SendUpgradeData.UpFeil,len);
		ret = RET_OK;
	}
	return ret;
}

