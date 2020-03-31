#ifndef __BLE_H__
#define  __BLE_H__

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


#include "BaseThread.h"
#include "framework.h"
#include "QueueCommon.h"
#include "BIF.h"

#define BLE_THREAD_PERIOD   	(500*1000)//MS

//#define BLE_DEBUG
#ifdef BLE_DEBUG /*MCU_DEBUG*/ 	
	#define BLELOG(format,...) printf("== BLE == FILE: %s, FUN: %s, LINE: %d "format"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
	#define BLE_NO(format,...) printf(format,##__VA_ARGS__)
#else	
	#define BLELOG(format,...)
	#define BLE_NO(format,...)
#endif

#define BLUETOOTH_UART_DEVICE   (char*)"/dev/ttyHS0"     /* for bluetooth */
#define BLE_UART_SPEED	            115200
#define BLE_UART_STOP_BITS          1
#define BLE_UART_DATA_BITS          8
#define BLE_UART_CHECK_BIT          'n'
#define BLE_UART_TIMEOUT_MSECONDS   50
#define BLE_BUFF_LEN                1024
#define BLE_CMD_MAX_COUNT			2

class CBle : public TiotThread
{
	public:
	    static CBle* GetInstance();
	    static void FreeInstance();
	    static void *ble_RevQueue_Process(void *arg);
		CBle();
		virtual ~CBle();
		BOOL Init();
	    BOOL Deinit();
		BOOL GetInitSts();
	    void Run();
		void ble_Sync_Process(void);
		void ble_UartRx_Process(void);
		uint8_t ble_ReceiveQueue(uint8_t Id);
		uint8_t ble_SendQueue(uint8_t Id);
		uint32_t ble_ReadUartData(unsigned char *pData, unsigned int size);


		int bleUartInit(void);
		int SetUartParity(int fd, int databits, int stopbits, int parity);
		int SetUartSpeed(int fd, int speed);

		
		Framework*      mFramework;
		int ble_rev_msg_len;
        QueueInfo_ST bleQueueInfoRev;
        QueueInfo_ST bleQueueInfoSnd;
		
	protected:
	    static CBle* m_instance;
		int 	m_FdUart;
	    BOOL    m_binit;
	    BOOL    Processing();
	    BOOL    TimeoutProcessing();
		void    ReceiveQueueTestTask();
 		void    SendQueueTestTask();

        
        int ble_Receive_FW_Veichle_Info(void);
 		int ble_Receive_FW_Token(void);

	private:
        pthread_t   Ble_RevQueue_Id;
		uint8_t m_U8ShakeHandFlg;		
		int QuId_BLE_2_FW;	
		BIF_VeichleInformation_UN g_stBleBif;
};

#endif
