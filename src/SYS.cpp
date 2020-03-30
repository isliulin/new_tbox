#include <fstream>
#include <iostream>
#include <string.h>
#include "json.h"
#include <sys/time.h>
#include "SYS.h"


uint16_t SYS_GetHostSerialNumber()
{
	return 0;
}

uint8_t SYS_GetHardwareSts()
{
	return 0;
}
uint8_t SYS_GetPhoneSts()
{
	return 0;
}
uint8_t SYS_VPAK_4GValue()
{
	return 0;
}
uint8_t SYS_GetWifiFault()
{
	return 0;
}
uint8_t SYS_GetSIMSts()
{
	return 0;
}
uint8_t SYS_GetUSBFault()
{
	return 0;
}
uint8_t SYS_GetBTFault()
{
	return 0;
}
uint8_t SYS_LogSwitch()
{
	return 0;
}
uint8_t SYS_HostEngineSVersions(uint8_t *pData)
{
	memset(pData,0,11);

	return TRUE;
}

