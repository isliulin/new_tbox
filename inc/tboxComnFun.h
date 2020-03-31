#ifndef  TBOXCOMMONFUN_H
#define  TBOXCOMMONFUN_H
#include "common.h"
extern int Common_GetFileCRCAndLen(char *fileName, unsigned int *crc,uint32_t *len);
extern unsigned int Crc16Check(unsigned char* pData, uint32_t len, unsigned int sumInitCrc);


#endif
