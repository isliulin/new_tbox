#include <stdint.h>
#include "TCTL.h"
#include "VCTL.h"

VCTL_RemoteCtrl_ST			g_stVctl[21];
VCTL_VeichleConfig_ST		g_stVcfg[46];


uint8_t VCTL_Dispatch(uint8_t *CtlTotal, uint8_t *CfgTotal)
{
	uint8_t total = 0;
	uint8_t i = 0;
	uint8_t tempCtl = 0;
	uint8_t tempCfg = 0;
	TCTL_CtrlItem_ST item;

	total = TCTL_GetCtrlTotal();
	for(i = 0; i < total; i ++)
	{
	 	item = TCTL_GetCtrlItem(i);
		switch(item.SubitemCode)
		{
			case TCTL_CarLock:
				g_stVctl[tempCtl].CtrlCode = VehicleBody_LockID; 
				g_stVctl[tempCtl].CtrlParameter = item.SubitemParam[0];
				printf("jason add VCTL_Dispatch  TCTL_CarLock %d \r\n",g_stVctl[tempCtl].CtrlParameter);

				tempCtl ++;
				break;		
			case TCTL_FindCar:
				g_stVctl[tempCtl].CtrlCode = VehicleBody_LockID; 
				g_stVctl[tempCtl].CtrlParameter = item.SubitemParam[0];
				tempCtl ++;
				break;
			case TCTL_SetPower:
				g_stVctl[tempCtl].CtrlCode = VehicleBody_LockID; 
				g_stVctl[tempCtl].CtrlParameter = item.SubitemParam[1];
				tempCtl ++;
				break;
			case TCTL_SetEngine:
				g_stVctl[tempCtl].CtrlCode = VehicleBody_LockID; 
				g_stVctl[tempCtl].CtrlParameter = item.SubitemParam[1];
				tempCtl ++;
				break;
			case TCTL_SetSleepTime:
				g_stVctl[tempCtl].CtrlCode = VehicleBody_LockID; 
				g_stVctl[tempCtl].CtrlParameter = item.SubitemParam[1];
				tempCtl ++;
				break;
			case TCTL_ImmediatelySleep:
				g_stVctl[tempCtl].CtrlCode = VehicleBody_LockID; 
				g_stVctl[tempCtl].CtrlParameter = item.SubitemParam[1];
				tempCtl ++;
				break;
			case TCTL_SetSleepUpStsTime://
/*				g_stVcfg[tempCfg].ParameterID = ReportTime_ID; 
				g_stVcfg[tempCfg].ParameterLen = 1;
				g_stVcfg[tempCfg].ParameterValue[] = item.SubitemParam[1];*/
				tempCfg ++;
				break;
			case TCTL_CheckBleSts://

				break;
			case TCTL_SetBroad:

				break;
			case TCTL_DownKey:

				break;
			case TCTL_ConfigCarType://
				g_stVcfg[tempCfg].ParameterID = SetCar_ID; 
				g_stVcfg[tempCfg].ParameterLen = 1;
				g_stVcfg[tempCfg].ParameterValue = item.SubitemParam[1];
				tempCfg ++;
				break;
			case TCTL_SetAlarmVol://

				break;
			case TCTL_ResetEC3CAN://

				break;
			case TCTL_SetDeviceNum://

				break;
			case TCTL_FactoryReset://

				break;
			case TCTL_ImmediatelyRouse://

				break;
			case TCTL_SetLOG://

				break;
			case TCTL_UpLOG://

				break;
			case TCTL_UpGPS://

				break;
			case TCTL_ReturnCar://

				break;
			case TCTL_SetBleTokenTime://

				break;

			default:
				break;

		}
	}
	*CtlTotal = tempCtl;

}

VCTL_RemoteCtrl_ST VCTL_GetRemoteCtrl(uint8_t index)
{
    uint16_t Value = 0 ;
    VCTL_RemoteCtrl_ST VCTLRemoteCtrl;
    
   // VCTLRemoteCtrl = g_stVctl[index];
    memcpy(&VCTLRemoteCtrl,&g_stVctl[index],sizeof(VCTL_RemoteCtrl_ST));
   // VCTLRemoteCtrl.CtrlSerialNumber = ((g_stVctl[index].CtrlSerialNumber << 8) & 0xFF00) | ((g_stVctl[index].CtrlSerialNumber >> 8) & 0x00FF);

    return VCTLRemoteCtrl;
}

VCTL_VeichleConfig_ST VCTL_GetVeichleConfig(uint8_t index)
{
    uint16_t Value = 0 ;
    VCTL_VeichleConfig_ST VCTLVeichleConfig;
    
    
    VCTLVeichleConfig = g_stVcfg[index];

    return VCTLVeichleConfig;
}



