#ifndef BIF_H
#define BIF_H

#include <stdio.h>
//add chen
#include "common.h"


#define BIF_YEAR_SCALE   (1)
#define BIF_YEAR_OFFSET  (100)
#define BIF_BLESTS_DATA_LEN  (256)

#pragma pack(1)

typedef struct  BIF_InfoHead_Tag												 
{	
	uint8_t			   		HeadFlag;
	uint16_t			   	BodyLen; 	
	uint8_t			   		CmdID;	
	uint16_t			   	SerialNumber;   
	uint8_t			   		Attr[2]; 			
	
}BIF_InfoHead_ST;

typedef struct  BIF_BleState_Tag
{                       
    uint8_t     MAC[6];
    uint8_t     WorkSts;
    uint8_t     LinkSts;
    uint8_t     SecretKey[6];
    uint8_t     BroadName[6];
}BIF_BleState_ST;

typedef union BIF_InfoBody_Tag
{

	BIF_BleState_ST				BleState;

}BIF_InfoBody_UN;

typedef struct BIF_RemoteCtrl_Tag
{

	BIF_InfoHead_ST				Head;	
	BIF_InfoBody_UN    			Body;
}BIF_RemoteCtrl_ST;



typedef union BIF_VeichleInformation_Tag
{
	uint8_t Buf[BIF_BLESTS_DATA_LEN];	
	BIF_RemoteCtrl_ST    	stBifInfo;
}BIF_VeichleInformation_UN;


/*typedef union BIF_VeichleInformation_Tag
{
    uint8_t             Buf[BIF_BLESTS_DATA_LEN];   
    BIF_BleState_ST     BleState;

}BIF_VeichleInformation_UN;
*/
#pragma pack()


uint8_t BIF_Init(void);
uint8_t BIF_DeInit(void);
uint8_t bif_Analysis(uint8_t *outBuf,uint8_t *inBuf,uint8_t inLen,uint8_t *outLen);
uint16_t bif_Crc16Check(uint8_t *pBuf,uint8_t len);

uint8_t BIF_Parse(uint8_t *pBuf, uint8_t len, BIF_VeichleInformation_UN *bleBif);

uint16_t VIF_GetBodyLen(void);
uint8_t BIF_GetCmdID(void);
uint16_t BIF_GetSerialNumber(void);
uint8_t BIF_GetAttr(uint8_t *pData,uint16_t &len);
uint8_t BIF_GetMAC(uint8_t *pData,uint16_t &len);
uint8_t BIF_GetWorkSts(void);
uint8_t BIF_GetSecretKey(uint8_t *pData,uint16_t &len);
uint8_t BIF_GetBroadName(uint8_t *pData,uint16_t &len);

#endif
