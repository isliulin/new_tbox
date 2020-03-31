#ifndef TIF_H
#define TIF_H


#define TIF_SPEED_SCALE   (2)
#define TIF_SPEED_OFFSET  (-1)

//add chen
#include "common.h"



uint16_t TIF_GetBodyLen(void);

uint8_t TIF_GetCmdID(void);

uint16_t TIF_GetSerialNumber(void);
uint8_t TIF_GetAttr(uint8_t *pData,uint16_t &len);


uint8_t TIF_GetStsLat(void);

uint8_t TIF_GetStsLon(void);

uint8_t TIF_GetStsGpsAntenna(void);

uint8_t TIF_GetStsAutoLock(void);
uint8_t TIF_GetStsAccPlcPowerType(void);

uint8_t TIF_GetStsOpenCover(void);

uint8_t TIF_GetStsBusFault(void);

uint8_t TIF_GetStsHostPowerCut(void);

uint8_t TIF_GetStsBackBatPowerCut(void);

uint8_t TIF_GetStsBackBatLowPower(void);

uint8_t TIF_GetStsRs485BaudChange(void);

uint8_t TIF_GetStsCanBaudChange(void);

uint8_t TIF_GetStsDevHerar(void);
uint8_t TIF_GetStsDevSleep(void);

uint8_t TIF_GetStsHostLowPower(void);

uint8_t TIF_GetStsCollision(void);

uint8_t TIF_GetStsRemove(void);

uint8_t TIF_GetStsPlcControlled(void);

uint8_t TIF_GetStsPosiVailed(void);

uint8_t TIF_GetStsCanTimerSendState(void);

uint8_t TIF_GetStsRs485TimerSendState(void);

uint8_t TIF_GetStsFlashfault(void);

uint8_t TIF_GetStsCAN0fault(void);
uint8_t TIF_GetStsCAN1fault(void);

uint8_t TIF_GetStsPositionModuleState(void);

uint8_t TIF_GetStsPositionModuleState(void);

uint8_t TIF_GetStsCAN1BUSOFF(void);

uint8_t TIF_GetStsMCUConnect4GError(void);

uint32_t TIF_GetLatDeg(void);

uint32_t TIF_GetLonDeg(void);

uint16_t TIF_GetHight(void);
uint16_t TIF_GetDirection(void);
uint8_t TIF_GetYear(void);
uint8_t TIF_GetMon(void);

uint8_t TIF_GetDay(void);
uint8_t TIF_GetHour(void);

uint8_t TIF_GetMin(void);

uint8_t TIF_GetSec(void);

uint8_t TIF_GetRemainOil(void);

uint16_t TIF_GetBPSoc(void);

uint16_t TIF_GetCarSpeed(void);

uint32_t TIF_GetOdo(void);


uint8_t TIF_GetDoorStatusDrivingDoor(void);

uint8_t TIF_GetDoorStatusCopilotDoor(void);

uint8_t TIF_GetDoorStatusLeftRearDoor(void);

uint8_t TIF_GetDoorStatusRightRearDoor(void);
uint8_t TIF_GetDoorStatusRearCanopy(void);

uint8_t TIF_GetDoorStatusEngineCover(void);
uint8_t TIF_GetLockStatusRightRearLock(void);

uint8_t TIF_GetLockStatusLeftRearLock(void);

uint8_t TIF_GetLockStatusCopilotLock(void);

uint8_t TIF_GetLockStatusDrivingLock(void);

uint8_t TIF_GetRoofStatus(void);

uint8_t TIF_GetWindowStatusLeftFrontWindow(void);

uint8_t TIF_GetWindowStatusRightFrontWindow(void);

uint8_t TIF_GetWindowStatusLeftRearWindow(void);

uint8_t TIF_GetWindowStatusRightRearWindow(void);


uint8_t TIF_GetLampStautsHeadlights(void);

uint8_t TIF_GetLampStautsPositionlights(void);

uint8_t TIF_GetLampStautsNearlights(void);

uint8_t TIF_GetLampStautsRearfoglights(void);

uint8_t TIF_GetLampStautsFrontfoglights(void);

uint8_t TIF_GetLampStautsLeftlights(void);

uint8_t TIF_GetLampStautsRightlights(void);



uint8_t TIF_GetMcuVersion(uint8_t *pData,uint16_t &len);

uint8_t TIF_GetMotorStatus(void);

uint16_t TIF_GetMotorSpeed(void);

uint8_t TIF_GetEngineStatus(void);
uint16_t TIF_GetEngineSpeed(void);
uint8_t TIF_GetGearInfor(void);
uint8_t TIF_GetIgnInfo(void);

uint8_t TIF_GetHandParkStatus(void);

uint8_t TIF_GetBeltStatus(void);

uint8_t TIF_GetChargeStatus(void);
uint8_t TIF_GetChargeGunStatus(void);
uint8_t TIF_GetBreakStatus(void);
uint16_t TIF_GetRemainEndurance(void);

uint16_t TIF_GetBPVol(void);

uint16_t TIF_GetBPSur(void);
uint16_t TIF_GetMaintVol(void);

uint8_t TIF_GetPowerForceSts(void);

uint8_t TIF_GetCarIDState(void);
uint8_t TIF_GetCarGwNodeMiss(void);

uint8_t TIF_GetEngineSystem(void);
uint8_t TIF_GetTCUSystem(void);
uint8_t TIF_GetEmissionSystem(void);
uint8_t TIF_GetACUSystem(void);
uint8_t TIF_GetESPSystem(void);
uint8_t TIF_GetABSSystem(void);
uint8_t TIF_GetEPSSystem(void);
uint8_t TIF_GetOilPressSystem(void);
uint8_t TIF_GetOilLowSystem(void);
uint8_t TIF_GetBreakFluidSystem(void);
uint8_t TIF_GetChargeSystem(void);
uint8_t TIF_GetBreakSystem(void);
uint8_t TIF_GetTyrePressSystem(void);
uint8_t TIF_GetStartStopSystem(void);
uint8_t TIF_GetLampSystem(void);
uint8_t TIF_GetELockSystem(void);
uint8_t TIF_GetEngineTempSytem(void);
uint8_t TIF_GetEPBSystem(void);
uint8_t TIF_GetUpperBeamSystem(void);
uint8_t TIF_GetNavigateSystem(void);
uint8_t TIF_GetCrashSystem(void);
uint8_t TIF_GetRoalSystem(void);
uint8_t TIF_GetBlindMonitorSystem(void);
uint8_t TIF_GetOperateAC(void);
uint8_t TIF_GetHighVolSystem(void);
uint8_t TIF_GetInsulationSystem(void);
uint8_t TIF_GetInterlockSystem(void);
uint8_t TIF_GetCellSystem(void);
uint8_t TIF_GetMotorSystem(void);
uint8_t TIF_GetEParkSystem(void);
uint8_t TIF_GetEQuantityForCellSystem(void);
uint8_t TIF_GetTempeForCellSystem(void);
uint8_t TIF_GetTempeForMotorSystem(void);
uint8_t TIF_GetNaviCnstSpdSystem(void);
uint8_t TIF_GetBattChargerSystem(void);
uint8_t TIF_GetACSystem(void);
uint8_t TIF_GetAuxSystem(void);
uint8_t TIF_GetEmergyBreakSystem(void);
uint8_t TIF_GetRadarSystem(void);
uint8_t TIF_GetElcSystem(void);
uint8_t TIF_GetLeftFrontTyrePress(void);
uint8_t TIF_GetLeftFrontTyreTemp(void);
uint8_t TIF_GetRightFrontTyrePress(void);
uint8_t TIF_GetRightFrontTyreTemp(void);
uint8_t TIF_GetLeftRearTyrePress(void);
uint8_t TIF_GetLeftRearTyreTemp(void);
uint8_t TIF_GetRightRearTyrePress(void);
uint8_t TIF_GetRightRearTyreTemp(void);
uint8_t TIF_GetDCDCSystem(void);
uint8_t TIF_GetBCMSystem(void);
uint8_t TIF_GetRelaySystem(void);
uint8_t TIF_GetTSROutSpdSystem(void);
uint8_t TIF_GetTSRLimSpdSystem(void);
uint8_t TIF_GetAEBinvolvedSystem(void);
uint8_t TIF_GetABSinvolvedSystem(void);
uint8_t TIF_GetASRinvolvedSystem(void);
uint8_t TIF_GetESPinvolvedSystem(void);
uint8_t TIF_GetDSMSystem(void);
uint8_t TIF_GetLeaveWheelPromp(void);
uint8_t TIF_GetACCStatus(void);
uint8_t TIF_GetAccSpdSystem(void);
uint8_t TIF_GetFCWSystem(void);
uint8_t TIF_GetFCWStatus(void);
uint8_t TIF_GetTimerForFCWSystem(void);
uint8_t TIF_GetBreakForFCWSystem(void);
uint8_t TIF_GetLDWStatus(void);
uint8_t TIF_GetLDWSystem(void);
uint8_t TIF_GetWheelForLDWSystem(void);
uint8_t TIF_GetLKAStatus(void);
uint8_t TIF_GetLKAinvolvedSystem(void);
uint8_t TIF_GetLKADriverPromp(void);
uint8_t TIF_GetLKADriverForWheel(void);
uint8_t TIF_GetBSDStatus(void);
uint8_t TIF_GetLeftBSDSystem(void);
uint8_t TIF_GetRightBSDSystem(void);
uint8_t TIF_GetTimeForWheelBSD(void);
uint8_t TIF_GetTimeForBreakBSD(void);
uint8_t TIF_GetAccelPedalForBSD(void);
uint8_t TIF_GetLeftForFlowSystem(void);
uint8_t TIF_GetRightForFlowSystem(void);
uint8_t TIF_GetRearForFlowSystem(void);
uint8_t TIF_GetBreakForFlowSystem(void);
uint8_t TIF_GetAccelPedalForFlow(void);
uint8_t TIF_GetLeftAUXSystem(void);
uint8_t TIF_GetRightAUXSystem(void);
uint8_t TIF_GetRearAUXSystem(void);
uint8_t TIF_GetBreakAUXSystem(void);
uint8_t TIF_GetAccelPedalAUXSystem(void);
uint8_t TIF_GetDoorcrash(void);
uint8_t TIF_GetEnginestop(void);
uint8_t TIF_GetCarcrash(void);
uint8_t TIF_GetTrailCarEvent(void);
uint8_t TIF_GetSkyNotCloseAlm(void);
uint8_t TIF_GetCtrlCode(void);


#endif
