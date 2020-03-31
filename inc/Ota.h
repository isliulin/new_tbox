#ifndef _OTATHREAD_H_
#define _OTATHREAD_H_

#include "common.h"
#include "HMAC_MD5.h"
#include "VUPG.h"


/**/
/**/
//#define OTA_DEBUG   

#define SDK_14_SUPPORT
//#define SDK_13_SUPPORT
//#define SDK_12_SUPPORT

#ifdef OTA_DEBUG
#define OTALOG(format,...) printf("### OTA ### %s [%d] "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_NO(format,...) printf(format,##__VA_ARGS__)
#else

#define OTALOG(format, ...)
#define OTA_NO(format, ...)
#endif

#define OTA_MD5_LEN    16
#define OTA_URL_LEN    256
#define OTA_CMD_LEN    256

#define OTA_THREAD_PERIOD    (200*1000)//MS
#define TIME_WAIT_1HOUR      (60*60*24)

#define CNT_URL_TIMEOUT_5S        ((5*1000*1000)/OTA_THREAD_PERIOD)
#define CNT_URL_TIMEOUT_15S        ((15*1000*1000)/OTA_THREAD_PERIOD)

#define CNT_SYNC_5S        ((5*1000*1000)/OTA_THREAD_PERIOD)
#define CNT_DOWNLOAD_DELAY_100MS   ((1000*1000)/OTA_THREAD_PERIOD)
#define CNT_DOWNLOAD_RETRY        3

#define ISP_BUF_SIZE          512

#define ISP_CMD_SIZE          256

#define HTTP_DOWNLOAD_TIMEOUT          (30)

#ifndef OTA_MQTT_UPDATE_FILE
#define OTA_MQTT_UPDATE_FILE         (char*)"/data/mqttupdate.tar.gz"
#define OTA_RM_MQTT_UPDATE_FILE   (char*)"rm -f  /data/mqttupdate.tar.gz"
#endif

#ifndef OTA_BLE_FILE_NAME
#define OTA_BLE_FILE_NAME         (char*)"/data/BLEAPP"
#define OTA_RM_BLE_FILE_NAME         (char*)"rm -f  /data/BLEAPP"
#endif

#ifndef OTA_MCU_FILE_NAME
#define OTA_MCU_FILE_NAME         (char*)"/data/MCU.bin"
#define OTA_RM_MCU_FILE_NAME         (char*)"rm -f  /data/MCU.bin"
#endif

#ifndef OTA_LTE_FILE_NAME

#ifdef SDK_14_SUPPORT
#define OTA_LTE_FILE_NAME         (char*)"/data/ostbox16"
#define OTA_RM_LTE_FILE_NAME         (char*)"rm -f  /data/ostbox16"
#endif

#ifdef SDK_13_SUPPORT
#define OTA_LTE_FILE_NAME         (char*)"/data/ostbox17"
#define OTA_RM_LTE_FILE_NAME         (char*)"rm -f  /data/ostbox17"
#endif

#ifdef SDK_12_SUPPORT
#define OTA_LTE_FILE_NAME             (char*)"/data/ostbox18"
#define OTA_RM_LTE_FILE_NAME         (char*)"rm -f  /data/ostbox18"
#endif

#endif

typedef enum OTA_WorkMode_Tag {
  WORKMODE_NOTREADY,
  WORKMODE_INIT,
  WORKMODE_NORMAL,
  WORKMODE_ALL

} OTA_WorkMode_ENUM;

typedef enum OTA_ptcUpgradeFlow_Tag {
  FLOW_STEP1_TSP_UPGRADE_CMD_DETECT = (0),
  FLOW_STEP2_TBOX_VERSION_QUERY,
  FLOW_STEP3_TSP_VERSION_INFO,//URL & OSID
  FLOW_STEP_ALL

} OTA_ptcUpgradeFlow_ENUM;

/*------------------------------------------
0：成功;
1：失败-URL 地址相关错误
2：失败-下载失败
3：失败-MD5 检测失败
4:   本地存在升级文件
5：OS版本相同
------------------------------------------*/

typedef enum OTA_PakDownLoadResult_Tag {
  RES_OTA_DOWNLOAD_SUCCESS = (0),
  RES_OTA_DOWNLOAD_ERROR_URL,
  RES_OTA_DOWNLOAD_ERROR_DOWNLOAD,
  RES_OTA_DOWNLOAD_ERROR_MD5ERROR,
  RES_OTA_DOWNLOAD_ERROR_LOCALFILEEXIST,
  RES_OTA_DOWNLOAD_ERROR_SAMEOSVERSION,
  RES_OTA_DOWNLOAD_ALL

} OTA_PakDownLoadResult_ENUM;

typedef enum OTA_UPGRAGE_STEP_Tag {
  STEP_OTA_UPGRADE_INIT = (0),
  STEP_OTA_UPGRADE_DOWNLOAD,
  STEP_OTA_UPGRADE_DELAYTIME,
  STEP_OTA_UPGRADE_FILECHECK,
  STEP_OTA_UPGRADE_ISP_ECU,
  STEP_OTA_UPGRADE_ISP_LTE,
  STEP_OTA_UPGRADE_ISP_FINISH,
  //STEP_OTA_UPGRADE_ISP_MCU,
  //STEP_OTA_UPGRADE_ISP_BLE,
  //STEP_OTA_UPGRADE_ISP_LTE,
  //STEP_OTA_UPGRADE_FINISH_RESETREQ,
  STEP_OTA_UPGRADE_ALL

} OTA_UPGRAGE_STEP_ENUM;

typedef struct OTA_DownLoadVersion_Tag {
  uint16_t osid;
  uint8_t url[OTA_URL_LEN];
} OTA_DownLoadVersion_ST;

typedef struct OTA_DownLoadAddress_Tag {
  uint32_t offset;
  uint32_t reqlen;

} OTA_DownLoadAddress_ST;

typedef struct OTA_FwDataInfo_Tag {
  uint8_t TspSync;
  uint8_t McuSync;
  uint8_t LocalOsid;
  uint8_t CurOrLast;

} OTA_FwDataInfo_ST;

typedef union OTA_FWQueue_Tag {

  uint8_t EcuIspRes;
  uint8_t TspReq;
  OTA_DownLoadVersion_ST version;
  OTA_DownLoadAddress_ST transfer;
  OTA_FwDataInfo_ST fwData;

} OTA_FWQueue_UN;

typedef struct OTA_RevData_Tag {
  uint8_t GR;
  uint8_t ID;
  OTA_FWQueue_UN quedata;
} OTA_RevData_ST;

typedef struct OTA_ptcSendData_Tag {
  uint8_t buf[128];
  uint8_t ack;

} OTA_ptcSendData_UN;

typedef struct OTA_thdSendData_Tag {
  uint8_t data;
  uint8_t thd_t_ota_FlowState;

} OTA_thdSendData_ST;

typedef union OTA_SendDataUN_Tag {
  uint8_t buf[ISP_BUF_SIZE];
  OTA_ptcSendData_UN stPtc;
  OTA_thdSendData_ST stThd;
} OTA_SendData_ENUM;

typedef struct OTA_SendData_Tag {
  uint8_t GR;
  uint8_t ID;
  uint8_t Len;

  OTA_SendData_ENUM data;
} OTA_SendData_ST;

typedef struct OTA_SOCFileInfo_Tag {
  BOOL m_NeedIsp;

  uint32_t offfset;
  uint16_t reqLen;
  uint8_t res;
  OTA_SocSendInfo_ST Info;

  uint8_t m_upgStep;
  BOOL m_IsReq;

} OTA_SOCFileInfo_ST;

typedef struct OTA_Progress_Tag {

  OTA_SOCFileInfo_ST m_stMcu;
  OTA_SOCFileInfo_ST m_stBle;
  OTA_SOCFileInfo_ST m_stLte;

  uint8_t m_IspFileCnt;

  OTA_ptcUpgradeFlow_ENUM eumCheckStep;
  OTA_UPGRAGE_STEP_ENUM m_u8UpgradeStep;
  OTA_PakDownLoadResult_ENUM m_DownLoadRes;

  uint8_t m_OtaUpgFlg;

  //uint8_t m_u8McuIspStep;
  //uint8_t m_u8BleIspStep;
  //uint8_t m_u8LteIspStep;

} OTA_Progress_ST;

class COta : public TiotThread {

 public:
  static COta *GetInstance();
  static void FreeInstance();

  COta();

  virtual ~COta();

  BOOL Init();
  void Deinit();
  uint8_t GetInitSts(void);
  void Run();

 protected:

  BOOL Processing();
  BOOL TimeoutProcessing();

  static COta *m_instance;
  BOOL m_binit;

 private:

  OTA_Progress_ST m_stProgress;
  OTA_WorkMode_ENUM m_u8WorkMode;
  uint8_t m_u8VerCheckFlag;

  //OTA_RevData_ST  m_stRevMsg;
  QueueInfo_ST m_stRevMsg;
  //OTA_SendData_ST m_stSndMsg;
  QueueInfo_ST m_stSndMsg;
  void ReceiveQueueTestTask();
  void SendQueueTestTask();

#if 0
  BOOL m_bMcuNeedIsp;
  BOOL m_bBleNeedIsp;
  BOOL m_bLteNeedIsp;

  uint8_t ota_NvmInitSts;
  uint8_t ota_FwInitSts;
  uint8_t ota_PmInitSts;
  uint8_t ota_TspInitSts;

#endif

  int QuId_OTA_2_FW;
  int QuId_FW_2_OTA;

 private:

  uint16_t LocalOSID;
  uint16_t TspOSID;
  uint8_t m_VcurOrlast;
  uint8_t m_TspReq;

  uint8_t TspUrl[OTA_URL_LEN];

  time_t m_tLastTime;
  time_t m_tCurrentTime;

 public:
  HMAC_MD5 m_hmacMd5;

 private:

  uint8_t ota_SendQueue(uint8_t PackId, uint8_t *pBuf);
  BOOL ota_Sync_Process(void);
  BOOL ota_QueueReceive_Process(void);
  BOOL ota_Upgrade_Process(void);
  BOOL ota_STP1_CheckOTA_Condition(void);
  BOOL ota_STP2_DownLoadFile(void);
  BOOL ota_STP3_CheckDownLoadFile(void);
  BOOL ota_STP4_UpgradeMcu_Process(void);
  BOOL ota_STP4_UpgradeBle_Process(void);
  BOOL ota_STP5_UpgradeLte_Process(void);
  BOOL ota_STP6_Upgrade_Finish(void);
  int ota_SendQueue(uint8_t Gr, uint8_t Id);
  BOOL COta::ota_SearchUpgradeFile(void);

 public:
  uint8_t OTA_IsDownLoading(void);//queue
  uint8_t OTA_IsUpgrading(void);//queue
  uint8_t OTA_UpgradeStateReprot(void);//queue
  uint8_t OTA_OTAFinishResetReq(void);//queue


};

#endif

