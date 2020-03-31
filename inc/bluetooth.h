
#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include "common.h"
#include "TBoxDataPool.h"
//#include "LTEModuleAtCtrl.h"
#include "ATControl.h"
//#include "GBT32960.h"
#include "FAWACP.h"
#include "VoiceCall.h"
//#include "Message.h"
#include "SMSControl.h"
#include "DataCall.h"
#include "WDSControl.h"
#include "NASControl.h"
#include "dsi_netctrl.h"
#include "TBoxDataPool.h"
#include "GpioWake.h"
#include "OTAWiFi.h"
#include "FactoryPattern_Communication.h"

#ifndef BLE_DEBUG
	#define BLE_DEBUG 	0
#endif

#if BLE_DEBUG 	
	#define BLELOG(format,...) printf("== MCU == FILE: %s, FUN: %s, LINE: %d "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define BLE_NO(format,...) printf(format,##__VA_ARGS__)
#else	
	#define BLELOG(format,...)
	#define BLE_NO(format,...)
#endif



#define BLUETOOTH_UART_DEVICE   (char*)"/dev/ttyHS0"     /* for bluetooth */

#define CTRL_NUMBER_MAX			9


#define BLE_UART_SPEED	        115200
#define MCU_UART_STOP_BITS      1
#define MCU_UART_DATA_BITS      8
#define MCU_UART_CHECK_BIT      'n'
#define MCU_UART_TIMEOUT_MSECONDS  50

#define BT_BUFF_LEN                1024
#define BLE_UPGRADE_BUFF			4096
#define CONTENT_MAX_LEN         200

#define START_SYNC_CMD          0x00	//从机同步参数指令

#define BLE_GENERAL_RESP        0x01	//从机通用应答
#define TBOX_GENERAL_RESP       0x81	//主机通用应答
#define BLE_Control_CMD         0x02	//从机蓝牙控制
#define TBOX_Setup_CMD          0x82	//TBOX设置从机参数
#define BLE_Reply_State         0x03	//从机查询参数应答
#define TBOX_Check_State        0x83	//主机查询参数
#define BLE_Control_Reply       0x04


#define 	CMD_NEED_SEND_SECOND_OPEN_LOCK     		0x01
#define 	CMD_NEED_SEND_SECOND_POWRR_INVALID   	0x02

#define   BLE_CONNECT_STATUS_UNKOWN     		0x02
#define   BLE_CONNECT_STATUS_DISCONNECT     	0x01
#define   BLE_CONNECT_STATUS_CONNECTED     	0x00


#ifndef BLE_FILE_NAME
#define BLE_FILE_NAME         (char*)"/data/BLEAPP"
#endif

#ifndef RM_BLE_FILE
#define RM_BLE_FILE           (char*)"rm -rf /data/BLEAPP"
#endif


class CMcu;
class CFAWACP;
class bleUart
{
private:
	bleUart();
public:
	static bleUart *Get_BleInstance();
	static void *StartBleThread(void *arg);
	void *BleThread(void);
	int SendReqConnectStatusToBLE(void);
	void Run_BleTask();
	int bleUartInit();
	int setUartSpeed(int fd, int speed);
	int setUartParity(int fd, int databits, int stopbits, int parity);
	int bleUartReceiveData();
	//int registerCallback_reportDate(callBack_reportDate func);
	uint32_t checkBleUartData(unsigned char *pData, unsigned int size);
	int unpackBleUartData(uint8_t *pData, unsigned int datalen);
	unsigned int Crc16Check(unsigned char* pData, uint32_t len);
	int unpack_Control(unsigned char *pData, unsigned int len);
	int unpack_blereply(unsigned char *pData, unsigned int len);
	int unpack_bleinfo(unsigned char *pData, unsigned int len);
	int packDataWithRespone(uint8_t responeCmd, uint8_t subCmd, uint8_t *needToPackData, uint16_t len, uint16_t serialNumber);
	
	int packbleProtocolData(uint8_t responeCmd, uint8_t subCmd, uint8_t *needToPackData, uint16_t len, uint16_t serialNum);
	int escape_bleUart_data(unsigned char *pData, int len);
	static int cb_bleRemoteCtrlCmd();
	void cb_bleRemoteConfigInfo(void);

	int unpack_bleRemoteCtrl(unsigned char *pData, uint16_t datalen);
	int write_bleUart_data(unsigned char *pData, int len);
	void close_uart();
	int calculate_files_CRC(char *fileName, uint32_t *crc);
	//unsigned int crc32Check(unsigned char *buff, unsigned int size);
	unsigned int crc32Check(unsigned int crc, unsigned char *buff, unsigned int size);

	int pack_bleUart_upgrade_data(unsigned char cmd, bool isReceivedDataCorrect, int mcuOrPlcFlag);
	unsigned short int pack_bleupgrade_cmd(unsigned char *pData, int len, int mcuOrPlcFlag);
	unsigned short int pack_bleupgrade_data(unsigned char *pData, int len, bool isReceivedDataCorrect, int mcuOrPlcFlag);
	void ble_apply_for_data(unsigned char* pData, uint32_t len);
	uint8_t GetBleConnectStatus(void);
	int SendCarInfoToBLE(void);
	void SetBleConnectStatus(uint8_t u8ConnectStatus);

	uint32_t dataSize;
	uint32_t crc32;
	int m_id;
	uint8_t m_BleReqFlag;
	uint8_t m_BleToken[6];
	uint8_t m_BleConnectStatus;
	uint8_t m_u8BleUpdateConnectTimeout;

    	unsigned char dataPacketOffsetAddr[4];
	unsigned char dataPacketLen[2];
	unsigned char fileName[64];
	CFAWACP *pfawacp;
	CMcu *m_CMcu;
	pthread_t m_BleThreadId;

private:
	int fd;
	static bleUart * m_bleUart;
};

extern TBoxDataPool *dataPool;
//extern LTEModuleAtCtrl *LteAtCtrl;

#endif
