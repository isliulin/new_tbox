#ifndef __NVM_H__
#define  __NVM_H__

#include "BaseThread.h"
#include "framework.h"
#include "QueueCommon.h"

#define NVM_DEBUG  1

#if NVM_DEBUG
#define NVMLOG(format, ...) printf("== NVM == FILE: %s, FUN: %s, LINE: %d " format "\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define NVM_NO(format, ...) printf(format,##__VA_ARGS__)
#else
#define NVMLOG(format,...)
#define NVM_NO(format,...)
#endif

#define C_TRUE 0
#define C_FALSE -1

#define SSSS_LEN   4
#define SSS_LEN    8
#define SS_LEN     16
#define S_LEN      32
#define M_LEN      64
#define L_LEN      128
#define XL_LEN     256

#define BUFSIZE 1024

#define SAVE_FILE_PATH  "/data/nvm"

#define NVM_THREAD_PERIOD    (200*1000)//MS
#define CNT_SYNC_5S        ((5*1000*1000)/NVM_THREAD_PERIOD)

#pragma pack(1)

typedef enum NVM_WorkMode_Tag {
  NVMWORK_NOTREADY,
  NVMWORK_INIT,
  NVMWORK_NORMAL,
  NVMWORK_ALL
} NVM_WorkMode_ENUM;

typedef struct NVM_FirmwareParameter_Tag {
  uint16_t VIN;
  uint16_t IMEI;
  uint16_t CMEI;
  uint16_t ICCID;
} NVM_FirmwareParameter_ST;

typedef struct NVM_UpgradeParameter_Tag {
  uint16_t m_OLD_OSID;
  uint16_t m_OLD_MCU_VERSION;
  uint16_t m_OLD_BLE_VERSION;
  uint16_t m_OLD_OS_VERSION;
  uint16_t m_NEW_OSID;
  uint16_t m_NEW_MCU_VERSION;
  uint16_t m_NEW_BLE_VERSION;
  uint16_t m_NEW_OS_VERSION;
} NVM_UpgradeParameter_ST;

typedef struct NVM_MQTT_Config_Tag {
  uint8_t m_HOST[M_LEN];    //MQTT Server Host
  uint16_t m_PORTS;
  uint16_t m_uReportFrequency;
  uint8_t m_PSK_ID[M_LEN];
  uint8_t m_Car_Type[SS_LEN];
  uint8_t m_hexPSK[M_LEN];
  uint8_t m_strPSK[M_LEN];
  uint16_t m_Alarm;
  uint16_t m_reportFrequency;
  bool m_LogSwitch;
  uint8_t m_PASSWARD[M_LEN];
  uint8_t m_CILENT_Id[M_LEN];
  uint8_t m_USERNAME[M_LEN];
  uint16_t m_PORT;
  uint16_t m_KEEP_ALIVE;
  uint16_t m_MSG_MAX_SIZE;
  uint16_t m_TOPIC_MAX_SIZE;
  int m_TestOrNormal;
} NVM_MQTT_Config_ST;

typedef struct NVM_DataParameter_Tag {
  NVM_FirmwareParameter_ST m_stFirmwareParameter;
  NVM_UpgradeParameter_ST m_stUpgradeParameter;
  NVM_MQTT_Config_ST m_MQTTConfig_;
} NVM_DataParameter_ST;

typedef struct {
  char cp[XL_LEN];
} NVMConfigPart;
typedef struct {
  char sp[XL_LEN];
} NVMSystemPart;
typedef struct {
  char ep[XL_LEN];
} NVMErrorPart;

typedef struct NVM_FileFormat_Tag {
  NVMConfigPart config_part_;
  NVMSystemPart system_part_;
  NVMErrorPart error_part_;
  uint32_t checkSum;
} NVM_FileFormat_ST;

class CNvm : public TiotThread {
 public:
  static CNvm *GetInstance();
  static void FreeInstance();
  CNvm();
  virtual ~CNvm();
  BOOL Init();
  void Deinit();
  void Run();
  Framework *mFramework;

 protected:
  BOOL Processing();
  BOOL TimeoutProcessing();
  static CNvm *m_instance;
  BOOL m_binit;
  void ReceiveQueueTestTask();
  void SendQueueTestTask();

 public:
  //init crc list
  static void initCrcTable();
  //gen crc32
  static uint32_t getCRC32(uint32_t crc, uint8_t *buffer, uint32_t size);
  static int calcImgCRC(uint8_t *buffer, uint32_t *incrc);

 private:
  bool nvm_SyncProcess();

  int nvm_SendQueue(uint8_t Gr, uint8_t Id);
  int nvm_RecvQueue();

  int WriteFile();
  int ReadFile();
  int ParseFile();
  int ParseData(uint8_t *buffer, uint32_t size);

  int NVMMonitor();
  int NVMCompare();
  int nvm_Init();
  int GetInitSts();
  int NVMSendProcess();
  int NVMRecvProcess();

 private:
  int QuId_NVM_2_FW;
  int QuId_FW_2_NVM;
  NVM_DataParameter_ST m_stDataParameter;

  NVM_WorkMode_ENUM m_u8WorkMode;
  QueueInfo_ST m_stRevMsg;
  QueueInfo_ST m_stSndMsg;

  NVM_FileFormat_ST m_wFileFormat;
  NVM_FileFormat_ST m_rFileFormat;

  static uint32_t m_u32CrcTable[XL_LEN];
};

#endif
