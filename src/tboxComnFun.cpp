
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#include "tboxComnFun.h"


int Common_GetFileCRCAndLen(char *fileName, unsigned int *crc,uint32_t *len);


unsigned int Crc16Check(unsigned char* pData, uint32_t len, unsigned int sumInitCrc)
{
	//unsigned int sumInitCrc = 0xffff;
	unsigned int ui16Crc = 0;
	unsigned int i;
	unsigned char j;
	unsigned char shiftBit;
	
	for(i = 0; i < len; i ++)
	{		
		sumInitCrc ^= pData[i];
		
		for(j = 0; j < 8; j ++)
		{
			shiftBit = sumInitCrc&0x01;
			sumInitCrc >>= 1;
			if(shiftBit != 0)
			{
				sumInitCrc ^= 0xa001;
			}		
		}
	}
	
	ui16Crc = sumInitCrc;
	return ui16Crc;
}

int Common_GetFileCRCAndLen(char *fileName, unsigned int *crc,uint32_t *len)
{
	int nRead = 0;
	uint8_t buff[1024] = {0};
	int fd = 0;
	unsigned int u16InitCrc = 0xffff;

	if(fileName == NULL || crc == NULL || len == NULL)
	{
		return -1;
	}

	fd = open(fileName, O_RDONLY);
	if (fd < 0)
	{
		printf("Open file:%s error.\n", fileName);
		return -1;
	}
	else
	{
		printf("Open file:%s success.\n", fileName);
	}

	//fseek(fd, 0L, SEEK_END);
	
	//*len = ftell(fd);
	
	//fseek(fd, 0L, SEEK_SET);

	
	memset(buff, 0, sizeof(buff));
	
	while ((nRead = read(fd, buff, sizeof(buff))) > 0)
	{
		u16InitCrc = Crc16Check(buff, nRead, u16InitCrc);
		*len += nRead;
	}
	
	*crc = u16InitCrc;
	
	close(fd);
	fd = -1;

	return 0;
}

