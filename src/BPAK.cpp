#include "BPAK.h"
#include "TIF.h"
#include <fstream>
#include <iostream>
#include <string>
#include "json.h"
#include <sys/time.h>



BPAK_VeichleInformation_UN g_bpak_stBpak;
uint8_t BPAK_InitSts = FALSE;


uint8_t BPAK_Init()
{
	BPAK_InitSts = TRUE;
	memset(g_bpak_stBpak.Buf,0,sizeof(g_bpak_stBpak));
	g_bpak_stBpak.stBpakInfo.GPSInfoSignNum 		= BPAK_VeichleInfo_GPSNum;
    g_bpak_stBpak.stBpakInfo.RemainOilSignNum 		= BPAK_VeichleInfo_RemainedOilNum;
	g_bpak_stBpak.stBpakInfo.OdoSignNum 			= BPAK_VeichleInfo_OdometerNum;
    g_bpak_stBpak.stBpakInfo.MaintVolSignNum 		= BPAK_VeichleInfo_MaintVolNum;
    g_bpak_stBpak.stBpakInfo.CarSpeedSignNum 		= BPAK_VeichleInfo_CarSpeedNum;
    g_bpak_stBpak.stBpakInfo.DoorStatusSignNum 		= BPAK_VeichleInfo_DoorStatusNum;
    g_bpak_stBpak.stBpakInfo.LockStatusSignNum 		= BPAK_VeichleInfo_LockStatusNum;
    g_bpak_stBpak.stBpakInfo.RoofStatusSignNum 		= BPAK_VeichleInfo_RoofStatusNum;
    g_bpak_stBpak.stBpakInfo.WindowStatusSignNum 	= BPAK_VeichleInfo_WindowStateNum;


    g_bpak_stBpak.stBpakInfo.LampStautsSignNum 		= BPAK_VeichleInfo_LampStautsNum;

    g_bpak_stBpak.stBpakInfo.EngineStatusSignNum 	= BPAK_VeichleInfo_EngineStateNum;
    g_bpak_stBpak.stBpakInfo.EngineSpeedSignNum 	= BPAK_VeichleInfo_EngineSpeedNum;
    g_bpak_stBpak.stBpakInfo.GearInforSignNum 		= BPAK_VeichleInfo_GearStateNum;
    g_bpak_stBpak.stBpakInfo.HandParkStatusSignNum 	= BPAK_VeichleInfo_HandBrakeStateNum;
    g_bpak_stBpak.stBpakInfo.IgnInfoSignNum 		= BPAK_VeichleInfo_IgnInfoNum;
    
    g_bpak_stBpak.stBpakInfo.BPStsSignNum 			= BPAK_VeichleInfo_BPStsNum;
    
    g_bpak_stBpak.stBpakInfo.ChargeStatusSignNum 	= BPAK_VeichleInfo_ChargeStateNum;
    g_bpak_stBpak.stBpakInfo.ChargeGunStatusSignNum = BPAK_VeichleInfo_ChargeGunStatusNum;
    g_bpak_stBpak.stBpakInfo.BreakStatusSignNum 	= BPAK_VeichleInfo_BreakStatusNum;
    g_bpak_stBpak.stBpakInfo.RemainEnduranceSignNum = BPAK_VeichleInfo_RemainEnduranceNum;
    g_bpak_stBpak.stBpakInfo.BeltStatusSignNum 		= BPAK_VeichleInfo_BeltStatusNum;
    g_bpak_stBpak.stBpakInfo.MotorStatusSignNum 	= BPAK_VeichleInfo_MotorWorkStateNum;
    g_bpak_stBpak.stBpakInfo.MotorSpeedSignNum 		= BPAK_VeichleInfo_MotorSpeedNum;
    g_bpak_stBpak.stBpakInfo.PowerForceStsSignNum 	= BPAK_VeichleInfo_PowerdNum;
	g_bpak_stBpak.stBpakInfo.OsVersionSignNum		= BPAK_VeichleInfo_VerTboxOSNum;
    g_bpak_stBpak.stBpakInfo.McuVersionSignNum		= BPAK_VeichleInfo_VerTboxMCUNum;
    g_bpak_stBpak.stBpakInfo.BleVersionSignNum		= BPAK_VeichleInfo_VerTboxBLENum;
    g_bpak_stBpak.stBpakInfo.CarGwNodeMissSignNum 	= BPAK_VeichleInfo_GwNodeMissNum;
	g_bpak_stBpak.stBpakInfo.BPAK_4GValueSignNum 	= BPAK_VeichleInfo_4GSIGNALVALUENum;
	
	return BPAK_InitSts;

}
uint8_t BPAK_DeInit()
{
	BPAK_InitSts = FALSE;
	return BPAK_InitSts;
}


void bpak_TimeDeal(uint8_t* pTemp, uint8_t isFlagLen)
{
	BPAK_AcpTimeStamp_St  AcpTimeStamp;  //TimeStampʱ���
	memset(&AcpTimeStamp, 0, sizeof(AcpTimeStamp));
	if(isFlagLen == 1)
	{
		AcpTimeStamp.Element_LenInfo.Element_Len = 6;
		*pTemp++ = AcpTimeStamp.Element_LenInfo.Element_Len;
	}		
	bpak_TimeStampPart(pTemp, &(AcpTimeStamp.TimeStampInfo));
}

void bpak_TimeStampPart(uint8_t* pTemp, BPAK_TimeStamp_St *pTimeStamp)
{
	if(NULL != pTemp && NULL != pTimeStamp)
	{
		struct tm *p_tm = NULL; //ʱ��Ĵ���
		time_t tmp_time;
		tmp_time = time(NULL);
		p_tm = gmtime(&tmp_time);
		struct timeval Time;
		gettimeofday(&Time, NULL);//��ȡʱ�����΢��

		pTimeStamp->Year = p_tm->tm_year - 90;
		pTimeStamp->Month = p_tm->tm_mon + 1;
		pTimeStamp->Day = p_tm->tm_mday;
		pTimeStamp->Hour = p_tm->tm_hour;
		pTimeStamp->Minutes = p_tm->tm_min;
		pTimeStamp->Seconds = p_tm->tm_sec;
		pTimeStamp->msSeconds = Time.tv_usec / 1000; //����

		*pTemp++ = ((pTimeStamp->Year)<<2)|(((pTimeStamp->Month)>>2)&0x03);//ʱ���һ�ֽ�
		*pTemp++ = (((pTimeStamp->Month)<<6)&0xc0)|((pTimeStamp->Day)<<1)|(((pTimeStamp->Hour)>>4)&0x01);//ʱ��ڶ��ֽ�
		*pTemp++ = (((pTimeStamp->Hour)<<4)&0xf0)|(((pTimeStamp->Minutes)>>2)&0x0f);//ʱ��ڶ��ֽ�
		*pTemp++ = (((pTimeStamp->Minutes)<<6)&0xc0)|(pTimeStamp->Seconds);//ʱ������ֽ�
		*pTemp++ = (((pTimeStamp->msSeconds)>>8)&0xff);
		*pTemp++ = (((pTimeStamp->msSeconds)>>0)&0xff);
	}
	//printf("send to tsp time mon is %d day is %d\n",pTimeStamp->Month,pTimeStamp->Day );
}

    
uint8_t BPAK_Printf(void)
{
    #if PRINTF_FLAG
	printf("Bpak_Direction:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Direction);
	printf("Bpak_Sec:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Sec);
	printf("Bpak_Min:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Min);
	printf("Bpak_Hour:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Hour);
	printf("Bpak_Day:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Day);
	printf("Bpak_Mon:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Mon);
	printf("Bpak_Year:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Year);
	printf("Bpak_High:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.High);
	printf("Bpak_LatSts:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.LatSts);
	printf("Bpak_LatDeg:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.LatDeg);
	printf("Bpak_LonSts:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.LonSts);
	printf("Bpak_LonDeg:%d\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.LonDeg);
	printf("Bpak_PosiVailed:%d\n\n",g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.PosiVailed);
	
	printf("Bpak_RemainOil:%d\n\n",g_bpak_stBpak.stBpakInfo.RemainOil.BYTES.RemainOilVule);
	
	printf("Bpak_Odo:%d\n",g_bpak_stBpak.stBpakInfo.Odo);
	printf("Bpak_CarSpeed:%d\n\n",g_bpak_stBpak.stBpakInfo.CarSpeed);
	
	printf("Bpak_DrivingDoor:%d\n",g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.DrivingDoor);
	printf("Bpak_CopilotDoor:%d\n",g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.CopilotDoor);
	printf("Bpak_LeftRearDoor:%d\n",g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.LeftRearDoor);
	printf("Bpak_RightRearDoor:%d\n",g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.RightRearDoor);
	printf("Bpak_RearCanopy:%d\n",g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.RearCanopy);
	printf("Bpak_EngineCover:%d\n\n",g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.EngineCover);
	
	printf("Bpak_MaintVol:%d\n\n",g_bpak_stBpak.stBpakInfo.MaintVol);
	
	printf("Bpak_RightRearLock:%d\n",g_bpak_stBpak.stBpakInfo.LockStatus.BYTES.RightRearLock);
	printf("Bpak_LeftRearLock:%d\n",g_bpak_stBpak.stBpakInfo.LockStatus.BYTES.LeftRearLock);
	printf("Bpak_CopilotLock:%d\n",g_bpak_stBpak.stBpakInfo.LockStatus.BYTES.CopilotLock);
	printf("Bpak_DrivingLock:%d\n\n",g_bpak_stBpak.stBpakInfo.LockStatus.BYTES.DrivingLock);
	
	printf("Bpak_RoofStatus:%d\n\n",g_bpak_stBpak.stBpakInfo.RoofStatus);
	
	printf("Bpak_LeftFrontWindow:%d\n",g_bpak_stBpak.stBpakInfo.WindowStatus.BYTES.LeftFrontWindow);
	printf("Bpak_RightFrontWindow:%d\n",g_bpak_stBpak.stBpakInfo.WindowStatus.BYTES.RightFrontWindow);
	printf("Bpak_LeftRearWindowHigh:%d\n",g_bpak_stBpak.stBpakInfo.WindowStatus.BYTES.LeftRearWindow);
	printf("Bpak_RightRearWindow:%d\n\n",g_bpak_stBpak.stBpakInfo.WindowStatus.BYTES.RightRearWindow);
	
	printf("Bpak_Headlights:%d\n",g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Headlights);
	printf("Bpak_Positionlights:%d\n",g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Positionlights);
	printf("Bpak_Nearlights:%d\n",g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Nearlights);
	printf("Bpak_Rearfoglights:%d\n",g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Rearfoglights);
	printf("Bpak_Frontfoglights:%d\n",g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Frontfoglights);
	printf("Bpak_Leftlights:%d\n",g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Leftlights);
	printf("Bpak_Rightlights:%d\n\n",g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Rightlights);

	printf("Bpak_EngineStatus:%d\n",g_bpak_stBpak.stBpakInfo.EngineStatus);
	printf("Bpak_EngineSpeed:%d\n",g_bpak_stBpak.stBpakInfo.EngineSpeed);
	printf("Bpak_GearInfor:%d\n",g_bpak_stBpak.stBpakInfo.GearInfor);
	printf("Bpak_HandParkStatus:%d\n",g_bpak_stBpak.stBpakInfo.HandParkStatus);
	printf("Bpak_IgnInfo:%d\n\n",g_bpak_stBpak.stBpakInfo.IgnInfo);

	printf("Bpak_BPVol:%d\n",g_bpak_stBpak.stBpakInfo.BPSts.BYTES.BPVol);
    printf("Bpak_BPSur:%d\n",g_bpak_stBpak.stBpakInfo.BPSts.BYTES.BPSur);
    printf("Bpak_BPSoc:%d\n\n",g_bpak_stBpak.stBpakInfo.BPSts.BYTES.BPSoc);

	printf("Bpak_ChargeStatus:%d\n",g_bpak_stBpak.stBpakInfo.ChargeStatus);
	printf("Bpak_ChargeGunStatus:%d\n",g_bpak_stBpak.stBpakInfo.ChargeGunStatus);
	printf("Bpak_BreakStatus:%d\n",g_bpak_stBpak.stBpakInfo.BreakStatus);
	printf("Bpak_RemainEndurance:%d\n",g_bpak_stBpak.stBpakInfo.RemainEndurance);
	printf("Bpak_BeltStatus:%d\n",g_bpak_stBpak.stBpakInfo.BeltStatus);
	printf("Bpak_MotorStatus:%d\n\n",g_bpak_stBpak.stBpakInfo.MotorStatus);

	printf("Bpak_MotorSpeed:%d\n",g_bpak_stBpak.stBpakInfo.MotorSpeed);
	printf("Bpak_PowerForceSts:%d\n\n",g_bpak_stBpak.stBpakInfo.PowerForceSts);
	
	printf("Bpak_McuVersion:%d\n",g_bpak_stBpak.stBpakInfo.McuVersion);
	printf("Bpak_CarGwNodeMiss:%d\n\n\n",g_bpak_stBpak.stBpakInfo.CarGwNodeMiss);

    #endif
    return 0;
    
}





uint8_t bpak_PakageVecihle(uint8_t *buf,uint8_t msgType)
{
    uint8_t Value = 0 ;
	uint8_t ret = RET_FAIL ;
    uint16_t len = 0 ;
	uint16_t tempCount = 0 ;
	static uint8_t collectPacketSN = 1;
	if((NULL == buf) )
	{	
		ret = RET_INVALID;
	}
	else
	{
		g_bpak_stBpak.stBpakInfo.Header.BYTES.Acp_AppID 	= BPAK_VEICHLEINFO_APPID;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.PrivateFlag 	= 1;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.Reserved_ID	= 0;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.MsgType		= msgType;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.CryptFlag		= BPAK_AcpCryptFlag_NULL;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.TestFlag 		= BPAK_VERSION_FLAG;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.Reserved_T	= 0;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.VersionFlag	= 1;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.Version		= 0;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.MsgCtrlFlag	= 2;
		g_bpak_stBpak.stBpakInfo.Header.BYTES.MsgLength		= sizeof(BPAK_VeichleInfo_ST)
															- sizeof(BPAK_AcpAppHeader_UN);
		g_bpak_stBpak.stBpakInfo.SendMode = 0;

		if(collectPacketSN == 254)
			collectPacketSN = 1;
		g_bpak_stBpak.stBpakInfo.CollectPacketSN = collectPacketSN ++;
		bpak_TimeDeal(g_bpak_stBpak.stBpakInfo.TimeStamp , 0);
		tempCount = BPAK_SIGNCODE_TOTAL;
		g_bpak_stBpak.stBpakInfo.SignCodeTotal[0] = (tempCount >> 8) & 0xFF;
		g_bpak_stBpak.stBpakInfo.SignCodeTotal[0] = tempCount & 0xFF;
	    /*YOU CAN ONLY USE BIF_xxxFUNCTION*/
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Direction = TIF_GetDirection();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Sec = TIF_GetSec();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Min = TIF_GetMin();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Hour = TIF_GetHour();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Day = TIF_GetDay();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Mon = TIF_GetMon();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.Year = TIF_GetYear();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.LatSts = TIF_GetStsLat();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.LatDeg = TIF_GetLatDeg();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.LonSts = TIF_GetStsLon();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.LonSts = TIF_GetLonDeg();
	    g_bpak_stBpak.stBpakInfo.GPSInfo.BYTES.PosiVailed = TIF_GetStsPosiVailed();
	    
	    g_bpak_stBpak.stBpakInfo.RemainOil.BYTES.RemainOilVule = TIF_GetRemainOil();
		
	    g_bpak_stBpak.stBpakInfo.Odo =TIF_GetOdo();
	    g_bpak_stBpak.stBpakInfo.MaintVol = TIF_GetMaintVol();
	    g_bpak_stBpak.stBpakInfo.CarSpeed = TIF_GetCarSpeed();
	    
	    g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.DrivingDoor = TIF_GetDoorStatusDrivingDoor();
	    g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.CopilotDoor = TIF_GetDoorStatusCopilotDoor();
	    g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.LeftRearDoor = TIF_GetDoorStatusLeftRearDoor();
	    g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.RightRearDoor = TIF_GetDoorStatusRightRearDoor();
	    g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.RearCanopy = TIF_GetDoorStatusRearCanopy();
	    g_bpak_stBpak.stBpakInfo.DoorStatus.BYTES.EngineCover = TIF_GetDoorStatusEngineCover();
	    
	    g_bpak_stBpak.stBpakInfo.LockStatus.BYTES.RightRearLock = TIF_GetLockStatusRightRearLock();
	    g_bpak_stBpak.stBpakInfo.LockStatus.BYTES.LeftRearLock = TIF_GetLockStatusLeftRearLock();
	    g_bpak_stBpak.stBpakInfo.LockStatus.BYTES.CopilotLock = TIF_GetLockStatusCopilotLock();
	    g_bpak_stBpak.stBpakInfo.LockStatus.BYTES.DrivingLock= TIF_GetLockStatusDrivingLock();
	    
	    g_bpak_stBpak.stBpakInfo.RoofStatus = TIF_GetRoofStatus();
	    
	    g_bpak_stBpak.stBpakInfo.WindowStatus.BYTES.LeftFrontWindow = TIF_GetWindowStatusLeftFrontWindow();
	    g_bpak_stBpak.stBpakInfo.WindowStatus.BYTES.RightFrontWindow = TIF_GetWindowStatusRightFrontWindow();
	    g_bpak_stBpak.stBpakInfo.WindowStatus.BYTES.LeftRearWindow = 	TIF_GetWindowStatusLeftRearWindow();
	    g_bpak_stBpak.stBpakInfo.WindowStatus.BYTES.RightRearWindow = TIF_GetWindowStatusRightRearWindow();

	    g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Headlights = TIF_GetLampStautsHeadlights();
	    g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Positionlights = TIF_GetLampStautsPositionlights();
	    g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Nearlights = TIF_GetLampStautsNearlights();
	    g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Rearfoglights = TIF_GetLampStautsRearfoglights();
	    g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Frontfoglights = TIF_GetLampStautsFrontfoglights();
	    g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Leftlights = TIF_GetLampStautsLeftlights();
	    g_bpak_stBpak.stBpakInfo.LampStauts.BYTES.Rightlights = TIF_GetLampStautsRightlights();

	    g_bpak_stBpak.stBpakInfo.EngineStatus = TIF_GetEngineStatus();
	    g_bpak_stBpak.stBpakInfo.EngineSpeed = TIF_GetEngineSpeed();
	    g_bpak_stBpak.stBpakInfo.GearInfor = TIF_GetGearInfor();
	    g_bpak_stBpak.stBpakInfo.HandParkStatus = TIF_GetHandParkStatus();
	    g_bpak_stBpak.stBpakInfo.IgnInfo = TIF_GetIgnInfo();
	    
	    g_bpak_stBpak.stBpakInfo.BPSts.BYTES.BPVol = TIF_GetBPVol();
	    g_bpak_stBpak.stBpakInfo.BPSts.BYTES.BPSur = TIF_GetBPSur();
	    g_bpak_stBpak.stBpakInfo.BPSts.BYTES.BPSoc = TIF_GetBPSoc();
	    
	    g_bpak_stBpak.stBpakInfo.ChargeStatus = TIF_GetChargeStatus();
	    g_bpak_stBpak.stBpakInfo.ChargeGunStatus = TIF_GetChargeGunStatus();
	    g_bpak_stBpak.stBpakInfo.BreakStatus = TIF_GetBreakStatus();
	    g_bpak_stBpak.stBpakInfo.RemainEndurance = TIF_GetRemainEndurance();
	    g_bpak_stBpak.stBpakInfo.BeltStatus = TIF_GetBeltStatus();
	    g_bpak_stBpak.stBpakInfo.MotorStatus = TIF_GetMotorStatus();
	    g_bpak_stBpak.stBpakInfo.MotorSpeed = TIF_GetMotorSpeed();
	    g_bpak_stBpak.stBpakInfo.PowerForceSts = TIF_GetPowerForceSts();
	    
	    TIF_GetMcuVersion(g_bpak_stBpak.stBpakInfo.McuVersion,len);
	    
	    g_bpak_stBpak.stBpakInfo.CarGwNodeMiss = TIF_GetCarGwNodeMiss();
	//    buf = g_bpak_stBpak.Buf;
		memcpy(buf, g_bpak_stBpak.Buf, BPAK_VEICHLE_DATA_LEN );
		ret = RET_OK;
	}
    
    return ret;
}




uint8_t BPAK_PackageData(uint8_t PAKID,uint8_t* pData)
{
	uint8_t ret = RET_FAIL;
	if(BPAK_InitSts == FALSE)
	{
		ret = RET_FAIL;
	}
	else if(NULL == pData)
	{
		ret = RET_INVALID;
	}
	else
	{
		switch(PAKID)
		{
		    case BPAK_REPROT_VECIHLE_INFORMATION:
		        
		        ret = bpak_PakageVecihle(pData,BPAK_MessageType_VeichleInfo);
				BPAK_Printf();
		        break;
		    
		/*      case RESPONSE_REMOTE_CTL_ACK:
		        
		        break;
		    case RESPONSE_UPGRADE_ACK:
		        
		        break;
		    case RESPONSE_UPGRADE_ACK:
		        
		        break;
		    case RESPONSE_UPGRADE_ACK:
		        
		        break;    
		        */
		    default:
		        break;
		}
	}
   return ret;
}

