#include "TPAK.h"
#include "TIF.h"
#include "VIF.h"
#include <fstream>
#include <iostream>
#include <string>
#include "json.h"
#include <sys/time.h>



TPAK_VeichleInformation_UN g_tpak_stTpak;
uint8_t TPAK_InitSts = FALSE;


uint8_t TPAK_Init()
{
	TPAK_InitSts = TRUE;
	memset(g_tpak_stTpak.Buf,0,sizeof(g_tpak_stTpak));
	g_tpak_stTpak.stTpakInfo.Body.GPSInfoSignNum 		= TPAK_VeichleInfo_GPSNum;
    g_tpak_stTpak.stTpakInfo.Body.RemainOilSignNum 		= TPAK_VeichleInfo_RemainedOilNum;
	g_tpak_stTpak.stTpakInfo.Body.OdoSignNum 			= TPAK_VeichleInfo_OdometerNum;
    g_tpak_stTpak.stTpakInfo.Body.MaintVolSignNum 		= TPAK_VeichleInfo_MaintVolNum;
    g_tpak_stTpak.stTpakInfo.Body.CarSpeedSignNum 		= TPAK_VeichleInfo_CarSpeedNum;
    g_tpak_stTpak.stTpakInfo.Body.DoorStatusSignNum 		= TPAK_VeichleInfo_DoorStatusNum;
    g_tpak_stTpak.stTpakInfo.Body.LockStatusSignNum 		= TPAK_VeichleInfo_LockStatusNum;
    g_tpak_stTpak.stTpakInfo.Body.RoofStatusSignNum 		= TPAK_VeichleInfo_RoofStatusNum;
    g_tpak_stTpak.stTpakInfo.Body.WindowStatusSignNum 	= TPAK_VeichleInfo_WindowStateNum;


    g_tpak_stTpak.stTpakInfo.Body.LampStautsSignNum 		= TPAK_VeichleInfo_LampStautsNum;

    g_tpak_stTpak.stTpakInfo.Body.EngineStatusSignNum 	= TPAK_VeichleInfo_EngineStateNum;
    g_tpak_stTpak.stTpakInfo.Body.EngineSpeedSignNum 	= TPAK_VeichleInfo_EngineSpeedNum;
    g_tpak_stTpak.stTpakInfo.Body.GearInforSignNum 		= TPAK_VeichleInfo_GearStateNum;
    g_tpak_stTpak.stTpakInfo.Body.HandParkStatusSignNum 	= TPAK_VeichleInfo_HandBrakeStateNum;
    g_tpak_stTpak.stTpakInfo.Body.IgnInfoSignNum 		= TPAK_VeichleInfo_IgnInfoNum;
    
    g_tpak_stTpak.stTpakInfo.Body.BPStsSignNum 			= TPAK_VeichleInfo_BPStsNum;
    
    g_tpak_stTpak.stTpakInfo.Body.ChargeStatusSignNum 	= TPAK_VeichleInfo_ChargeStateNum;
    g_tpak_stTpak.stTpakInfo.Body.ChargeGunStatusSignNum = TPAK_VeichleInfo_ChargeGunStatusNum;
    g_tpak_stTpak.stTpakInfo.Body.BreakStatusSignNum 	= TPAK_VeichleInfo_BreakStatusNum;
    g_tpak_stTpak.stTpakInfo.Body.RemainEnduranceSignNum = TPAK_VeichleInfo_RemainEnduranceNum;
    g_tpak_stTpak.stTpakInfo.Body.BeltStatusSignNum 		= TPAK_VeichleInfo_BeltStatusNum;
    g_tpak_stTpak.stTpakInfo.Body.MotorStatusSignNum 	= TPAK_VeichleInfo_MotorWorkStateNum;
    g_tpak_stTpak.stTpakInfo.Body.MotorSpeedSignNum 		= TPAK_VeichleInfo_MotorSpeedNum;
    g_tpak_stTpak.stTpakInfo.Body.PowerForceStsSignNum 	= TPAK_VeichleInfo_PowerdNum;
	g_tpak_stTpak.stTpakInfo.Body.OsVersionSignNum		= TPAK_VeichleInfo_VerTboxOSNum;
    g_tpak_stTpak.stTpakInfo.Body.McuVersionSignNum		= TPAK_VeichleInfo_VerTboxMCUNum;
    g_tpak_stTpak.stTpakInfo.Body.BleVersionSignNum		= TPAK_VeichleInfo_VerTboxBLENum;
    g_tpak_stTpak.stTpakInfo.Body.CarGwNodeMissSignNum 	= TPAK_VeichleInfo_GwNodeMissNum;
	g_tpak_stTpak.stTpakInfo.Body.TPAK_4GValueSignNum 	= TPAK_VeichleInfo_4GSIGNALVALUENum;

	
	return TPAK_InitSts;

}
uint8_t TPAK_DeInit()
{
	TPAK_InitSts = FALSE;
	return TPAK_InitSts;
}


void tpak_TimeDeal(uint8_t* pTemp, uint8_t isFlagLen)
{
	TPAK_HeadTimeStamp_UN  AcpTimeStamp;  //TimeStampʱ���
	memset(&AcpTimeStamp, 0, sizeof(AcpTimeStamp));
	if(isFlagLen == 1)
	{
		AcpTimeStamp.BYTES.Length = 6;
		*pTemp++ = AcpTimeStamp.BYTES.Length;
	}		
	tpak_TimeStampPart(pTemp, &AcpTimeStamp);
}

void tpak_TimeStampPart(uint8_t* pTemp, TPAK_HeadTimeStamp_UN *pTimeStamp)
{
	if(NULL != pTemp && NULL != pTimeStamp)
	{
		struct tm *p_tm = NULL; //ʱ��Ĵ���
		time_t tmp_time;
		tmp_time = time(NULL);
		p_tm = gmtime(&tmp_time);
		struct timeval Time;
		gettimeofday(&Time, NULL);//��ȡʱ�����΢��

		pTimeStamp->BYTES.Year = p_tm->tm_year - 90;
		pTimeStamp->BYTES.Month = p_tm->tm_mon + 1;
		pTimeStamp->BYTES.Day = p_tm->tm_mday;
		pTimeStamp->BYTES.Hour = p_tm->tm_hour;
		pTimeStamp->BYTES.Minutes = p_tm->tm_min;
		pTimeStamp->BYTES.Seconds = p_tm->tm_sec;
		pTimeStamp->BYTES.msSeconds = Time.tv_usec / 1000; //����

		*pTemp++ = ((pTimeStamp->BYTES.Year)<<2)|(((pTimeStamp->BYTES.Month)>>2)&0x03);//ʱ���һ�ֽ�
		*pTemp++ = (((pTimeStamp->BYTES.Month)<<6)&0xc0)|((pTimeStamp->BYTES.Day)<<1)|(((pTimeStamp->BYTES.Hour)>>4)&0x01);//ʱ��ڶ��ֽ�
		*pTemp++ = (((pTimeStamp->BYTES.Hour)<<4)&0xf0)|(((pTimeStamp->BYTES.Minutes)>>2)&0x0f);//ʱ��ڶ��ֽ�
		*pTemp++ = (((pTimeStamp->BYTES.Minutes)<<6)&0xc0)|(pTimeStamp->BYTES.Seconds);//ʱ������ֽ�
		*pTemp++ = (((pTimeStamp->BYTES.msSeconds)>>8)&0xff);
		*pTemp++ = (((pTimeStamp->BYTES.msSeconds)>>0)&0xff);
	}
	//printf("send to tsp time mon is %d day is %d\n",pTimeStamp->Month,pTimeStamp->Day );
}

    
uint8_t TPAK_Printf(void)
{

#if PRINTF_FLAG

	printf("Tpak_Direction:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Direction);
	printf("Tpak_Sec:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Sec);
	printf("Tpak_Min:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Min);
	printf("Tpak_Hour:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Hour);
	printf("Tpak_Day:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Day);
	printf("Tpak_Mon:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Mon);
	printf("Tpak_Year:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Year);
	printf("Tpak_High:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.High);
	printf("Tpak_LatSts:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.LatSts);
	printf("Tpak_LatDeg:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.LatDeg);
	printf("Tpak_LonSts:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.LonSts);
	printf("Tpak_LonDeg:%d\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.LonDeg);
	printf("Tpak_PosiVailed:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.PosiVailed);
	
	printf("Tpak_RemainOil:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.RemainOil.BYTES.RemainOilVule);
	
	printf("Tpak_Odo:%d\n",g_tpak_stTpak.stTpakInfo.Body.Odo);
	printf("Tpak_CarSpeed:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.CarSpeed);
	
	printf("Tpak_DrivingDoor:%d\n",g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.DrivingDoor);
	printf("Tpak_CopilotDoor:%d\n",g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.CopilotDoor);
	printf("Tpak_LeftRearDoor:%d\n",g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.LeftRearDoor);
	printf("Tpak_RightRearDoor:%d\n",g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.RightRearDoor);
	printf("Tpak_RearCanopy:%d\n",g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.RearCanopy);
	printf("Tpak_EngineCover:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.EngineCover);
	
	printf("Tpak_MaintVol:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.MaintVol);
	
	printf("Tpak_RightRearLock:%d\n",g_tpak_stTpak.stTpakInfo.Body.LockStatus.BYTES.RightRearLock);
	printf("Tpak_LeftRearLock:%d\n",g_tpak_stTpak.stTpakInfo.Body.LockStatus.BYTES.LeftRearLock);
	printf("Tpak_CopilotLock:%d\n",g_tpak_stTpak.stTpakInfo.Body.LockStatus.BYTES.CopilotLock);
	printf("Tpak_DrivingLock:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.LockStatus.BYTES.DrivingLock);
	
	printf("Tpak_RoofStatus:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.RoofStatus);
	
	printf("Tpak_LeftFrontWindow:%d\n",g_tpak_stTpak.stTpakInfo.Body.WindowStatus.BYTES.LeftFrontWindow);
	printf("Tpak_RightFrontWindow:%d\n",g_tpak_stTpak.stTpakInfo.Body.WindowStatus.BYTES.RightFrontWindow);
	printf("Tpak_LeftRearWindowHigh:%d\n",g_tpak_stTpak.stTpakInfo.Body.WindowStatus.BYTES.LeftRearWindow);
	printf("Tpak_RightRearWindow:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.WindowStatus.BYTES.RightRearWindow);
	
	printf("Tpak_Headlights:%d\n",g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Headlights);
	printf("Tpak_Positionlights:%d\n",g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Positionlights);
	printf("Tpak_Nearlights:%d\n",g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Nearlights);
	printf("Tpak_Rearfoglights:%d\n",g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Rearfoglights);
	printf("Tpak_Frontfoglights:%d\n",g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Frontfoglights);
	printf("Tpak_Leftlights:%d\n",g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Leftlights);
	printf("Tpak_Rightlights:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Rightlights);

	printf("Tpak_EngineStatus:%d\n",g_tpak_stTpak.stTpakInfo.Body.EngineStatus);
	printf("Tpak_EngineSpeed:%d\n",g_tpak_stTpak.stTpakInfo.Body.EngineSpeed);
	printf("Tpak_GearInfor:%d\n",g_tpak_stTpak.stTpakInfo.Body.GearInfor);
	printf("Tpak_HandParkStatus:%d\n",g_tpak_stTpak.stTpakInfo.Body.HandParkStatus);
	printf("Tpak_IgnInfo:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.IgnInfo);

	printf("Tpak_BPVol:%d\n",g_tpak_stTpak.stTpakInfo.Body.BPSts.BYTES.BPVol);
    printf("Tpak_BPSur:%d\n",g_tpak_stTpak.stTpakInfo.Body.BPSts.BYTES.BPSur);
    printf("Tpak_BPSoc:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.BPSts.BYTES.BPSoc);

	printf("Tpak_ChargeStatus:%d\n",g_tpak_stTpak.stTpakInfo.Body.ChargeStatus);
	printf("Tpak_ChargeGunStatus:%d\n",g_tpak_stTpak.stTpakInfo.Body.ChargeGunStatus);
	printf("Tpak_BreakStatus:%d\n",g_tpak_stTpak.stTpakInfo.Body.BreakStatus);
	printf("Tpak_RemainEndurance:%d\n",g_tpak_stTpak.stTpakInfo.Body.RemainEndurance);
	printf("Tpak_BeltStatus:%d\n",g_tpak_stTpak.stTpakInfo.Body.BeltStatus);
	printf("Tpak_MotorStatus:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.MotorStatus);

	printf("Tpak_MotorSpeed:%d\n",g_tpak_stTpak.stTpakInfo.Body.MotorSpeed);
	printf("Tpak_PowerForceSts:%d\n\n",g_tpak_stTpak.stTpakInfo.Body.PowerForceSts);
	
	printf("Tpak_McuVersion:%d\n",g_tpak_stTpak.stTpakInfo.Body.McuVersion);
	printf("Tpak_CarGwNodeMiss:%d\n\n\n",g_tpak_stTpak.stTpakInfo.Body.CarGwNodeMiss);
#endif
    return 0;
    
}





uint8_t tpak_PakageVeichleData(uint16_t *len,uint8_t *buf,uint8_t msgType)
{
    uint8_t Value = 0 ;
	uint8_t ret = RET_FAIL ;
    uint16_t TempLen = 0 ;
	uint16_t tempCount = 0 ;
	static uint8_t collectPacketSN = 1;
	if((NULL == buf) || NULL == len)
	{	
		ret = RET_INVALID;
	}
	else
	{
		g_tpak_stTpak.stTpakInfo.HeadFlag					= 0x7e;
		g_tpak_stTpak.stTpakInfo.Length 					= 0;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.Acp_AppID 	= TPAK_VEICHLEINFO_APPID;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.PrivateFlag 	= 1;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.Reserved_ID	= 0;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.MsgType		= msgType;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.CryptFlag		= TPAK_AcpCryptFlag_NULL;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.TestFlag 		= TPAK_VERSION_FLAG;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.Reserved_T	= 0;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.VersionFlag	= 1;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.Version		= 0;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.MsgCtrlFlag	= 2;
		g_tpak_stTpak.stTpakInfo.Header.BYTES.MsgLength		= sizeof(TPAK_VeichleInfo_ST)
															- sizeof(TPAK_AcpAppHeader_UN) - TPAK_Message_Head_Len;
		g_tpak_stTpak.stTpakInfo.Length						= sizeof(TPAK_VeichleInfo_ST) - TPAK_Message_Head_Len;
		tpak_TimeDeal(&g_tpak_stTpak.stTpakInfo.TimeStamp.Data[1], 1);
		*len = g_tpak_stTpak.stTpakInfo.Length + TPAK_Message_Head_Len;

		g_tpak_stTpak.stTpakInfo.Body.SendMode = 0;
		if(collectPacketSN == 254)
			collectPacketSN = 1;
		g_tpak_stTpak.stTpakInfo.Body.CollectPacketSN = collectPacketSN ++;
		tpak_TimeDeal(g_tpak_stTpak.stTpakInfo.Body.GatherTime.Data , 0);
		tempCount = TPAK_SIGNCODE_TOTAL;
		g_tpak_stTpak.stTpakInfo.Body.SignCodeTotal[0] = (tempCount >> 8) & 0xFF;
		g_tpak_stTpak.stTpakInfo.Body.SignCodeTotal[0] = tempCount & 0xFF;

	    /*YOU CAN ONLY USE BIF_xxxFUNCTION*/
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Direction = VIF_GetDirection();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Sec = VIF_GetSec();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Min = VIF_GetMin();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Hour = VIF_GetHour();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Day = VIF_GetDay();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Mon = VIF_GetMon();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.Year = VIF_GetYear();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.LatSts = VIF_GetStsLat();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.LatDeg = VIF_GetLatDeg();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.LonSts = VIF_GetStsLon();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.LonSts = VIF_GetLonDeg();
	    g_tpak_stTpak.stTpakInfo.Body.GPSInfo.BYTES.PosiVailed = VIF_GetStsPosiVailed();
	    
	    g_tpak_stTpak.stTpakInfo.Body.RemainOil.BYTES.RemainOilVule = VIF_GetRemainOil();
		
	    g_tpak_stTpak.stTpakInfo.Body.Odo =VIF_GetOdo();
	    g_tpak_stTpak.stTpakInfo.Body.MaintVol = VIF_GetMaintVol();
	    g_tpak_stTpak.stTpakInfo.Body.CarSpeed = VIF_GetCarSpeed();
	    
	    g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.DrivingDoor = VIF_GetDoorStatusDrivingDoor();
	    g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.CopilotDoor = VIF_GetDoorStatusCopilotDoor();
	    g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.LeftRearDoor = VIF_GetDoorStatusLeftRearDoor();
	    g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.RightRearDoor = VIF_GetDoorStatusRightRearDoor();
	    g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.RearCanopy = VIF_GetDoorStatusRearCanopy();
	    g_tpak_stTpak.stTpakInfo.Body.DoorStatus.BYTES.EngineCover = VIF_GetDoorStatusEngineCover();
	    
	    g_tpak_stTpak.stTpakInfo.Body.LockStatus.BYTES.RightRearLock = VIF_GetLockStatusRightRearLock();
	    g_tpak_stTpak.stTpakInfo.Body.LockStatus.BYTES.LeftRearLock = VIF_GetLockStatusLeftRearLock();
	    g_tpak_stTpak.stTpakInfo.Body.LockStatus.BYTES.CopilotLock = VIF_GetLockStatusCopilotLock();
	    g_tpak_stTpak.stTpakInfo.Body.LockStatus.BYTES.DrivingLock= VIF_GetLockStatusDrivingLock();
	    
	    g_tpak_stTpak.stTpakInfo.Body.RoofStatus = VIF_GetRoofStatus();
	    
	    g_tpak_stTpak.stTpakInfo.Body.WindowStatus.BYTES.LeftFrontWindow = VIF_GetWindowStatusLeftFrontWindow();
	    g_tpak_stTpak.stTpakInfo.Body.WindowStatus.BYTES.RightFrontWindow = VIF_GetWindowStatusRightFrontWindow();
	    g_tpak_stTpak.stTpakInfo.Body.WindowStatus.BYTES.LeftRearWindow = 	VIF_GetWindowStatusLeftRearWindow();
	    g_tpak_stTpak.stTpakInfo.Body.WindowStatus.BYTES.RightRearWindow = VIF_GetWindowStatusRightRearWindow();

	    g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Headlights = VIF_GetLampStautsHeadlights();
	    g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Positionlights = VIF_GetLampStautsPositionlights();
	    g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Nearlights = VIF_GetLampStautsNearlights();
	    g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Rearfoglights = VIF_GetLampStautsRearfoglights();
	    g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Frontfoglights = VIF_GetLampStautsFrontfoglights();
	    g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Leftlights = VIF_GetLampStautsLeftlights();
	    g_tpak_stTpak.stTpakInfo.Body.LampStauts.BYTES.Rightlights = VIF_GetLampStautsRightlights();

	    g_tpak_stTpak.stTpakInfo.Body.EngineStatus = VIF_GetEngineStatus();
	    g_tpak_stTpak.stTpakInfo.Body.EngineSpeed = VIF_GetEngineSpeed();
	    g_tpak_stTpak.stTpakInfo.Body.GearInfor = VIF_GetGearInfor();
	    g_tpak_stTpak.stTpakInfo.Body.HandParkStatus = VIF_GetHandParkStatus();
	    g_tpak_stTpak.stTpakInfo.Body.IgnInfo = VIF_GetIgnInfo();
	    
	    g_tpak_stTpak.stTpakInfo.Body.BPSts.BYTES.BPVol = VIF_GetBPVol();
	    g_tpak_stTpak.stTpakInfo.Body.BPSts.BYTES.BPSur = VIF_GetBPSur();
	    g_tpak_stTpak.stTpakInfo.Body.BPSts.BYTES.BPSoc = VIF_GetBPSoc();
	    
	    g_tpak_stTpak.stTpakInfo.Body.ChargeStatus = VIF_GetChargeStatus();
	    g_tpak_stTpak.stTpakInfo.Body.ChargeGunStatus = VIF_GetChargeGunStatus();
	    g_tpak_stTpak.stTpakInfo.Body.BreakStatus = VIF_GetBreakStatus();
	    g_tpak_stTpak.stTpakInfo.Body.RemainEndurance = VIF_GetRemainEndurance();
	    g_tpak_stTpak.stTpakInfo.Body.BeltStatus = VIF_GetBeltStatus();
	    g_tpak_stTpak.stTpakInfo.Body.MotorStatus = VIF_GetMotorStatus();
	    g_tpak_stTpak.stTpakInfo.Body.MotorSpeed = VIF_GetMotorSpeed();
	    g_tpak_stTpak.stTpakInfo.Body.PowerForceSts = VIF_GetPowerForceSts();
	    
	    VIF_GetMcuVersion(g_tpak_stTpak.stTpakInfo.Body.McuVersion,TempLen);
	    
	    g_tpak_stTpak.stTpakInfo.Body.CarGwNodeMiss = VIF_GetCarGwNodeMiss();
	//    buf = g_tpak_stTpak.Buf;
		g_tpak_stTpak.stTpakInfo.Length	 = ((g_tpak_stTpak.stTpakInfo.Length << 8) & 0xFF00) | ((g_tpak_stTpak.stTpakInfo.Length >> 8) & 0x00FF);
		g_tpak_stTpak.stTpakInfo.Length	 = ((g_tpak_stTpak.stTpakInfo.Length << 8) & 0xFF00) | ((g_tpak_stTpak.stTpakInfo.Length >> 8) & 0x00FF);
		memcpy(buf, g_tpak_stTpak.Buf, TPAK_VEICHLE_DATA_LEN );
		ret = RET_OK;
	}
    
    return ret;
}

uint8_t tpak_PakageCtrlACKData(uint16_t *len,uint8_t* pData, uint8_t Type)
{
	uint8_t ret = RET_FAIL ;
	if((NULL == pData) || NULL == len)
	{	
		ret = RET_INVALID;
	}
	else
	{
		g_tpak_stTpak.stTpakCtrlACK.HeadFlag					= 0x7e;
		g_tpak_stTpak.stTpakCtrlACK.Length						= 0;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.Acp_AppID 	    = TPAK_RemoteCtrlACK_APPID;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.PrivateFlag 	= 1;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.Reserved_ID	= 0;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.MsgType		= 1;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.CryptFlag		= TPAK_AcpCryptFlag_NULL;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.TestFlag 		= TPAK_VERSION_FLAG;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.Reserved_T	= 0;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.VersionFlag	= 1;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.Version		= 0;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.MsgCtrlFlag	= 2;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.MsgLength		= sizeof(TPAK_HeadTimeStamp_UN) + 1
		                                                          + sizeof(TPAK_RemoteCtrlACKBody_ST);
		g_tpak_stTpak.stTpakCtrlACK.Length						= g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.MsgLength + sizeof(TPAK_AcpAppHeader_UN);
		*len = g_tpak_stTpak.stTpakCtrlACK.Length + TPAK_Message_Head_Len;
		g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.MsgLength	 = ((g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.MsgLength << 8) & 0xFF00) | ((g_tpak_stTpak.stTpakCtrlACK.Header.BYTES.MsgLength >> 8) & 0x00FF);
		g_tpak_stTpak.stTpakCtrlACK.Length	 = ((g_tpak_stTpak.stTpakCtrlACK.Length << 8) & 0xFF00) | ((g_tpak_stTpak.stTpakCtrlACK.Length >> 8) & 0x00FF);
		tpak_TimeDeal(g_tpak_stTpak.stTpakCtrlACK.TimeStamp.Data, 1);
        	g_tpak_stTpak.stTpakCtrlACK.ErrorElem                   = 0x00;
		g_tpak_stTpak.stTpakCtrlACK.Body.Total                  = 0x01;
		
		g_tpak_stTpak.stTpakCtrlACK.Body.CtrlCode               = TIF_GetCtrlCode();
		g_tpak_stTpak.stTpakCtrlACK.Body.CtrlParameter[0]       = VIF_GetCtrlResult();
		memcpy(pData, g_tpak_stTpak.Buf, TPAK_VEICHLE_DATA_LEN );
		ret = RET_OK;
	}
	return ret;

}

uint8_t tpak_PakageSyncData(uint16_t *len,uint8_t* pData, uint8_t Type)
{
	uint8_t ret = RET_FAIL;
	if((NULL == pData) || NULL == len)
	{	
		ret = RET_INVALID;
	}
	else
	{
		g_tpak_stTpak.stTpakSync.HeadFlag					= 0x7e;
		g_tpak_stTpak.stTpakSync.Length						= 0;
		g_tpak_stTpak.stTpakSync.Header.BYTES.Acp_AppID 	= TPAK_TimeSync_APPID;
		g_tpak_stTpak.stTpakSync.Header.BYTES.PrivateFlag 	= 1;
		g_tpak_stTpak.stTpakSync.Header.BYTES.Reserved_ID	= 0;
		g_tpak_stTpak.stTpakSync.Header.BYTES.MsgType		= 1;
		g_tpak_stTpak.stTpakSync.Header.BYTES.CryptFlag		= TPAK_AcpCryptFlag_NULL;
		g_tpak_stTpak.stTpakSync.Header.BYTES.TestFlag 		= TPAK_VERSION_FLAG;
		g_tpak_stTpak.stTpakSync.Header.BYTES.Reserved_T	= 0;
		g_tpak_stTpak.stTpakSync.Header.BYTES.VersionFlag	= 1;
		g_tpak_stTpak.stTpakSync.Header.BYTES.Version		= 0;
		g_tpak_stTpak.stTpakSync.Header.BYTES.MsgCtrlFlag	= 2;
		g_tpak_stTpak.stTpakSync.Header.BYTES.MsgLength		= 0;
		g_tpak_stTpak.stTpakSync.Length						+= 5;
		*len = g_tpak_stTpak.stTpakSync.Length + TPAK_Message_Head_Len;
		g_tpak_stTpak.stTpakSync.Length	 = ((g_tpak_stTpak.stTpakSync.Length << 8) & 0xFF00) | ((g_tpak_stTpak.stTpakSync.Length >> 8) & 0x00FF);
		g_tpak_stTpak.stTpakSync.Length	 = ((g_tpak_stTpak.stTpakSync.Length << 8) & 0xFF00) | ((g_tpak_stTpak.stTpakSync.Length >> 8) & 0x00FF);
		tpak_TimeDeal(&g_tpak_stTpak.stTpakInfo.TimeStamp.Data[1], 1);
		g_tpak_stTpak.stTpakInfo.Length						+= 7;
	}
}
uint8_t tpak_PakageUpgradeData(uint16_t *len,uint8_t* pData, uint8_t type, uint8_t result)
{

	uint8_t ret = RET_FAIL;
	if((NULL == pData) || NULL == len)
	{	
		ret = RET_INVALID;
	}
	else
	{
		g_tpak_stTpak.stUpgrade.HeadFlag					= 0x7e;
		g_tpak_stTpak.stUpgrade.Length 					= 0;
		g_tpak_stTpak.stUpgrade.Header.BYTES.Acp_AppID 	= TPAK_TimeSync_APPID;
		g_tpak_stTpak.stUpgrade.Header.BYTES.PrivateFlag	= 1;
		g_tpak_stTpak.stUpgrade.Header.BYTES.Reserved_ID	= 0;
		g_tpak_stTpak.stUpgrade.Header.BYTES.MsgType		= type;
		g_tpak_stTpak.stUpgrade.Header.BYTES.CryptFlag 	= TPAK_AcpCryptFlag_NULL;
		g_tpak_stTpak.stUpgrade.Header.BYTES.TestFlag		= TPAK_VERSION_FLAG;
		g_tpak_stTpak.stUpgrade.Header.BYTES.Reserved_T	= 0;
		g_tpak_stTpak.stUpgrade.Header.BYTES.VersionFlag	= 1;
		g_tpak_stTpak.stUpgrade.Header.BYTES.Version		= 0;
		g_tpak_stTpak.stUpgrade.Header.BYTES.MsgCtrlFlag	= 2;
		g_tpak_stTpak.stUpgrade.Header.BYTES.MsgLength 	= 0;
		g_tpak_stTpak.stUpgrade.Length 					+= 4;
		tpak_TimeDeal(&g_tpak_stTpak.stTpakInfo.TimeStamp.Data[1], 1);
		g_tpak_stTpak.stUpgrade.Length 					+= 7;
		switch(type)
		{
			case 4:
				g_tpak_stTpak.stUpgrade.Header.BYTES.MsgLength 	    = 0;
				g_tpak_stTpak.stUpgrade.Length 					    ++;
				break;
			case 5:
				g_tpak_stTpak.stUpgrade.Header.BYTES.MsgLength 	    = 1;
				g_tpak_stTpak.stUpgrade.Result					    = result;
				g_tpak_stTpak.stUpgrade.Length 					    += 2;
				break;
			default:
		        break;
		}
		
		*len = g_tpak_stTpak.stUpgrade.Length + TPAK_Message_Head_Len;
		g_tpak_stTpak.stUpgrade.Length	 = ((g_tpak_stTpak.stUpgrade.Length << 8) & 0xFF00) | ((g_tpak_stTpak.stUpgrade.Length >> 8) & 0x00FF);
		g_tpak_stTpak.stUpgrade.Length	 = ((g_tpak_stTpak.stUpgrade.Length << 8) & 0xFF00) | ((g_tpak_stTpak.stUpgrade.Length >> 8) & 0x00FF);

	}

}


uint8_t TPAK_PackageData(uint16_t *len,uint8_t PAKID, uint8_t* pData, uint8_t type,  uint8_t result)
{
	uint8_t ret = RET_FAIL;
	if(TPAK_InitSts == FALSE)
	{
		ret = RET_FAIL;
	}
	else if((NULL == pData) || NULL == len)
	{
		ret = RET_INVALID;
	}
	else
	{
		switch(PAKID)
		{
		    case TPAK_VEICHLEINFO_APPID:
		        ret = tpak_PakageVeichleData(len,pData, TPAK_MessageType_VeichleInfo);
				TPAK_Printf();
		        break;
	     	case TPAK_RemoteCtrlACK_APPID:
		        ret = tpak_PakageCtrlACKData(len,pData, type);
				TPAK_Printf();
		        break;
		    case TPAK_TimeSync_APPID:
		        ret = tpak_PakageSyncData(len,pData, type);
				TPAK_Printf();
		        break;
			case TPAK_Upgrade_APPID:
		        ret = tpak_PakageUpgradeData(len,pData, type, result);
				TPAK_Printf();
		        break;
		    default:
		        break;
		}
	}
   return ret;
}

