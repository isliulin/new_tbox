#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <unistd.h>
#include "string.h"
#include "BaseThread.h"
#include "QueueCommon.h"
#include "VUPG.h"

#define FW_DEBUG

#ifdef FW_DEBUG
#define FWLOG(format,...) printf("### FW ### %s [%d] "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
#define FW_NO(format,...) printf(format,##__VA_ARGS__)
#else

#define FWLOG(format,...)
#define FW_NO(format,...)
#endif

struct msgstru
{
    int  Gr;
    int  ID;
};


//#define MCU_SYNC_CNT   100

typedef enum FW_ThreadID_Tag
{
    ID_THREAD_OTA,
    ID_THREAD_FW,
    ID_THREAD_PWR,
    ID_THREAD_MCU,
    ID_THREAD_TSP,
    ID_THREAD_BLE,
    ID_THREAD_NVM,
    ID_THREAD_LOG,
    ID_THREAD_ALL

}FW_ThreadID_EN;

class Framework : public TiotThread
{
public:
    Framework();
    static Framework* GetInstance();
    BOOL Run();
    BOOL Init();
    BOOL Information_Parse(char Gr,char id,uint16_t len);
    void Rcv_Queue_Mcu_To_FW();
    void Rcv_Queue_Ble_To_FW();
    void Rcv_Queue_Tsp_To_FW();
    void Rcv_Queue_Ota_To_FW();
    void Rcv_Queue_Pwr_To_FW();
    void Rcv_Queue_Nvm_To_FW();
    void Rcv_Queue_Log_To_FW();

    int fw_SendQueue(uint8_t Id);
    int fw_McuSendQueue(uint8_t Id);
    int fw_PwrSendQueue(uint8_t Id);
    int fw_OtaSendQueue(uint8_t Id);
    int fw_NvmSendQueue(uint8_t Id);
    int fw_TspSendQueue(uint8_t Id);
    int fw_BleSendQueue(uint8_t Id);
    int fw_LogSendQueue(uint8_t Id);
	
    BOOL fw_ParseMcu(uint8_t Id);
    BOOL fw_ParseMcu_VecihleReprot(void);
    BOOL fw_ParseMcu_SyncAck(void);
	BOOL fw_ParseMcu_CommAck(void);
	BOOL fw_ParseMcu_Remoet_Ctl_Ack(void);
	BOOL fw_ParseMcu_Upg_Begain(void);

	BOOL fw_ParseTsp(uint8_t Id);
	BOOL fw_ParseTsp_RemoetCtl(void);
	BOOL fw_ParseTsp_Token(void);
	
	BOOL fw_ParseBle(uint8_t Id);
    BOOL fw_ParseBle_Token_Req(void);
    BOOL fw_ParseBle_Ctl_Cmd(void);
	
	BOOL fw_ParseOta(uint8_t Id,uint16_t len);
	BOOL fw_ParsePwr(uint8_t Id);



#if VIVIAN_ADD_MONITOR
    void fw_Thread_Monitor_Init(void);
    void fw_Thread_Monitor_Process(void);
#endif

protected:
	BOOL Framework_InitSts;
	BOOL Processing();
	void SendQueueTestTask();
	void ReceiveQueueTestTask();
    static Framework* m_instance;
	    int fw_rev_msg_len;


public:
    int   ID_Queue_MCU_To_FW;
    int   ID_Queue_BLE_To_FW;
    int   ID_Queue_TSP_To_FW;
    int   ID_Queue_OTA_To_FW;
    int   ID_Queue_PWR_To_FW;
    int   ID_Queue_NVM_To_FW;
    int   ID_Queue_LOG_To_FW;
	
    int   ID_Queue_FW_To_MCU;
    int   ID_Queue_FW_To_BLE;
    int   ID_Queue_FW_To_TSP;
    int   ID_Queue_FW_To_OTA;
    int   ID_Queue_FW_To_PWR;
    int   ID_Queue_FW_To_NVM;
    int   ID_Queue_FW_To_LOG;

#if   1 //VIVIAN_ADD_MONITOR

private:
    BOOL m_fw_syncFlg[ID_THREAD_ALL];
    uint16_t m_fw_syncCnt[ID_THREAD_ALL];

#endif

};

#endif // FRAMEWORK_H
