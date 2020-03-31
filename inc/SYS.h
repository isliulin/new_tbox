#ifndef SYS_H
#define SYS_H

#include "common.h"
uint16_t SYS_GetHostSerialNumber();
uint8_t SYS_GetHardwareSts();
uint8_t SYS_GetPhoneSts();
uint8_t SYS_VPAK_4GValue();
uint8_t SYS_GetWifiFault();
uint8_t SYS_GetSIMSts();
uint8_t SYS_GetUSBFault();
uint8_t SYS_GetBTFault();
uint8_t SYS_LogSwitch();
uint8_t SYS_HostEngineSVersions(uint8_t *pData);


#endif

