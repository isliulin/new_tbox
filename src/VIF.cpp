#include "VIF.h"




uint8_t VIF_InitSts = FALSE;
VIF_VeichleInformation_UN 	g_stVif;


uint8_t VIF_Init(void)
{
	/*if uart com is failure ,should be false*/
	VIF_InitSts = TRUE;
	memset(g_stVif.Buf,0,sizeof(g_stVif));
	return VIF_InitSts;
}
	
uint8_t VIF_DeInit(void)
{

	VIF_InitSts = FALSE;
	return VIF_InitSts;
}

uint8_t VIF_Printf(void)
{
	int i = 0;
	for(i = 0; i < 150; i ++)
	{
		printf("Buf[%d]:%02X\n",i,g_stVif.Buf[i]);
	}
	printf("\n\n");
	printf("HeadFlag:%02x\n",g_stVif.stVifInfo.Head.HeadFlag);
	printf("BodyLen:%02x\n",g_stVif.stVifInfo.Head.BodyLen);
	printf("CmdID:%02x\n",g_stVif.stVifInfo.Head.CmdID);	
	printf("SerialNumber:%02x\n",g_stVif.stVifInfo.Head.SerialNumber);
	printf("Attr:%s\n",g_stVif.stVifInfo.Head.Attr);
	printf("Lat:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Lat);
	printf("Lon:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Lon);
	printf("GpsAntenna:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.GpsAntenna);
	printf("AutoLock:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.AutoLock);
	printf("AccPlcPowerType:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.AccPlcPowerType);
	printf("OpenCover:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.OpenCover);
	printf("BusFault:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.BusFault);
	printf("HostPowerCut:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.HostPowerCut);
	printf("BackBatPowerCut:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.BackBatPowerCut);
	printf("BackBatLowPower:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.BackBatLowPower);
	printf("Rs485BaudChange:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Rs485BaudChange);
	printf("CanBaudChange:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CanBaudChange);
	printf("DevHerar:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.DevHerar);
	printf("DevSleep:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.DevSleep);
	printf("HostLowPower:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.HostLowPower);
	printf("Collision:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Collision);
	printf("Remove:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Remove);
	printf("PlcControlled:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.PlcControlled);
	printf("PosiVailed:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.PosiVailed);
	printf("CanTimerSendState:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CanTimerSendState);
	printf("Rs485TimerSendState:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Rs485TimerSendState);
	printf("Flashfault:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Flashfault);
	printf("CAN0fault:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CAN0fault);
	printf("CAN1fault:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CAN1fault);
	printf("PositionModuleState:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.PositionModuleState);
	printf("CAN0BUSOFF:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CAN0BUSOFF);
	printf("CAN1BUSOFF:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CAN1BUSOFF);
	printf("MCUConnect4GError:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.MCUConnect4GError);
	printf("LatDeg:%f\n",g_stVif.stVifInfo.Body.VifInfoBody.LatDeg);
	printf("LonDeg:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LonDeg);
	printf("Hight:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Hight);
	printf("Direction:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Direction);
	printf("Year:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Year);
	printf("Mon:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Mon);
	printf("Day:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Day);
	printf("Hour:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Hour);
	printf("Min:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Min);
	printf("Sec:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Sec);
	printf("RemainOil:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.RemainOil);
	printf("BPSoc:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.BPSoc);
	printf("CarSpeed:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.CarSpeed);
	printf("Odo:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.Odo);
	printf("DrivingDoor:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.DrivingDoor);
	printf("CopilotDoor:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.CopilotDoor);
	printf("LeftRearDoor:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.LeftRearDoor);
	printf("RightRearDoor:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.RightRearDoor);
	printf("RearCanopy:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.RearCanopy);
	printf("EngineCover:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.EngineCover);
	printf("RightRearLock:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LockStatus.BYTES.RightRearLock);
	printf("LeftRearLock:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LockStatus.BYTES.LeftRearLock);
	printf("CopilotLock:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LockStatus.BYTES.CopilotLock);
	printf("DrivingLock:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LockStatus.BYTES.DrivingLock);
	printf("RoofStatus:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.RoofStatus);
	printf("LeftFrontWindow:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.LeftFrontWindow);
	printf("RightFrontWindow:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.RightFrontWindow);
	printf("LeftRearWindowHigh:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.LeftRearWindowHigh);
	printf("LeftRearWindowLow:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.LeftRearWindowLow);
	printf("RightRearWindow:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.RightRearWindow);
	printf("Headlights:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Headlights);
	printf("Positionlights:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Positionlights);
	printf("Nearlights:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Nearlights);
	printf("Rearfoglights:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Rearfoglights);
	printf("Frontfoglights:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Frontfoglights);
	printf("Leftlights:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Leftlights);
	printf("Rightlights:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Rightlights);
	printf("McuVersion:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.McuVersion);
	printf("MotorStatus:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.MotorStatus);
	printf("MotorSpeed:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.MotorSpeed);
	printf("EngineStatus:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.EngineStatus);
	printf("EngineSpeed:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.EngineSpeed);
	printf("GearInfor:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.GearInfor);
	printf("IgnInfo:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.IgnInfo);
	printf("HandParkStatus:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.HandParkStatus);
	printf("BeltStatus:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.BeltStatus);
	printf("ChargeStatus:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.ChargeStatus);
	printf("ChargeGunStatus:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.ChargeGunStatus);
	printf("BreakStatus:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.BreakStatus);
	printf("RemainEndurance:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.RemainEndurance);
	
	printf("BPVol:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.BPVol);
	printf("BPSur:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.BPSur);
	printf("MaintVol:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.MaintVol);
	printf("PowerForceSts:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.PowerForceSts);
	printf("CarIDState:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.CarIDState);
	printf("CarGwNodeMiss:%02x\n",g_stVif.stVifInfo.Body.VifInfoBody.CarGwNodeMiss);
	
	return 0;
	
}

uint8_t vif_Analysis(uint8_t *outBuf,uint8_t *inBuf,uint8_t inLen,uint8_t *outLen)
{
	uint8_t i = 0;
	uint8_t tempLen = 0;
	uint8_t ret = RET_FAIL;
	if(VIF_InitSts == FALSE)
	{
		return ret;
	}
	else if((NULL == outBuf) ||(NULL == inBuf) ||(NULL == outLen) || (inLen > VIF_VEICHLE_DATA_LEN))
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

uint16_t vif_Crc16Check(uint8_t *pBuf,uint8_t len)
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
uint8_t vif_AllotInfo(uint8_t *pBuf,uint8_t len)
{
	uint8_t cmdID = 0;

	cmdID = *(pBuf + VIF_INFO_CMDID);
	switch(cmdID)
	{
	case VIF_REPROT_VECIHLE_INFORMATION:
		memcpy(g_stVif.Buf ,pBuf,len);
		break;
	default:
    	break;
	}
}
*/
uint8_t VIF_Parse(uint8_t *pBuf, uint8_t len, VIF_VeichleInformation_UN *mcuVif)
	
{
	uint8_t ret = RET_FAIL;
	uint8_t pData[VIF_VEICHLE_DATA_LEN] = {0};
	uint16_t checkCrc = 0;
	uint8_t bodylen = 0;
	uint8_t datalen = 0;
	int i= 0;

	if(VIF_InitSts == FALSE)
	{
		return ret;
	}
	else if((NULL == pBuf) || (len >= VIF_VEICHLE_DATA_LEN))
	{	
		return RET_INVALID;
	}
	else
	{
		vif_Analysis(pData,pBuf,len,&datalen);
		bodylen = (pData[1] << 8) + pData[2];
		checkCrc = (pData[1 + VIF_VEICHLE_DATA_HEAD_LEN + bodylen] << 8) 
					+ pData[1 + VIF_VEICHLE_DATA_HEAD_LEN + bodylen + 1];
		if(checkCrc != vif_Crc16Check(&pData[1],bodylen+VIF_VEICHLE_DATA_HEAD_LEN))
		{
			ret = RET_INVALID;
		}
		else
		{
			memcpy(g_stVif.Buf ,pData,datalen);
			memcpy(mcuVif->Buf ,g_stVif.Buf,datalen);
			ret = RET_OK;
		}
		
	}

	return ret;
}

uint16_t VIF_GetBodyLen(void)
{
	return g_stVif.stVifInfo.Head.BodyLen;
}

uint8_t VIF_GetCmdID(void)
{
	return g_stVif.stVifInfo.Head.CmdID;
}

uint16_t VIF_GetSerialNumber(void)
{
	return g_stVif.stVifInfo.Head.SerialNumber;
}
uint8_t VIF_GetAttr(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((TRUE == VIF_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{
		pData = g_stVif.stVifInfo.Head.Attr;
		len = 2;
		ret = RET_OK;
	}
	return ret;
}

uint32_t VIF_GetStsLat(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Lat;
}

uint32_t VIF_GetStsLon(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Lon;
}

uint8_t VIF_GetStsGpsAntenna(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.GpsAntenna;
}

uint8_t VIF_GetStsAutoLock(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.AutoLock;
}

uint8_t VIF_GetStsAccPlcPowerType(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.AccPlcPowerType;
}

uint8_t VIF_GetStsOpenCover(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.OpenCover;
}

uint8_t VIF_GetStsBusFault(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.BusFault;
}

uint8_t VIF_GetStsHostPowerCut(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.HostPowerCut;
}

uint8_t VIF_GetStsBackBatPowerCut(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.BackBatPowerCut;
}

uint8_t VIF_GetStsBackBatLowPower(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.BackBatLowPower;
}

uint8_t VIF_GetStsRs485BaudChange(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Rs485BaudChange;
}

uint8_t VIF_GetStsCanBaudChange(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CanBaudChange;
}

uint8_t VIF_GetStsDevHerar(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.DevHerar;
}

uint8_t VIF_GetStsDevSleep(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.DevSleep;
}

uint8_t VIF_GetStsHostLowPower(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.HostLowPower;
}

uint8_t VIF_GetStsCollision(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Collision;
}

uint8_t VIF_GetStsRemove(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Remove;
}

uint8_t VIF_GetStsPlcControlled(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.PlcControlled;
}

uint8_t VIF_GetStsPosiVailed(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.PosiVailed;
}

uint8_t VIF_GetStsCanTimerSendState(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CanTimerSendState;
}

uint8_t VIF_GetStsRs485TimerSendState(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Rs485TimerSendState;
}

uint8_t VIF_GetStsFlashfault(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.Flashfault;
}

uint8_t VIF_GetStsCAN0fault(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CAN0fault;
}

uint8_t VIF_GetStsCAN1fault(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CAN1fault;
}

uint8_t VIF_GetStsPositionModuleState(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.PositionModuleState;
}

uint8_t VIF_GetStsCAN0BUSOFF(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CAN0BUSOFF;
}

uint8_t VIF_GetStsCAN1BUSOFF(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.CAN1BUSOFF;
}

uint8_t VIF_GetStsMCUConnect4GError(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sts.bitState_t.MCUConnect4GError;
}


uint32_t VIF_GetLatDeg(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LatDeg;
}

uint32_t VIF_GetLonDeg(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LonDeg;
}

uint16_t VIF_GetHight(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Hight;
}

uint16_t VIF_GetDirection(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Direction;
}

uint8_t VIF_GetYear(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Year;
}

uint8_t VIF_GetMon(void)
{
	
	return g_stVif.stVifInfo.Body.VifInfoBody.Mon + 1;
}

uint8_t VIF_GetDay(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Day;
}

uint8_t VIF_GetHour(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Hour;
}

uint8_t VIF_GetMin(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Min;
}

uint8_t VIF_GetSec(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Sec;
}

uint8_t VIF_GetRemainOil(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.RemainOil;
}

uint16_t VIF_GetBPSoc(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.BPSoc;
}

uint16_t VIF_GetCarSpeed(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.CarSpeed;
}

uint32_t VIF_GetOdo(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.Odo;
}

uint8_t VIF_GetDoorStatusDrivingDoor(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.DrivingDoor;
}

uint8_t VIF_GetDoorStatusCopilotDoor(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.CopilotDoor;
}

uint8_t VIF_GetDoorStatusLeftRearDoor(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.LeftRearDoor;
}

uint8_t VIF_GetDoorStatusRightRearDoor(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.RightRearDoor;
}

uint8_t VIF_GetDoorStatusRearCanopy(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.RearCanopy;
}

uint8_t VIF_GetDoorStatusEngineCover(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.DoorStatus.BYTES.EngineCover;
}

uint8_t VIF_GetLockStatusRightRearLock(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LockStatus.BYTES.RightRearLock;
}

uint8_t VIF_GetLockStatusLeftRearLock(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LockStatus.BYTES.LeftRearLock;
}

uint8_t VIF_GetLockStatusCopilotLock(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LockStatus.BYTES.CopilotLock;
}

uint8_t VIF_GetLockStatusDrivingLock(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LockStatus.BYTES.DrivingLock;
}


uint8_t VIF_GetRoofStatus(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.RoofStatus;
}


uint8_t VIF_GetWindowStatusLeftFrontWindow(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.LeftFrontWindow;
}

uint8_t VIF_GetWindowStatusRightFrontWindow(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.RightFrontWindow;
}

uint8_t VIF_GetWindowStatusLeftRearWindow(void)
{
	uint8_t LeftRearWindow;
	LeftRearWindow = ((g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.LeftRearWindowHigh << 2) & 0x04)
						+ ((g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.LeftRearWindowLow) & 0x03);
	return LeftRearWindow;

}

uint8_t VIF_GetWindowStatusRightRearWindow(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.WindowStatus.BYTES.RightRearWindow;
}

uint8_t VIF_GetLampStautsHeadlights(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Headlights;
}

uint8_t VIF_GetLampStautsPositionlights(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Positionlights;
}

uint8_t VIF_GetLampStautsNearlights(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Nearlights;
}

uint8_t VIF_GetLampStautsRearfoglights(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Rearfoglights;
}

uint8_t VIF_GetLampStautsFrontfoglights(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Frontfoglights;
}

uint8_t VIF_GetLampStautsLeftlights(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Leftlights;
}

uint8_t VIF_GetLampStautsRightlights(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.LampStauts.BYTES.Rightlights;
}

uint8_t VIF_GetMcuVersion(uint8_t *pData,uint16_t &len)
{
	uint8_t ret = RET_FAIL;
	if ((TRUE == VIF_InitSts) || (NULL == pData))
	{
		ret = RET_INVALID;
	}
	else
	{
//		pData = g_stVif.stVifInfo.McuVersion;
		memcpy(pData, g_stVif.stVifInfo.Body.VifInfoBody.McuVersion, 12);
		len = 12;
		ret = RET_OK;
	}
	return ret;
}

uint8_t VIF_GetMotorStatus(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.MotorStatus;
}

uint16_t VIF_GetMotorSpeed(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.MotorSpeed;
}

uint8_t VIF_GetEngineStatus(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.EngineStatus;
}

uint16_t VIF_GetEngineSpeed(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.EngineSpeed;
}

uint8_t VIF_GetGearInfor(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.GearInfor;
}

uint8_t VIF_GetIgnInfo(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.IgnInfo;
}

uint8_t VIF_GetHandParkStatus(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.HandParkStatus;
}

uint8_t VIF_GetBeltStatus(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.BeltStatus;
}

uint8_t VIF_GetChargeStatus(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.ChargeStatus;
}

uint8_t VIF_GetChargeGunStatus(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.ChargeGunStatus;
}

uint8_t VIF_GetBreakStatus(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.BreakStatus;
}

uint16_t VIF_GetRemainEndurance(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.RemainEndurance;
}

uint16_t VIF_GetBPVol(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.BPVol;
}

uint16_t VIF_GetBPSur(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.BPSur;
}

uint16_t VIF_GetMaintVol(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.MaintVol;
}

uint8_t VIF_GetPowerForceSts(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.PowerForceSts;
}

uint8_t VIF_GetCarIDState(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.CarIDState;
}
uint8_t VIF_GetCarGwNodeMiss(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.CarGwNodeMiss;
}
/*

uint8_t VIF_GetEngineSystem(void)
{
	return g_stVif.stVifInfo.Body.VifInfoBody.TroubleInfo.EngineSystem;
}

uint8_t VIF_GetTCUSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TCUSystem;
}
uint8_t VIF_GetEmissionSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.EmissionSystem;
}
uint8_t VIF_GetACUSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ACUSystem;
}
uint8_t VIF_GetESPSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ESPSystem;
}
uint8_t VIF_GetABSSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ABSSystem;
}
uint8_t VIF_GetEPSSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.EPSSystem;
}
uint8_t VIF_GetOilPressSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.OilPressSystem;
}
uint8_t VIF_GetOilLowSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.OilLowSystem;
}
uint8_t VIF_GetBreakFluidSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BreakFluidSystem;
}
uint8_t VIF_GetChargeSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ChargeSystem;
}
uint8_t VIF_GetBreakSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BreakSystem;
}
uint8_t VIF_GetTyrePressSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyrePressSystem;
}
uint8_t VIF_GetStartStopSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.StartStopSystem;
}
uint8_t VIF_GetLampSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LampSystem;
}
uint8_t VIF_GetELockSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ELockSystem;
}
uint8_t VIF_GetEngineTempSytem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.EngineTempSytem;
}
uint8_t VIF_GetEPBSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.EPBSystem;
}
uint8_t VIF_GetUpperBeamSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.UpperBeamSystem;
}
uint8_t VIF_GetNavigateSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.NavigateSystem;
}
uint8_t VIF_GetCrashSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.CrashSystem;
}
uint8_t VIF_GetRoalSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.RoalSystem;
}
uint8_t VIF_GetBlindMonitorSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BlindMonitorSystem;
}
uint8_t VIF_GetOperateAC(void)
{
	return g_stVif.stVifInfo.TroubleInfo.OperateAC;
}
uint8_t VIF_GetHighVolSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.HighVolSystem;
}
uint8_t VIF_GetInsulationSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.InsulationSystem;
}
uint8_t VIF_GetInterlockSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.InterlockSystem;
}
uint8_t VIF_GetCellSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.CellSystem;
}
uint8_t VIF_GetMotorSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.MotorSystem;
}
uint8_t VIF_GetEParkSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.EParkSystem;
}
uint8_t VIF_GetEQuantityForCellSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.EQuantityForCellSystem;
}
uint8_t VIF_GetTempeForCellSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TempeForCellSystem;
}
uint8_t VIF_GetTempeForMotorSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TempeForMotorSystem;
}
uint8_t VIF_GetNaviCnstSpdSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.NaviCnstSpdSystem;
}
uint8_t VIF_GetBattChargerSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BattChargerSystem;
}
uint8_t VIF_GetACSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ACSystem;
}
uint8_t VIF_GetAuxSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.AuxSystem;
}
uint8_t VIF_GetEmergyBreakSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.EmergyBreakSystem;
}
uint8_t VIF_GetRadarSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.RadarSystem;
}
uint8_t VIF_GetElcSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ElcSystem;
}
uint8_t VIF_GetLeftFrontTyrePress(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyreSystem.BYTES.LeftFrontTyrePress;
}
uint8_t VIF_GetLeftFrontTyreTemp(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyreSystem.BYTES.LeftFrontTyreTemp;
}
uint8_t VIF_GetRightFrontTyrePress(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyreSystem.BYTES.RightFrontTyrePress;
}
uint8_t VIF_GetRightFrontTyreTemp(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyreSystem.BYTES.RightFrontTyreTemp;
}
uint8_t VIF_GetLeftRearTyrePress(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyreSystem.BYTES.LeftRearTyrePress;
}
uint8_t VIF_GetLeftRearTyreTemp(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyreSystem.BYTES.LeftRearTyreTemp;
}
uint8_t VIF_GetRightRearTyrePress(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyreSystem.BYTES.RightRearTyrePress;
}
uint8_t VIF_GetRightRearTyreTemp(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TyreSystem.BYTES.RightRearTyreTemp;
}
uint8_t VIF_GetDCDCSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.DCDCSystem;
}
uint8_t VIF_GetBCMSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BCMSystem;
}
uint8_t VIF_GetRelaySystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.RelaySystem;
}
uint8_t VIF_GetTSROutSpdSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TSROutSpdSystem;
}
uint8_t VIF_GetTSRLimSpdSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TSRLimSpdSystem;
}
uint8_t VIF_GetAEBinvolvedSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.AEBinvolvedSystem;
}
uint8_t VIF_GetABSinvolvedSystem(void)
{

	return g_stVif.stVifInfo.TroubleInfo.ABSinvolvedSystem;
}
uint8_t VIF_GetASRinvolvedSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ASRinvolvedSystem;
}
uint8_t VIF_GetESPinvolvedSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ESPinvolvedSystem;
}
uint8_t VIF_GetDSMSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.DSMSystem;
}
uint8_t VIF_GetLeaveWheelPromp(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LeaveWheelPromp;
}
uint8_t VIF_GetACCStatus(void)
{
	return g_stVif.stVifInfo.TroubleInfo.ACCStatus;
}
uint8_t VIF_GetAccSpdSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.AccSpdSystem;
}
uint8_t VIF_GetFCWSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.FCWSystem;
}
uint8_t VIF_GetFCWStatus(void)
{
	return g_stVif.stVifInfo.TroubleInfo.FCWStatus;
}
uint8_t VIF_GetTimerForFCWSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TimerForFCWSystem;
}
uint8_t VIF_GetBreakForFCWSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BreakForFCWSystem;
}
uint8_t VIF_GetLDWStatus(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LDWStatus;
}
uint8_t VIF_GetLDWSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LDWSystem;
}
uint8_t VIF_GetWheelForLDWSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.WheelForLDWSystem;
}
uint8_t VIF_GetLKAStatus(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LKAStatus;
}
uint8_t VIF_GetLKAinvolvedSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LKAinvolvedSystem;
}
uint8_t VIF_GetLKADriverPromp(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LKADriverPromp;
}
uint8_t VIF_GetLKADriverForWheel(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LKADriverForWheel;
}
uint8_t VIF_GetBSDStatus(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BSDStatus;
}
uint8_t VIF_GetLeftBSDSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LeftBSDSystem;
}
uint8_t VIF_GetRightBSDSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.RightBSDSystem;
}
uint8_t VIF_GetTimeForWheelBSD(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TimeForWheelBSD;
}
uint8_t VIF_GetTimeForBreakBSD(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TimeForBreakBSD;
}
uint8_t VIF_GetAccelPedalForBSD(void)
{
	return g_stVif.stVifInfo.TroubleInfo.AccelPedalForBSD;
}
uint8_t VIF_GetLeftForFlowSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LeftForFlowSystem;
}
uint8_t VIF_GetRightForFlowSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.RightForFlowSystem;
}
uint8_t VIF_GetRearForFlowSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.RearForFlowSystem;
}
uint8_t VIF_GetBreakForFlowSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BreakForFlowSystem;
}
uint8_t VIF_GetAccelPedalForFlow(void)
{
	return g_stVif.stVifInfo.TroubleInfo.AccelPedalForFlow;
}
uint8_t VIF_GetLeftAUXSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.LeftAUXSystem;
}
uint8_t VIF_GetRightAUXSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.RightAUXSystem;
}
uint8_t VIF_GetRearAUXSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.RearAUXSystem;
}
uint8_t VIF_GetBreakAUXSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.BreakAUXSystem;
}
uint8_t VIF_GetAccelPedalAUXSystem(void)
{
	return g_stVif.stVifInfo.TroubleInfo.AccelPedalAUXSystem;
}
uint8_t VIF_GetDoorcrash(void)
{
	return g_stVif.stVifInfo.TroubleInfo.Doorcrash;
}
uint8_t VIF_GetEnginestop(void)
{
	return g_stVif.stVifInfo.TroubleInfo.Enginestop;
}
uint8_t VIF_GetCarcrash(void)
{
	return g_stVif.stVifInfo.TroubleInfo.Carcrash;
}
uint8_t VIF_GetTrailCarEvent(void)
{
	return g_stVif.stVifInfo.TroubleInfo.TrailCarEvent;
}
uint8_t VIF_GetSkyNotCloseAlm(void)
{
	return g_stVif.stVifInfo.TroubleInfo.SkyNotCloseAlm;
}
*/

uint8_t VIF_GetResponseID(void)
{
	return g_stVif.stVifInfo.Body.VifACKBody.ACKID;
}
uint8_t VIF_GetResponseNumber(void)
{
	return g_stVif.stVifInfo.Body.VifACKBody.ACKNumber;
}
uint8_t VIF_GetResponseResult(void)
{
	return g_stVif.stVifInfo.Body.VifACKBody.ACKResult;
}


uint8_t VIF_GetCtrlCode(void)
{
	return g_stVif.stVifInfo.Body.VifCtrlACKBody.CtrlCode;
}
uint8_t VIF_GetCtrlResult(void)
{
	return g_stVif.stVifInfo.Body.VifCtrlACKBody.CtrlResult;
}



