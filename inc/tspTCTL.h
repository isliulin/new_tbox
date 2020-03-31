#ifndef TSPTCTL_H
#define TSPTCTL_H
#include "TCTL.h"

void tsp_SndQueue_RemoteCtrl(uint8_t *pBuf, uint8_t len);

uint8_t tspTCTL_Parse(uint8_t *pBuf, uint8_t len, fwTCTL_RemoteCtrl_UN *pData);



#endif

