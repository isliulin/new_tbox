#include <fstream>
#include <iostream>
#include <string>
#include "SYS.h"
#include "VPAK.h"
#include "tboxComnFun.h"

VPAK_RemoteCtrlInformation_UN g_vpak_stVpak;
uint8_t VPAK_InitSts = FALSE;



uint8_t VPAK_Init(void)
{
	/*if uart com is failure ,should be false*/
	VPAK_InitSts = TRUE;
	memset(g_vpak_stVpak.Buf,0,sizeof(g_vpak_stVpak));
	return VPAK_InitSts;
}
	
uint8_t VPAK_DeInit(void)
{

	VPAK_InitSts = FALSE;
	return VPAK_InitSts;
}


/*uint16_t VPAK_PakageVeichleInfoData(void)
{
	uint8_t Value = 0 ;
	uint16_t TifSpeed = 0;


	g_stBpak.stBpakInfo.SpeedValue = BIF_GetSpeedValue(void);

	
}*/



uint8_t vpak_Analysis(uint8_t *outBuf,uint8_t *inBuf,uint16_t inLen ,uint16_t *outLen)
{
	uint8_t i = 0;
	uint8_t tempLen = 0;
	uint8_t ret = RET_FAIL;
	if(VPAK_InitSts == FALSE)
	{
	printf("$$$$$$$$$$$#####VIVIAN#######UNNIT\r\n");
		return ret;
	}
	else if((NULL == outBuf) ||(NULL == inBuf) || (inLen > MSG_MAX_LEN))
	{	
	printf("$$$$$$$$$$$#####VIVIAN#######vpak_Analysis INVALUDE \r\n");
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

		
	printf("$$$$$$$$$$$#####VIVIAN#######%d\r\n",*outLen);
		ret = RET_OK;

	}
	

	return ret;
}


uint16_t vpak_Crc16Check(uint8_t *pBuf,uint8_t len)
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

uint8_t vpak_PackageCtrlData(uint16_t *len, uint8_t *buf, uint8_t index, uint16_t serialNO)
{
	uint8_t Value = 0 ;
	uint16_t tempLen = 0 ;
	uint16_t tempCount = 0 ;
	static uint8_t CollectPacketSN = 1;

	g_vpak_stVpak.stVpakCtrl.Head.HeadFlag	 	= 0x7e;
	g_vpak_stVpak.stVpakCtrl.Head.BodyLen  		= 2;
	g_vpak_stVpak.stVpakCtrl.Head.CmdID			= 0x84;
	g_vpak_stVpak.stVpakCtrl.Head.SerialNumber 	= serialNO;
	g_vpak_stVpak.stVpakCtrl.Head.Attr[0]		= 0x00;
	g_vpak_stVpak.stVpakCtrl.Head.Attr[1]		= 0x00;

//	g_vpak_stVpak.stVpakCtrl.Parameter			= VCTL_GetRemoteCtrl(index);
	memcpy(&g_vpak_stVpak.stVpakCtrl.Parameter	,&VCTL_GetRemoteCtrl(index),sizeof(VCTL_RemoteCtrl_ST));
	g_vpak_stVpak.stVpakCtrl.Head.BodyLen = ((g_vpak_stVpak.stVpakCtrl.Head.BodyLen << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakCtrl.Head.BodyLen >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakCtrl.Head.SerialNumber = ((g_vpak_stVpak.stVpakCtrl.Head.SerialNumber << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakCtrl.Head.SerialNumber >> 8) & 0x00FF);

	g_vpak_stVpak.stVpakCtrl.CheckCrc 	= vpak_Crc16Check(&(g_vpak_stVpak.Buf[1]),2 + 7);
	g_vpak_stVpak.stVpakCtrl.EndFlag 	= 0x7e;
	g_vpak_stVpak.stVpakCtrl.CheckCrc = ((g_vpak_stVpak.stVpakCtrl.CheckCrc << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakCtrl.CheckCrc >> 8) & 0x00FF);

	vpak_Analysis(buf ,g_vpak_stVpak.Buf ,2 + 9,&tempLen);
	*len = tempLen;
	g_vpak_stVpak.stVpakCtrl.Head.BodyLen = ((g_vpak_stVpak.stVpakCtrl.Head.BodyLen << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakCtrl.Head.BodyLen >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakCtrl.Head.SerialNumber = ((g_vpak_stVpak.stVpakCtrl.Head.SerialNumber << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakCtrl.Head.SerialNumber >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakCtrl.CheckCrc = ((g_vpak_stVpak.stVpakCtrl.CheckCrc << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakCtrl.CheckCrc >> 8) & 0x00FF);
	
	return RET_OK;
}

uint8_t vpak_PackageConfigData(uint16_t *len, uint8_t *buf, uint8_t Total, uint16_t serialNO)
{
	uint8_t i = 0 ;
	uint8_t Value = 0 ;
	uint16_t tempLen = 0 ;
	uint16_t tempCount = 0 ;
	static uint8_t CollectPacketSN = 1;

	g_vpak_stVpak.stVpakConfig.Head.HeadFlag	 	= 0x7e;
	g_vpak_stVpak.stVpakConfig.Head.BodyLen  		= 0;
//	g_vpak_stVpak.stVpakConfig.Head.BodyLen = ((g_vpak_stVpak.stVpakConfig.Head.BodyLen << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakConfig.Head.BodyLen >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakConfig.Head.CmdID			= 0x82;
	g_vpak_stVpak.stVpakConfig.Head.SerialNumber 	= serialNO;
//	g_vpak_stVpak.stVpakConfig.Head.SerialNumber = ((g_vpak_stVpak.stVpakConfig.Head.SerialNumber << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakConfig.Head.SerialNumber >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakConfig.Head.Attr[0]			= 0x00;
	g_vpak_stVpak.stVpakConfig.Head.Attr[1]			= 0x00;

	g_vpak_stVpak.stVpakConfig.ParameterTotal 		= Total;
	g_vpak_stVpak.stVpakConfig.Head.BodyLen  		+= 8;
	for(i = 0; i < Total; i ++)
	{
		g_vpak_stVpak.stVpakConfig.ParameterList[i] 	= VCTL_GetVeichleConfig(i);
		g_vpak_stVpak.stVpakConfig.Head.BodyLen  		+= 16;
		g_vpak_stVpak.stVpakConfig.Head.BodyLen  		+= (uint16_t)g_vpak_stVpak.stVpakConfig.ParameterList[i].ParameterLen;
	}
	
//	g_vpak_stVpak.stVpakConfig.Head.BodyLen = ((g_vpak_stVpak.stVpakConfig.Head.BodyLen << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakConfig.Head.BodyLen >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakConfig.CheckCrc 	= vpak_Crc16Check(&(g_vpak_stVpak.Buf[1]),g_vpak_stVpak.stVpakConfig.Head.BodyLen / 8 + 7);
	g_vpak_stVpak.stVpakConfig.EndFlag 		= 0x7e;

	vpak_Analysis(buf ,g_vpak_stVpak.Buf ,g_vpak_stVpak.stVpakConfig.Head.BodyLen + 7 * 8,&tempLen);
	*len = tempLen;
	g_vpak_stVpak.stVpakConfig.Head.BodyLen = ((g_vpak_stVpak.stVpakConfig.Head.BodyLen << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakConfig.Head.BodyLen >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakConfig.Head.SerialNumber = ((g_vpak_stVpak.stVpakConfig.Head.SerialNumber << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakConfig.Head.SerialNumber >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakConfig.CheckCrc = ((g_vpak_stVpak.stVpakConfig.CheckCrc << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakConfig.CheckCrc >> 8) & 0x00FF);

	
	return 0;
}

uint8_t vpak_PackageHostData(uint16_t *len, uint8_t *buf, uint16_t serialNO)
{
	uint8_t Value = 0 ;
	uint16_t tempLen = 0 ;
	uint16_t tempCount = 0 ;
	static uint8_t CollectPacketSN = 1;

	g_vpak_stVpak.stVpakHostSts.Head.HeadFlag	 	= 0x7e;
	g_vpak_stVpak.stVpakHostSts.Head.BodyLen  		= 21;
	g_vpak_stVpak.stVpakHostSts.Head.BodyLen = ((g_vpak_stVpak.stVpakHostSts.Head.BodyLen << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakHostSts.Head.BodyLen >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakHostSts.Head.CmdID			= 0x85;
	g_vpak_stVpak.stVpakHostSts.Head.SerialNumber 	= serialNO;
	g_vpak_stVpak.stVpakHostSts.Head.SerialNumber = ((g_vpak_stVpak.stVpakHostSts.Head.SerialNumber << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakHostSts.Head.SerialNumber >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakHostSts.Head.Attr[0]		= 0x00;
	g_vpak_stVpak.stVpakHostSts.Head.Attr[1]		= 0x00;

	g_vpak_stVpak.stVpakHostSts.HostSerialNumber	= SYS_GetHostSerialNumber();
	g_vpak_stVpak.stVpakHostSts.HardwareSts			= SYS_GetHardwareSts();
	g_vpak_stVpak.stVpakHostSts.PhoneSts			= SYS_GetPhoneSts();
	g_vpak_stVpak.stVpakHostSts.VPAK_4GValue		= SYS_VPAK_4GValue();
	g_vpak_stVpak.stVpakHostSts.WifiFault			= SYS_GetWifiFault();
	g_vpak_stVpak.stVpakHostSts.SIMSts				= SYS_GetSIMSts();
	g_vpak_stVpak.stVpakHostSts.USBFault			= SYS_GetUSBFault();
	g_vpak_stVpak.stVpakHostSts.BTFault				= SYS_GetBTFault();
	g_vpak_stVpak.stVpakHostSts.LogSwitch			= SYS_LogSwitch();
	
	SYS_HostEngineSVersions(g_vpak_stVpak.stVpakHostSts.HostEngineSVersions);
	
	g_vpak_stVpak.stVpakHostSts.CheckCrc 	= vpak_Crc16Check(&(g_vpak_stVpak.Buf[1]),21 + 7);
	g_vpak_stVpak.stVpakHostSts.EndFlag 	= 0x7e;
	g_vpak_stVpak.stVpakHostSts.CheckCrc = ((g_vpak_stVpak.stVpakHostSts.CheckCrc << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakHostSts.CheckCrc >> 8) & 0x00FF);

	vpak_Analysis(buf ,g_vpak_stVpak.Buf ,21 + 9 , &tempLen);
	
	*len = tempLen;
	
	return 0;
}

uint8_t vpak_PackageHostACK(uint16_t *len,uint8_t *buf,uint8_t ACKID,uint16_t serialNO, uint8_t ACKRes)
{
	uint8_t Value = 0 ;
	uint16_t tempLen = 0 ;
	uint16_t tempCount = 0 ;
	uint8_t	ret = RET_FAIL;
	static uint8_t CollectPacketSN = 1;

	g_vpak_stVpak.stVpakHostACK.Head.HeadFlag	 	= 0x7e;
	g_vpak_stVpak.stVpakHostACK.Head.BodyLen  		= 4;
	g_vpak_stVpak.stVpakHostACK.Head.BodyLen = ((g_vpak_stVpak.stVpakHostACK.Head.BodyLen << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakHostACK.Head.BodyLen >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakHostACK.Head.CmdID			= VPAK_HostACK_CMDID;
	g_vpak_stVpak.stVpakHostACK.Head.SerialNumber 	= serialNO;
	g_vpak_stVpak.stVpakHostACK.Head.SerialNumber  = ((g_vpak_stVpak.stVpakHostACK.Head.SerialNumber  << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakHostACK.Head.SerialNumber  >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakHostACK.Head.Attr[0]		= 0x00;
	g_vpak_stVpak.stVpakHostACK.Head.Attr[1]		= 0x00;

	g_vpak_stVpak.stVpakHostACK.ACKID				= ACKID;
	g_vpak_stVpak.stVpakHostACK.ACKNumber			= serialNO;
	g_vpak_stVpak.stVpakHostACK.ACKNumber  = ((g_vpak_stVpak.stVpakHostACK.ACKNumber  << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakHostACK.ACKNumber  >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakHostACK.Result				= ACKRes;
	
	SYS_HostEngineSVersions(g_vpak_stVpak.stVpakHostSts.HostEngineSVersions);
	
	g_vpak_stVpak.stVpakHostACK.CheckCrc 	= vpak_Crc16Check(&(g_vpak_stVpak.Buf[1]),4 + 7);
	g_vpak_stVpak.stVpakHostACK.EndFlag 	= 0x7e;
	
	g_vpak_stVpak.stVpakHostACK.CheckCrc  = ((g_vpak_stVpak.stVpakHostACK.CheckCrc   << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakHostACK.CheckCrc   >> 8) & 0x00FF);
	ret = vpak_Analysis(buf ,g_vpak_stVpak.Buf ,4 + 9,&tempLen);
	*len = tempLen;

	
	return ret;
}



uint8_t vpak_PackageSendUpgReq(uint16_t *len, uint8_t *buf, uint16_t serialNO)
{

	uint16_t tempLen = 0 ;
	uint8_t	ret = RET_FAIL;
	uint32_t u32FileLen = 0;
	uint16_t crc = 0;
	uint8_t ptc_len = 0;
	OTA_SocSendInfo_ST *pInfo =  NULL;

	g_vpak_stVpak.stVpakSendUpgReq.Head.HeadFlag	 	= 0x7e;
	//g_vpak_stVpak.stVpakSendUpgReq.Head.BodyLen = ((g_vpak_stVpak.stVpakSendUpgReq.Head.BodyLen << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakSendUpgReq.Head.BodyLen >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakSendUpgReq.Head.CmdID			= VPAK_SendUpgrade_CMDID;
	g_vpak_stVpak.stVpakSendUpgReq.Head.SerialNumber 	= serialNO;
	g_vpak_stVpak.stVpakSendUpgReq.Head.SerialNumber  = ((g_vpak_stVpak.stVpakSendUpgReq.Head.SerialNumber  << 8) & 0xFF00) | ((g_vpak_stVpak.stVpakSendUpgReq.Head.SerialNumber  >> 8) & 0x00FF);
	g_vpak_stVpak.stVpakSendUpgReq.Head.Attr[0]		= 0x00;
	g_vpak_stVpak.stVpakSendUpgReq.Head.Attr[1]		= 0x00;

	
	g_vpak_stVpak.stVpakSendUpgReq.Type = 0;

	pInfo = VUPG_GetUpgHostReq();
	ptc_len = 26;
	
	g_vpak_stVpak.stVpakSendUpgReq.filelen[0] = ((pInfo->m_FileLen >>24)&0xFF); 
	g_vpak_stVpak.stVpakSendUpgReq.filelen[1] = ((pInfo->m_FileLen >>16)&0xFF); 
	g_vpak_stVpak.stVpakSendUpgReq.filelen[2] = ((pInfo->m_FileLen >>8)&0xFF); 
	g_vpak_stVpak.stVpakSendUpgReq.filelen[3] = ((pInfo->m_FileLen >>0)&0xFF); 
	
	g_vpak_stVpak.stVpakSendUpgReq.crc[0] = ((pInfo->m_nCRC >>8)&0xFF);
	g_vpak_stVpak.stVpakSendUpgReq.crc[1] = ((pInfo->m_nCRC >>0)&0xFF);
	
	g_vpak_stVpak.stVpakSendUpgReq.filenameLen = 13;
	memcpy(g_vpak_stVpak.stVpakSendUpgReq.filename ,"/data/MCU.bin",g_vpak_stVpak.stVpakSendUpgReq.filenameLen);
	g_vpak_stVpak.stVpakSendUpgReq.fileverLen = 4;
	memcpy(g_vpak_stVpak.stVpakSendUpgReq.fileversion ,"v101",g_vpak_stVpak.stVpakSendUpgReq.fileverLen);	
	g_vpak_stVpak.stVpakSendUpgReq.Head.BodyLen = ((ptc_len << 8) & 0xFF00) |\
													((ptc_len >> 8) & 0x00FF);
	
	crc 	= vpak_Crc16Check(&(g_vpak_stVpak.Buf[1]),ptc_len + 7);
	
	g_vpak_stVpak.stVpakSendUpgReq.CheckCrc  = ((crc   << 8) & 0xFF00) | \
												((crc   >> 8) & 0x00FF);
		
	g_vpak_stVpak.stVpakSendUpgReq.EndFlag 	= 0x7e;
	

	ret = vpak_Analysis(buf ,g_vpak_stVpak.Buf ,ptc_len + 9,&tempLen);
	*len = tempLen;

			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_BEGAIN\r\n");
			printf("\r\n");
			printf("##########FW##############\n");
			for(uint16_t i =0; i< 48; i++)
			printf("%0x ",g_vpak_stVpak.Buf[i]);
			
			printf("\r\n");
			
			printf("##########FWF##############\n");
			

	printf("#########vpak_PackageSendUpg###REQ####  VPACK :%d  %0x %0x\r\n",ret,g_vpak_stVpak.stVpakSendUpgReq.crc,g_vpak_stVpak.stVpakSendUpgReq.filelen);
	
	return ret;
}






uint8_t vpak_PackageSendUpgData(uint16_t *len, uint8_t *buf, uint16_t serialNO)
{
	uint8_t Value = 0 ;
	uint16_t tempLen = 0 ;
	uint16_t tempCount = 0 ;
	uint8_t	ret = RET_FAIL;
	static uint8_t CollectPacketSN = 1;
	uint16_t nLen = 0;
	uint16_t crc = 0;
	OTA_SendMcuData_ST* pData = NULL;

	if(len == NULL) printf("null");
	

	g_vpak_stVpak.stVpakSendUpgData.Head.HeadFlag	 	= 0x7e;
	g_vpak_stVpak.stVpakSendUpgData.Head.CmdID			= VPAK_UpgradeDateAACK_CMDID;
	//g_vpak_stVpak.stVpakSendUpgData.Head.SerialNumber 	= serialNO;
	g_vpak_stVpak.stVpakSendUpgData.Head.SerialNumber  = ((serialNO << 8) & 0xFF00) |
														 ((serialNO  >> 8) & 0x00FF);
	
	g_vpak_stVpak.stVpakSendUpgData.Head.Attr[0]		= 0x00;
	g_vpak_stVpak.stVpakSendUpgData.Head.Attr[1]		= 0x00;

	//g_vpak_stVpak.stVpakSendUpgData.AckType = SUCCESS;

	pData = VUPG_GetUpgHostData();
	
	memcpy(&(g_vpak_stVpak.stVpakSendUpgData.data),(void *)pData,sizeof(OTA_SendMcuData_ST));

	nLen = sizeof(OTA_SendMcuData_ST);
	printf("VPAK######len:%d#######\n",nLen);
	//g_vpak_stVpak.stVpakSendUpgData.Head.BodyLen  		= nLen;
	g_vpak_stVpak.stVpakSendUpgData.Head.BodyLen = ((nLen << 8) & 0xFF00) | ((nLen >> 8) & 0x00FF);
	
	crc 	= vpak_Crc16Check(&(g_vpak_stVpak.Buf[1]),nLen + 7);
	
	g_vpak_stVpak.stVpakSendUpgData.EndFlag 	= 0x7e;
	
	g_vpak_stVpak.stVpakSendUpgData.CheckCrc  = ((crc << 8) & 0xFF00) |\
		((crc >> 8) & 0x00FF);
#if 1
				printf("[SUCCESS] beforeID_FW_2_MCU_PTC_UPG_DATA\r\n");
				printf("\r\n");
				printf("VPAK######vpak_PackageSendUpgData#######\n");
				for(int i =0; i< nLen + MCU_PTC_HEAD_LEN+MCU_PTC_TAIL_LEN; i++)
				printf("%02x ",g_vpak_stVpak.Buf[i]);
				
				printf("\r\n");
				
				printf("VPAK#####vpak_PackageSendUpgData#########\n");
#endif

	
	ret = vpak_Analysis(buf ,g_vpak_stVpak.Buf ,nLen + 9,&tempLen);
	printf("[vpak_Analysis]ret:%d tempLen:%d\r\n",ret,tempLen);

	*len = tempLen;
#if 0
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA:%d\r\n",tempLen);
#else
			printf("[SUCCESS] ID_FW_2_MCU_PTC_UPG_DATA:%d\r\n",tempLen);
			printf("\r\n");
			printf("VPAK######vpak_PackageSendUpgData#######\n");
			for(int i =0; i< nLen + MCU_PTC_HEAD_LEN+MCU_PTC_TAIL_LEN; i++)
			printf("%02x ",g_vpak_stVpak.Buf[i]);
			
			printf("\r\n");
			
			printf("VPAK#####vpak_PackageSendUpgData#########\n");
#endif
			
	return ret;
}

uint8_t VPAK_PackageData(uint16_t *len,uint8_t PAKID, uint8_t* pData, uint8_t ACKID, uint8_t ACKRes)
{
	uint8_t CtlTotal;
	uint8_t CfgTotal;
	uint8_t ret = RET_FAIL;
   	switch(PAKID)
    {
        case VPAK_REMOTECTRL_CMDID:
            VCTL_Dispatch(&CtlTotal, &CfgTotal);
            ret = vpak_PackageCtrlData(len,pData, 0, 0);
            break;
        
      	case VPAK_VeichleConfig_CMDID:
			VCTL_Dispatch(&CtlTotal, &CfgTotal);
            ret = vpak_PackageConfigData(len,pData, CfgTotal, 0);
            break;
        case VPAK_HostStatusReport_CMDID:
            ret = vpak_PackageHostData(len,pData, 0);
            break;
        case VPAK_HostACK_CMDID:
            ret = vpak_PackageHostACK(len,pData, ACKID, 0, ACKRes);
            break;    
		case VPAK_SendUpgrade_CMDID:
			ret = vpak_PackageSendUpgReq(len,pData,0);
			break;
		case VPAK_UpgradeDateAACK_CMDID:
			printf("VPAK_PackageData ##BEFORE####UPG_PackageSendUpgData###\n");
			ret = UPG_PackageSendUpgData(len,pData,0);
			break;
        default:
            break;
    }
   
	return ret;
}





