/*
=========================================================

---VIF------->
--MCU---->VIF-------->TIF--->TPAK--->TSP
--MCU---->VIF-------->BIF--->BPAK--->BLE

----
=========================================================
*/
#include "TIF.h"
#include "VIF.h"

uint8_t TIF_InitSts = FALSE;

uint16_t TIF_GetBodyLen(void)
{
    uint16_t Value = 0 ;
    uint16_t TifBodyLen = 0;

    /*YOU CAN ONLY USE VIF_xxxFUNCTION*/
    Value = VIF_GetBodyLen();
    
    TifBodyLen = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifBodyLen;
}

uint8_t TIF_GetCmdID(void)
{
    uint8_t TifCmdID = 0;

    TifCmdID = VIF_GetCmdID();
    return TifCmdID;
}

uint16_t TIF_GetSerialNumber(void)
{
    uint16_t Value = 0 ;
    uint16_t TifSerialNumber = 0;
    
    Value = VIF_GetSerialNumber();
    
    TifSerialNumber = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);

    return TifSerialNumber;
}
uint8_t TIF_GetAttr(uint8_t *pData,uint16_t &len)
{
    uint8_t ret = RET_FAIL;
    if ((TRUE == TIF_InitSts) || (NULL == pData))
    {
        ret = RET_INVALID;
    }
    else
    {
       ret = VIF_GetAttr(pData, len);
    }
    return ret;
}



uint8_t TIF_GetStsLat(void)
{
    uint8_t TifStsLat = 0;

    TifStsLat = VIF_GetStsLat();
    
    return TifStsLat;
}

uint8_t TIF_GetStsLon(void)
{
    uint8_t TifStsLon = 0;

    TifStsLon = VIF_GetStsLon();
    
    return TifStsLon;
}

uint8_t TIF_GetStsGpsAntenna(void)
{
    uint8_t TifGpsAntenna = 0;

    TifGpsAntenna = VIF_GetStsGpsAntenna();
    
    return TifGpsAntenna;

}

uint8_t TIF_GetStsAutoLock(void)
{
    uint8_t TifStsAutoLock = 0;

    TifStsAutoLock = VIF_GetStsAutoLock();
    
    return TifStsAutoLock;

}

uint8_t TIF_GetStsAccPlcPowerType(void)
{
    uint8_t TifStsAccPlcPowerType = 0;

    TifStsAccPlcPowerType = VIF_GetStsAccPlcPowerType();
    
    return TifStsAccPlcPowerType;

}

uint8_t TIF_GetStsOpenCover(void)
{
    uint8_t TifStsOpenCover = 0;

    TifStsOpenCover = VIF_GetStsOpenCover();
    
    return TifStsOpenCover;

}

uint8_t TIF_GetStsBusFault(void)
{
    uint8_t TifStsBusFault = 0;

    TifStsBusFault = VIF_GetStsBusFault();
    
    return TifStsBusFault;

}

uint8_t TIF_GetStsHostPowerCut(void)
{
    uint8_t TifStsHostPowerCut = 0;

    TifStsHostPowerCut = VIF_GetStsHostPowerCut();
    
    return TifStsHostPowerCut;

}

uint8_t TIF_GetStsBackBatPowerCut(void)
{
    uint8_t TifStsBackBatPowerCut = 0;

    TifStsBackBatPowerCut = VIF_GetStsBackBatPowerCut();
    
    return TifStsBackBatPowerCut;

}

uint8_t TIF_GetStsBackBatLowPower(void)
{
    uint8_t TifStsBackBatLowPower = 0;

    TifStsBackBatLowPower = VIF_GetStsBackBatLowPower();
    
    return TifStsBackBatLowPower;

}

uint8_t TIF_GetStsRs485BaudChange(void)
{
    uint8_t TifStsRs485BaudChange = 0;

    TifStsRs485BaudChange = VIF_GetStsRs485BaudChange();
    
    return TifStsRs485BaudChange;

}

uint8_t TIF_GetStsCanBaudChange(void)
{
    uint8_t TifStsCanBaudChange = 0;

    TifStsCanBaudChange = VIF_GetStsCanBaudChange();
    
    return TifStsCanBaudChange;

}

uint8_t TIF_GetStsDevHerar(void)
{
    uint8_t TifStsDevHerar = 0;

    TifStsDevHerar = VIF_GetStsDevHerar();
    
    return TifStsDevHerar;

}

uint8_t TIF_GetStsDevSleep(void)
{
    uint8_t TifStsDevSleep = 0;

    TifStsDevSleep = VIF_GetStsDevSleep();
    
    return TifStsDevSleep;

}

uint8_t TIF_GetStsHostLowPower(void)
{
    uint8_t TifStsHostLowPower = 0;

    TifStsHostLowPower = VIF_GetStsHostLowPower();
    
    return TifStsHostLowPower;

}

uint8_t TIF_GetStsCollision(void)
{
    uint8_t TifStsCollision = 0;

    TifStsCollision = VIF_GetStsCollision();
    
    return TifStsCollision;

}

uint8_t TIF_GetStsRemove(void)
{
    uint8_t TifStsRemove = 0;

    TifStsRemove = VIF_GetStsRemove();
    
    return TifStsRemove;

}

uint8_t TIF_GetStsPlcControlled(void)
{
    uint8_t TifStsPlcControlled = 0;

    TifStsPlcControlled = VIF_GetStsPlcControlled();
    
    return TifStsPlcControlled;

}

uint8_t TIF_GetStsPosiVailed(void)
{
    uint8_t TifStsPosiVailed = 0;

    TifStsPosiVailed = VIF_GetStsPosiVailed();
    
    return TifStsPosiVailed;

}

uint8_t TIF_GetStsCanTimerSendState(void)
{
    uint8_t TifStsCanTimerSendState = 0;

    TifStsCanTimerSendState = VIF_GetStsCanTimerSendState();
    
    return TifStsCanTimerSendState;

}

uint8_t TIF_GetStsRs485TimerSendState(void)
{
    uint8_t TifStsRs485TimerSendState = 0;

    TifStsRs485TimerSendState = VIF_GetStsRs485TimerSendState();
    
    return TifStsRs485TimerSendState;

}

uint8_t TIF_GetStsFlashfault(void)
{
    uint8_t TifStsFlashfault = 0;

    TifStsFlashfault = VIF_GetStsFlashfault();
    
    return TifStsFlashfault;

}

uint8_t TIF_GetStsCAN0fault(void)
{
    uint8_t TifStsCAN0fault = 0;

    TifStsCAN0fault = VIF_GetStsCAN0fault();
    
    return TifStsCAN0fault;

}

uint8_t TIF_GetStsCAN1fault(void)
{
    uint8_t TifStsCAN1fault = 0;

    TifStsCAN1fault = VIF_GetStsCAN1fault();
    
    return TifStsCAN1fault;

}

uint8_t TIF_GetStsPositionModuleState(void)
{
    uint8_t TifStsPositionModuleState = 0;

    TifStsPositionModuleState= VIF_GetStsPositionModuleState();
    
    return TifStsPositionModuleState;

}

uint8_t TIF_GetStsCAN1BUSOFF(void)
{
    uint8_t TifStsCAN1BUSOFF = 0;

    TifStsCAN1BUSOFF = VIF_GetStsCAN1BUSOFF();
    
    return TifStsCAN1BUSOFF;

}

uint8_t TIF_GetStsMCUConnect4GError(void)
{
    uint8_t TifStsMCUConnect4GError = 0;

    TifStsMCUConnect4GError = VIF_GetStsMCUConnect4GError();
    
    return TifStsMCUConnect4GError;

}


uint32_t TIF_GetLatDeg(void)
{
    uint32_t Value = 0 ;
    uint32_t TifLatDeg = 0;

    /*YOU CAN ONLY USE VIF_xxxFUNCTION*/
    Value = VIF_GetLatDeg();
    
    TifLatDeg = (Value & 0x000000FF) * 1000000 + 
        		(((float)((Value   >> 8) & 0x000000FF) + 
        		((float)(((Value >> 8) & 0x0000FF00) + 
        		((uint8_t)(Value >> 24) & 0x000000FF)) / 50000)) / 60) * 1000000;
    
    return TifLatDeg;

}

uint32_t TIF_GetLonDeg(void)
{
    uint32_t Value = 0 ;
    uint32_t TifLonDeg= 0;

    /*YOU CAN ONLY USE VIF_xxxFUNCTION*/
    Value = VIF_GetLonDeg();
    
    TifLonDeg= (Value & 0x000000FF) * 1000000 +  
				(((float)((Value   >> 8) & 0x000000FF) + 
				((float)(((Value >> 8) & 0x0000FF00) + 
				((uint8_t)(Value >> 24) & 0x000000FF)) / 50000)) / 60) * 1000000;
    
    return TifLonDeg;

}

uint16_t TIF_GetHight(void)
{
    uint16_t Value = 0 ;
    uint16_t TifHight = 0;

    /*YOU CAN ONLY USE VIF_xxxFUNCTION*/
    Value = VIF_GetHight();
    
    TifHight = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifHight;

}

uint16_t TIF_GetDirection(void)
{
    uint16_t Value = 0 ;
    uint16_t TifDirection = 0;

    /*YOU CAN ONLY USE VIF_xxxFUNCTION*/
    Value = VIF_GetDirection();
    
    TifDirection = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifDirection;

}

uint8_t TIF_GetYear(void)
{
    uint8_t TifYear = 0;

    TifYear = VIF_GetYear() + 100 - 1990;
    return TifYear;

}

uint8_t TIF_GetMon(void)
{
    uint8_t TifMon = 0;

    TifMon = VIF_GetMon() - 1;
    return TifMon;

}

uint8_t TIF_GetDay(void)
{
    uint8_t TifDay = 0;

    TifDay = VIF_GetDay() + 1;
    return TifDay;

}

uint8_t TIF_GetHour(void)
{
    uint8_t TifHour = 0;

    TifHour = VIF_GetHour();
    return TifHour;

}

uint8_t TIF_GetMin(void)
{
    uint8_t TifMin = 0;

    TifMin = VIF_GetMin();
    return TifMin;

}

uint8_t TIF_GetSec(void)
{
    uint8_t TifSec = 0;

    TifSec = VIF_GetSec();
    return TifSec;

}

uint8_t TIF_GetRemainOil(void)
{
    uint8_t TifRemainOil = 0;

    TifRemainOil = VIF_GetRemainOil();
    return TifRemainOil;

}

uint16_t TIF_GetBPSoc(void)
{
    uint16_t Value = 0 ;
    uint16_t TifBPSoc = 0;

    /*YOU CAN ONLY USE VIF_xxxFUNCTION*/
    Value = VIF_GetBPSoc();
    
    TifBPSoc = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifBPSoc;

}

uint16_t TIF_GetCarSpeed(void)
{
    uint16_t Value = 0 ;
    uint16_t TifCarSpeed = 0;

    /*YOU CAN ONLY USE VIF_xxxFUNCTION*/
    Value = VIF_GetCarSpeed();
    
    TifCarSpeed = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifCarSpeed;

}

uint32_t TIF_GetOdo(void)
{
    uint32_t Value = 0 ;
    uint32_t TifCarSpeed = 0;

    /*YOU CAN ONLY USE VIF_xxxFUNCTION*/
    Value = VIF_GetCarSpeed();
    
    TifCarSpeed = ((Value << 24) & 0xFF000000) | ((Value << 8) & 0x00FF0000)| ((Value >> 8) & 0x0000FF00)| ((Value >> 24) & 0x000000FF);
    
    return TifCarSpeed;

}



uint8_t TIF_GetDoorStatusDrivingDoor(void)
{
    uint8_t TifDoorStatusDrivingDoor = 0;

    TifDoorStatusDrivingDoor = VIF_GetDoorStatusDrivingDoor();
    
    return TifDoorStatusDrivingDoor;

}

uint8_t TIF_GetDoorStatusCopilotDoor(void)
{
    uint8_t TifDoorStatusCopilotDoor = 0;

    TifDoorStatusCopilotDoor = VIF_GetDoorStatusCopilotDoor();
    
    return TifDoorStatusCopilotDoor;

}

uint8_t TIF_GetDoorStatusLeftRearDoor(void)
{
    uint8_t TifDoorStatusLeftRearDoor = 0;

    TifDoorStatusLeftRearDoor = VIF_GetDoorStatusLeftRearDoor();
    
    return TifDoorStatusLeftRearDoor;

}

uint8_t TIF_GetDoorStatusRightRearDoor(void)
{
    uint8_t TifDoorStatusRightRearDoor= 0;

    TifDoorStatusRightRearDoor = VIF_GetDoorStatusRightRearDoor();
    
    return TifDoorStatusRightRearDoor;

}

uint8_t TIF_GetDoorStatusRearCanopy(void)
{
    uint8_t TifDoorStatusRearCanopy = 0;

    TifDoorStatusRearCanopy = VIF_GetDoorStatusRearCanopy();
    
    return TifDoorStatusRearCanopy;

}

uint8_t TIF_GetDoorStatusEngineCover(void)
{
    uint8_t TifDoorStatusEngineCover = 0;

    TifDoorStatusEngineCover= VIF_GetDoorStatusEngineCover();
    
    return TifDoorStatusEngineCover;

}

uint8_t TIF_GetLockStatusRightRearLock(void)
{
    uint8_t TifLockStatusRightRearLock = 0;

    TifLockStatusRightRearLock = VIF_GetLockStatusRightRearLock();
    
    return TifLockStatusRightRearLock;

}

uint8_t TIF_GetLockStatusLeftRearLock(void)
{
    uint8_t TifLockStatusLeftRearLock = 0;

    TifLockStatusLeftRearLock = VIF_GetLockStatusLeftRearLock();
    
    return TifLockStatusLeftRearLock;

}

uint8_t TIF_GetLockStatusCopilotLock(void)
{
    uint8_t TifLockStatusCopilotLock = 0;

    TifLockStatusCopilotLock = VIF_GetLockStatusCopilotLock();
    
    return TifLockStatusCopilotLock;

}

uint8_t TIF_GetLockStatusDrivingLock(void)
{
    uint8_t TifLockStatusDrivingLock = 0;

    TifLockStatusDrivingLock = VIF_GetLockStatusDrivingLock();
    
    return TifLockStatusDrivingLock;

}


uint8_t TIF_GetRoofStatus(void)
{
    uint8_t TifRoofStatus = 0;

    TifRoofStatus = VIF_GetRoofStatus();
    
    return TifRoofStatus;

}

uint8_t TIF_GetWindowStatusLeftFrontWindow(void)
{
    uint8_t TifWindowStatusLeftFrontWindow = 0;

    TifWindowStatusLeftFrontWindow = VIF_GetWindowStatusLeftFrontWindow();
    
    return TifWindowStatusLeftFrontWindow;

}

uint8_t TIF_GetWindowStatusRightFrontWindow(void)
{
    uint8_t TifWindowStatusRightFrontWindow = 0;

    TifWindowStatusRightFrontWindow = VIF_GetWindowStatusRightFrontWindow();
    
    return TifWindowStatusRightFrontWindow;

}

uint8_t TIF_GetWindowStatusLeftRearWindow(void)//?
{
    uint8_t TifWindowStatusLeftRearWindow = 0;

    TifWindowStatusLeftRearWindow = VIF_GetWindowStatusLeftRearWindow();
    
    return TifWindowStatusLeftRearWindow;

}

uint8_t TIF_GetWindowStatusRightRearWindow(void)
{
    uint8_t TifWindowStatusRightRearWindow = 0;

    TifWindowStatusRightRearWindow = VIF_GetWindowStatusRightRearWindow();
    
    return TifWindowStatusRightRearWindow;

}


uint8_t TIF_GetLampStautsHeadlights(void)
{
    uint8_t TifLampStautsHeadlights = 0;

    TifLampStautsHeadlights = VIF_GetLampStautsHeadlights();
    
    return TifLampStautsHeadlights;

}

uint8_t TIF_GetLampStautsPositionlights(void)
{
    uint8_t TifLampStautsPositionlights = 0;

    TifLampStautsPositionlights = VIF_GetLampStautsPositionlights();
    
    return TifLampStautsPositionlights;

}

uint8_t TIF_GetLampStautsNearlights(void)
{
    uint8_t TifLampStautsNearlights = 0;

    TifLampStautsNearlights = VIF_GetLampStautsNearlights();
    
    return TifLampStautsNearlights;

}

uint8_t TIF_GetLampStautsRearfoglights(void)
{
    uint8_t TifLampStautsRearfoglights = 0;

    TifLampStautsRearfoglights = VIF_GetLampStautsRearfoglights();
    
    return TifLampStautsRearfoglights;

}

uint8_t TIF_GetLampStautsFrontfoglights(void)
{
    uint8_t TifLampStautsFrontfoglights = 0;

    TifLampStautsFrontfoglights = VIF_GetLampStautsFrontfoglights();
    
    return TifLampStautsFrontfoglights;

}

uint8_t TIF_GetLampStautsLeftlights(void)
{
    uint8_t TifLampStautsLeftlights = 0;

    TifLampStautsLeftlights = VIF_GetLampStautsLeftlights();
    
    return TifLampStautsLeftlights;

}

uint8_t TIF_GetLampStautsRightlights(void)
{
    uint8_t TifLampStautsRightlights = 0;

    TifLampStautsRightlights = VIF_GetLampStautsRightlights();
    
    return TifLampStautsRightlights;

}




uint8_t TIF_GetMcuVersion(uint8_t *pData,uint16_t &len)
{
    uint8_t ret = RET_FAIL;
    if ((TRUE == TIF_InitSts) || (NULL == pData))
    {
        ret = RET_INVALID;
    }
    else
    {
        ret = VIF_GetMcuVersion(pData, len);
    }
    return ret;
}

uint8_t TIF_GetMotorStatus(void)
{
    uint8_t TifMotorStatus = 0;

    TifMotorStatus = VIF_GetMotorStatus() & 0x01;
    return TifMotorStatus;

}

uint16_t TIF_GetMotorSpeed(void)
{
    uint16_t Value = 0 ;
    uint16_t TifMotorSpeed = 0;

    /*YOU CAN ONLY USE TIF_xxxFUNCTION*/
    Value = VIF_GetMotorSpeed();
    
    TifMotorSpeed = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifMotorSpeed;

}

uint8_t TIF_GetEngineStatus(void)
{
    uint8_t TifEngineStatus = 0;

    TifEngineStatus = VIF_GetEngineStatus() & 0x07;
    return TifEngineStatus;

}

uint16_t TIF_GetEngineSpeed(void)
{
    uint16_t Value = 0 ;
    uint16_t TifEngineSpeed = 0;

    /*YOU CAN ONLY USE TIF_xxxFUNCTION*/
    Value = VIF_GetEngineSpeed();
    
    TifEngineSpeed = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifEngineSpeed;


}

uint8_t TIF_GetGearInfor(void)
{
    uint8_t TifGearInfor = 0;

    TifGearInfor = VIF_GetGearInfor() & 0x0F;
    return TifGearInfor;

}

uint8_t TIF_GetIgnInfo(void)
{
    uint8_t TifIgnInfo= 0;

    TifIgnInfo = VIF_GetIgnInfo() & 0x0F;
    return TifIgnInfo;

}

uint8_t TIF_GetHandParkStatus(void)
{
    uint8_t TifHandParkStatus= 0;

    TifHandParkStatus = VIF_GetHandParkStatus() & 0x03;
    return TifHandParkStatus;

}

uint8_t TIF_GetBeltStatus(void)
{
    uint8_t TifBeltStatus= 0;

    TifBeltStatus = VIF_GetBeltStatus() & 0x03;
    return TifBeltStatus;

}

uint8_t TIF_GetChargeStatus(void)
{
    uint8_t TifChargeStatus= 0;

    TifChargeStatus = VIF_GetChargeStatus() & 0x03;
    return TifChargeStatus;

}

uint8_t TIF_GetChargeGunStatus(void)
{
    uint8_t TifChargeGunStatus= 0;

    TifChargeGunStatus = VIF_GetChargeGunStatus() & 0x01;
    return TifChargeGunStatus;

}

uint8_t TIF_GetBreakStatus(void)
{
    uint8_t TifBreakStatus = 0;

    TifBreakStatus = VIF_GetBreakStatus() & 0x03;
    return TifBreakStatus;

}

uint16_t TIF_GetRemainEndurance(void)
{
    uint16_t Value = 0 ;
    uint16_t TifRemainEndurance = 0;

    /*YOU CAN ONLY USE TIF_xxxFUNCTION*/
    Value = VIF_GetRemainEndurance();
    
    TifRemainEndurance = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifRemainEndurance;

}

uint16_t TIF_GetBPVol(void)
{
    uint16_t Value = 0 ;
    uint16_t TifBPVol = 0;

    /*YOU CAN ONLY USE TIF_xxxFUNCTION*/
    Value = VIF_GetBPVol();
    
    TifBPVol = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifBPVol;

}

uint16_t TIF_GetBPSur(void)
{
    uint16_t Value = 0 ;
    uint16_t TifBPSur = 0;

    /*YOU CAN ONLY USE TIF_xxxFUNCTION*/
    Value = VIF_GetBPSur();
    
    TifBPSur = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifBPSur;
}

uint16_t TIF_GetMaintVol(void)
{
    uint16_t Value = 0 ;
    uint16_t TifMaintVol = 0;

    /*YOU CAN ONLY USE TIF_xxxFUNCTION*/
    Value = VIF_GetMaintVol();
    
    TifMaintVol = ((Value << 8) & 0xFF00) | ((Value >> 8) & 0x00FF);
    
    return TifMaintVol;

}

uint8_t TIF_GetPowerForceSts(void)
{
    uint8_t TifPowerForceSts = 0;

    TifPowerForceSts = VIF_GetPowerForceSts() & 0x01;
    return TifPowerForceSts;

}

uint8_t TIF_GetCarIDState(void)
{
    uint8_t TifCarIDState = 0;

    TifCarIDState = VIF_GetCarIDState();
    return TifCarIDState;

}

uint8_t TIF_GetCarGwNodeMiss(void)
{
    uint8_t TifCarGwNodeMiss = 0;

    if(VIF_GetCarGwNodeMiss() == 2)
    {
        TifCarGwNodeMiss = 1;
    }
    else
    {
        TifCarGwNodeMiss = 0;
    }
    return TifCarGwNodeMiss;

}


/*
uint8_t TIF_GetEngineSystem(void)
{
    uint8_t TifEngineSystem = 0;

    TifEngineSystem = VIF_GetEngineSystem();
    return TifEngineSystem;

}
uint8_t TIF_GetTCUSystem(void)
{
    uint8_t TifTCUSystem = 0;

    TifTCUSystem = VIF_GetTCUSystem();
    return TifTCUSystem;

}
uint8_t TIF_GetEmissionSystem(void)
{
    uint8_t TifEmissionSystem = 0;

    TifEmissionSystem = VIF_GetEmissionSystem();
    return TifEmissionSystem;

}
uint8_t TIF_GetACUSystem(void)
{
    uint8_t TifACUSystem = 0;

    TifACUSystem = VIF_GetACUSystem();
    return TifACUSystem;

}
uint8_t TIF_GetESPSystem(void)
{
    uint8_t TifESPSystem = 0;

    TifESPSystem = VIF_GetESPSystem();
    return TifESPSystem;

}
uint8_t TIF_GetABSSystem(void)
{
    uint8_t TifABSSystem = 0;

    TifABSSystem = VIF_GetABSSystem();
    return TifABSSystem;

}
uint8_t TIF_GetEPSSystem(void)
{
    uint8_t TifEPSSystem = 0;

    TifEPSSystem = VIF_GetEPSSystem();
    return TifEPSSystem;

}
uint8_t TIF_GetOilPressSystem(void)
{
    uint8_t TifOilPressSystem = 0;

    TifOilPressSystem = VIF_GetOilPressSystem();
    return TifOilPressSystem;

}
uint8_t TIF_GetOilLowSystem(void)
{
    uint8_t TifOilLowSystem = 0;

    TifOilLowSystem = VIF_GetOilLowSystem();
    return TifOilLowSystem;

}
uint8_t TIF_GetBreakFluidSystem(void)
{
    uint8_t TifBreakFluidSystem = 0;

    TifBreakFluidSystem = VIF_GetBreakFluidSystem();
    return TifBreakFluidSystem;

}
uint8_t TIF_GetChargeSystem(void)
{
    uint8_t TifChargeSystem = 0;

    TifChargeSystem = VIF_GetChargeSystem();
    return TifChargeSystem;

}
uint8_t TIF_GetBreakSystem(void)
{
    uint8_t TifBreakSystem = 0;

    TifBreakSystem = VIF_GetBreakSystem();
    return TifBreakSystem;

}
uint8_t TIF_GetTyrePressSystem(void)
{
    uint8_t TifTyrePressSystem = 0;

    TifTyrePressSystem = VIF_GetTyrePressSystem();
    return TifTyrePressSystem;

}
uint8_t TIF_GetStartStopSystem(void)
{
    uint8_t TifStartStopSystem = 0;

    TifStartStopSystem = VIF_GetStartStopSystem();
    return TifStartStopSystem;

}
uint8_t TIF_GetLampSystem(void)
{
    uint8_t TifLampSystem = 0;

    TifLampSystem = VIF_GetLampSystem();
    return TifLampSystem;

}
uint8_t TIF_GetELockSystem(void)
{
    uint8_t TifELockSystem = 0;

    TifELockSystem = VIF_GetELockSystem();
    return TifELockSystem;

}
uint8_t TIF_GetEngineTempSytem(void)
{
    uint8_t TifEngineTempSytem = 0;

    TifEngineTempSytem = VIF_GetEngineTempSytem();
    return TifEngineTempSytem;

}
uint8_t TIF_GetEPBSystem(void)
{
    uint8_t TifEPBSystem = 0;

    TifEPBSystem = VIF_GetEPBSystem();
    return TifEPBSystem;

}
uint8_t TIF_GetUpperBeamSystem(void)
{
    uint8_t TifUpperBeamSystem = 0;

    TifUpperBeamSystem = VIF_GetUpperBeamSystem();
    return TifUpperBeamSystem;

}
uint8_t TIF_GetNavigateSystem(void)
{
    uint8_t TifNavigateSystem = 0;

    TifNavigateSystem = VIF_GetNavigateSystem();
    return TifNavigateSystem;

}
uint8_t TIF_GetCrashSystem(void)
{
    uint8_t TifCrashSystem = 0;

    TifCrashSystem = VIF_GetCrashSystem();
    return TifCrashSystem;

}
uint8_t TIF_GetRoalSystem(void)
{
    uint8_t TifRoalSystem = 0;

    TifRoalSystem = VIF_GetRoalSystem();
    return TifRoalSystem;

}
uint8_t TIF_GetBlindMonitorSystem(void)
{
    uint8_t TifBlindMonitorSystem = 0;

    TifBlindMonitorSystem = VIF_GetBlindMonitorSystem();
    return TifBlindMonitorSystem;

}
uint8_t TIF_GetOperateAC(void)
{
    uint8_t TifOperateAC = 0;

    TifOperateAC = VIF_GetOperateAC();
    return TifOperateAC;

}
uint8_t TIF_GetHighVolSystem(void)
{
    uint8_t TifHighVolSystem = 0;

    TifHighVolSystem = VIF_GetHighVolSystem();
    return TifHighVolSystem;

}
uint8_t TIF_GetInsulationSystem(void)
{
    uint8_t TifInsulationSystem = 0;

    TifInsulationSystem = VIF_GetInsulationSystem();
    return TifInsulationSystem;

}
uint8_t TIF_GetInterlockSystem(void)
{
    uint8_t TifInterlockSystem = 0;

    TifInterlockSystem = VIF_GetInterlockSystem();
    return TifInterlockSystem;

}
uint8_t TIF_GetCellSystem(void)
{
    uint8_t TifCellSystem = 0;

    TifCellSystem = VIF_GetCellSystem();
    return TifCellSystem;

}
uint8_t TIF_GetMotorSystem(void)
{
    uint8_t TifMotorSystem = 0;

    TifMotorSystem = VIF_GetMotorSystem();
    return TifMotorSystem;

}
uint8_t TIF_GetEParkSystem(void)
{
    uint8_t TifEParkSystem = 0;

    TifEParkSystem = VIF_GetEParkSystem();
    return TifEParkSystem;

}
uint8_t TIF_GetEQuantityForCellSystem(void)
{
    uint8_t TifEQuantityForCellSystem = 0;

    TifEQuantityForCellSystem = VIF_GetEQuantityForCellSystem();
    return TifEQuantityForCellSystem;

}
uint8_t TIF_GetTempeForCellSystem(void)
{
    uint8_t TifTempeForCellSystem = 0;

    TifTempeForCellSystem = VIF_GetTempeForCellSystem();
    return TifTempeForCellSystem;

}
uint8_t TIF_GetTempeForMotorSystem(void)
{
    uint8_t TifTempeForMotorSystem = 0;

    TifTempeForMotorSystem = VIF_GetTempeForMotorSystem();
    return TifTempeForMotorSystem;

}
uint8_t TIF_GetNaviCnstSpdSystem(void)
{
    uint8_t TifNaviCnstSpdSystem = 0;

    TifNaviCnstSpdSystem = VIF_GetNaviCnstSpdSystem();
    return TifNaviCnstSpdSystem;

}
uint8_t TIF_GetBattChargerSystem(void)
{
    uint8_t TifBattChargerSystem = 0;

    TifBattChargerSystem = VIF_GetBattChargerSystem();
    return TifBattChargerSystem;

}
uint8_t TIF_GetACSystem(void)
{
    uint8_t TifACSystem = 0;

    TifACSystem = VIF_GetACSystem();
    return TifACSystem;

}
uint8_t TIF_GetAuxSystem(void)
{
    uint8_t TifAuxSystem = 0;

    TifAuxSystem = VIF_GetAuxSystem();
    return TifAuxSystem;

}
uint8_t TIF_GetEmergyBreakSystem(void)
{
    uint8_t TifEmergyBreakSystem = 0;

    TifEmergyBreakSystem = VIF_GetEmergyBreakSystem();
    return TifEmergyBreakSystem;

}
uint8_t TIF_GetRadarSystem(void)
{
    uint8_t TifRadarSystem = 0;

    TifRadarSystem = VIF_GetRadarSystem();
    return TifRadarSystem;

}
uint8_t TIF_GetElcSystem(void)
{
    uint8_t TifElcSystem = 0;

    TifElcSystem = VIF_GetElcSystem();
    return TifElcSystem;

}
uint8_t TIF_GetLeftFrontTyrePress(void)
{
    uint8_t TifLeftFrontTyrePress = 0;

    TifLeftFrontTyrePress = VIF_GetLeftFrontTyrePress();
    return TifLeftFrontTyrePress;

}
uint8_t TIF_GetLeftFrontTyreTemp(void)
{
    uint8_t TifLeftFrontTyreTemp = 0;

    TifLeftFrontTyreTemp = VIF_GetLeftFrontTyreTemp();
    return TifLeftFrontTyreTemp;

}
uint8_t TIF_GetRightFrontTyrePress(void)
{
    uint8_t TifRightFrontTyrePress = 0;

    TifRightFrontTyrePress = VIF_GetRightFrontTyrePress();
    return TifRightFrontTyrePress;

}
uint8_t TIF_GetRightFrontTyreTemp(void)
{
    uint8_t TifRightFrontTyreTemp = 0;

    TifRightFrontTyreTemp = VIF_GetRightFrontTyreTemp();
    return TifRightFrontTyreTemp;

}
uint8_t TIF_GetLeftRearTyrePress(void)
{
    uint8_t TifLeftRearTyrePress = 0;

    TifLeftRearTyrePress = VIF_GetLeftRearTyrePress();
    return TifLeftRearTyrePress;

}
uint8_t TIF_GetLeftRearTyreTemp(void)
{
    uint8_t TifLeftRearTyreTemp = 0;

    TifLeftRearTyreTemp = VIF_GetLeftRearTyreTemp();
    return TifLeftRearTyreTemp;

}
uint8_t TIF_GetRightRearTyrePress(void)
{
    uint8_t TifRightRearTyrePress = 0;

    TifRightRearTyrePress = VIF_GetRightRearTyrePress();
    return TifRightRearTyrePress;

}
uint8_t TIF_GetRightRearTyreTemp(void)
{
    uint8_t TifRightRearTyreTemp = 0;

    TifRightRearTyreTemp = VIF_GetRightRearTyreTemp();
    return TifRightRearTyreTemp;

}
uint8_t TIF_GetDCDCSystem(void)
{
    uint8_t TifDCDCSystem = 0;

    TifDCDCSystem = VIF_GetDCDCSystem();
    return TifDCDCSystem;

}
uint8_t TIF_GetBCMSystem(void)
{
    uint8_t TifBCMSystem = 0;

    TifBCMSystem = VIF_GetBCMSystem();
    return TifBCMSystem;

}
uint8_t TIF_GetRelaySystem(void)
{
    uint8_t TifRelaySystem = 0;

    TifRelaySystem = VIF_GetRelaySystem();
    return TifRelaySystem;

}
uint8_t TIF_GetTSROutSpdSystem(void)
{
    uint8_t TifTSROutSpdSystem = 0;

    TifTSROutSpdSystem = VIF_GetTSROutSpdSystem();
    return TifTSROutSpdSystem;

}
uint8_t TIF_GetTSRLimSpdSystem(void)
{
    uint8_t TifTSRLimSpdSystem = 0;

    TifTSRLimSpdSystem = VIF_GetTSRLimSpdSystem();
    return TifTSRLimSpdSystem;

}
uint8_t TIF_GetAEBinvolvedSystem(void)
{
    uint8_t TifAEBinvolvedSystem = 0;

    TifAEBinvolvedSystem = VIF_GetAEBinvolvedSystem();
    return TifAEBinvolvedSystem;

}
uint8_t TIF_GetABSinvolvedSystem(void)
{
    uint8_t TifABSinvolvedSystem = 0;

    TifABSinvolvedSystem = VIF_GetABSinvolvedSystem();
    return TifABSinvolvedSystem;

}
uint8_t TIF_GetASRinvolvedSystem(void)
{
    uint8_t TifASRinvolvedSystem = 0;

    TifASRinvolvedSystem = VIF_GetASRinvolvedSystem();
    return TifASRinvolvedSystem;

}
uint8_t TIF_GetESPinvolvedSystem(void)
{
    uint8_t TifESPinvolvedSystem = 0;

    TifESPinvolvedSystem = VIF_GetESPinvolvedSystem();
    return TifESPinvolvedSystem;

}
uint8_t TIF_GetDSMSystem(void)
{
    uint8_t TifDSMSystem = 0;

    TifDSMSystem = VIF_GetDSMSystem();
    return TifDSMSystem;

}
uint8_t TIF_GetLeaveWheelPromp(void)
{
    uint8_t TifLeaveWheelPromp = 0;

    TifLeaveWheelPromp = VIF_GetLeaveWheelPromp();
    return TifLeaveWheelPromp;

}
uint8_t TIF_GetACCStatus(void)
{
    uint8_t TifACCStatus = 0;

    TifACCStatus = VIF_GetACCStatus();
    return TifACCStatus;

}
uint8_t TIF_GetAccSpdSystem(void)
{
    uint8_t TifAccSpdSystem = 0;

    TifAccSpdSystem = VIF_GetAccSpdSystem();
    return TifAccSpdSystem;

}
uint8_t TIF_GetFCWSystem(void)
{
    uint8_t TifFCWSystem = 0;

    TifFCWSystem = VIF_GetFCWSystem();
    return TifFCWSystem;

}
uint8_t TIF_GetFCWStatus(void)
{
    uint8_t TifFCWStatus = 0;

    TifFCWStatus = VIF_GetFCWStatus();
    return TifFCWStatus;

}
uint8_t TIF_GetTimerForFCWSystem(void)
{   
	uint8_t TifTimerForFCWSystem = 0;

    TifTimerForFCWSystem = VIF_GetTimerForFCWSystem();
    return TifTimerForFCWSystem;
}
uint8_t TIF_GetBreakForFCWSystem(void)
{
    uint8_t TifBreakForFCWSystem = 0;

    TifBreakForFCWSystem = VIF_GetBreakForFCWSystem();
    return TifBreakForFCWSystem;

}
uint8_t TIF_GetLDWStatus(void)
{
    uint8_t TifLDWStatus = 0;

    TifLDWStatus = VIF_GetLDWStatus();
    return TifLDWStatus;
}
uint8_t TIF_GetLDWSystem(void)
{
    uint8_t TifLDWSystem = 0;

    TifLDWSystem = VIF_GetLDWSystem();
    return TifLDWSystem;
}
uint8_t TIF_GetWheelForLDWSystem(void)
{
    uint8_t TifWheelForLDWSystem = 0;

    TifWheelForLDWSystem = VIF_GetWheelForLDWSystem();
    return TifWheelForLDWSystem;
}
uint8_t TIF_GetLKAStatus(void)
{
    uint8_t TifLKAStatus = 0;

    TifLKAStatus = VIF_GetLKAStatus();
    return TifLKAStatus;
}
uint8_t TIF_GetLKAinvolvedSystem(void)
{
    uint8_t TifLKAinvolvedSystem = 0;

    TifLKAinvolvedSystem = VIF_GetLKAinvolvedSystem();
    return TifLKAinvolvedSystem;
}
uint8_t TIF_GetLKADriverPromp(void)
{
    uint8_t TifLKADriverPromp = 0;

    TifLKADriverPromp = VIF_GetLKADriverPromp();
    return TifLKADriverPromp;
}
uint8_t TIF_GetLKADriverForWheel(void)
{
    uint8_t TifLKADriverForWheel = 0;

    TifLKADriverForWheel = VIF_GetLKADriverForWheel();
    return TifLKADriverForWheel;
}
uint8_t TIF_GetBSDStatus(void)
{
    uint8_t TifBSDStatus = 0;

    TifBSDStatus = VIF_GetBSDStatus();
    return TifBSDStatus;
}
uint8_t TIF_GetLeftBSDSystem(void)
{
    uint8_t TifLeftBSDSystem = 0;

    TifLeftBSDSystem = VIF_GetLeftBSDSystem();
    return TifLeftBSDSystem;
}
uint8_t TIF_GetRightBSDSystem(void)
{
    uint8_t TifRightBSDSystem = 0;

    TifRightBSDSystem = VIF_GetRightBSDSystem();
    return TifRightBSDSystem;
}
uint8_t TIF_GetTimeForWheelBSD(void)
{
    uint8_t TifTimeForWheelBSD = 0;

    TifTimeForWheelBSD = VIF_GetTimeForWheelBSD();
    return TifTimeForWheelBSD;
}
uint8_t TIF_GetTimeForBreakBSD(void)
{
    uint8_t TifTimeForBreakBSD = 0;

    TifTimeForBreakBSD = VIF_GetTimeForBreakBSD();
    return TifTimeForBreakBSD;
}
uint8_t TIF_GetAccelPedalForBSD(void)
{
    uint8_t TifAccelPedalForBSD = 0;

    TifAccelPedalForBSD = VIF_GetAccelPedalForBSD();
    return TifAccelPedalForBSD;
}
uint8_t TIF_GetLeftForFlowSystem(void)
{
    uint8_t TifLeftForFlowSystem = 0;

    TifLeftForFlowSystem = VIF_GetLeftForFlowSystem();
    return TifLeftForFlowSystem;
}
uint8_t TIF_GetRightForFlowSystem(void)
{
    uint8_t TifRightForFlowSystem = 0;

    TifRightForFlowSystem = VIF_GetRightForFlowSystem();
    return TifRightForFlowSystem;
}
uint8_t TIF_GetRearForFlowSystem(void)
{
    uint8_t TifRearForFlowSystem = 0;

    TifRearForFlowSystem = VIF_GetRearForFlowSystem();
    return TifRearForFlowSystem;
}
uint8_t TIF_GetBreakForFlowSystem(void)
{
    uint8_t TifBreakForFlowSystem = 0;

    TifBreakForFlowSystem = VIF_GetBreakForFlowSystem();
    return TifBreakForFlowSystem;
}
uint8_t TIF_GetAccelPedalForFlow(void)
{
    uint8_t TifAccelPedalForFlow = 0;

    TifAccelPedalForFlow = VIF_GetAccelPedalForFlow();
    return TifAccelPedalForFlow;
}
uint8_t TIF_GetLeftAUXSystem(void)
{
    uint8_t TifLeftAUXSystem = 0;

    TifLeftAUXSystem = VIF_GetLeftAUXSystem();
    return TifLeftAUXSystem;
}
uint8_t TIF_GetRightAUXSystem(void)
{
    uint8_t TifRightAUXSystem = 0;

    TifRightAUXSystem = VIF_GetRightAUXSystem();
    return TifRightAUXSystem;
}
uint8_t TIF_GetRearAUXSystem(void)
{
    uint8_t TifRearAUXSystem = 0;

    TifRearAUXSystem = VIF_GetRearAUXSystem();
    return TifRearAUXSystem;
}
uint8_t TIF_GetBreakAUXSystem(void)
{
    uint8_t TifBreakAUXSystem = 0;

    TifBreakAUXSystem = VIF_GetBreakAUXSystem();
    return TifBreakAUXSystem;
}
uint8_t TIF_GetAccelPedalAUXSystem(void)
{
    uint8_t TifAccelPedalAUXSystem = 0;

    TifAccelPedalAUXSystem = VIF_GetAccelPedalAUXSystem();
    return TifAccelPedalAUXSystem;
}
uint8_t TIF_GetDoorcrash(void)
{
    uint8_t TifDoorcrash = 0;

    TifDoorcrash = VIF_GetDoorcrash();
    return TifDoorcrash;
}
uint8_t TIF_GetEnginestop(void)
{
    uint8_t TifEnginestop = 0;

    TifEnginestop = VIF_GetEnginestop();
    return TifEnginestop;
}
uint8_t TIF_GetCarcrash(void)
{
    uint8_t TifCarcrash = 0;

    TifCarcrash = VIF_GetCarcrash();
    return TifCarcrash;
}
uint8_t TIF_GetTrailCarEvent(void)
{
    uint8_t TifTrailCarEvent = 0;

    TifTrailCarEvent = VIF_GetTrailCarEvent();
    return TifTrailCarEvent;
}
uint8_t TIF_GetSkyNotCloseAlm(void)
{
    uint8_t TifSkyNotCloseAlm = 0;

    TifSkyNotCloseAlm = VIF_GetSkyNotCloseAlm();
    return TifSkyNotCloseAlm;
}
*/

uint8_t TIF_GetResponseID(void)
{
    uint8_t TifResponseID = 0;

    TifResponseID = VIF_GetResponseID();
    return TifResponseID;

}
uint8_t TIF_GetResponseNumber(void)
{
    uint8_t TifResponseNumber = 0;

    TifResponseNumber = VIF_GetResponseNumber();
    return TifResponseNumber;

}
uint8_t TIF_GetResponseResult(void)
{
    uint8_t TifResponseResult = 0;

    TifResponseResult = VIF_GetResponseResult();
    return TifResponseResult;

}


uint8_t TIF_GetCtrlCode(void)
{
    uint8_t TifCtrlCode = 0;
    switch( VIF_GetCtrlCode())
    {
        case 0x01:
            TifCtrlCode = 1;
            break;
        case 0x02:
        break;
        case 0x03:
            break;
        case 0x04:
            TifCtrlCode = 2;
            break;
        case 0x05:
            break;
        case 0x06:
            break;
        case 0x07:
            TifCtrlCode = 3;
            break;
        case 0x09:
            break;
        case 0x0a:
            TifCtrlCode = 5;
            break;
        case 0x0b:
            TifCtrlCode = 6;
            break;
        case 0x0F:
            TifCtrlCode = 4;
            break;
        case 0x10:
            break;
        case 0x11:
            break;
        case 0x12:
            break;
        case 0x13:
            break;
        case 0x14:
            break;
        case 0x15:
            break;
        case 0x16:
            break;
        case 0x17:
            break;
        case 0x18:
            break;

    }
    return TifCtrlCode;

}
uint8_t TIF_GetCtrlResult(void)
{
    uint8_t TifCtrlResult = 0;

    TifCtrlResult = VIF_GetCtrlResult();
    return TifCtrlResult;
}



